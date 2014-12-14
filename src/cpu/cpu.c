/*
 * CPU stress test plugin for Test Gear
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
#include "testgear/plugin.h"

#define MAX_THREADS 64

static pthread_t stress_thread[MAX_THREADS];
static bool running = false;
static int threads;

static void *cpu_stress(void *args)
{
    long counter;

    while (running)
        counter++;
}

static int cpu_load(void)
{
    // Set defaults
    set_int("threads", 1);
}

static int cpu_start(void)
{
    int i;
    pthread_attr_t thread_attr;
    struct sched_param sched_parameter;

    if (running == true)
    {
        log_error("CPU stress test already started");
        return EXIT_FAILURE;
    }

    threads = get_int("threads");

    // Initialize thread attributes
    pthread_attr_init(&thread_attr);

    // Set thread scheduling policy
    pthread_attr_setschedpolicy(&thread_attr, SCHED_FIFO);

    // Set thread priority to maximum
    sched_parameter.sched_priority = sched_get_priority_max(SCHED_FIFO);
    pthread_attr_setschedparam(&thread_attr, &sched_parameter);

    log_info("Starting CPU stress (threads=%d)", threads);

    // Start CPU stress thread(s)
    running = true;
    for (i=0; i<threads; i++)
        pthread_create(&stress_thread[i], &thread_attr, cpu_stress, NULL);

    return EXIT_SUCCESS;
}

static int cpu_stop(void)
{
    int i;

    if (running == false)
    {
        log_error("CPU stress test already stopped");
        return EXIT_FAILURE;
    }

    log_info("Stopping CPU stress test");

    // Stop thread(s)
    running = false;
    for (i=0; i<threads; i++)
        pthread_join(stress_thread[i], NULL);

    return EXIT_SUCCESS;
}


// Plugin properties
static struct plugin_properties cpu_properties[] =
{
    {   .name = "threads",
        .type = INT,
        .description = "Number of CPU stress threads (default: 1)" },

    {   .name = "start",
        .type = COMMAND,
        .function = cpu_start,
        .description = "Start CPU stress command" },

    {   .name = "stop",
        .type = COMMAND,
        .function = cpu_stop,
        .description = "Stop CPU stress command" },

    { }
};

// Plugin configuration
struct plugin cpu = {
    .name = "cpu",
    .version = "0.1",
    .description = "CPU stress test plugin",
    .author = "Martin Lund",
    .license = "BSD-3",
    .properties = cpu_properties,
    .load = cpu_load,
};

plugin_register(cpu);
