/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */

#ifndef MUI_EVENT_LOOP_H
#define MUI_EVENT_LOOP_H

#include <cstdint>
#include <functional>

namespace mui
{

    typedef std::function<void (int fd, uint32_t mask, void* data)> event_callback;
    typedef std::function<void (int fd, void* data)> timer_event_callback;

    enum
    {
        EVENT_WRITEABLE = 1 << 0,
        EVENT_READABLE = 1 << 1
    };

    class EventLoop
    {
    public:

        EventLoop();

        ~EventLoop();

        void add_fd(int fd, uint32_t mask, event_callback func, void* data = 0);

        void rem_fd(int fd);

        int wait(int timeout = -1);

        int run();

        void quit();

        void close();

        int start_timer(unsigned long milliseconds, timer_event_callback func, void* data = 0);

        int start_periodic_timer(unsigned long milliseconds, timer_event_callback func, void* data = 0);

        void cancel_periodic_timer(int fd);

        void add_idle_callback(event_callback func);

    protected:

        void init();

        int epoll_fd {-1};
    };

}

#endif
