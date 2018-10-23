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

    using animation_callback = std::function<void (float_t value)>;
    using easing_func = std::function<float_t (float_t percent)>;

    namespace detail
    {
        /**
         * Base class for an animation.
         */
        class IAnimation
        {
        public:

            virtual void start() = 0;

            /**
             * Periodic call to the animation.
             *
             * This must be called until it returns false. When it returns false,
             * the animation is done.
             */
            virtual bool next() = 0;

            /**
             * Stop the animation.
             */
            virtual void stop() = 0;

            /**
             * Returns true if the animation is currently running.
             */
            virtual bool running() const { return m_running; }

            void add_callback(animation_callback callback = nullptr)
            {
                m_callbacks.push_back(callback);
            }

            virtual ~IAnimation()
            {}

        protected:
            /**
             * The running state of the animation.
             */
            bool m_running{false};

            std::vector<animation_callback> m_callbacks;
        };
    }

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
    class Animation : public detail::IAnimation
    {
    public:

        /**
         * @param[in] callback Called whenever the animation value changes. May be nullptr.
         */
        Animation(float_t start, float_t end, animation_callback callback,
                  std::chrono::milliseconds duration, easing_func func = easing_linear);

        float_t starting() const { return m_start; }
        void starting(float_t start) { m_start = start; }
        float_t ending() const { return m_end; }
        void ending(float_t end) { m_end = end; }
        void duration(std::chrono::milliseconds dur) { m_duration = dur; }

        /**
         * Start the animation.
         */
        virtual void start() override;

        /**
         * Periodic call to the animation.
         *
         * This must be called until it returns false. When it returns false,
         * the animation is done.
         */
        virtual bool next() override;

        /**
         * Stop the animation.
         */
        virtual void stop() override;

        // don't know if this is a good thing to have while running
        void set_easing_func(easing_func func);
        void reverse(bool rev) { m_reverse = rev; }

        virtual ~Animation() {}

    protected:

        float_t m_start{0};
        float_t m_end{0};
        easing_func m_easing{easing_linear};
        float_t m_current{0};
        std::chrono::milliseconds m_duration;
        std::chrono::time_point<std::chrono::steady_clock> m_start_time;
        std::chrono::time_point<std::chrono::steady_clock> m_stop_time;
        bool m_reverse{false};
    };

    class Widget;

    namespace experimental
    {
        /**
         * Series of animations as a single animation.
         *
         * It's often useful to put together a series of animations into a
         * sequence. This class manages a series of animations objects and runs
         * them as one big animation. You can even add animation sequences to
         * other animation sequences to build up complex sequences.
         *
         * You can skip the current sub-animation, you can restart to the
         * beginning, of the sequence, or you can stop the entire thing.
         */
        class AnimationSequence : public detail::IAnimation
        {
        public:

            AnimationSequence()
            {}

            /**
             * Add a sub animation to the sequence.
             */
            void add(detail::IAnimation* animation)
            {
                assert(animation);
                if (!animation)
                    return;

                auto i = find(m_animations.begin(), m_animations.end(), animation);
                if (i == m_animations.end())
                {
                    animation->add_callback([this, animation](float_t value)
                    {
                        ignoreparam(value);

                        if (!animation->running())
                            next();
                    });
                    m_animations.push_back(animation);
                }
            }

            /**
             * Remove a sub animation from the sequence.
             */
            void remove(detail::IAnimation* animation)
            {
                if (!animation)
                    return;

                auto i = find(m_animations.begin(), m_animations.end(), animation);
                if (i != m_animations.end())
                {
                    m_animations.erase(i);
                }
            }

            /**
             * Skip to the next sub animation in the sequence.
             */
            bool skip()
            {
                if (m_current >= m_animations.size())
                    return false;

                m_animations[m_current]->stop();

                if (++m_current >= m_animations.size())
                    return false;

                m_animations[m_current]->start();

                return true;
            }

            virtual void reset()
            {
                // TODO
            }

            virtual void start() override
            {
                // TODO: this does not handle an already running sequence

                if (m_current >= m_animations.size())
                    return;

                m_animations[m_current]->start();

                m_running = true;
            }

            virtual bool next() override
            {
                DBG("seq next");

                if (!running())
                    return true;

                if (++m_current >= m_animations.size())
                {
                    DBG("seq finished");
                    m_running = false;
                    return m_running;
                }

                m_animations[m_current]->start();

                return m_running;
            }

            virtual void stop() override
            {
                if (m_current >= m_animations.size())
                    return;

                m_animations[m_current]->stop();

                m_running = false;
            }

            virtual ~AnimationSequence()
            {}

        protected:
            using animation_array = std::vector<detail::IAnimation*>;

            animation_array m_animations;
            size_t m_current{0};
        };

        /**
         * Animation object with built in timer.
         *
         * An Animation usually involves setting up a timer to run the animation
         * at a periodic interval. This wraps an animation around a built in
         * timer to run the animation.
         */
        class AutoAnimation : public Animation
        {
        public:
            AutoAnimation(float_t start, float_t end,
                          std::chrono::milliseconds duration,
                          easing_func func = easing_linear,
                          animation_callback callback = nullptr);

            virtual void start() override;
            virtual void stop() override;

            virtual ~AutoAnimation()
            {}

        protected:

            PeriodicTimer m_timer;
        };

        class PropertyAnimator : public AutoAnimation
        {
        public:

            using property_callback = std::function<void (int v)>;

            PropertyAnimator(float start, float end,
                             std::chrono::milliseconds duration,
                             easing_func func = easing_linear)
                : AutoAnimation(start, end, duration, func,
                                std::bind(&PropertyAnimator::do_callback, this, std::placeholders::_1))
            {
            }

            void set_prop_callback(property_callback callback)
            {
                m_prop_callback = callback;
            }

        protected:

            void do_callback(float value)
            {
                if (m_prop_callback)
                    m_prop_callback(value);
            }

            property_callback m_prop_callback{nullptr};
        };

    }
}

#endif
