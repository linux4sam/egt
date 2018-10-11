/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "mui/animation.h"
#include "mui/app.h"
#include "mui/widget.h"
#include <cassert>
#include <cmath>

using namespace std;

namespace mui
{
    static inline float_t interpolate(easing_func easing, float_t percent, float_t start,
                                      float_t end, bool reverse = false)
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

    constexpr auto PI = float_t(3.14159265358979323846);
    constexpr auto NEGCOSPI = float_t(1.61803398874989);

    float_t easing_linear(float_t percent)
    {
        return percent;
    }

    float_t easing_easy(float_t percent)
    {
        return percent * percent * (3.0f - 2.0f * percent);
    }

    float_t easing_easy_slow(float_t percent)
    {
        percent = easing_easy(percent);
        return percent * percent * (3.0f - 2.0f * percent);
    }

    float_t easing_extend(float_t percent)
    {
        percent = (percent * 1.4f) - 0.2f;
        return 0.5f - std::cos(PI * percent) / NEGCOSPI;
    }

    static inline float_t power(float_t base, float_t exponent)
    {
        if (exponent == 0.0f)
            return 1.0f;
        else if (base == 0.0f && exponent > 0.0f)
            return 0.0f;

        return std::exp(exponent * std::log(base));
    }

    float_t easing_drop(float_t percent)
    {
        return percent * percent;
    }

    float_t easing_drop_slow(float_t percent)
    {
        return percent * percent * percent * percent * percent;
    }

    float_t easing_snap(float_t percent)
    {
        percent = percent * percent;
        percent = (percent * 1.4f) - 0.2f;
        return 0.5f - std::cos(PI * percent) / NEGCOSPI;
    }

    float_t easing_bounce(float_t percent)
    {
        if (percent > 0.9f)
        {
            float_t result = percent - 0.95f;
            return 1.0f + result * result * 20.0f - (0.05f * 0.05f * 20.0f);
        }
        else if (percent > 0.75f)
        {
            float_t result = percent - 0.825f;
            return 1.0f + result * result * 16.0f - (0.075f * 0.075f * 16.0f);
        }
        else if (percent > 0.5f)
        {
            float_t result = percent - 0.625f;
            return 1.0f + result * result * 12.0f - (0.125f * 0.125f * 12.0f);
        }

        percent = percent * 2.0f;
        return percent * percent;
    }

    float_t easing_bouncy(float_t percent)
    {
        float_t result = 1.0f;
        float_t scale = 5.0f;
        float_t start = 0.5f;
        float_t step = 0.2f;

        if (percent < start)
        {
            result = percent / start;
            return result * result;
        }

        while (step > 0.01f)
        {
            if (percent < start + step)
            {
                step = step / 2.0f;
                result = (percent - (start + step)) * scale;
                result =  result * result;
                result = result + 1.0f - power(step * scale, 2.0f);
                break;
            }
            else
            {
                start = start + step;
                step = step * 0.6f;
            }
        }

        return result;
    }

    float_t easing_rubber(float_t percent)
    {
        if (percent > 0.9f)
        {
            float_t result = percent - 0.95f;
            return 1.0f - result * result * 20.0f + (0.05f * 0.05f * 20.0f);
        }
        else if (percent > 0.75f)
        {
            float_t result = percent - 0.825f;
            return 1.0f + result * result * 18.0f - (0.075f * 0.075f * 18.0f);
        }
        else if (percent > 0.5f)
        {
            float_t result = percent - 0.625f;
            return 1.0f - result * result * 14.0f + (0.125f * 0.125f * 14.0f);
        }

        percent = percent * 2.0f;
        return percent * percent;
    }

    float_t easing_spring(float_t percent)
    {
        percent = percent * percent;
        float_t result = std::sin(PI * percent * percent * 10.0f - PI / 2.0f) / 4.0f;
        result = result * (1.0f - percent) + 1.0f;
        if (percent < 0.3f)
            result = result * easing_easy(percent / 0.3f);
        return result;
    }

    float_t easing_boing(float_t percent)
    {
        percent = power(percent, 1.5f);
        float_t result = std::sin(PI * power(percent, 2.0f) * 20.0f - PI / 2.0f) / 4.0f;
        result = result * (1.0f - percent) + 1.0f;
        if (percent < 0.2f)
            result = result * easing_easy(percent / 0.2f);
        return result;
    }

