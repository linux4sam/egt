/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/math.h"
#include "egt/animation.h"
#include "egt/app.h"
#include "egt/detail/math.h"
#include "egt/widget.h"
#include <cassert>
#include <cmath>

namespace egt
{
inline namespace v1
{

Animation::Animation(float_t start, float_t end,
                     animation_callback_t callback,
                     std::chrono::milliseconds duration,
                     easing_func_t func)
    : m_start(start),
      m_end(end),
      m_easing(func),
      m_duration(duration)
{
    if (callback)
        add_callback(callback);
}

void Animation::start()
{
    m_start_time = std::chrono::steady_clock::now();
    m_stop_time = m_start_time + m_duration;
    m_running = true;
    m_current = m_start;
    if (m_round)
        m_current = std::round(m_current);
    for (auto& callback : m_callbacks)
        callback(m_current);
}

bool Animation::next()
{
    if (!running())
        return false;

    auto now = std::chrono::steady_clock::now();
    if (now >= m_stop_time)
    {
        m_running = false;
        m_current = m_end;
        if (m_round)
            m_current = std::round(m_current);
        for (auto& callback : m_callbacks)
            callback(m_current);
    }
    else
    {
        auto percent = std::chrono::duration<float_t, std::milli>(now - m_start_time).count() /
                       std::chrono::duration<float_t, std::milli>(m_stop_time - m_start_time).count();
        auto result = detail::interpolate(m_easing, percent, m_start, m_end, m_reverse);

        if (m_round)
            result = std::round(result);

        if (!detail::float_compare(result, m_current))
        {
            m_current = result;
            for (auto& callback : m_callbacks)
                callback(m_current);
        }
    }

    return m_running;
}

void Animation::set_easing_func(easing_func_t func)
{
    if (!running())
        m_easing = func;
}

void Animation::stop()
{
    m_running = false;
}

AutoAnimation::AutoAnimation(float_t start, float_t end,
                             std::chrono::milliseconds duration,
                             easing_func_t func,
                             animation_callback_t callback)
    : Animation(start, end, callback, duration, func),
      m_timer(std::chrono::milliseconds(30))
{
    m_timer.on_timeout([this]()
    {
        if (!next())
        {
            stop();
        }
    });
}

void AutoAnimation::start()
{
    Animation::start();
    m_timer.start();
}

void AutoAnimation::stop()
{
    m_timer.cancel();
    Animation::stop();
}

}
}
