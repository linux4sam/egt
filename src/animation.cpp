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

Animation::Animation(EasingScalar start, EasingScalar end,
                     const AnimationCallback& callback,
                     std::chrono::milliseconds duration,
                     EasingFunc func)
    : m_start(start),
      m_end(end),
      m_easing(std::move(func)),
      m_duration(duration)
{
    add_callback(callback);
}

void Animation::start()
{
    m_elapsed = 0;
    m_intermediate_time = std::chrono::steady_clock::now();
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
    m_elapsed += std::chrono::duration<EasingScalar, std::milli>(now - m_intermediate_time).count();
    m_intermediate_time = now;

    auto percent = m_elapsed / m_duration.count();

    if (percent >= 1)
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
        auto result = detail::interpolate(m_easing, percent, m_start, m_end, m_reverse);

        if (m_round)
            result = std::round(result);

        if (!detail::float_equal(result, m_current))
        {
            m_current = result;
            for (auto& callback : m_callbacks)
                callback(m_current);
        }
    }

    return m_running;
}

void Animation::easing_func(EasingFunc func)
{
    if (!running())
        m_easing = std::move(func);
}

void Animation::stop()
{
    m_running = false;
}

AutoAnimation::AutoAnimation(EasingScalar start, EasingScalar end,
                             std::chrono::milliseconds duration,
                             const EasingFunc& func,
                             const AnimationCallback& callback)
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

AutoAnimation::AutoAnimation(std::chrono::milliseconds duration,
                             const EasingFunc& func,
                             const AnimationCallback& callback)
    : AutoAnimation({}, {}, duration, func, callback)
{}

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

void AutoAnimation::interval(std::chrono::milliseconds duration)
{
    m_timer.change_duration(duration);
}

}
}
