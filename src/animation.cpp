/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "animation.h"
#include "widget.h"
#include "event_loop.h"
#include <cmath>
#include <iostream>
#include <cassert>
#include "app.h"

using namespace std;

namespace mui
{

    static float interpolate(easing_func easing, float percent, float start,
                             float end, bool reverse = false)
    {
        if (percent < 0.0)
            return start;
        else if (percent > 1.0)
            return end;

        if (reverse)
            percent = 1.0 - easing(1.0 - percent);
        else
            percent = easing(percent);
        return start * (1.0 - percent) + end * percent;
    }

    static const float NegCosPi = 1.61803398874989;

    float easing_linear(float percent)
    {
        return percent;
    }

    float easing_easy(float percent)
    {
        return percent * percent * (3.0 - 2.0 * percent);
    }

    float easing_easy_slow(float percent)
    {
        percent = easing_easy(percent);
        return percent * percent * (3.0 - 2.0 * percent);
    }

    float easing_extend(float percent)
    {
        percent = (percent * 1.4) - 0.2;
        return 0.5 - cosf(M_PI * percent) / NegCosPi;
    }

    static float power(float base, float exponent)
    {
        if (exponent == 0.0)
            return 1.0;
        else if (base == 0 && exponent > 0.0)
            return 0.0;

        return exp(exponent * logf(base));
    }

    float easing_drop(float percent)
    {
        return percent * percent;
    }

    float easing_drop_slow(float percent)
    {
        return percent * percent * percent * percent * percent;
    }

    float easing_snap(float percent)
    {
        percent = percent * percent;
        percent = (percent * 1.4) - 0.2;
        return 0.5 - cosf(M_PI * percent) / NegCosPi;
    }

    float easing_bounce(float percent)
    {
        if (percent > 0.9)
        {
            float result = percent - 0.95;
            return 1.0 + result * result * 20 - (0.05 * 0.05 * 20);
        }
        else if (percent > 0.75)
        {
            float result = percent - 0.825;
            return 1.0 + result * result * 16 - (0.075 * 0.075 * 16);
        }
        else if (percent > 0.5)
        {
            float result = percent - 0.625;
            return 1.0 + result * result * 12 - (0.125 * 0.125 * 12);
        }

        percent = percent * 2.0;
        return percent * percent;
    }

    float easing_bouncy(float percent)
    {
        float result = 1.0;
        float scale = 5.0;
        float start = 0.5;
        float step = 0.2;

        if (percent < start)
        {
            result = percent / start;
            return result * result;
        }

        while (step > 0.01)
        {
            if (percent < start + step)
            {
                step = step / 2.0;
                result = (percent - (start + step)) * scale;
                result =  result * result;
                result = result + 1.0 - power(step * scale, 2.0);
                break;
            }
            else
            {
                start = start + step;
                step = step * 0.6;
            }
        }

        return result;
    }

    float easing_rubber(float percent)
    {
        if (percent > 0.9)
        {
            float result = percent - 0.95;
            return 1.0 - result * result * 20. + (0.05 * 0.05 * 20.0);
        }
        else if (percent > 0.75)
        {
            float result = percent - 0.825;
            return 1.0 + result * result * 18.0 - (0.075 * 0.075 * 18.0);
        }
        else if (percent > 0.5)
        {
            float result = percent - 0.625;
            return 1.0 - result * result * 14.0 + (0.125 * 0.125 * 14.0);
        }

        percent = percent * 2;
        return percent * percent;
    }

    float easing_spring(float percent)
    {
        percent = percent * percent;
        float result = sinf(M_PI * percent * percent * 10.0 - M_PI / 2.0) / 4.0;
        result = result * (1.0 - percent) + 1.0;
        if (percent < 0.3)
            result = result * easing_easy(percent / 0.3);
        return result;
    }

    float easing_boing(float percent)
    {
        percent = power(percent, 1.5);
        float result = sinf(M_PI * power(percent, 2.0) * 20.0 - M_PI / 2.0) / 4.0;
        result = result * (1.0 - percent) + 1.0;
        if (percent < 0.2)
            result = result * easing_easy(percent / 0.2);
        return result;
    }

    Animation::Animation(float start, float end,
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

    inline static bool float_compare(float a, float b)
    {
        return std::fabs(a - b) < 1e-6;
    }

    bool Animation::next()
    {
        if (!running())
            return true;

        auto now = chrono::steady_clock::now();
        if (now > m_stop_time)
        {
            float result = m_end;

            if (!float_compare(result, m_current))
            {
                m_current = m_end;
                m_callback(m_current, m_data);
            }

            m_running = false;
        }
        else
        {
            float percent = chrono::duration<double, milli>(now - m_start_time).count() /
                            chrono::duration<double, milli>(m_stop_time - m_start_time).count();
            float result = interpolate(m_easing, percent, m_start, m_end, m_reverse);

            if (!float_compare(result, m_current))
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
    }

    void WidgetPositionAnimator::start()
    {
        m_fd = main_app().event().start_periodic_timer(33,
                WidgetPositionAnimator::timer_callback, this);
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

    void WidgetPositionAnimator::timer_callback(int fd, void* data)
    {
        WidgetPositionAnimator* a = reinterpret_cast<WidgetPositionAnimator*>(data);
        assert(a);
        if (!a->next())
        {
            main_app().event().cancel_periodic_timer(a->m_fd);
            a->m_fd = -1;
        }
    }

    void WidgetPositionAnimator::callback(float value, void* data)
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

    void AnimationTimer::animation_callback(float value, void* data)
    {
        AnimationTimer* a = reinterpret_cast<AnimationTimer*>(data);
        assert(a);

        a->step(value);
    }

}
