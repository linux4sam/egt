/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_VALUE_H
#define EGT_VALUE_H

#include <cassert>
#include <egt/animation.h>
#include <egt/detail/math.h>
#include <egt/detail/object.h>

namespace egt
{
inline namespace v1
{

/**
 * Manages a single value.
 */
template<class T>
class Value : public detail::Object
{
public:

    /**
     * @param[in] value Current value.
     */
    explicit Value(T value = {}) noexcept
        : m_value(value)
    {
    }

    operator T() const
    {
        return m_value;
    }

    Value<T>& operator=(T value)
    {
        set_value(value);
        return *this;
    }

    /**
     * Get the current value.
     */
    inline T value() const { return m_value; }

    /**
     * Set value.
     *
     * @param[in] value Value to set.
     * @return The old value.
     */
    T set_value(T value)
    {
        T orig = m_value;

        if (detail::change_if_diff<T>(m_value, value))
            invoke_handlers(eventid::property_changed);

        return orig;
    }

    virtual ~Value() = default;

protected:

    /**
     * The current value.
     */
    T m_value{};
};

/**
 * Manages a value in a range.
 */
template<class T>
class RangeValue : public detail::Object
{
public:

    /**
     * @param[in] min Minimum value for the range.
     * @param[in] max Maximum value in the range.
     * @param[in] value Current value in the range.
     */
    RangeValue(T min, T max, T value = {}) noexcept
        : m_min(min),
          m_max(max),
          m_value(detail::clamp<T>(value, m_min, m_max))
    {
        assert(m_max > m_min);
    }

    operator T() const
    {
        return m_value;
    }

    RangeValue<T>& operator=(T value)
    {
        set_value(value);
        return *this;
    }

    /**
     * Get the current value.
     */
    inline T value() const { return m_value; }

    /**
     * Set value.
     *
     * If the value is above max, the value will be set to max.  If the
     * value is below min, the value will be set to min.
     *
     * @param[in] value Value to set.
     * @return The old value.
     */
    virtual T set_value(T value)
    {
        assert(m_max > m_min);

        auto orig = m_value;

        value = detail::clamp<T>(value, m_min, m_max);

        if (detail::change_if_diff<T>(m_value, value))
            invoke_handlers(eventid::property_changed);

        return orig;
    }

    /**
     * Get the min value.
     */
    inline T min() const { return m_min; }

    /**
     * Set the min value.
     *
     * @param[in] min The min value.
     */
    inline void set_min(T min)
    {
        if (detail::change_if_diff<>(m_min, min))
            set_value(m_value);
    }

    /**
    * Get the max value.
    */
    inline T max() const { return m_max; }

    /**
     * Set the max value.
     *
     * @param[in] max The max value.
     */
    inline void set_max(T max)
    {
        if (detail::change_if_diff<>(m_max, max))
            set_value(m_value);
    }

    virtual ~RangeValue() = default;

protected:

    /**
     * The min value.
     */
    T m_min{};

    /**
     * The max value.
     */
    T m_max{};

    /**
     * The current value.
     */
    T m_value{};
};

template<class T>
class SmoothRangeValue : public RangeValue<T>
{
public:
    SmoothRangeValue(T min, T max, T value = {}, std::chrono::milliseconds dur = std::chrono::milliseconds(500)) noexcept
        : RangeValue<T>(min, max, value)
    {
        m_pending = value;

        m_animation.set_duration(dur);
        m_animation.set_easing_func(easing_linear);
        m_animation.on_change([this](T value)
        {
            if (detail::change_if_diff<T>(this->m_value, value))
                this->invoke_handlers(eventid::property_changed);
        });
    }

    virtual T set_value(T value) override
    {
        assert(this->m_max > this->m_min);

        /*
         * Instead of directly setting m_value here, we set m_pending instead
         * and then fire an animator to slowly change m_value to pending.
         */
        auto orig = m_pending;

        value = detail::clamp<T>(value, this->m_min, this->m_max);

        if (detail::change_if_diff<T>(m_pending, value))
            animate(this->m_value, m_pending);

        return orig;
    }

protected:

    void animate(const T starting, const T ending)
    {
        m_animation.stop();
        m_animation.set_starting(starting);
        m_animation.set_ending(ending);
        m_animation.start();
    }

    T m_pending{};
    PropertyAnimatorType<T> m_animation;
};

}
}

#endif
