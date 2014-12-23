/*
 * USB test plugin for Test Gear
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
#include <time.h>
#include <usb.h>
#include "testgear/plugin.h"

#define USB_BUFSIZE_MAX 512

static pthread_t usb_transfer_thread;
static volatile bool running = false;
static struct usb_device *device;
static usb_dev_handle *handle;
static char tx_buffer[USB_BUFSIZE_MAX];
static char rx_buffer[USB_BUFSIZE_MAX];
static int status = 0;
static char *mode;
static int ep_in;
static int ep_out;
static int buffer_size;
static int timeout;
static int tx_errors = 0;
static int rx_errors = 0;
static int verify_errors = 0;
static double rate;
static bool verify = false;

struct usb_device *find_device(int vid, int pid)
{
    struct usb_bus *bus;
    struct usb_device *device;

    // Traverse through busses and devices to find device
    bus = usb_busses;
    while (bus != NULL)
    {
        device = bus->devices;
        while (device != NULL)
        {
            if ((device->descriptor.idVendor == vid) && (device->descriptor.idProduct == pid))
                return device;
            device = device->next;
        }
        bus = bus->next;
    }

    // Device not found
    return NULL;
}

static void *transfer(void *args)
{
    struct timespec start, stop;
    double time_start, time_stop;
    int counter;
    int tx_counter = 0;
    int rx_counter = 0;

    // Get clock start time
    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1)
        log_error("Cannot get clock time");
    time_start = start.tv_sec + start.tv_nsec * 0.000000001;

    if (strcmp(mode, "loopback") == 0)
    {
        // Loopback test
        while (running)
        {
            // Send data
            counter = usb_bulk_write(handle, ep_out, tx_buffer, buffer_size, timeout);
            if (counter < 0)
            {
                log_error("Error sending data: %s", usb_strerror());
                tx_errors++;
                usleep(200000); // Wait 200 ms
                continue;
            }
            tx_counter += counter;

            // Receive data
            counter = usb_bulk_read(handle, ep_in, rx_buffer, buffer_size, timeout);
            if (rx_counter < 0)
            {
                log_error("Error reading data: %s", usb_strerror());
                usleep(200000); // Wait 200 ms
                rx_errors++;
                continue;
            }
            rx_counter += counter;

            // Verify tx vs rx buffer
            if (verify)
            {
                int i;
                for (i=0; i<counter; i++)
                    if (rx_buffer[i] != tx_buffer[i])
                        verify_errors++;
            }
        }
    } else
    {
        // TX test
        while (running)
        {
            // Send data
            counter = usb_bulk_write(handle, ep_out, tx_buffer, buffer_size, timeout);
            if (counter < 0)
            {
                log_error("Error sending data: %s", usb_strerror());
                tx_errors++;
                usleep(200000); // Wait 200 ms
                continue;
            }
            tx_counter += counter;
        }
    }

    // Get clock stop time
    if (clock_gettime(CLOCK_MONOTONIC, &stop) == -1)
        log_error("Cannot get clock time");
    time_stop = stop.tv_sec + stop.tv_nsec * 0.000000001;

    // Print elapsed time
    double seconds = time_stop - time_start;
    log_info("Elapsed time: %f s", seconds);

    // Print size of data transmitted
    log_info("Number of bytes sent: %d bytes", tx_counter);
    log_info("Number of bytes received: %d bytes", rx_counter);

    // Report performance result
    if (seconds)
        rate = (tx_counter + rx_counter) / seconds;
    log_info("Bandwidth rate: %.2f bytes/s", rate);
    log_info("TX errors: %d", tx_errors);
    log_info("RX errors: %d", rx_errors);
    log_info("Verify errors: %d", verify_errors);

    return;
}

static int usb_load(void)
{
    // Set defaults
    set_string("mode", "loopback");
    set_int("vid", 0x0);
    set_int("pid", 0x0);
    set_int("ep_in", 0x0);
    set_int("ep_out", 0x0);
    set_int("buffer_size", 512);
    set_char("verify", 0);
    set_int("timeout", 1000);
    set_int("tx_errors", 0);
    set_int("rx_errors", 0);
    set_int("verify_errors", 0);
    set_double("rate", 0);
}

static int usb_start(void)
{
    int i;
    pthread_attr_t thread_attr;
    struct sched_param sched_parameter;

    if (running == true)
    {
        log_error("USB test already running");
        return EXIT_FAILURE;
    }

    mode    = get_string("mode");
    int vid = get_int("vid");
    int pid = get_int("pid");
    ep_in   = get_int("ep_in");
    ep_out  = get_int("ep_out");
    timeout = get_int("timeout");
    verify  = get_char("verify");
    buffer_size = get_int("buffer_size");

    // Check test mode
    if ((strcmp(mode, "loopback") != 0) && (strcmp(mode, "tx") != 0))
    {
        log_error("Invalid USB test mode (must be \"loopback\" or \"tx\")");
        return EXIT_FAILURE;
    }

    // Check buffer size
    if ((buffer_size != 512) && (buffer_size != 64))
    {
        log_error("Invalid USB buffer size (must be 512 or 64)");
        return EXIT_FAILURE;
    }

    // Fill transmit buffer
    srand(time(NULL));
    for (i=0; i < buffer_size; i++)
        tx_buffer[i] = rand();

    // Log configuration
    log_info("USB test configuration:");
    log_info(" mode=%s", mode);
    log_info(" vid=%x", vid);
    log_info(" pid=%x", pid);
    log_info(" ep_in=%d", ep_in);
    log_info(" ep_out=%d", ep_out);
    log_info(" buffer_size=%d", buffer_size);
    log_info(" timeout=%d", timeout);

    // Initialize
    usb_init();         // Initialize libusb
    usb_find_busses();  // Find all USB busses on system
    usb_find_devices(); // Find all devices on all USB busses

    device = find_device(vid, pid);
    if (device == NULL)
    {
        log_error("Cannot find device (vid=%x, pid=%x)", vid, pid);
        return EXIT_FAILURE;
    }

    handle = usb_open(device);
    if (handle == NULL)
    {
        log_error("Cannot open usb device (vid=%x, pid=%x)", vid, pid);
        return EXIT_FAILURE;
    }

    if (usb_claim_interface(handle, 0) < 0)
    {
        log_error("Cannot claim interface 0: %s", usb_strerror());
        usb_close(handle);
        return EXIT_FAILURE;
    }

    // Initialize thread attributes
    pthread_attr_init(&thread_attr);

    // Set thread scheduling policy
    pthread_attr_setschedpolicy(&thread_attr, SCHED_FIFO);

    // Set thread priority to maximum
    sched_parameter.sched_priority = sched_get_priority_max(SCHED_FIFO);
    pthread_attr_setschedparam(&thread_attr, &sched_parameter);

    log_info("Starting USB test");

    // Reset performance metrics
    rx_errors = 0;
    tx_errors = 0;
    rate = 0;

    // Start USB transfer test thread
    running = true;
    pthread_create(&usb_transfer_thread, &thread_attr, transfer, NULL);

    return EXIT_SUCCESS;
}

static int usb_stop(void)
{
    int i;

    if (running == false)
    {
        log_error("USB test already stopped");
        return EXIT_FAILURE;
    }

    log_info("Stopping USB test");

    // Stop transfer thread
    running = false;
    pthread_join(usb_transfer_thread, NULL);

    // Shut down USB
    usb_release_interface(handle, 0);
    usb_close(handle);

    // Save results
    set_int("tx_errors", tx_errors);
    set_int("rx_errors", rx_errors);
    set_int("verify_errors", verify_errors);
    set_double("rate", rate);

    return EXIT_SUCCESS;
}


// Plugin properties
static struct plugin_properties usb_properties[] =
{
    {   .name = "mode",
        .type = STRING,
        .description = "Test mode (loopback,tx - default: loopback)" },

    {   .name = "verify",
        .type = CHAR,
        .description = "Verify tx/rx data (0,1 - default: 1)" },

    {   .name = "vid",
        .type = INT,
        .description = "USB device vendor ID" },

    {   .name = "pid",
        .type = INT,
        .description = "USB device product ID" },

    {   .name = "ep_in",
        .type = INT,
        .description = "USB input endpoint" },

    {   .name = "ep_out",
        .type = INT,
        .description = "USB output endpoint" },

    {   .name = "buffer_size",
        .type = INT,
        .description = "USB buffer size (64,512 - default: 512)" },

    {   .name = "timeout",
        .type = INT,
        .description = "USB communication timeout" },

    {   .name = "tx_errors",
        .type = INT,
        .description = "USB TX error count" },

    {   .name = "rx_errors",
        .type = INT,
        .description = "USB RX error count" },

    {   .name = "verify_errors",
        .type = INT,
        .description = "TX vs RX data verification errors" },

    {   .name = "rate",
        .type = DOUBLE,
        .description = "Tested USB bandwidth rate [b/s]" },

    {   .name = "start",
        .type = COMMAND,
        .function = usb_start,
        .description = "Start USB test command" },

    {   .name = "stop",
        .type = COMMAND,
        .function = usb_stop,
        .description = "Stop USB test command" },

    { }
};

// Plugin configuration
struct plugin usb =
{
    .name = "usb",
    .version = "0.1",
    .description = "USB test plugin",
    .author = "Martin Lund",
    .license = "BSD-3",
    .properties = usb_properties,
    .load = usb_load,
};

plugin_register(usb);
