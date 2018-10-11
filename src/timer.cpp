/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "mui/app.h"
#include "mui/event_loop.h"
#include "mui/timer.h"
#include <cassert>
#include <unistd.h>

using namespace std;

namespace mui
{

    Timer::Timer(uint64_t duration) noexcept
        : m_timer(main_app().event().io()),
          m_duration(duration)
    {}

    void Timer::start()
    {
        m_timer.cancel();
        m_timer.expires_from_now(asio::chrono::milliseconds(m_duration));
        m_timer.async_wait(std::bind(&Timer::timer_callback, this, std::placeholders::_1));
        m_running = true;
    }

    void Timer::start_with_duration(uint64_t duration)
    {
        m_duration = duration;
        start();
    }

    void Timer::cancel()
    {
        m_timer.cancel();
    }

    void Timer::timer_callback(const asio::error_code& error)
    {
        m_running = false;

        if (error)
            return;

        timeout();
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

    PeriodicTimer::PeriodicTimer(uint64_t duration) noexcept
        : Timer(duration)
    {
    }

    void PeriodicTimer::start()
    {
        m_timer.cancel();
        m_timer.expires_from_now(asio::chrono::milliseconds(m_duration));
        m_timer.async_wait(std::bind(&PeriodicTimer::timer_callback, this, std::placeholders::_1));
        m_running = true;
    }

    void PeriodicTimer::timer_callback(const asio::error_code& error)
    {
        m_running = false;

        if (error)
            return;

        start();
        timeout();
    }

}
