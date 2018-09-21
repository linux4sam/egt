/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_VALUEWIDGET_H
#define MUI_VALUEWIDGET_H

#include <mui/widget.h>

namespace mui
{

    /**
     * A widget that manages an unbounded value.
     */
    template<class T>
    class ValueWidget : public Widget
    {
    public:

        ValueWidget(const Point& point, const Size& size, T value = T())
            : Widget(point.x, point.y, size.w, size.h),
              m_value(value)
        {}

        /**
         * Set value.
         *
         * If this results in changing the value, it will damage() the widget.
         */
        virtual void value(T v)
        {
            if (v != m_value)
            {
                m_value = v;
                damage();
            }
        }

        /**
         * Get value.
         */
        T value() const { return m_value; }

        virtual ~ValueWidget()
        {}

    protected:
        T m_value;
    };

    /**
     * Radial dial widget that a user uses to select a value.
     */
    class Radial : public ValueWidget<float>
    {
    public:
        Radial(const Point&, const Size& size);

        void label(const std::string& label)
        {
            m_label = label;
        }

        virtual int handle(int event);

        virtual void draw(const Rect& rect);

    protected:
        std::string m_label;
    };

    /**
    * A widget that manages a value that must reside between a min and max
    * value range.
    */
    template<class T>
    class ValueRangeWidget : public Widget
    {
    public:

        ValueRangeWidget(const Point& point, const Size& size, T min, T max,
                         T value = T())
            : Widget(point.x, point.y, size.w, size.h),
              m_min(min),
              m_max(max),
              m_value(value)
        {}

        /**
         * Set value.
         *
         * If the value is above max, the value will be set to max.  If the
         * value is below min, the value will be set to min.
         *
         * If this results in changing the value, it will damage() the widget.
         */
        virtual void value(T v)
        {
            if (v > m_max)
                v = m_max;

            if (v < m_min)
                v = m_min;

            if (v != m_value)
            {
                m_value = v;
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
    };

    /**
     * Displays a progress bar based on a value.
     */
    class ProgressBar : public ValueRangeWidget<int>
    {
    public:
        ProgressBar(const Point& point = Point(), const Size& size = Size());

        virtual void draw(const Rect& rect);
    };

    /**
     * Displays a level meter based on a value.
     */
    class LevelMeter : public ValueRangeWidget<int>
    {
    public:
        LevelMeter(const Point& point = Point(), const Size& size = Size());

        virtual void draw(const Rect& rect);
    };

    /**
     * Displays an analog meter based on a value.
     *
     * <http://www.peteronion.org.uk/GtkExamples/GladeTutorials.html>
     */
    class AnalogMeter : public ValueRangeWidget<int>
    {
    public:
        AnalogMeter(const Point& point = Point(), const Size& size = Size());

        virtual void draw(const Rect& rect);
    };

    class SpinProgress : public ValueRangeWidget<int>
    {
    public:
        SpinProgress(const Point& point = Point(), const Size& size = Size());

        virtual void draw(const Rect& rect);
    };

}

#endif
