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

    float_t easing_linear(float_t p)
    {
        return p;
    }

    float_t easing_easy(float_t p)
    {
        return p * p * (3.0f - 2.0f * p);
    }

    float_t easing_easy_slow(float_t p)
    {
        p = easing_easy(p);
        return p * p * (3.0f - 2.0f * p);
    }

    float_t easing_extend(float_t p)
    {
        p = (p * 1.4f) - 0.2f;
        return 0.5f - std::cos(PI * p) / NEGCOSPI;
    }

    static inline float_t power(float_t base, float_t exponent)
    {
        if (exponent == 0.0f)
            return 1.0f;
        else if (base == 0.0f && exponent > 0.0f)
            return 0.0f;

        return std::exp(exponent * std::log(base));
    }

    float_t easing_drop(float_t p)
    {
        return p * p;
    }

    float_t easing_drop_slow(float_t p)
    {
        return p * p * p * p * p;
    }

    float_t easing_snap(float_t p)
    {
        p = p * p;
        p = (p * 1.4f) - 0.2f;
        return 0.5f - std::cos(PI * p) / NEGCOSPI;
    }

    float_t easing_bounce(float_t p)
    {
        if (p > 0.9f)
        {
            float_t result = p - 0.95f;
            return 1.0f + result * result * 20.0f - (0.05f * 0.05f * 20.0f);
        }
        else if (p > 0.75f)
        {
            float_t result = p - 0.825f;
            return 1.0f + result * result * 16.0f - (0.075f * 0.075f * 16.0f);
        }
        else if (p > 0.5f)
        {
            float_t result = p - 0.625f;
            return 1.0f + result * result * 12.0f - (0.125f * 0.125f * 12.0f);
        }

        p = p * 2.0f;
        return p * p;
    }

    float_t easing_bouncy(float_t p)
    {
        float_t result = 1.0f;
        float_t scale = 5.0f;
        float_t start = 0.5f;
        float_t step = 0.2f;

        if (p < start)
        {
            result = p / start;
            return result * result;
        }

        while (step > 0.01f)
        {
            if (p < start + step)
            {
                step = step / 2.0f;
                result = (p - (start + step)) * scale;
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

    float_t easing_rubber(float_t p)
    {
        if (p > 0.9f)
        {
            float_t result = p - 0.95f;
            return 1.0f - result * result * 20.0f + (0.05f * 0.05f * 20.0f);
        }
        else if (p > 0.75f)
        {
            float_t result = p - 0.825f;
            return 1.0f + result * result * 18.0f - (0.075f * 0.075f * 18.0f);
        }
        else if (p > 0.5f)
        {
            float_t result = p - 0.625f;
            return 1.0f - result * result * 14.0f + (0.125f * 0.125f * 14.0f);
        }

        p = p * 2.0f;
        return p * p;
    }

    float_t easing_spring(float_t p)
    {
        p = p * p;
        float_t result = std::sin(PI * p * p * 10.0f - PI / 2.0f) / 4.0f;
        result = result * (1.0f - p) + 1.0f;
        if (p < 0.3f)
            result = result * easing_easy(p / 0.3f);
        return result;
    }

    float_t easing_boing(float_t p)
    {
        p = power(p, 1.5f);
        float_t result = std::sin(PI * power(p, 2.0f) * 20.0f - PI / 2.0f) / 4.0f;
        result = result * (1.0f - p) + 1.0f;
        if (p < 0.2f)
            result = result * easing_easy(p / 0.2f);
        return result;
    }

    float_t easing_quadratic_easein(float_t p)
    {
        return p * p;
    }

    float_t easing_quadratic_easeout(float_t p)
    {
        return -(p * (p - 2));
    }

    float_t easing_quadratic_easeinout(float_t p)
    {
        if (p < 0.5)
        {
            return 2 * p * p;
        }
        else
        {
            return (-2 * p * p) + (4 * p) - 1;
        }
    }

    float_t easing_cubic_easein(float_t p)
    {
        return p * p * p;
    }

    float_t easing_cubic_easeout(float_t p)
    {
        float_t f = (p - 1);
        return f * f * f + 1;
    }

    float_t easing_cubic_easeinout(float_t p)
    {
        if (p < 0.5)
        {
            return 4 * p * p * p;
        }
        else
        {
            float_t f = ((2 * p) - 2);
            return 0.5 * f * f * f + 1;
        }
    }

    float_t easing_quartic_easein(float_t p)
    {
        return p * p * p * p;
    }

    float_t easing_quartic_easeout(float_t p)
    {
        float_t f = (p - 1);
        return f * f * f * (1 - p) + 1;
    }

    float_t easing_quartic_easeinout(float_t p)
    {
        if (p < 0.5)
        {
            return 8 * p * p * p * p;
        }
        else
        {
            float_t f = (p - 1);
            return -8 * f * f * f * f + 1;
        }
    }

    float_t easing_quintic_easein(float_t p)
    {
        return p * p * p * p * p;
    }

    float_t easing_quintic_easeout(float_t p)
    {
        float_t f = (p - 1);
        return f * f * f * f * f + 1;
    }

    float_t easing_quintic_easeinout(float_t p)
    {
        if (p < 0.5)
        {
            return 16 * p * p * p * p * p;
        }
        else
        {
            float_t f = ((2 * p) - 2);
            return  0.5 * f * f * f * f * f + 1;
        }
    }

    float_t easing_sine_easein(float_t p)
    {
        return std::sin((p - 1) * M_PI_2) + 1;
    }

    float_t easing_sine_easeout(float_t p)
    {
        return std::sin(p * M_PI_2);
    }

    float_t easing_sine_easeinout(float_t p)
    {
        return 0.5 * (1 - std::cos(p * M_PI));
    }

    float_t easing_circular_easein(float_t p)
    {
        return 1 - std::sqrt(1 - (p * p));
    }

    float_t easing_circular_easeout(float_t p)
    {
        return std::sqrt((2 - p) * p);
    }

    float_t easing_circular_easeinout(float_t p)
    {
        if (p < 0.5)
        {
            return 0.5 * (1 - std::sqrt(1 - 4 * (p * p)));
        }
        else
        {
            return 0.5 * (std::sqrt(-((2 * p) - 3) * ((2 * p) - 1)) + 1);
        }
    }

    float_t easing_exponential_easein(float_t p)
    {
        return (p == 0.0) ? p : std::pow(2, 10 * (p - 1));
    }

    float_t easing_exponential_easeout(float_t p)
    {
        return (p == 1.0) ? p : 1 - std::pow(2, -10 * p);
    }

    float_t easing_exponential_easeinout(float_t p)
    {
        if (p == 0.0 || p == 1.0)
            return p;

        if (p < 0.5)
        {
            return 0.5 * std::pow(2, (20 * p) - 10);
        }
        else
        {
            return -0.5 * std::pow(2, (-20 * p) + 10) + 1;
        }
    }

    Animation::Animation(float_t start, float_t end,
                         animation_callback callback,
                         std::chrono::milliseconds duration,
                         easing_func func)
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
    }

    inline static bool float_t_compare(float_t a, float_t b)
    {
        return std::fabs(a - b) < 1e-6f;
    }

    bool Animation::next()
    {
        if (!running())
            return false;

        auto now = chrono::steady_clock::now();
        if (now > m_stop_time)
        {
            m_running = false;
            float_t result = m_end;

            if (!float_t_compare(result, m_current))
            {
                m_current = m_end;
                for (auto& callback : m_callbacks)
                    callback(m_current);
            }
        }
        else
        {
            float_t percent = chrono::duration<float_t, milli>(now - m_start_time).count() /
                              chrono::duration<float_t, milli>(m_stop_time - m_start_time).count();
            float_t result = interpolate(m_easing, percent, m_start, m_end, m_reverse);

            if (!float_t_compare(result, m_current))
            {
                m_current = result;
                for (auto& callback : m_callbacks)
                    callback(m_current);
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

    namespace experimental
    {

        AutoAnimation::AutoAnimation(float_t start, float_t end,
                                     std::chrono::milliseconds duration,
                                     easing_func func,
                                     animation_callback callback)
            : Animation(start, end, callback, duration, func),
              m_timer(std::chrono::milliseconds(30))
        {
            m_timer.on_timeout([this]()
            {
                if (!next())
                {
                    m_timer.cancel();
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
