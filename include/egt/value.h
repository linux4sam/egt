/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_VALUE_H
#define EGT_VALUE_H

#include <cassert>
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
    T set_value(T value)
    {
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

}
}

#endif
