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
            : Widget(point, size),
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
            : Widget(point, size),
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

    protected:
        Font m_font;
    };

    class SpinProgress : public ValueRangeWidget<int>
    {
    public:
        SpinProgress(const Point& point = Point(), const Size& size = Size());

        virtual void draw(const Rect& rect);
    };

    /**
    * @todo This should be a ValueRangeWidget<int>.
    */
    class Slider : public Widget
    {
    public:
        enum
        {
            ORIENTATION_HORIZONTAL,
            ORIENTATION_VERTICAL,
        };

        Slider(int min, int max, const Point& point = Point(),
               const Size& size = Size(),
               int orientation = ORIENTATION_HORIZONTAL);

        virtual int handle(int event);

        virtual void draw(const Rect& rect);

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
                invoke_handlers();
            }
        }

        virtual ~Slider();

    protected:

        // position to offset
        inline int normalize(int pos)
        {
            if (m_orientation == ORIENTATION_HORIZONTAL)
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
            if (m_orientation == ORIENTATION_HORIZONTAL)
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
        int m_moving_x;
        int m_start_pos;
        int m_orientation;
    };

}

#endif
