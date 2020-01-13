/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_RADIAL_H
#define EGT_RADIAL_H

#include <algorithm>
#include <egt/detail/flags.h>
#include <egt/detail/math.h>
#include <egt/detail/meta.h>
#include <egt/frame.h>
#include <egt/painter.h>
#include <egt/text.h>
#include <egt/value.h>
#include <egt/widget.h>
#include <memory>
#include <string>
#include <vector>

/**
 * @file
 * @brief Radial widget.
 */

namespace egt
{
inline namespace v1
{
namespace experimental
{

/**
 * Radial widget that draws a series of RangleValues on a circle.
 *
 * @image html widget_radial.png
 * @image latex widget_radial.png "widget_radial" width=5cm
 *
 * @ingroup controls
 */
template<class T>
class RadialType : public Widget
{
public:

    enum class RadialFlag
    {
        /**
         * Use the value for the center text of the widget.
         */
        text_value = detail::bit(0),

        /**
         * When drawing the value, use rounded ends.  The default is square.
         */
        rounded_cap = detail::bit(1),

        /**
         * This value is modified by user input.
         */
        input_value = detail::bit(2),
    };

    using RadialFlags = detail::Flags<RadialFlag>;

    /**
     * @param[in] rect Rectangle for the widget.
     */
    explicit RadialType(const Rect& rect = {})
        : Widget(rect)
    {
        this->name("Radial" + std::to_string(m_widgetid));
        this->flags().set(Widget::Flag::grab_mouse);
    }

    explicit RadialType(Frame& parent, const Rect& rect = {})
        : RadialType<T>(rect)
    {
        parent.add(*this);
    }

    virtual detail::Object::RegisterHandle add(const std::shared_ptr<RangeValue<T>>& range,
            const Color& color = {},
            default_dim_type width = 10,
            RadialFlags flags = {})
    {
        // TODO: m_handle_counter can wrap, making the handle non-unique
        auto handle = ++this->m_handle_counter;
        this->m_values.emplace_back(range, color, width, flags, handle);

        // when a value changes, damage
        range->on_event([this, flags](Event&)
        {
            this->damage();
        }, {EventId::property_changed});

        damage();

        return handle;
    }

    using Widget::color;

    virtual void color(detail::Object::RegisterHandle handle, const Color& color)
    {
        for (auto& value : this->m_values)
        {
            if (value.handle == handle)
            {
                value.color = color;
                this->damage();
                break;
            }
        }
    }

    /**
     * Get the current text of the radial.
     */
    virtual const std::string& text() const { return m_text; }

    /**
     * Set the center label text of the dial.
     */
    virtual void text(const std::string& text)
    {
        if (detail::change_if_diff<>(m_text, text))
            this->damage();
    }

    virtual void handle(Event& event) override
    {
        Widget::handle(event);

        switch (event.id())
        {
        case EventId::pointer_click:
        case EventId::pointer_drag:
        {
            bool changed = false;
            auto angle = this->touch_to_degrees(this->display_to_local(event.pointer().point));

            for (auto& value : this->m_values)
            {
                if (value.flags.is_set(RadialFlag::input_value))
                {
                    auto v = this->degrees_to_value(value.range->min(),
                                                    value.range->max(),
                                                    angle);
                    auto orig = value.range->value(v);
                    if (!changed)
                        changed = (orig != v);
                }
            }

            if (changed)
                this->invoke_handlers(EventId::input_property_changed);
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
        detail::ignoreparam(rect);

        widget.draw_box(painter, Palette::ColorId::bg, Palette::ColorId::border);

        const auto b = widget.content_area();
        const auto c = b.center();
        auto text = widget.text();
        const auto smalldim = std::min(b.width(), b.height());

        default_dim_type maxwidth = 0;
        for (auto& value : widget.m_values)
        {
            if (value.width > maxwidth)
                maxwidth = value.width;
        }

        for (auto& value : widget.m_values)
        {
            const auto radius = smalldim * 0.5f - (maxwidth * 0.5f);
            const auto angle1 = detail::to_radians<float>(-90, widget.start_angle());
            const auto angle2 = detail::to_radians<float>(-90,
                                widget.value_to_degrees(value.range->min(),
                                        value.range->max(),
                                        value.range->value()));

            painter.set(value.color);
            painter.line_width(value.width);
            auto cr = painter.context().get();
            if (value.flags.is_set(RadialFlag::rounded_cap))
                cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
            else
                cairo_set_line_cap(cr, CAIRO_LINE_CAP_BUTT);

            painter.draw(Arc(c, radius, angle1, angle2));
            painter.stroke();

            if (value.flags.is_set(RadialFlag::text_value))
            {
                text = std::to_string(value.range->value());
            }
        }

        if (!text.empty())
        {
            auto target = Rect(Size(smalldim, smalldim));
            target.move_to_center(b.center());
            auto font = detail::TextWidget::scale_font(target.size(), text,
                        widget.font());

            detail::draw_text(painter,
                              widget.content_area(),
                              text,
                              font,
                              {},
                              AlignFlag::center,
                              Justification::middle,
                              widget.color(Palette::ColorId::label_text).color());
        }
    }

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
    inline T value_to_degrees(T min, T max, T value) const
    {
        const auto n = (static_cast<float>(value) -
                        static_cast<float>(min)) /
                       (static_cast<float>(max) - static_cast<float>(min));
        return n * 360.;
    }

    /**
     * Normalize degrees to a value.
     */
    inline T degrees_to_value(T min, T max, T degrees) const
    {
        const auto n = degrees / 360.;
        return (n * (max - min)) + min;
    }

    /**
     * The starting angle in degrees for the min values.
     */
    inline float start_angle() const
    {
        return m_start_angle;
    }

    /**
     * Set the starting angle in degrees for the min values.
     */
    inline void start_angle(float value)
    {
        m_start_angle = value;
    }

    virtual ~RadialType() = default;

protected:

    template<class T2>
    struct ValueData
    {
        ValueData(std::shared_ptr<RangeValue<T2>> r,
                  const Color c,
                  size_t w = 10,
                  RadialFlags f = {},
                  detail::Object::RegisterHandle h = 0) noexcept
            : range(r),
              color(c),
              width(w),
              flags(f),
              handle(h)
        {}

        std::shared_ptr<RangeValue<T2>> range;
        Color color;
        default_dim_type width{};
        RadialFlags flags{};
        detail::Object::RegisterHandle handle{0};
    };

    /**
     * Center text of the widget.
     */
    std::string m_text;

    /**
     * The second value of the widget.
     */
    std::vector<ValueData<T>> m_values;

    /**
     * Counter used to generate unique handles for each handle registration.
     */
    detail::Object::RegisterHandle m_handle_counter{0};

    /**
     * The starting angle in degrees for the min values.
     */
    float m_start_angle{0.f};
};

/**
 * Helper type for a default radial.
 */
using Radial = RadialType<int>;
using RadialF = RadialType<float>;

}
}
}

#endif
