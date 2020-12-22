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
     * @param[in] start Start value for the range.
     * @param[in] end End value in the range.
     * @param[in] value Current value in the range.
     */
    RangeValue(T start, T end, T value = {}) noexcept
        : m_start(start),
          m_end(end)
    {
        if (m_start < m_end)
            value = detail::clamp<T>(value, m_start, m_end);
        else
            value = detail::clamp<T>(value, m_end, m_start);
    }

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
     * If the value is above end, the value will be set to end.  If the
     * value is below start, the value will be set to start.
     *
     * @param[in] value Value to set.
     * @return true if changed.
     */
    bool value(T value)
    {
        if (m_start < m_end)
            value = detail::clamp<T>(value, m_start, m_end);
        else
            value = detail::clamp<T>(value, m_end, m_start);

        if (detail::change_if_diff<T>(m_value, value))
        {
            on_value_changed.invoke();
            return true;
        }

        return false;
    }

    /// Get the start value.
    EGT_NODISCARD T start() const { return m_start; }

    /**
     * Set the start value.
     *
     * @param[in] start The start value.
     */
    void start(T start)
    {
        if (detail::change_if_diff<>(m_start, start))
            value(m_value);
    }

    /// Get the end value.
    EGT_NODISCARD T end() const { return m_end; }

    /**
     * Set the end value.
     *
     * @param[in] end The end value.
     */
    void end(T end)
    {
        if (detail::change_if_diff<>(m_end, end))
            value(m_value);
    }

protected:

    /// The start value.
    T m_start{};

    /// The end value.
    T m_end{};

    /// The current value.
    T m_value{};
};

}
}

#endif
