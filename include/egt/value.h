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
#include <egt/object.h>
#include <egt/signal.h>

namespace egt
{
inline namespace v1
{

/**
 * Manages a single value.
 */
template<class T>
class Value : public Object
{
public:

    /**
     * Event signal.
     * @{
     */
    /// Invoked when the value changes.
    Signal<> on_value_changed;
    /** @} */

    /**
     * @param[in] value Current value.
     */
    explicit Value(T value = {}) noexcept
        : m_value(value)
    {
    }

    EGT_OPS_NOCOPY_MOVE(Value);
    ~Value() noexcept override = default;

    /// Get the current value.
    explicit operator T() const
    {
        return m_value;
    }

    /// Assign new value.
    Value<T>& operator=(T value)
    {
        value(value);
        return *this;
    }

    /// Get the current value.
    T value() const { return m_value; }

    /**
     * Set value.
     *
     * @param[in] value Value to set.
     * @return true if changed.
     */
    bool value(T value)
    {
        if (detail::change_if_diff<T>(m_value, value))
        {
            on_value_changed.invoke();
            return true;
        }

        return false;
    }

protected:

    /// The current value.
    T m_value{};
};

/**
 * Manages a value in a range.
 */
template<class T>
class RangeValue : public Object
{
public:

    /**
     * Event signal.
     * @{
     */
    /// Invoked when the value of the widget changes.
    Signal<> on_value_changed;
    /** @} */

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

    RangeValue() noexcept = default;
    EGT_OPS_NOCOPY_MOVE(RangeValue);
    ~RangeValue() noexcept override = default;

    /// Get the current value.
    explicit operator T() const
    {
        return m_value;
    }

    /// Assign the current value.
    RangeValue<T>& operator=(T value)
    {
        value(value);
        return *this;
    }

    /// Get the current value.
    EGT_NODISCARD T value() const { return m_value; }

    /**
     * Set value.
     *
     * If the value is above max, the value will be set to max.  If the
     * value is below min, the value will be set to min.
     *
     * @param[in] value Value to set.
     * @return true if changed.
     */
    bool value(T value)
    {
        assert(m_max > m_min);

        value = detail::clamp<T>(value, m_min, m_max);

        if (detail::change_if_diff<T>(m_value, value))
        {
            on_value_changed.invoke();
            return true;
        }

        return false;
    }

    /// Get the min value.
    EGT_NODISCARD T min() const { return m_min; }

    /**
     * Set the min value.
     *
     * @param[in] min The min value.
     */
    void min(T min)
    {
        if (detail::change_if_diff<>(m_min, min))
            value(m_value);
    }

    /// Get the max value.
    EGT_NODISCARD T max() const { return m_max; }

    /**
     * Set the max value.
     *
     * @param[in] max The max value.
     */
    void max(T max)
    {
        if (detail::change_if_diff<>(m_max, max))
            value(m_value);
    }

protected:

    /// The min value.
    T m_min{};

    /// The max value.
    T m_max{};

    /// The current value.
    T m_value{};
};

}
}

#endif
