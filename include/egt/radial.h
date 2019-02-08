/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_RADIAL_H
#define EGT_RADIAL_H

#include <egt/valuewidget.h>

namespace egt
{
inline namespace v1
{

namespace detail
{
template<class T>
struct flags_hash;
}

/**
 * Radial dial widget that a user uses to select a value.
 *
 * @image html widget_radial.png
 * @image latex widget_radial.png "widget_radial" width=5cm
 */
template<class T = int>
class RadialType : public ValueRangeWidget<T>
{
public:

    enum class radial_flags
    {
        primary_value,
        primary_handle,
        secondary_value,
        text
    };

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] min Minimum value for the range.
     * @param[in] max Maximum value in the range.
     * @param[in] value Current value in the range.
     */
    RadialType(const Rect& rect, T min, T max, T value = T())
        : ValueRangeWidget<T>(rect, min, max, value)
    {
        this->set_boxtype(Theme::boxtype::none);
        this->flag_set(widgetmask::GRAB_MOUSE);
    }

    RadialType(Frame& parent, const Rect& rect, T min, T max, T value = T())
        : RadialType<T>(rect, min, max, value)
    {
        parent.add(this);
    }

    inline T value2() const
    {
        return m_value2;
    }

    virtual void set_value2(T v)
    {
        assert(this->m_max > this->m_min);

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
        case eventid::RAW_POINTER_DOWN:
        {
            this->set_active(true);
            return 1;
        }
        case eventid::RAW_POINTER_UP:
        {
            this->set_active(false);
            return 1;
        }
        case eventid::RAW_POINTER_MOVE:
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

        auto color1 = this->palette().color(Palette::MID);
        auto color2 = this->palette().color(Palette::HIGHLIGHT);
        auto color3 = this->palette().color(Palette::DARK);
        auto color4 = this->palette().color(Palette::HIGHLIGHT);

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

        if (m_rflags.find(radial_flags::primary_value) != m_rflags.end())
        {
            // value arc
            painter.set_color(color2);
            painter.set_line_width(linew - (linew / 3));
            painter.arc(Arc(c, radius, angle1, angle2));
            painter.stroke();
        }

        if (m_rflags.find(radial_flags::primary_handle) != m_rflags.end())
        {
            // handle
            painter.set_color(color3);
            painter.set_line_width(linew);
            painter.arc(Arc(c, radius, angle2 - 0.3, angle2));
            painter.stroke();
        }

        if (m_rflags.find(radial_flags::secondary_value) != m_rflags.end())
        {
            // secondary value
            float angle3 = to_radians<float>(-90,
                                             this->value_to_degrees(this->value2()));
            painter.set_color(color4);
            painter.set_line_width(linew);
            painter.arc(Arc(c, radius, angle3 - 0.01, angle3 + 0.01));
            painter.stroke();
        }

        if (m_rflags.find(radial_flags::text) != m_rflags.end())
        {
            if (!m_text.empty())
                painter.draw_text(m_text, this->box(), this->palette().color(Palette::TEXT),
                                  alignmask::CENTER, 0, Font(72));
        }
    }

    using flag_array = std::unordered_set<radial_flags, detail::flags_hash<T>>;

    void set_radial_flags(flag_array flags)
    {
        m_rflags = flags;
    }

    virtual ~RadialType()
    {}

protected:

    float touch_to_degrees(const Point& point)
    {
        const Point c = this->center();
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

    RadialType() = delete;

    flag_array m_rflags{radial_flags::primary_value,
                   radial_flags::primary_handle,
                   radial_flags::secondary_value,
                   radial_flags::text};
};

namespace detail
{
template<class T>
struct flags_hash
{
    std::size_t operator()(typename RadialType<T>::radial_flags const& s) const noexcept
    {
        return std::hash<int> {}(static_cast<int>(s));
    }
};
}

using Radial = RadialType<>;

}
}

#endif
