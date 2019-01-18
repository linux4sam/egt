/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_VALUEWIDGET_H
#define EGT_VALUEWIDGET_H

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
    {}

    /**
     * Set value.
     *
     * If the value is above max, the value will be set to max.  If the
     * value is below min, the value will be set to min.
     *
     * If this results in changing the value, it will damage() the widget.
     */
    virtual void set_value(T v)
    {
        if (v > m_max)
            v = m_max;

        if (v < m_min)
            v = m_min;

        if (v != m_value)
        {
            m_value = v;
            damage();
            invoke_handlers(eventid::PROPERTY_CHANGED);
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
 * Radial dial widget that a user uses to select a value.
 *
 * @image html widget_radial.png
 * @image latex widget_radial.png "widget_radial" width=5cm
 */
template<class T>
class Radial : public ValueRangeWidget<T>
{
public:

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] min Minimum value for the range.
     * @param[in] max Maximum value in the range.
     * @param[in] value Current value in the range.
     */
    Radial(const Rect& rect, T min, T max, T value = T())
        : ValueRangeWidget<T>(rect, min, max, value)
    {
        this->set_boxtype(Theme::boxtype::none);
        this->flag_set(widgetmask::GRAB_MOUSE);
    }

    Radial(Frame& parent, const Rect& rect, T min, T max, T value = T())
        : Radial<T>(rect, min, max, value)
    {
        parent.add(this);
    }

    inline T value2() const
    {
        return m_value2;
    }

    virtual void set_value2(T v)
    {
        if (v > this->m_max)
            v = this->m_max;

        if (v < this->m_min)
            v = this->m_min;

        if (v != this->m_value2)
        {
            this->m_value2 = v;
            this->damage();
            this->invoke_handlers(eventid::PROPERTY_CHANGED);
        }
    }

    /**
     * Set the center label text of the dial.
     */
    void text(const std::string& text)
    {
        m_text = text;
    }

    virtual int handle(eventid event) override
    {
        auto ret = Widget::handle(event);

        switch (event)
        {
        case eventid::MOUSE_DOWN:
        {
            this->set_active(true);
            return 1;
        }
        case eventid::MOUSE_UP:
        {
            this->set_active(false);
            return 1;
        }
        case eventid::MOUSE_MOVE:
        {
            if (this->active())
            {
                auto angle = this->touch_to_degrees(this->from_screen(event_mouse()));
                auto v = this->degrees_to_value(angle);
                this->set_value(v);

                return 1;
            }
            break;
        }
        default:
            break;
        }

        return ret;
    }

    virtual void draw(Painter& painter, const Rect& rect) override
    {
        ignoreparam(rect);

        auto v = this->value_to_degrees(this->value());
        float linew = 40;

        auto color1 = this->palette().color(Palette::BG);
        color1.alpha(0x55);
        auto color2 = this->palette().color(Palette::HIGHLIGHT);
        auto color3 = this->palette().color(Palette::MID);

        float radius = this->w() / 2 - (linew / 2);
        float angle1 = to_radians<float>(-90, 0);
        float angle2 = to_radians<float>(-90, v);

        auto c = this->center();

        Painter::AutoSaveRestore sr(painter);

        // bottom full circle
        painter.set_color(color1);
        painter.set_line_width(linew);
        painter.arc(Arc(c, radius, 0, 2 * M_PI));
        painter.stroke();

        // value arc
        painter.set_color(color2);
        painter.set_line_width(linew - (linew / 3));
        painter.arc(Arc(c, radius, angle1, angle2));
        painter.stroke();

        // handle
        painter.set_color(color3);
        painter.set_line_width(linew);
        painter.arc(Arc(c, radius, angle2 - 0.3, angle2));
        painter.stroke();

        // secondary value
        auto color4 = Color::RED;
        float angle3 = to_radians<float>(-90,
                                         this->value_to_degrees(this->value2()));
        painter.set_color(color4);
        painter.set_line_width(linew);
        painter.arc(Arc(c, radius, angle3 - 0.01, angle3 + 0.01));
        painter.stroke();

        if (!m_text.empty())
            painter.draw_text(m_text, this->box(), this->palette().color(Palette::TEXT),
                              alignmask::CENTER, 0, Font(72));
    }

    virtual ~Radial()
    {}

protected:

    float touch_to_degrees(const Point& point)
    {
        const Point c = this->center() - this->box().point();
        float radians = c.angle_to<float>(point);
        float angle = to_degrees(radians);
        if (angle < 0)
            angle = angle + 360.;
        return angle;
    }

    /**
     * Normalize a value to degrees.
     */
    float value_to_degrees(T value)
    {
        float n = (static_cast<float>(value) -
                   static_cast<float>(this->m_min)) /
                  (static_cast<float>(this->m_max) - static_cast<float>(this->m_min));
        return n * 360.;
    }

    /**
     * Normalize degrees to a value.
     */
    T degrees_to_value(float degrees)
    {
        float n = degrees / 360.;
        return (n * (this->m_max - this->m_min)) + this->m_min;
    }

    std::string m_text;

    T m_value2{};

    Radial() = delete;
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

    virtual void draw(Painter& painter, const Rect& rect);

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

    virtual void draw(Painter& painter, const Rect& rect);

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

    virtual void draw(Painter& painter, const Rect& rect);

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

    virtual void draw(Painter& painter, const Rect& rect);

    virtual ~SpinProgress()
    {}

protected:
    Font m_dynamic_font;
};

}
}

#endif
