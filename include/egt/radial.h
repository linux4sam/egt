/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_RADIAL_H
#define EGT_RADIAL_H

#include <algorithm>
#include <egt/detail/math.h>
#include <egt/flags.h>
#include <egt/frame.h>
#include <egt/painter.h>
#include <egt/valuewidget.h>
#include <memory>
#include <string>

namespace egt
{
inline namespace v1
{

/**
 * Radial dial widget that a user uses to select a value.
 *
 * @image html widget_radial.png
 * @image latex widget_radial.png "widget_radial" width=5cm
 *
 * @ingroup controls
 */
template<class T>
class RadialType : public ValueRangeWidget<T>
{
public:

    enum class flag
    {
        primary_value,
        primary_handle,
        secondary_value,
        text,
        text_value
    };

    using flags_type = Flags<flag>;

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] min Minimum value for the range.
     * @param[in] max Maximum value in the range.
     * @param[in] value Current value in the range.
     */
    explicit RadialType(const Rect& rect, T min = 0, T max = 100, T value = 0)
        : ValueRangeWidget<T>(rect, min, max, value)
    {
        this->set_boxtype(Theme::boxtype::blank);
        this->flags().set(Widget::flag::grab_mouse);

        this->radial_flags().set({flag::primary_value,
                                  flag::primary_handle,
                                  flag::secondary_value,
                                  flag::text});
    }

    explicit RadialType(T min = 0, T max = 100, T value = 0)
        : RadialType<T>(Rect(), min, max, value)
    {}

    explicit RadialType(Frame& parent, const Rect& rect = {}, T min = 0, T max = 100, T value = 0)
        : RadialType<T>(rect, min, max, value)
    {
        parent.add(*this);
    }

    explicit RadialType(Frame& parent, T min = 0, T max = 100, T value = 0)
        : RadialType<T>(parent, Rect(), min, max, value)
    {}

    /**
     * Get a const ref of the flags.
     */
    inline const flags_type& radial_flags() const { return m_radial_flags; }

    /**
     * Get a modifiable ref of the flags.
     */
    inline flags_type& radial_flags() { return m_radial_flags; }

    /**
     * Get the second value of the radial.
     */
    inline T value2() const
    {
        return m_value2;
    }

    /**
     * Set the second value of the widget.
     */
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
            this->invoke_handlers(eventid::property_changed);
        }
    }

    /**
     * Set the center label text of the dial.
     */
    void text(const std::string& text)
    {
        m_text = text;
    }

    virtual void handle(Event& event) override
    {
        Widget::handle(event);

        switch (event.id())
        {
        case eventid::pointer_click:
        case eventid::pointer_drag:
        {
            auto angle = this->touch_to_degrees(this->display_to_local(event.pointer().point));
            auto v = this->degrees_to_value(angle);
            auto orig = this->set_value(v);
            if (orig != v)
                this->invoke_handlers(eventid::input_property_changed);
        }
        default:
            break;
        }
    }

    virtual void draw(Painter& painter, const Rect& rect) override
    {
        Drawer<RadialType<T>>::draw(*this, painter, rect);
    }

    static void default_draw(RadialType<T>& widget, Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        widget.draw_box(painter, Palette::ColorId::bg, Palette::ColorId::border);

        const auto b = widget.content_area();

        const auto v = widget.value_to_degrees(widget.value());

        auto color1 = widget.color(Palette::ColorId::button_fg, Palette::GroupId::disabled).color();
        auto color2 = widget.color(Palette::ColorId::button_fg).color();
        auto color3 = widget.color(Palette::ColorId::button_bg).color();
        auto color4 = widget.color(Palette::ColorId::button_fg).color();

        float smalldim = std::min(b.width(), b.height());
        float linew = smalldim / 10.;
        float radius = smalldim / 2. - (linew / 2.);
        auto angle1 = detail::to_radians<float>(-90, 0);
        auto angle2 = detail::to_radians<float>(-90, v);

        auto c = b.center();

        // bottom full circle
        painter.set(color1);
        painter.set_line_width(linew);
        painter.draw(Arc(c, radius, 0, 2 * detail::pi<float>()));
        painter.stroke();

        if (widget.radial_flags().is_set(flag::primary_value))
        {
            // value arc
            painter.set(color2);
            painter.set_line_width(linew - (linew / 3));
            painter.draw(Arc(c, radius, angle1, angle2));
            painter.stroke();
        }

        if (widget.radial_flags().is_set(flag::primary_handle))
        {
            // handle
            painter.set(color3);
            painter.set_line_width(linew);
            painter.draw(Arc(c, radius, angle2 - 0.3, angle2));
            painter.stroke();
        }

        if (widget.radial_flags().is_set(flag::secondary_value))
        {
            // secondary value
            float angle3 = detail::to_radians<float>(-90,
                           widget.value_to_degrees(widget.value2()));
            painter.set(color4);
            painter.set_line_width(linew);
            painter.draw(Arc(c, radius, angle3 - 0.01, angle3 + 0.01));
            painter.stroke();
        }

        if (widget.radial_flags().is_set(flag::text))
        {
            if (!widget.m_text.empty())
            {
                painter.set(widget.color(Palette::ColorId::text).color());
                painter.set(Font(72));

                auto size = painter.text_size(widget.m_text);
                Rect target = detail::align_algorithm(size, b, alignmask::center, 0);

                painter.draw(target.point());
                painter.draw(widget.m_text);
            }
        }
        else if (widget.radial_flags().is_set(flag::text_value))
        {
            auto txt = std::to_string(widget.value());

            painter.set(widget.color(Palette::ColorId::text).color());
            painter.set(Font(72));

            auto size = painter.text_size(txt);
            Rect target = detail::align_algorithm(size, b, alignmask::center, 0);

            painter.draw(target.point());
            painter.draw(txt);
        }
    }

    /**
     * Get the current text of the radial.
     */
    virtual const std::string& text() const { return m_text; }

    inline float touch_to_degrees(const Point& point) const
    {
        const auto b = this->content_area();
        const Point c = b.center();
        auto radians = c.angle_to<float>(point);
        float angle = detail::to_degrees(radians);
        if (angle < 0)
            angle = angle + 360.;
        return angle;
    }

    /**
     * Normalize a value to degrees.
     */
    inline float value_to_degrees(T value) const
    {
        float n = (static_cast<float>(value) -
                   static_cast<float>(this->m_min)) /
                  (static_cast<float>(this->m_max) - static_cast<float>(this->m_min));
        return n * 360.;
    }

    /**
     * Normalize degrees to a value.
     */
    inline T degrees_to_value(float degrees) const
    {
        float n = degrees / 360.;
        return (n * (this->m_max - this->m_min)) + this->m_min;
    }

    virtual ~RadialType() = default;

protected:

    /**
     * Center text of the widget.
     */
    std::string m_text;

    /**
     * The second value of the widget.
     */
    T m_value2{};

    /**
     * Radial flags.
     */
    flags_type m_radial_flags;
};

/**
 * Helper type for a default radial.
 */
using Radial = RadialType<int>;

}
}

#endif
