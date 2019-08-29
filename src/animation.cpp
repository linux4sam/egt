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

using namespace std;

namespace egt
{
inline namespace v1
{

namespace detail
{
float_t interpolate(easing_func_t easing, float_t percent, float_t start,
                    float_t end, bool reverse)
{
    if (percent < 0.0f)
        return start;
    else if (percent > 1.0f)
        return end;

    if (reverse)
        percent = 1.0f - easing(1.0f - percent);
    else
        percent = easing(percent);

    return start * (1.0f - percent) + end * percent;
}
}

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
    m_start_time = chrono::steady_clock::now();
    m_stop_time = m_start_time + m_duration;
    m_running = true;
    m_current = m_start;
    for (auto& callback : m_callbacks)
        callback(m_current);
}

bool Animation::next()
{
    if (!running())
        return false;

    auto now = chrono::steady_clock::now();
    if (now >= m_stop_time)
    {
        m_running = false;
        m_current = m_end;
        for (auto& callback : m_callbacks)
            callback(m_current);
    }
    else
    {
        float_t percent = chrono::duration<float_t, milli>(now - m_start_time).count() /
                          chrono::duration<float_t, milli>(m_stop_time - m_start_time).count();
        float_t result = detail::interpolate(m_easing, percent, m_start, m_end, m_reverse);

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
