/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_ANIMATION_H
#define MUI_ANIMATION_H

/**
 * @file
 * @brief Working with animations.
 */

#include <mui/timer.h>
#include <cstdint>
#include <chrono>
#include <vector>
#include <functional>

namespace mui
{
    using float_t = float;

    /**
     * Predefined easing functions.
     *
     * All easing functions take a percent, and return a percent. You can easily
     * create your own easing functions.  For example, the simplest of easing
     * functions is a linear function that is implemented like so:
     *
     * @begincode
     * float_t easing_linear(float_t p)
     * {
     *     return p;
     * }
     * @endcode
     *
     * See <https://easings.net> for more information about other easing
     * functions.
     * @{
     */
    float_t easing_linear(float_t p);
    float_t easing_easy(float_t p);
    float_t easing_easy_slow(float_t p);
    float_t easing_extend(float_t p);
    float_t easing_drop(float_t p);
    float_t easing_drop_slow(float_t p);
    float_t easing_snap(float_t p);
    float_t easing_bounce(float_t p);
    float_t easing_bouncy(float_t p);
    float_t easing_rubber(float_t p);
    float_t easing_spring(float_t p);
    float_t easing_boing(float_t p);

    float_t easing_quadratic_easein(float_t p);
    float_t easing_quadratic_easeout(float_t p);
    float_t easing_quadratic_easeinout(float_t p);

    float_t easing_cubic_easein(float_t p);
    float_t easing_cubic_easeout(float_t p);
    float_t easing_cubic_easeinout(float_t p);

    float_t easing_quartic_easein(float_t p);
    float_t easing_quartic_easeout(float_t p);
    float_t easing_quartic_easeinout(float_t p);

    float_t easing_quintic_easein(float_t p);
    float_t easing_quintic_easeout(float_t p);
    float_t easing_quintic_easeinout(float_t p);

    float_t easing_sine_easein(float_t p);
    float_t easing_sine_easeout(float_t p);
    float_t easing_sine_easeinout(float_t p);

    float_t easing_circular_easein(float_t p);
    float_t easing_circular_easeout(float_t p);
    float_t easing_circular_easeinout(float_t p);

    float_t easing_exponential_easein(float_t p);
    float_t easing_exponential_easeout(float_t p);
    float_t easing_exponential_easeinout(float_t p);
    /** @} */

    using animation_callback = std::function<void (float_t value, void* data)>;
    using easing_func = std::function<float_t (float_t percent)>;

    /**
     * Animation class with configurable easing function.
     *
     * An animation is a containr that basically runs from a start value to an
     * end value over a duration of time. For example, the first value of the
     * animation will be the start value at duration 0 and the last value of the
     * animation will be the end value at the duration total.
     *
     * What happens between the start of the duration and the end of the
     * duration, relative to time, is dictated by what's called an easing
     * function. In other words, the easing function controls the skew of the
     * animation value relative to time.
     */
    class Animation
    {
    public:

        /**
         *
         */
        Animation(float_t start, float_t end, animation_callback callback,
                  uint64_t duration, easing_func func = easing_linear,
                  void* data = 0);

        float_t starting() const { return m_start; }
        void starting(float_t start) { m_start = start; }
        float_t ending() const { return m_end; }
        void ending(float_t end) { m_end = end; }
        void duration(float_t dur) { m_duration = dur; }

        virtual void start();
        virtual bool next();
        virtual void stop();

        bool running() const { return m_running; }
        void set_easing_func(easing_func func);
        void reverse(bool rev) { m_reverse = rev; }

        virtual ~Animation() {}

    protected:

        float_t m_start;
        float_t m_end;
        animation_callback m_callback;
        easing_func m_easing;
        float_t m_current;
        uint64_t m_duration;
        std::chrono::time_point<std::chrono::steady_clock> m_start_time;
        std::chrono::time_point<std::chrono::steady_clock> m_stop_time;
        bool m_running;
        bool m_reverse;
        void* m_data;
    };

    class Widget;

    namespace experimental
    {
        class WidgetPositionAnimator : public Animation
        {
        public:
            enum
            {
                CORD_X,
                CORD_Y
            };

            WidgetPositionAnimator(Widget* widget,
                                   int coordinate,
                                   int start, int end,
                                   uint64_t duration,
                                   easing_func func = easing_linear);

            WidgetPositionAnimator(const std::vector<Widget*>& widgets,
                                   int coordinate,
                                   int start, int end,
                                   uint64_t duration,
                                   easing_func func = easing_linear);

            void start();

            void reset();

            void timer_callback();

            static void callback(float_t value, void* data);

        protected:
            std::vector<Widget*> m_widgets;
            int m_coord;
            PeriodicTimer m_timer;
        };

        class AnimationTimer : public PeriodicTimer
        {
        public:
            AnimationTimer(int start, int end, uint64_t duration, easing_func func);

            void start();

            virtual void step(int value) = 0;

        protected:

            void timeout();

            static void animation_callback(float_t value, void* data);

            Animation m_animation;
        };

    }
}

#endif
