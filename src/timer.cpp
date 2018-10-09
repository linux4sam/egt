/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "timer.h"
#include "event_loop.h"
#include <cassert>
#include <unistd.h>
#include "app.h"

namespace mui
{

    Timer::Timer(uint64_t duration)
        : m_fd(-1),
          m_duration(duration)
    {
    }

    void Timer::start()
    {
        //if (m_fd >= 0)
        //  cancel();

        m_fd = main_app().event().start_timer(m_duration, Timer::timer_callback, this);
    }

    void Timer::start(uint64_t duration)
    {
        m_duration = duration;
        start();
    }

    void Timer::cancel()
    {
        if (m_fd >= 0)
        {
            close(m_fd);
            m_fd = -1;
        }
    }

    void Timer::timer_callback(int fd, void* data)
    {
        Timer* timer = reinterpret_cast<Timer*>(data);
        assert(timer);

        timer->timeout();
    }

    void Timer::timeout()
    {
        for (auto& callback : m_callbacks)
            callback();
    }

    Timer::~Timer()
    {
        cancel();
    }

    PeriodicTimer::PeriodicTimer(uint64_t duration)
        : Timer(duration)
    {
    }

    void PeriodicTimer::start()
    {
        m_fd = main_app().event().start_periodic_timer(m_duration, PeriodicTimer::timer_callback, this);
    }

    void PeriodicTimer::cancel()
    {
        if (m_fd >= 0)
        {
            main_app().event().cancel_periodic_timer(m_fd);
            close(m_fd);
            m_fd = -1;
        }
    }

    PeriodicTimer::~PeriodicTimer()
    {
        cancel();
    }

}
