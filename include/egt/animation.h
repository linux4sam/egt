/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_ANIMATION_H
#define EGT_ANIMATION_H

/**
 * @file
 * @brief Working with animations.
 */

#include <chrono>
#include <egt/easing.h>
#include <egt/timer.h>
#include <egt/utils.h>
#include <functional>
#include <memory>
#include <vector>

namespace egt
{
inline namespace v1
{
/**
 * Easing function value type.
 */
using float_t = float;

/**
 * @defgroup animation Animation
 * Animation related classes.
 */

/**
 * Animation callback type.
 */
using animation_callback_t = std::function<void (float_t value)>;

/**
 * Easing function type.
 */
using easing_func_t = std::function<float_t (float_t percent)>;

namespace detail
{

/**
 * Interpolate function used internally.
 *
 * @param[in] easing The easing function to use.
 * @param[in] percent The current percent of the range.
 * @param[in] start Starting value.
 * @param[in] end Ending value.
 * @param[in] reverse Should we reverse start and end.
 */
template<class Functor, class T>
T interpolate(Functor&& easing, T percent, T start, T end, bool reverse = false)
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

/**
 * Base class for an animation.
 */
class IAnimation : public detail::noncopyable
{
public:

    /**
     * Start the animation.
     */
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

    /**
     * Register a callback for the animation.
     *
     * More than one callback can be registered.
     *
     * The callback will be called for each step of the animation, usually only
     * when the animation value has actually changed.
     *
     * @todo Need to implement removing callbacks, similar to Object class.
     */
    inline void add_callback(animation_callback_t callback = nullptr)
    {
        if (callback)
            m_callbacks.emplace_back(std::move(callback));
    }

    /**
     * Clear all callbacks.
     */
    inline void clear_callbacks()
    {
        m_callbacks.clear();
    }

    virtual ~IAnimation() = default;

protected:

    /**
     * The running state of the animation.
     */
    bool m_running{false};

    /**
     * Registered callbacks for the animation.
     */
    std::vector<animation_callback_t> m_callbacks;
};
}

/**
 * Animation class with configurable easing function.
 *
 * An Animation is a container that basically runs from a start value to an
 * end value over a duration of time. For example, the first value of the
 * animation will be the start value at duration 0 and the last value of the
 * animation will be the end value at the duration total.
 *
 * What happens between the start of the duration and the end of the
 * duration, relative to time, is dictated by what's called an easing
 * function. In other words, the easing function controls the skew of the
 * animation value relative to time.
 *
 * Usually, this class will not be used directly and instead one of the
 * helper classes derived from this are easier to use, like @see
 * PropertyAnimator.
 *
 * @ingroup animation
 */
class Animation : public detail::IAnimation
{
public:

    /**
     * @param[in] start The starting value of the animation.
     * @param[in] end The ending value of the animation.
     * @param[in] callback Called whenever the animation value changes. May be nullptr.
     * @param[in] duration The duration of the animation.
     * @param[in] func The easing function to use.
     */
    Animation(float_t start,
              float_t end,
              animation_callback_t callback,
              std::chrono::milliseconds duration,
              easing_func_t func = easing_linear);

    /**
     * Get the starting value.
     */
    inline float_t starting() const { return m_start; }

    /**
     * @note Calling this while running is undefined behavior.
     */
    inline void set_starting(float_t start) { m_start = start; }

    /**
     * Get the ending value.
     */
    inline float_t ending() const { return m_end; }

    /**
     * @note Calling this while running is undefined behavior.
     */
    inline void set_ending(float_t end) { m_end = end; }

    /**
     * @note Calling this while running is undefined behavior.
     */
    inline void set_duration(std::chrono::milliseconds dur) { m_duration = dur; }

    /**
     * @note Calling this while running is undefined behavior.
     */
    void set_easing_func(easing_func_t func);

    /**
     * @note Calling this while running is undefined behavior.
     */
    inline void set_reverse(bool rev) { m_reverse = rev; }

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

    /**
     * Get the current value.
     */
    inline float_t current() const { return m_current; }

    /**
     * Should the value be rounded?
     *
     * When set to true, the resulting @b value will be rounded to the nearest
     * integer.
     *
     * @param[in] enable When true, rounding is enabled. It is false by default.
     */
    inline void set_rounding(bool enable) { m_round = enable; }

    virtual ~Animation() = default;

protected:

    /**
     * Starting value.
     */
    float_t m_start{0};

    /**
     * Ending value.
     */
    float_t m_end{0};

    /**
     * Easing function.
     */
    easing_func_t m_easing{easing_linear};

    /**
     * Current value.
     */
    float_t m_current{0};

    /**
     * Duration of the animation.
     */
    std::chrono::milliseconds m_duration;

    /**
     * Absolute time when the animation was started.
     */
    std::chrono::time_point<std::chrono::steady_clock> m_start_time;

    /**
     * Absolute time when the animation will end.
     */
    std::chrono::time_point<std::chrono::steady_clock> m_stop_time;

    /**
     * Is the animation running in reverse.
     */
    bool m_reverse{false};

    /**
     * Should the dispatched value be rounded?
     */
    bool m_round{false};
};

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
 *
 * @ingroup animation
 */
class AnimationSequence : public detail::IAnimation
{
public:

    /**
     * @param loop Should the animation sequence loop.
     */
    explicit AnimationSequence(bool loop = false) noexcept
        : m_loop(loop)
    {}

    /**
     * Add an animation to the animation sequence.
     *
     * @param animation The animation.
     */
    virtual void add(detail::IAnimation& animation)
    {
        // Nasty, but it gets the job done.  If a widget is passed in as a
        // reference, we don't own it, so create a "pointless" shared_ptr that
        // will not delete it.
        add(std::shared_ptr<detail::IAnimation>(&animation, [](detail::IAnimation*) {}));
    }

    /**
     * Add an animation to the animation sequence.
     *
     * This will take a reference to the shared_ptr and manage the lifetime of
     * the animation.
     *
     * @param animation The animation.
     */
    template<class T>
    void add(std::shared_ptr<T> animation)
    {
        add(std::dynamic_pointer_cast<detail::IAnimation>(animation));
    }

    /**
     * Add a sub animation to the sequence.
     *
     * @param animation The animation.
     */
    virtual void add(const std::shared_ptr<detail::IAnimation>& animation)
    {
        if (!animation)
            return;

        auto i = std::find_if(m_animations.begin(), m_animations.end(),
                              [animation](const std::shared_ptr<detail::IAnimation>& ptr)
        {
            return ptr.get() == animation.get();
        });

        if (i == m_animations.end())
        {
            m_animations.push_back(animation);

            animation->add_callback([this, animation](float_t)
            {
                // when the animation ever completes, we go next
                if (!animation->running())
                    next();
            });
        }
    }

    /**
     * Remove a sub animation from the sequence.
     *
     * @param animation The animation.
     */
    void remove(detail::IAnimation* animation)
    {
        if (!animation)
            return;

        auto i = std::find_if(m_animations.begin(), m_animations.end(),
                              [animation](const std::shared_ptr<detail::IAnimation>& ptr)
        {
            return ptr.get() == animation;
        });
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

    /**
     * Reset the animation sequence back to the beginning.
     */
    virtual void reset()
    {
        stop();
        m_current = 0;
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
        if (!running())
            return true;

        if (m_animations[m_current]->running())
            return false;

        if (++m_current >= m_animations.size())
        {
            if (m_loop)
            {
                m_current = 0;
            }
            else
            {
                m_current = 0;
                m_running = false;
                return m_running;
            }
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

    virtual ~AnimationSequence() = default;

protected:

    /**
     * Helper type for an array of animations.
     */
    using animation_array = std::vector<std::shared_ptr<detail::IAnimation>>;

    /**
     * The animations of the sequence.
     */
    animation_array m_animations;

    /**
     * Index of the current animation.
     */
    size_t m_current{0};

    /**
     * Should the animation loop.
     */
    bool m_loop{false};
};

/**
 * Animation object with built in timer.
 *
 * An Animation usually involves setting up a timer to run the animation
 * at a periodic interval. This wraps an animation around a built in
 * timer to run the animation.
 *
 * @ingroup animation
 */
class AutoAnimation : public Animation
{
public:

    /**
     * @param[in] start The starting value of the animation.
     * @param[in] end The ending value of the animation.
     * @param[in] duration The duration of the animation.
     * @param[in] func The easing function to use.
     * @param[in] callback Called whenever the animation value changes. May be nullptr.
     */
    AutoAnimation(float_t start,
                  float_t end,
                  std::chrono::milliseconds duration,
                  easing_func_t func = easing_linear,
                  animation_callback_t callback = nullptr);

    virtual void start() override;
    virtual void stop() override;

    virtual ~AutoAnimation() = default;

protected:

    /**
     * Periodic timer used to run the animation.
     */
    PeriodicTimer m_timer;
};

/**
 * Animates a property of a widget.
 *
 * This as a utility class that makes animating a specific property of a
 * widget easier.  Any property that matches
 * PropertyAnimator::property_callback_t can be used.
 *
 * @ingroup animation
 */
template <class T>
class PropertyAnimatorType : public AutoAnimation
{
public:

    /**
     * @param[in] start The starting value of the animation.
     * @param[in] end The ending value of the animation.
     * @param[in] duration The duration of the animation.
     * @param[in] func The easing function to use.
     */
    explicit PropertyAnimatorType(T start = T(),
                                  T end = T(),
                                  std::chrono::milliseconds duration = std::chrono::milliseconds(),
                                  easing_func_t func = easing_linear)
        : AutoAnimation(start, end, duration, func,
                        std::bind(&PropertyAnimatorType<T>::invoke_handlers,
                                  this, std::placeholders::_1))
    {}

    using property_callback_t = std::function<void (T v)>;

    /**
     * Register a callback handler for when the value changes.
     *
     * @param[in] callback The callback function to invoke.
     */
    inline void on_change(property_callback_t callback)
    {
        m_callbacks.push_back(callback);
    }

    /**
     * Clear all callbacks.
     */
    inline void clear_change_callbacks()
    {
        m_callbacks.clear();
    }

    virtual ~PropertyAnimatorType() = default;

protected:

    /**
     * Invoke handlers with the specified value.
     */
    void invoke_handlers(T value)
    {
        for (auto& callback : m_callbacks)
            callback(value);
    }

    using callback_array = std::vector<property_callback_t>;

    /**
     * Registered callbacks for the animation.
     */
    callback_array m_callbacks;
};

/**
 * Helper type.
 */
using PropertyAnimator = PropertyAnimatorType<int>;
using PropertyAnimatorF = PropertyAnimatorType<float>;

/**
 * Simple delay, useful to insert a delay in an AnimationSequence.
 *
 * This basically established as one-shot timer to create a delay.  It performs
 * no easing curve function evaluation.
 *
 * @ingroup animation
 */
class AnimationDelay : public detail::IAnimation
{
public:

    AnimationDelay() = delete;

    /**
     * @param duration The delay time for the animation.
     */
    explicit AnimationDelay(std::chrono::milliseconds duration) noexcept
        : m_timer(duration)
    {
        m_timer.on_timeout([this]()
        {
            if (!next())
            {
                stop();
            }
        });
    }

    virtual void start() override
    {
        m_timer.start();
    }

    virtual bool next() override
    {
        for (auto& callback : m_callbacks)
            callback(0);

        return false;
    }

    virtual void stop() override
    {
        m_timer.cancel();
    }

    virtual ~AnimationDelay() = default;

protected:

    /**
     * One-shot timer.
     */
    Timer m_timer;
};

}
}

#endif
