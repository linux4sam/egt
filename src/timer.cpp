/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/app.h"
#include "egt/eventloop.h"
#include "egt/timer.h"
#include <vector>
#include <iostream>

using namespace std;

namespace egt
{
inline namespace v1
{

static vector<Timer*> timers;

void dump_timers(ostream& out)
{
    for (auto& timer : timers)
    {
        out << "Timer (" << timer->name() << ") duration(" <<
            timer->duration().count() << " ms) " <<
            (timer->running() ? "running" : "idle") << endl;
    }
}

Timer::Timer() noexcept
    : m_timer(Application::instance().event().io())
{
    timers.push_back(this);
}

Timer::Timer(std::chrono::milliseconds duration) noexcept
    : m_timer(Application::instance().event().io()),
      m_duration(duration)
{
    timers.push_back(this);
}

void Timer::start()
{
    // error::operation_aborted occurs when expires_from_now() is called on the
    // timer while it is pending, we handle this ourselves with m_running
    m_running = false;
    m_timer.expires_after(m_duration);
    m_running = true;
#ifdef USE_PRIORITY_QUEUE
    m_timer.async_wait(
        Application::instance().event().queue().wrap(detail::priorities::high,
                [this](const asio::error_code & error)
    {
        internal_timer_callback(error);
    }));
#else
    m_timer.async_wait([this](const asio::error_code & error)
    {
        internal_timer_callback(error);
    });
#endif
}

void Timer::start_with_duration(std::chrono::milliseconds duration)
{
    m_duration = duration;
    start();
}

void Timer::change_duration(std::chrono::milliseconds duration)
{
    if (m_running)
    {
        cancel();
        m_duration = duration;
        start();
    }
    else
    {
        m_duration = duration;
    }
}

void Timer::cancel()
{
    do_cancel();
}

void Timer::do_cancel()
{
    m_running = false;
    m_timer.cancel();
}

void Timer::internal_timer_callback(const asio::error_code& error)
{
    if (error)
        return;

    // it is possible to call cancel() and have this handler still called
    // which creates a sort of race condition, so we stop an actual
    // callback from continuing if m_running is false
    if (m_running)
    {
        m_running = false;
        timeout();
    }
}

void Timer::timeout()
{
    invoke_handlers();
}

uint32_t Timer::on_timeout(timer_callback_t callback)
{
    if (callback)
    {
        // TODO: m_handle_counter can wrap, making the handle non-unique
        auto handle = ++m_handle_counter;
        m_callbacks.emplace_back(std::move(callback), handle);
        return handle;
    }

    return 0;
}

void Timer::clear_handlers()
{
    m_callbacks.clear();
}

void Timer::remove_handler(uint32_t handle)
{
    auto i = std::find_if(m_callbacks.begin(), m_callbacks.end(),
                          [handle](const CallbackMeta & meta)
    {
        return meta.handle == handle;
    });

    if (i != m_callbacks.end())
        m_callbacks.erase(i);
}

void Timer::invoke_handlers()
{
    for (auto& callback : m_callbacks)
        callback.callback();
}

Timer::~Timer() noexcept
{
    do_cancel();

    auto i = std::find(timers.begin(), timers.end(), this);
    if (i != timers.end())
        timers.erase(i);
}

PeriodicTimer::PeriodicTimer(std::chrono::milliseconds interval) noexcept
    : Timer(interval)
{}

void PeriodicTimer::start()
{
    // error::operation_aborted occurs when expires_from_now() is called on the
    // timer while it is pending, we handle this ourselves with m_running
    m_running = false;
    m_timer.expires_after(m_duration);
    m_running = true;

#ifdef USE_PRIORITY_QUEUE
    m_timer.async_wait(Application::instance().event().queue().wrap(detail::priorities::high,
                       [this](const asio::error_code & error)
    {
        internal_timer_callback(error);
    }))
#else
    m_timer.async_wait([this](const asio::error_code & error)
    {
        internal_timer_callback(error);
    });
#endif
}

void PeriodicTimer::internal_timer_callback(const asio::error_code& error)
{
    if (error)
        return;

    // it is possible to call cancel() and have this handler still called
    // which creates a sort of race condition, so we stop an actual
    // callback from continuing if m_running is false
    if (m_running)
    {
        start();
        timeout();
    }
}

}
}
