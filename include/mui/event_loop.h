/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */

#ifndef MUI_EVENT_LOOP_H
#define MUI_EVENT_LOOP_H

#include <stdint.h>

namespace mui
{

    typedef void (*event_callback)(int fd, uint32_t mask, void* data);
    typedef void (*timer_event_callback)(int fd, void* data);

    enum
    {
        EVENT_WRITEABLE = 1 << 0,
        EVENT_READABLE = 1 << 1
    };

    class EventLoop
    {
    public:

        static void init();

        static void add_fd(int fd, uint32_t mask, event_callback func, void* data = 0);

        static void rem_fd(int fd);

        static int wait(int timeout = -1);

        static int run();

        static void quit();

        static void close();

        static int start_timer(unsigned long milliseconds, timer_event_callback func, void* data = 0);

        static int start_periodic_timer(unsigned long milliseconds, timer_event_callback func, void* data = 0);

        static void cancel_periodic_timer(int fd);

        static void add_idle_callback(event_callback func);
    };

}

#endif
