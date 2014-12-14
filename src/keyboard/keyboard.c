/*
 * Keyboard input plugin for Test Gear
 *
 * Copyright (c) 2014, Martin Lund
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT
 * HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <linux/input.h>
#include "testgear/plugin.h"
#include "libinput.h"

static int open_restricted(const char *path, int flags, void *userdata)
{
    return open(path, flags);
}

static void close_restricted(int fd, void *userdata)
{
    close(fd);
}

static struct libinput_interface interface =
{
    .open_restricted = open_restricted,
    .close_restricted = close_restricted,
};


static int keyboard_load(void)
{
    // Set defaults
    set_int("key", 0);
    set_string("device", "");
}

static int keyboard_wait_event(void)
{
    struct libinput *input;
    struct libinput_device *input_device;
    struct libinput_event *ev;
    struct libinput_event_keyboard *kev;
    struct timeval tv;
    enum libinput_key_state state;
    fd_set rdfs;
    int fd, ready;

    input = libinput_path_create_context(&interface, NULL);
    if (input == NULL)
    {
        log_error("libinput_path_create_context() failed");
        return EXIT_FAILURE;
    }

    input_device = libinput_path_add_device(input, get_string("device"));
    if (input_device == NULL)
    {
        log_error("libinput_path_add_device() failed");
        return EXIT_FAILURE;
    }

    fd = libinput_get_fd(input);

    /* Wait up to 10 seconds */
    tv.tv_sec = 10;
    tv.tv_usec = 0;

    FD_ZERO(&rdfs);
    FD_SET(fd, &rdfs);

    libinput_device_ref(input_device);

    // Dispatch
    libinput_dispatch(input);

    /* Block until input becomes available or timeout */
    while ( select(fd + 1, &rdfs, NULL, NULL, &tv) >= 0)
    {
        while ((ev = libinput_get_event(input)))
        {
            if (libinput_event_get_type(ev) != LIBINPUT_EVENT_KEYBOARD_KEY)
            {
                log_info("Discarding non-keyboard event");
                libinput_event_destroy(ev);
                libinput_dispatch(input);
                continue;
            }

            // Keyboard event detected
            kev = libinput_event_get_keyboard_event(ev);
            state = libinput_event_keyboard_get_key_state(kev);
            if (state == LIBINPUT_KEY_STATE_PRESSED)
            {
                set_int("key", libinput_event_keyboard_get_key(kev));
                libinput_device_unref(input_device);
                libinput_path_remove_device(input_device);
                return EXIT_SUCCESS;
            }

            libinput_event_destroy(ev);
            libinput_dispatch(input);
        }

        libinput_dispatch(input);

        FD_ZERO(&rdfs);
        FD_SET(fd, &rdfs);
        tv.tv_sec = 10;
    }

    return EXIT_FAILURE;
}


// Plugin properties
static struct plugin_properties keyboard_properties[] =
{
    {   .name = "device",
        .type = STRING,
        .description = "Input device" },

    {   .name = "wait_key_event",
        .type = COMMAND,
        .function = keyboard_wait_event,
        .description = "Wait for key pressed event" },

    {   .name = "key",
        .type = INT,
        .description = "Key code" },

    { }
};

// Plugin configuration
struct plugin keyboard = {
    .name = "keyboard",
    .version = "0.1",
    .description = "Keyboard input plugin",
    .author = "Martin Lund",
    .license = "BSD-3",
    .properties = keyboard_properties,
    .load = keyboard_load,
};

plugin_register(keyboard);
