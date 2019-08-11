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
    virtual T set_value(T v)
    {
        T orig = m_value;
        if (detail::change_if_diff<T>(m_value, v))
        {
            damage();
            invoke_handlers(eventid::property_changed);
        }
        return orig;
    }

    /**
     * Get the value.
     */
    inline T value() const { return m_value; }

    virtual ~ValueWidget() = default;

protected:

    /**
     * The current value.
     */
    T m_value;
};

/**
 * Manages a value in a range.
 *
 * A Widget that manages a value that must reside between a min and max
 * value range.
 */
template<class T>
class ValueRangeWidget : public Widget
{
public:

    ValueRangeWidget() = delete;

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] min Minimum value for the range.
     * @param[in] max Maximum value in the range.
     * @param[in] value Current value in the range.
     */
    ValueRangeWidget(const Rect& rect, T min, T max,
                     T value = T()) noexcept
        : Widget(rect),
          m_min(min),
          m_max(max),
          m_value(value)
    {
        assert(m_max > m_min);

        if (m_value > m_max)
            m_value = m_max;

        if (m_value < m_min)
            m_value = m_min;
    }

    /**
     * Set value.
     *
     * If the value is above max, the value will be set to max.  If the
     * value is below min, the value will be set to min.
     *
     * If this results in changing the value, it will damage() the widget.
     *
     * @param[in] value Value to set.
     * @return The old value.
     */
    virtual T set_value(T value)
    {
        T orig = m_value;

        assert(m_max > m_min);

        if (value > m_max)
            value = m_max;

        if (value < m_min)
            value = m_min;

        if (detail::change_if_diff<T>(m_value, value))
        {
            damage();
            invoke_handlers(eventid::property_changed);
        }

        return orig;
    }

    /**
     * Get the min value.
     */
    inline T min() const { return m_min; }

    /**
     * Get the max value.
     */
    inline T max() const { return m_max; }

    /**
     * Set the min value.
     *
     * @param[in] v The min value.
     */
    virtual void set_min(T v)
    {
        if (detail::change_if_diff<>(m_min, v))
            damage();

        assert(m_max > m_min);
    }

    /**
     * Set the max value.
     *
     * @param[in] v The max value.
     */
    virtual void set_max(T v)
    {
        if (detail::change_if_diff<>(m_max, v))
            damage();

        assert(m_max > m_min);
    }

    /**
     * Get the current value.
     */
    inline T value() const
    {
        return m_value;
    }

    virtual ~ValueRangeWidget() = default;

protected:

    /**
     * The min value.
     */
    T m_min;

    /**
     * The max value.
     */
    T m_max;

    /**
     * The current value.
     */
    T m_value;
};

}
}

#endif
