/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */

#include "event_loop.h"
#include "widget.h"
#include "window.h"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <chrono>
#include <numeric>

using namespace std;

static int epoll_fd = -1;

namespace mui
{
    struct event_source
    {
        int fd;
        event_callback func;
        void* data;
    };

    void EventLoop::init()
    {
        if (epoll_fd < 0)
        {
            epoll_fd = epoll_create(16);
            assert(epoll_fd >= 0);
        }
    }

    void EventLoop::rem_fd(int fd)
    {
        if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) < 0)
        {
            //assert(0);
        }
        // crap
        //::close(fd);
    }

    void EventLoop::add_fd(int fd, uint32_t mask, event_callback func, void* data)
    {
        EventLoop::init();

        assert(fd >= 0);

        event_source* source;
        epoll_event ep;

        source = new event_source();
        source->fd = fd;
        source->func = func;
        source->data = data;

        ep.events = 0;
        if (mask & EVENT_READABLE)
            ep.events |= EPOLLIN;
        if (mask & EVENT_WRITEABLE)
            ep.events |= EPOLLOUT;
        ep.data.ptr = source;

        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ep) < 0)
        {
            assert(0);
            delete source;
        }
    }

#define ARRAY_LENGTH(a) (sizeof (a) / sizeof (a)[0])

    int EventLoop::wait(int timeout)
    {
        EventLoop::init();

        struct epoll_event ep[32];
        event_source* source;
        int i, count;
        uint32_t mask;

        count = epoll_wait(epoll_fd, ep, ARRAY_LENGTH(ep), timeout);
        if (count < 0)
            return -1;

        for (i = 0; i < count; i++)
        {
            source = reinterpret_cast<event_source*>(ep[i].data.ptr);
            mask = 0;
            if (ep[i].events & EPOLLIN)
                mask |= EVENT_READABLE;
            if (ep[i].events & EPOLLOUT)
                mask |= EVENT_WRITEABLE;

            source->func(source->fd, mask, source->data);
        }

        return 0;
    }

    static bool do_quit = false;

    void EventLoop::quit()
    {
        do_quit = true;
    }

    int EventLoop::run()
    {
        EventLoop::init();

#ifdef STATS
        vector<uint64_t> times;
#endif
        do_quit = false;
        while (!do_quit)
        {
#ifdef STATS
            auto start = chrono::steady_clock::now();
#endif
            for (auto& w : windows())
            {
                if (w->visible())
                    w->draw();
            }

            wait();

#ifdef STATS
            auto end = chrono::steady_clock::now();
            auto diff = end - start;
            times.push_back(chrono::duration<double, milli>(diff).count());

            static unsigned int COUNT = 50;
            if (times.size() == COUNT)
            {
                auto avg = std::accumulate(times.begin(), times.end(), 0) / COUNT;
                if (avg > 0)
                    cout << 1000. / avg << " fps" << endl;
                times.clear();
            }
#endif
        }

        return 0;
    }

    void EventLoop::close()
    {
        ::close(epoll_fd);
    }

    struct timer_data
    {
        timer_event_callback func;
        void* data;
    };

    static void timer_event(int fd, uint32_t mask, void* data)
    {
        struct timer_data* tdata = reinterpret_cast<struct timer_data*>(data);
        assert(tdata);

        tdata->func(fd, tdata->data);

        delete tdata;

        EventLoop::rem_fd(fd);

        close(fd);
    }

    static void ms2ts(struct timespec* ts, unsigned long ms)
    {
        ts->tv_sec = ms / 1000ULL;
        ts->tv_nsec = (ms % 1000ULL) * 1000000ULL;
    }

    int EventLoop::start_timer(unsigned long milliseconds, timer_event_callback func, void* data)
    {
        int fd = timerfd_create(CLOCK_MONOTONIC, 0);
        assert(fd >= 0);

        struct itimerspec ts;
        ms2ts(&ts.it_value, milliseconds);
        ts.it_interval = {0, 0};

        if (timerfd_settime(fd, 0, &ts, NULL) != 0)
        {
            assert(0);
        }

        struct timer_data* tdata = new timer_data;
        tdata->func = func;
        tdata->data = data;
        EventLoop::add_fd(fd, EVENT_READABLE, timer_event, tdata);

        return fd;
    }

    static void periodic_timer_event(int fd, uint32_t mask, void* data)
    {
        struct timer_data* tdata = reinterpret_cast<struct timer_data*>(data);

        tdata->func(fd, tdata->data);

        unsigned long long missed = 0;
        (void)read(fd, &missed, sizeof(missed));
        if (missed > 1)
        {
            DBG("missed timeouts: " << missed);
        }
    }

    int EventLoop::start_periodic_timer(unsigned long milliseconds, timer_event_callback func, void* data)
    {
        int fd = timerfd_create(CLOCK_MONOTONIC, 0);
        assert(fd >= 0);

        struct itimerspec ts;
        ms2ts(&ts.it_value, milliseconds);
        ms2ts(&ts.it_interval, milliseconds);

        if (timerfd_settime(fd, 0, &ts, NULL) != 0)
        {
            assert(0);
        }

        struct timer_data* tdata = new timer_data;
        tdata->func = func;
        tdata->data = data;
        EventLoop::add_fd(fd, EVENT_READABLE, periodic_timer_event, tdata);

        return fd;
    }

    void EventLoop::cancel_periodic_timer(int fd)
    {
        // TODO
        //delete tdata;

        EventLoop::rem_fd(fd);

        ::close(fd);
    }

    static std::vector<event_callback> idle;

    void EventLoop::add_idle_callback(event_callback func)
    {
        idle.push_back(func);
    }
}
