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
        if (m_start < m_end)
            value = detail::clamp<T>(value, m_start, m_end);
        else
            value = detail::clamp<T>(value, m_end, m_start);
    }

    /**
     * @param[in] props list of widget argument and its properties.
     */
    explicit ValueRangeWidget(Serializer::Properties& props)
        : ValueRangeWidget(props, false)
    {
    }

protected:

    explicit ValueRangeWidget(Serializer::Properties& props, bool is_derived)
        : Widget(props, true)
    {
        deserialize(props);

        if (!is_derived)
            deserialize_leaf(props);
    }

public:

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

        if (m_start < m_end)
            value = detail::clamp<T>(value, m_start, m_end);
        else
            value = detail::clamp<T>(value, m_end, m_start);

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
        {
            T value = m_value;
            if (m_start < m_end)
                value = detail::clamp<T>(value, m_start, m_end);
            else
                value = detail::clamp<T>(value, m_end, m_start);

            bool ret = detail::change_if_diff<T>(m_value, value);
            damage();
            if (ret)
                on_value_changed.invoke();
        }
    }

    /**
     * Set the end value.
     *
     * @param[in] v The end value.
     */
    void ending(T v)
    {
        if (detail::change_if_diff<>(m_end, v))
        {
            T value = m_value;
            if (m_start < m_end)
                value = detail::clamp<T>(value, m_start, m_end);
            else
                value = detail::clamp<T>(value, m_end, m_start);

            bool ret = detail::change_if_diff<T>(m_value, value);
            damage();
            if (ret)
                on_value_changed.invoke();
        }
    }

    /**
     * Get the current value.
     */
    EGT_NODISCARD T value() const
    {
        return m_value;
    }

    void serialize(Serializer& serializer) const override;

protected:

    /// The start value.
    T m_start;

    /// The end value.
    T m_end;

    /// The current value.
    T m_value;

private:

    void deserialize(Serializer::Properties& props);
};

template <class T>
void ValueRangeWidget<T>::serialize(Serializer& serializer) const
{
    Widget::serialize(serializer);

    const Serializer::Attributes attrs =
    {
        {"starting", detail::to_string(starting())},
        {"ending", detail::to_string(ending())},
    };

    serializer.add_property("value", this->value(), attrs);
}

template <class T>
void ValueRangeWidget<T>::deserialize(Serializer::Properties& props)
{
    props.erase(std::remove_if(props.begin(), props.end(), [&](auto & p)
    {
        if (std::get<0>(p) == "value")
        {
            float starting_value = 0, ending_value = 100;
            auto attrs = std::get<2>(p);
            for (auto& a : attrs)
            {
                switch (detail::hash(a.first))
                {
                case detail::hash("starting"):
                    starting_value = std::stof(a.second);
                    break;
                case detail::hash("ending"):
                    ending_value = std::stof(a.second);
                    break;
                }
            }

            this->ending(ending_value);
            this->starting(starting_value);
            this->value(std::stof(std::get<1>(p)));

            return true;
        }
        return false;
    }), props.end());
}

}
}

#endif
