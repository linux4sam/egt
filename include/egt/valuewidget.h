/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_VALUEWIDGET_H
#define EGT_VALUEWIDGET_H

#include <cassert>
#include <egt/widget.h>
#include <egt/painter.h>
#include <egt/frame.h>
#include <string>

namespace egt
{
inline namespace v1
{

/**
 * A widget that manages an unbounded value.
 *
 * While the value is technically unbounded, a type of bool will only allow
 * a boolean value.
 */
template<class T>
class ValueWidget : public Widget
{
public:

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] value Current value in the range.
     */
    ValueWidget(const Rect& rect, T value = T())
        : Widget(rect),
          m_value(value)
    {}

    /**
     * Set the value.
     *
     * If this results in changing the value, it will damage() the widget.
     */
    virtual void set_value(T v)
    {
        /// @todo Need to handle float comparison.
        if (v != m_value)
        {
            m_value = v;
            damage();
            invoke_handlers(eventid::PROPERTY_CHANGED);
        }
    }

    /**
     * Get the value.
     */
    inline T value() const { return m_value; }

    virtual ~ValueWidget()
    {}

protected:
    T m_value;

    ValueWidget() = delete;
};

/**
 * A widget that manages a value that must reside between a min and max
 * value range.
 */
template<class T>
class ValueRangeWidget : public Widget
{
public:

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
     */
    virtual T set_value(T value)
    {
        T orig = m_value;

        assert(m_max > m_min);

        if (value > m_max)
            value = m_max;

        if (value < m_min)
            value = m_min;

        if (value != m_value)
        {
            m_value = value;
            damage();
            invoke_handlers(eventid::PROPERTY_CHANGED);
        }

        return orig;
    }

    inline T min() const { return m_min; }
    inline T max() const { return m_max; }

    virtual void set_min(T v)
    {
        if (m_min != v)
        {
            m_min = v;
            damage();
        }
    }

    virtual void set_max(T v)
    {
        if (m_max != v)
        {
            m_max = v;
            damage();
        }
    }

    /**
     * Get the current value.
     */
    T value() const
    {
        return m_value;
    }

    virtual ~ValueRangeWidget()
    {}

protected:
    T m_min;
    T m_max;
    T m_value;

    ValueRangeWidget() = delete;
};

/**
 * Displays a progress bar based on a value.
 */
class ProgressBar : public ValueRangeWidget<int>
{
public:

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] min Minimum value for the range.
     * @param[in] max Maximum value in the range.
     * @param[in] value Current value in the range.
     */
    explicit ProgressBar(const Rect& rect = Rect(),
                         int min = 0, int max = 100, int value = 0) noexcept;

    virtual void draw(Painter& painter, const Rect& rect) override;

    virtual ~ProgressBar()
    {}
protected:
    Font m_dynamic_font;
};

/**
 * Displays a level meter based on a value.
 *
 * @image html widget_levelmeter.png
 * @image latex widget_levelmeter.png "widget_levelmeter" width=5cm
 */
class LevelMeter : public ValueRangeWidget<int>
{
public:

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] min Minimum value for the range.
     * @param[in] max Maximum value in the range.
     * @param[in] value Current value in the range.
     */
    explicit LevelMeter(const Rect& rect = Rect(),
                        int min = 0, int max = 100, int value = 0) noexcept;

    virtual void draw(Painter& painter, const Rect& rect) override;

    virtual ~LevelMeter()
    {}
};

/**
 * Displays an analog meter based on a percentage value.
 *
 * @image html widget_analogmeter.png
 * @image latex widget_analogmeter.png "widget_analogmeter" width=5cm
 */
class AnalogMeter : public ValueRangeWidget<int>
{
public:

    /**
     * @param[in] rect Rectangle for the widget.
     */
    explicit AnalogMeter(const Rect& rect = Rect());

    virtual void draw(Painter& painter, const Rect& rect) override;

    virtual ~AnalogMeter()
    {}

protected:
    Font m_font;
};

/**
 * Displays a spinning progress meter.
 */
class SpinProgress : public ValueRangeWidget<int>
{
public:

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] min Minimum value for the range.
     * @param[in] max Maximum value in the range.
     * @param[in] value Current value in the range.
     */
    explicit SpinProgress(const Rect& rect = Rect(),
                          int min = 0, int max = 100, int value = 0) noexcept;

    virtual void draw(Painter& painter, const Rect& rect) override;

    virtual ~SpinProgress()
    {}

protected:
    Font m_dynamic_font;
};

}
}

#endif