    Animation::Animation(float_t start, float_t end,
                         animation_callback callback,
                         uint64_t duration,
                         easing_func func,
                         void* data)
        : m_start(start),
          m_end(end),
          m_callback(callback),
          m_easing(func),
          m_current(0),
          m_duration(duration),
          m_running(false),
          m_reverse(false),
          m_data(data)
    {
    }

    void Animation::start()
    {
        m_start_time = chrono::steady_clock::now();
        m_stop_time = m_start_time + std::chrono::milliseconds(m_duration);
        m_running = true;
    }

    inline static bool float_t_compare(float_t a, float_t b)
    {
        return std::fabs(a - b) < 1e-6f;
    }

    bool Animation::next()
    {
        if (!running())
            return true;

        auto now = chrono::steady_clock::now();
        if (now > m_stop_time)
        {
            float_t result = m_end;

            if (!float_t_compare(result, m_current))
            {
                m_current = m_end;
                m_callback(m_current, m_data);
            }

            m_running = false;
        }
        else
        {
            float_t percent = chrono::duration<float_t, milli>(now - m_start_time).count() /
                chrono::duration<float_t, milli>(m_stop_time - m_start_time).count();
            float_t result = interpolate(m_easing, percent, m_start, m_end, m_reverse);

            if (!float_t_compare(result, m_current))
            {
                m_current = result;
                m_callback(m_current, m_data);
            }
        }

        return m_running;
    }

    void Animation::set_easing_func(easing_func func)
    {
        if (!running())
            m_easing = func;
    }

    void Animation::stop()
    {
        m_running = false;
    }

    WidgetPositionAnimator::WidgetPositionAnimator(Widget* widget,
                                                   int coordinate,
                                                   int start, int end,
                                                   uint64_t duration,
                                                   easing_func func)
        : Animation(start, end, WidgetPositionAnimator::callback,
                    duration, func, this),
          m_coord(coordinate)
    {
        m_widgets.push_back(widget);
    }

    WidgetPositionAnimator::WidgetPositionAnimator(std::vector<Widget*> widgets,
                                                   int coordinate,
                                                   int start, int end,
                                                   uint64_t duration,
                                                   easing_func func)
        : Animation(start, end, WidgetPositionAnimator::callback,
                    duration, func, this),
          m_widgets(widgets),
          m_coord(coordinate)
    {
        m_timer.add_handler(std::bind(&WidgetPositionAnimator::timer_callback, this));
    }

    void WidgetPositionAnimator::start()
    {
        m_timer.start_with_duration(30);
        Animation::start();
    }

    void WidgetPositionAnimator::reset()
    {
        for (auto& i : m_widgets)
        {
            if (m_coord == CORD_X)
                i->move(Point(m_start, i->y()));
            else
                i->move(Point(i->x(), m_start));
        }
    }

    void WidgetPositionAnimator::timer_callback()
    {
        if (!next())
        {
            m_timer.cancel();
        }
    }

    void WidgetPositionAnimator::callback(float_t value, void* data)
    {
        WidgetPositionAnimator* a = reinterpret_cast<WidgetPositionAnimator*>(data);
        assert(a);

        for (auto& i : a->m_widgets)
        {
            if (a->m_coord == CORD_X)
                i->move(Point(value, i->y()));
            else
                i->move(Point(i->x(), value));
        }
    }

    AnimationTimer::AnimationTimer(int start, int end, uint64_t duration, easing_func func)
        : PeriodicTimer(30),
          m_animation(start, end, AnimationTimer::animation_callback,
                      duration, func, this)
    {}

    void AnimationTimer::start()
    {
        PeriodicTimer::start();
        m_animation.start();
    }

    void AnimationTimer::timeout()
    {
        if (!m_animation.next())
        {
            PeriodicTimer::cancel();
        }
    }

    void AnimationTimer::animation_callback(float_t value, void* data)
    {
        AnimationTimer* a = reinterpret_cast<AnimationTimer*>(data);
        assert(a);

        a->step(value);
    }

}
