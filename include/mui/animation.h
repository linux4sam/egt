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
    typedef float float_t;

    /**
     * Predefined easing functions.
     * @{
     */
    float_t easing_linear(float_t percent);
    float_t easing_easy(float_t percent);
    float_t easing_easy_slow(float_t percent);
    float_t easing_extend(float_t percent);
    float_t easing_drop(float_t percent);
    float_t easing_drop_slow(float_t percent);
    float_t easing_snap(float_t percent);
    float_t easing_bounce(float_t percent);
    float_t easing_bouncy(float_t percent);
    float_t easing_rubber(float_t percent);
    float_t easing_spring(float_t percent);
    float_t easing_boing(float_t percent);
    /** @} */

    typedef std::function<void (float_t value, void* data)> animation_callback;
    typedef std::function<float_t (float_t percent)> easing_func;

    /**
     * Animation class with configurable easing function.
     *
     * See <https://easings.net> for more information about other easing
     * functions.
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

        WidgetPositionAnimator(std::vector<Widget*> widgets,
                               int coordinate,
                               int start, int end,
                               uint64_t duration,
                               easing_func func = easing_linear);

        void start();

        void reset();

        static void timer_callback(int fd, void* data);

        static void callback(float_t value, void* data);

    protected:
        std::vector<Widget*> m_widgets;
        int m_fd;
        int m_coord;
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

#endif
