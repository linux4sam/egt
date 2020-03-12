/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SLIDER_H
#define EGT_SLIDER_H

/**
 * @file
 * @brief Working with sliders.
 */

#include <cassert>
#include <egt/detail/alignment.h>
#include <egt/detail/enum.h>
#include <egt/detail/flags.h>
#include <egt/detail/math.h>
#include <egt/detail/meta.h>
#include <egt/detail/serialize.h>
#include <egt/frame.h>
#include <egt/painter.h>
#include <egt/textwidget.h>
#include <egt/valuewidget.h>
#include <egt/widgetflags.h>

namespace egt
{
inline namespace v1
{

/**
 * This is a slider that can be used to select value from a range.
 *
 * Typically @ref Slider, @ref SliderF are used as aliases.
 *
 * @image html widget_slider1.png
 * @image latex widget_slider1.png "widget_slider1" width=5cm
 * @image html widget_slider2.png
 * @image latex widget_slider2.png "widget_slider2" height=5cm
 *
 * @ingroup controls
 */
template <class T>
class SliderType : public ValueRangeWidget<T>
{
public:

    enum class SliderFlag
    {
        /**
         * Draw a rectangle handle.
         */
        rectangle_handle = detail::bit(0),

        /**
         * Draw a square handle.
         */
        square_handle = detail::bit(1),

        /**
         * Draw a round handle.
         */
        round_handle = detail::bit(2),

        /**
         * Show range labels.
         */
        show_labels = detail::bit(3),

        /**
         * Show value label.
         */
        show_label = detail::bit(4),

        /**
         * Horizontal slider origin (value start()), is to the left. Vertical is at
         * the bottom. Setting this flag will flip this origin.
         */
        origin_opposite = detail::bit(5),

        /**
         * Solid color line.
         */
        consistent_line = detail::bit(6),
    };

    using SliderFlags = detail::Flags<SliderType::SliderFlag>;

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] start Starting value for the range.
     * @param[in] end Ending value in the range.
     * @param[in] value Current value in the range.
     * @param[in] orient Vertical or horizontal Orientation.
     */
    explicit SliderType(const Rect& rect, T start = 0, T end = 100, T value = 0,
                        Orientation orient = Orientation::horizontal) noexcept;

    /**
     * @param[in] start Starting value for the range.
     * @param[in] end Ending value in the range.
     * @param[in] value Current value in the range.
     * @param[in] orient Vertical or horizontal Orientation.
     */
    SliderType(T start = 0, T end = 100, T value = 0,
               Orientation orient = Orientation::horizontal) noexcept
        : SliderType(Rect(), start, end, value, orient)
    {
    }

    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Rectangle for the widget.
     * @param[in] start Starting value for the range.
     * @param[in] end Ending value in the range.
     * @param[in] value Current value in the range.
     * @param[in] orient Vertical or horizontal Orientation.
     */
    SliderType(Frame& parent, const Rect& rect, T start = 0, T end = 100, T value = 0,
               Orientation orient = Orientation::horizontal) noexcept
        : SliderType(rect, start, end, value, orient)
    {
        parent.add(*this);
    }

    /**
     * @param[in] parent The parent Frame.
     * @param[in] start Starting value for the range.
     * @param[in] end Ending value in the range.
     * @param[in] value Current value in the range.
     * @param[in] orient Vertical or horizontal Orientation.
     */
    explicit SliderType(Frame& parent, T start = 0, T end = 100, T value = 0,
                        Orientation orient = Orientation::horizontal) noexcept
        : SliderType(Rect(), start, end, value, orient)
    {
        parent.add(*this);
    }

    virtual void handle(Event& event) override
    {
        Widget::handle(event);

        switch (event.id())
        {
        case EventId::raw_pointer_up:
            if (m_invoke_pending)
            {
                m_invoke_pending = false;
                this->on_value_changed.invoke();
            }
            break;
        case EventId::pointer_drag_start:
            m_start_offset = to_offset(this->m_value);
            break;
        case EventId::pointer_drag:
            if (m_orient == Orientation::horizontal)
            {
                auto diff = event.pointer().point - event.pointer().drag_start;
                value(to_value(m_start_offset + diff.x()));
            }
            else
            {
                auto diff = event.pointer().point - event.pointer().drag_start;
                value(to_value(m_start_offset - diff.y()));
            }
            break;
        default:
            break;
        }
    }

    virtual void draw(Painter& painter, const Rect& rect) override;

    using ValueRangeWidget<T>::value;

    virtual T value(T value) override
    {
        T orig = this->value();

        assert(this->m_end > this->m_start);

        if (value > this->m_end)
            value = this->m_end;

        if (value < this->m_start)
            value = this->m_start;

        if (detail::change_if_diff<>(this->m_value, value))
        {
            this->damage();

            // live update to handlers?
            if (false)
            {
                this->on_value_changed.invoke();
            }
            else
                m_invoke_pending = true;
        }

        return orig;
    }

    /**
     * Get the Orientation.
     */
    inline Orientation orient() const { return m_orient; }

    /**
     * Set the Orientation.
     */
    inline void orient(Orientation orient)
    {
        if (detail::change_if_diff<>(m_orient, orient))
            this->damage();
    }

    inline const SliderFlags& slider_flags() const { return m_slider_flags; }

    inline SliderFlags& slider_flags() { return m_slider_flags; }

    virtual void serialize(detail::Serializer& serializer) const override;

    virtual void deserialize(const std::string& name, const std::string& value,
                             const std::map<std::string, std::string>& attrs) override;

    virtual ~SliderType() = default;

protected:

    /// Convert a value to an offset.
    inline int to_offset(int value) const
    {
        const auto b = this->content_area();
        if (m_orient == Orientation::horizontal)
            return egt::detail::normalize<float>(value, this->m_start, this->m_end,
                                                 0, b.width() - handle_width());
        else
            return egt::detail::normalize<float>(value, this->m_start, this->m_end,
                                                 0, b.height() - handle_height());
    }

    /// Convert an offset to value.
    inline T to_value(int offset) const
    {
        const auto b = this->content_area();
        if (m_orient == Orientation::horizontal)
            return egt::detail::normalize<float>(offset, 0, b.width() - handle_width(),
                                                 this->m_start, this->m_end);
        else
            return egt::detail::normalize<float>(offset, 0, b.height() - handle_height(),
                                                 this->m_start, this->m_end);
    }

    int handle_width() const;

    int handle_height() const;

    inline Rect handle_box() const
    {
        return handle_box(this->m_value);
    }

    Rect handle_box(T value) const;

    void draw_label(Painter& painter, T value)
    {
        auto b = this->content_area();
        auto handle = handle_box(value);

        if (m_orient == Orientation::horizontal)
            handle -= Point(0, b.height() / 2.);
        else
            handle -= Point(b.width() / 2., 0);

        auto text = std::to_string(value);
        auto f = TextWidget::scale_font(handle.size(), text, this->font());

        painter.set(this->color(Palette::ColorId::label_text).color());
        painter.set(f);

        auto text_size = painter.text_size(text);
        Rect target = detail::align_algorithm(text_size,
                                              handle,
                                              AlignFlag::center,
                                              5);
        painter.draw(target.point());
        painter.draw(text);
    }

    void draw_handle(Painter& painter);

    void draw_line(Painter& painter, float xp, float yp);

    /**
     * Orientation of the slider.
     */
    Orientation m_orient{Orientation::horizontal};

    /**
     * When true, an invoke of events has been queued to occur.
     */
    bool m_invoke_pending{false};

    /**
     * Slider flags.
     */
    SliderFlags m_slider_flags{};

    /**
     * When dragging, the offset at the drag start.
     */
    int m_start_offset{0};
};

/**
 * Helper type alias.
 * @copybrief SliderType
 * @ingroup controls
 */
using Slider = SliderType<int>;

/**
 * Helper type alias.
 * @copybrief SliderType
 * @ingroup controls
 */
using SliderF = SliderType<float>;

template <class T>
SliderType<T>::SliderType(const Rect& rect, T start, T end, T value,
                          Orientation orient) noexcept
    : ValueRangeWidget<T>(rect, start, end, value),
      m_orient(orient)
{
    this->name("Slider" + std::to_string(this->m_widgetid));
    this->fill_flags(Theme::FillFlag::blend);
    this->flags().set(Widget::Flag::grab_mouse);
    this->slider_flags().set(SliderFlag::rectangle_handle);
}

template <class T>
void SliderType<T>::draw(Painter& painter, const Rect& rect)
{
    auto b = this->content_area();
    auto yp = b.y() + b.height() / 2.;
    auto xp = b.x() + b.width() / 2.;

    if (m_orient == Orientation::horizontal)
    {
        if (slider_flags().is_set(SliderFlag::show_labels) ||
            slider_flags().is_set(SliderFlag::show_label))
        {
            yp += b.height() / 4.;

            if (slider_flags().is_set(SliderFlag::show_label))
            {
                draw_label(painter, this->value());
            }
            else
            {
                draw_label(painter, this->start());
                draw_label(painter, this->start() + ((this->end() - this->start()) / 2));
                draw_label(painter, this->end());
            }
        }
    }
    else
    {
        if (slider_flags().is_set(SliderFlag::show_labels) ||
            slider_flags().is_set(SliderFlag::show_label))
        {
            xp += b.width() / 4.;

            if (slider_flags().is_set(SliderFlag::show_label))
            {
                draw_label(painter, this->value());
            }
            else
            {
                draw_label(painter, this->start());
                draw_label(painter, this->start() + ((this->end() - this->start()) / 2));
                draw_label(painter, this->end());
            }
        }

    }

    // line
    draw_line(painter, xp, yp);

    // handle
    draw_handle(painter);
}

template <class T>
int SliderType<T>::handle_width() const
{
    const auto b = this->content_area();
    auto width = b.width();
    auto height = b.height();

    if (slider_flags().is_set(SliderFlag::show_labels) ||
        slider_flags().is_set(SliderFlag::show_label))
    {
        if (m_orient == Orientation::horizontal)
            height /= 2;
        else
            width /= 2;
    }

    if (slider_flags().is_set(SliderFlag::square_handle) ||
        slider_flags().is_set(SliderFlag::round_handle))
    {
        if (m_orient == Orientation::horizontal)
            return std::min(width / 6, height);
        else
            return std::min(height / 6, width);
    }
    else
    {
        if (m_orient == Orientation::horizontal)
            return std::min(width / 6, height) * 2;
        else
            return std::min(height / 6, width);
    }
}

template <class T>
int SliderType<T>::handle_height() const
{
    const auto b = this->content_area();
    auto width = b.width();
    auto height = b.height();

    if (slider_flags().is_set(SliderFlag::show_labels) ||
        slider_flags().is_set(SliderFlag::show_label))
    {
        if (m_orient == Orientation::horizontal)
            height /= 2;
        else
            width /= 2;
    }

    if (slider_flags().is_set(SliderFlag::square_handle) ||
        slider_flags().is_set(SliderFlag::round_handle))
    {
        if (m_orient == Orientation::horizontal)
            return std::min(width / 6, height);
        else
            return std::min(height / 6, width);
    }
    else
    {
        if (m_orient == Orientation::horizontal)
            return std::min(width / 6, height);
        else
            return std::min(height / 6, width) * 2;
    }
}

template <class T>
Rect SliderType<T>::handle_box(T value) const
{
    const auto b = this->content_area();
    const auto dimw = handle_width();
    const auto dimh = handle_height();

    if (m_orient == Orientation::horizontal)
    {
        auto xv = b.x() + to_offset(value);
        if (slider_flags().is_set(SliderFlag::origin_opposite))
            xv = b.x() + b.width() - to_offset(value) - dimw;

        if (slider_flags().is_set(SliderFlag::show_labels) ||
            slider_flags().is_set(SliderFlag::show_label))
        {
            return {xv,
                    b.y() + b.height() / 4 - dimh / 2 + b.height() / 2,
                    dimw,
                    dimh};
        }
        else
        {
            return {xv,
                    b.y() + b.height() / 2 - dimh / 2,
                    dimw,
                    dimh};
        }
    }
    else
    {
        auto yv = b.y() + b.height() - to_offset(value) - dimh;
        if (slider_flags().is_set(SliderFlag::origin_opposite))
            yv = b.y() + to_offset(value);

        if (slider_flags().is_set(SliderFlag::show_labels) ||
            slider_flags().is_set(SliderFlag::show_label))
        {
            return {b.x() + b.width() / 4 - dimw / 2 + b.width() / 2,
                    yv,
                    dimw,
                    dimh};
        }
        else
        {
            return {b.x() + b.width() / 2 - dimw / 2,
                    yv,
                    dimw,
                    dimh};
        }
    }
}

template <class T>
void SliderType<T>::draw_handle(Painter& painter)
{
    const auto handle = handle_box();

    if (slider_flags().is_set(SliderFlag::round_handle))
    {
        this->theme().draw_circle(painter,
                                  Theme::FillFlag::blend,
                                  handle,
                                  this->color(Palette::ColorId::border),
                                  this->color(Palette::ColorId::button_bg),
                                  this->border());
    }
    else
    {
        this->theme().draw_box(painter,
                               Theme::FillFlag::blend,
                               handle,
                               this->color(Palette::ColorId::border),
                               this->color(Palette::ColorId::button_bg),
                               this->border(),
                               0,
                               this->border_radius());
    }
}

template <class T>
void SliderType<T>::draw_line(Painter& painter, float xp, float yp)
{
    const auto b = this->content_area();
    const auto handle = handle_box();

    Point a1;
    Point a2;
    Point b1;
    Point b2;

    if (m_orient == Orientation::horizontal)
    {
        a1 = Point(b.x(), yp);
        a2 = Point(handle.x(), yp);
        b1 = Point(handle.x(), yp);
        b2 = Point(b.x() + b.width(), yp);

        painter.line_width(handle.height() / 5.0);
    }
    else
    {
        a1 = Point(xp, b.y() + b.height());
        a2 = Point(xp, handle.y());
        b1 = Point(xp, handle.y());
        b2 = Point(xp, b.y());

        painter.line_width(handle.width() / 5.0);
    }

    if (slider_flags().is_set(SliderFlag::consistent_line))
    {
        painter.set(this->color(Palette::ColorId::button_fg,
                                Palette::GroupId::disabled).color());
        painter.draw(a1, b2);
        painter.stroke();
    }
    else
    {
        painter.set(this->color(Palette::ColorId::button_fg).color());
        painter.draw(a1, a2);
        painter.stroke();
        painter.set(this->color(Palette::ColorId::button_fg,
                                Palette::GroupId::disabled).color());
        painter.draw(b1, b2);
        painter.stroke();
    }
}

template <class T>
void SliderType<T>::serialize(detail::Serializer& serializer) const
{
    ValueRangeWidget<T>::serialize(serializer);

    serializer.add_property("sliderflags", m_slider_flags.to_string());
    serializer.add_property("orient", detail::enum_to_string(orient()));
    serializer.add_property("start", this->start());
    serializer.add_property("end", this->end());
    serializer.add_property("value", this->value());
}

template <class T>
void SliderType<T>::deserialize(const std::string& name, const std::string& value,
                                const std::map<std::string, std::string>& attrs)
{
    if (name == "sliderflags")
        m_slider_flags.from_string(value);
    else if (name == "orient")
        orient(detail::enum_from_string<Orientation>(value));
    else if (name == "start")
        this->start(std::stoi(value));
    else if (name == "end")
        this->end(std::stoi(value));
    else if (name == "value")
        this->value(std::stoi(value));
    else
        ValueRangeWidget<T>::deserialize(name, value, attrs);
}

template<>
EGT_API const std::pair<Slider::SliderFlag, char const*> detail::EnumStrings<Slider::SliderFlag>::data[7];

}
}

#endif
