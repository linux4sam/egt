/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_VALUEWIDGET_H
#define EGT_VALUEWIDGET_H

/**
 * @file
 * @brief Widgets for managing values.
 */

#include <cassert>
#include <egt/signal.h>
#include <egt/widget.h>

namespace egt
{
inline namespace v1
{

/**
 * Manages an unbounded value.
 *
 * While the value is technically unbounded, a type of bool will only allow
 * a boolean value.
 */
template<class T>
class ValueWidget : public Widget
{
public:

    /**
     * Event signal.
     * @{
     */
    /**
     * Invoked when the value of the widget changes.
     */
    Signal<> on_value_changed;
    /** @} */

    ValueWidget() = delete;

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] value Current value.
     */
    explicit ValueWidget(const Rect& rect, T value = T()) noexcept
        : Widget(rect),
          m_value(value)
    {}

    /**
     * Set the value.
     *
     * If this results in changing the value, it will damage() the widget.
     *
     * @return The old value.
     */
    T value(T v)
    {
        T orig = m_value;
        if (detail::change_if_diff<T>(m_value, v))
        {
            damage();
            on_value_changed.invoke();
        }
        return orig;
    }

    /**
     * Get the value.
     */
    T value() const { return m_value; }

protected:

    /**
     * The current value.
     */
    T m_value;
};

/**
 * Manages a value in a range.
 *
 * A Widget that manages a value that must reside between a start and end
 * value range.
 */
template<class T>
class ValueRangeWidget : public Widget
{
public:

    /**
     * Event signal.
     * @{
     */
    /**
     * Invoked when the value of the widget changes.
     */
    Signal<> on_value_changed;
    /** @} */

    ValueRangeWidget() = delete;

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] start Starting value for the range.
     * @param[in] end Ending value in the range.
     * @param[in] value Current value in the range.
     */
    ValueRangeWidget(const Rect& rect, T start, T end,
                     T value = T()) noexcept
        : Widget(rect),
          m_start(start),
          m_end(end),
          m_value(value)
    {
        assert(m_end > m_start);

        if (m_value > m_end)
            m_value = m_end;

        if (m_value < m_start)
            m_value = m_start;
    }

    /**
     * Set value.
     *
     * If the value is above end, the value will be set to end.  If the
     * value is below start, the value will be set to start.
     *
     * If this results in changing the value, it will damage() the widget.
     *
     * @param[in] value Value to set.
     * @return The old value.
     */
    virtual T value(T value)
    {
        T orig = m_value;

        assert(m_end > m_start);

        if (value > m_end)
            value = m_end;

        if (value < m_start)
            value = m_start;

        if (detail::change_if_diff<T>(m_value, value))
        {
            damage();
            on_value_changed.invoke();
        }

        return orig;
    }

    /**
     * Get the start value.
     */
    EGT_NODISCARD T starting() const { return m_start; }

    /**
     * Get the end value.
     */
    EGT_NODISCARD T ending() const { return m_end; }

    /**
     * Set the start value.
     *
     * @param[in] v The start value.
     */
    void starting(T v)
    {
        if (detail::change_if_diff<>(m_start, v))
            damage();

        assert(m_end > m_start);
    }

    /**
     * Set the end value.
     *
     * @param[in] v The end value.
     */
    void ending(T v)
    {
        if (detail::change_if_diff<>(m_end, v))
            damage();

        assert(m_end > m_start);
    }

    /**
     * Get the current value.
     */
    EGT_NODISCARD T value() const
    {
        return m_value;
    }

protected:

    /// The start value.
    T m_start;

    /// The end value.
    T m_end;

    /// The current value.
    T m_value;
};

}
}

#endif
