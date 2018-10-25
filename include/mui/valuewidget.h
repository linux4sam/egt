/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_VALUEWIDGET_H
#define MUI_VALUEWIDGET_H

#include <mui/widget.h>
#include <string>

namespace mui
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

        ValueWidget(const Rect& rect, T value = T())
            : Widget(rect),
              m_value(value)
        {}

        /**
         * Set the value.
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
         * Get the value.
         */
        inline T value() const { return m_value; }

        virtual ~ValueWidget()
        {}

    protected:
        T m_value;
    };

    /**
     * Radial dial widget that a user uses to select a value.
     *
     * @image html widget_radial.png
     * @image latex widget_radial.png "widget_radial" height=10cm
     */
    class Radial : public ValueWidget<float>
    {
    public:
        explicit Radial(const Rect& rect);

        void label(const std::string& label)
        {
            m_label = label;
        }

        virtual int handle(int event);

        virtual void draw(Painter& painter, const Rect& rect);

        virtual ~Radial()
        {}

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

        ValueRangeWidget(const Rect& rect, T min, T max,
                         T value = T())
            : Widget(rect),
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
        explicit ProgressBar(const Rect& rect = Rect());

        virtual void draw(Painter& painter, const Rect& rect);
    };

    /**
     * Displays a level meter based on a value.
     *
     * @image html widget_levelmeter.png
     * @image latex widget_levelmeter.png "widget_levelmeter" height=10cm
     */
    class LevelMeter : public ValueRangeWidget<int>
    {
    public:
        explicit LevelMeter(const Rect& rect = Rect());

        virtual void draw(Painter& painter, const Rect& rect);
    };

    /**
     * Displays an analog meter based on a value.
     *
     * @image html widget_analogmeter.png
     * @image latex widget_analogmeter.png "widget_analogmeter" height=10cm
     *
     * <http://www.peteronion.org.uk/GtkExamples/GladeTutorials.html>
     */
    class AnalogMeter : public ValueRangeWidget<int>
    {
    public:
        explicit AnalogMeter(const Rect& rect = Rect());

        virtual void draw(Painter& painter, const Rect& rect);

    protected:
        Font m_font;
    };

    class SpinProgress : public ValueRangeWidget<int>
    {
    public:
        explicit SpinProgress(const Rect& rect = Rect());

        virtual void draw(Painter& painter, const Rect& rect);
    };

    enum orientation
    {
        HORIZONTAL,
        VERTICAL,
    };

    /**
     * This is a slider that can be used to select fro a range of values.
     *
     * @todo This should be a ValueRangeWidget<int>.
     *
     * @image html widget_slider1.png
     * @image latex widget_slider1.png "widget_slider1" height=10cm
     * @image html widget_slider2.png
     * @image latex widget_slider2.png "widget_slider2" height=10cm
     */
    class Slider : public Widget
    {
    public:
        Slider(int min, int max, const Rect& rect = Rect(),
               orientation orient = orientation::HORIZONTAL);

        virtual int handle(int event) override;

        virtual void draw(Painter& painter, const Rect& rect) override;

        inline int position() const { return m_pos; }

        inline void position(int pos)
        {
            if (pos > m_max)
            {
                pos = m_max;
            }
            else if (pos < m_min)
            {
                pos = m_min;
            }

            if (pos != m_pos)
            {
                m_pos = pos;
                damage();
                invoke_handlers(EVT_PROPERTY_CHANGE);
            }
        }

        virtual ~Slider();

    protected:

        // position to offset
        inline int normalize(int pos)
        {
            if (m_orientation == orientation::HORIZONTAL)
            {
                int dim = h();
                return float(w() - dim) / float(m_max - m_min) * float(pos);
            }
            else
            {
                int dim = w();
                return float(h() - dim) / float(m_max - m_min) * float(pos);
            }
        }

        // offset to position
        inline int denormalize(int diff)
        {
            if (m_orientation == orientation::HORIZONTAL)
            {
                int dim = h();
                return float(m_max - m_min) / float(w() - dim) * float(diff);
            }
            else
            {
                int dim = w();
                return float(m_max - m_min) / float(h() - dim) * float(diff);
            }
        }

        int m_min;
        int m_max;
        int m_pos;
        int m_moving_x {0};
        int m_start_pos {0};
        orientation m_orientation;
    };

}

#endif
