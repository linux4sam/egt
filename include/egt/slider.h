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

#include <egt/app.h>
#include <egt/detail/alignment.h>
#include <egt/detail/enum.h>
#include <egt/detail/math.h>
#include <egt/detail/meta.h>
#include <egt/detail/screen/composerscreen.h>
#include <egt/flags.h>
#include <egt/frame.h>
#include <egt/imagegroup.h>
#include <egt/painter.h>
#include <egt/serialize.h>
#include <egt/textwidget.h>
#include <egt/valuewidget.h>
#include <egt/widgetflags.h>

namespace egt
{
inline namespace v1
{

/**
 * Base class for SliderType.
 */
struct SliderBase
{
    /// Slider flags.
    enum class SliderFlag
    {
        /// Draw a rectangle handle.
        rectangle_handle = detail::bit(0),

        /// Draw a square handle.
        square_handle = detail::bit(1),

        /// Draw a round handle.
        round_handle = detail::bit(2),

        /// Show range labels.
        show_labels = detail::bit(3),

        /// Show value label.
        show_label = detail::bit(4),

        /**
         * Horizontal slider origin (value start()), is to the left. Vertical is at
         * the botton. Setting this flag will flip this origin.
         */
        inverted = detail::bit(5),

        /// Solid color line.
        consistent_line = detail::bit(6),

        /// Hide line.
        hide_line = detail::bit(7),
    };

    /// Slider flags.
    using SliderFlags = Flags<SliderBase::SliderFlag>;
};

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
class SliderType : protected SliderBase, public ValueRangeWidget<T>
{
public:

    using SliderBase::SliderFlags;
    using SliderBase::SliderFlag;

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
    explicit SliderType(T start = 0, T end = 100, T value = 0,
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

    /**
     * @param[in] props list of widget argument and its properties.
     */
    explicit SliderType(Serializer::Properties& props) noexcept
        : SliderType(props, false)
    {
    }

protected:

    explicit SliderType(Serializer::Properties& props, bool is_derived) noexcept;

public:

    void handle(Event& event) override
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
                const auto diff = event.pointer().point - event.pointer().drag_start;
                if (slider_flags().is_set(SliderFlag::inverted))
                    update_value(to_value(m_start_offset - diff.x()));
                else
                    update_value(to_value(m_start_offset + diff.x()));
            }
            else
            {
                const auto diff = event.pointer().point - event.pointer().drag_start;
                if (slider_flags().is_set(SliderFlag::inverted))
                    update_value(to_value(m_start_offset + diff.y()));
                else
                    update_value(to_value(m_start_offset - diff.y()));
            }
            break;
        default:
            break;
        }
    }

    void draw(Painter& painter, const Rect& rect) override;

    /**
     * Get the image, if any, to draw the slider's handle for the given group.
     *
     * @param[in] group Palette::GroupId to get.
     * @param[in] allow_fallback If true, return the 'normal' image if no image
     *                           is set for the requested group.
     */
    EGT_NODISCARD Image* handle_image(Palette::GroupId group,
                                      bool allow_fallback = false) const;

    /**
     * Set the image used to draw the slider's handle.
     *
     * @param[in] image The image used to draw the handle.
     * @param[in] group Palette::GroupId to set.
     */
    void handle_image(const Image& image,
                      Palette::GroupId group = Palette::GroupId::normal);

    /**
     * Remove the image, if any, used to draw the slider's handle.
     *
     * @param[in] group Palette::GroupId to set.
     */
    void reset_handle_image(Palette::GroupId group = Palette::GroupId::normal);

    /**
     * Set the offset the label.
     *
     * @note This offset is added to the y() coordinate for horizontal sliders
     *       or to the x() coordinate for vertical sliders of the default
     *       position of labels, if any. It may be negative.
     * @param[in] offset
     */
    void label_offset(DefaultDim offset)
    {
        if (detail::change_if_diff<>(m_label_offset, offset))
            this->damage();
    }

    /**
     * Get the offset for the label.
     */
    EGT_NODISCARD DefaultDim label_offset() const { return m_label_offset; }

    /**
     * Set the offset for the handle.
     *
     * @note This offset is added to the y() coordinate for horizontal sliders
     *       or to the x() coordinate for vertical sliders of the default
     *       position of the handle. It may be negative.
     * @param[in] offset
     */
    void handle_offset(DefaultDim offset)
    {
        if (detail::change_if_diff<>(m_handle_offset, offset))
            this->damage();
    }

    /**
     * Get the offset for the handle.
     */
    EGT_NODISCARD DefaultDim handle_offset() const { return m_handle_offset; }

    /**
     * Set the margin for the handle image.
     *
     * @note This margin is the gap between the handle in min/max positions and
     *       the border of the content area. It may be negative to allow the
     *       handle crossing edges of the slider content area.
     * @param[in] margin
     */
    void handle_margin(DefaultDim margin)
    {
        if (detail::change_if_diff<>(m_handle_margin, margin))
            this->damage();
    }

    /**
     * Get the margin for the handle image.
     */
    EGT_NODISCARD DefaultDim handle_margin() const { return m_handle_margin; }

    using ValueRangeWidget<T>::value;

    T value(T value) override
    {
        T orig = this->value();

        update_value(value);

        if (m_invoke_pending)
        {
            m_invoke_pending = false;
            this->on_value_changed.invoke();
        }

        return orig;
    }

    /**
     *  Enable or disable the live update feature.
     *
     *  When enabled, the on_value_changed event happens also during
     *  cursor dragging.
     *
     *  @param[in] enable Enable or disable the live update mode.
     */
    void live_update(bool enable)
    {
        m_live_update = enable;
    }

    /**
     *  get live update value.
     */
    EGT_NODISCARD bool live_update() const { return m_live_update; }

    /**
     * Get the Orientation.
     */
    EGT_NODISCARD Orientation orient() const { return m_orient; }

    /**
     * Set the Orientation.
     *
     * @param[in] orient Vertical or horizontal Orientation.
     */
    void orient(Orientation orient)
    {
        if (detail::change_if_diff<>(m_orient, orient))
            this->damage();
    }

    /// Get the current slider flags.
    EGT_NODISCARD const SliderFlags& slider_flags() const { return m_slider_flags; }

    /// Get the current slider flags.
    SliderFlags& slider_flags() { return m_slider_flags; }

    using ValueRangeWidget<T>::min_size_hint;

    /// Default Slider size.
    static Size default_size();
    /// Change default Slider size.
    static void default_size(const Size& size);

    EGT_NODISCARD Size min_size_hint() const override
    {
        if (!this->m_min_size.empty())
            return this->m_min_size;

        return default_size() + Widget::min_size_hint();
    }

    void serialize(Serializer& serializer) const override;

protected:

    bool internal_drag() const override { return true; }
    bool internal_track_drag() const override { return true; }

    /// Convert a value to an offset.
    EGT_NODISCARD int to_offset(T value) const
    {
        if (detail::float_equal(static_cast<float>(this->m_start), static_cast<float>(this->m_end)))
            return m_handle_margin;

        const auto m = m_handle_margin;
        const auto b = this->content_area();
        if (m_orient == Orientation::horizontal)
            return egt::detail::normalize<float>(value, this->m_start, this->m_end,
                                                 m, b.width() - m - handle_width());
        else
            return egt::detail::normalize<float>(value, this->m_start, this->m_end,
                                                 m, b.height() - m - handle_height());
    }

    /// Convert an offset to value.
    EGT_NODISCARD T to_value(int offset) const
    {
        const auto m = m_handle_margin;
        const auto b = this->content_area();
        if (m_orient == Orientation::horizontal)
            return egt::detail::normalize<float>(offset, m, b.width() - m - handle_width(),
                                                 this->m_start, this->m_end);
        else
            return egt::detail::normalize<float>(offset, m, b.height() - m - handle_height(),
                                                 this->m_start, this->m_end);
    }

    /// Update the value without notifying the handlers.
    void update_value(T value)
    {
        T prev_value = this->m_value;
        if (this->set_value(value))
        {
            auto r = Rect::merge(handle_box(prev_value), handle_box());
            this->damage(r);

            if (slider_flags().is_set(SliderFlag::show_label))
            {
                std::string text;
                auto label_rect = label_box(prev_value, text);
                this->damage(label_rect);

                label_rect = label_box(this->m_value, text);
                this->damage(label_rect);
            }

            if (m_live_update)
            {
                this->on_value_changed.invoke();
            }
            else
                m_invoke_pending = true;
        }
    }

    /// Get the calculated handle width.
    EGT_NODISCARD int handle_width() const;

    /// Get the calculated handle height.
    EGT_NODISCARD int handle_height() const;

    /// Get the handle box for the current value.
    EGT_NODISCARD Rect handle_box() const
    {
        return handle_box(this->m_value);
    }

    /// Get the handle box for the specified value.
    EGT_NODISCARD Rect handle_box(T value) const;

    /// Get the label box and text for the specified value.
    EGT_NODISCARD Rect label_box(T value, std::string& text) const
    {
        const auto b = this->content_area();
        auto handle_rect = handle_box(value);

        if (m_orient == Orientation::horizontal)
            handle_rect += Point(0, m_label_offset - b.height() / 2.);
        else
            handle_rect += Point(m_label_offset - b.width() / 2., 0);

        text = format_label(value);

        const auto text_size = this->font().text_size(text);
        auto target = detail::align_algorithm(text_size,
                                              handle_rect,
                                              AlignFlag::center,
                                              5);

        /// When possible, force the label box within the content area.
        if (target.width() <= b.width())
        {
            if (target.x() < b.x())
                target.x(b.x());

            if (target.x() + target.width() > b.x() + b.width())
                target.x(b.x() + b.width() - target.width());
        }

        if (target.height() <= b.height())
        {
            if (target.y() < b.y())
                target.y(b.y());

            if (target.y() + target.height() > b.y() + b.height())
                target.y(b.y() + b.height() - target.height());
        }

        return target;
    }

    /// Draw the value label.
    void draw_label(Painter& painter, T value)
    {
        std::string text;
        const auto target = label_box(value, text);
        painter.draw(target.point());
        painter.set(this->color(Palette::ColorId::label_text));
        painter.draw(text);
    }

    /// Draw the handle.
    void draw_handle(Painter& painter, const Rect& handle_rect);

    /// Draw the line.
    void draw_line(Painter& painter, const Rect& handle_rect);

    /// Format the label text.
    static std::string format_label(T value)
    {
        return std::to_string(value);
    }

    /// Orientation of the slider.
    Orientation m_orient{Orientation::horizontal};

    /// When true, an invoke of events has been queued to occur.
    bool m_invoke_pending{false};

    /// When true, notify handlers even during drag.
    bool m_live_update{false};

    /// Slider flags.
    SliderFlags m_slider_flags{};

    /// When dragging, the offset at the drag start.
    int m_start_offset{0};

private:
    /// Default size.
    static Size m_default_size;
    static Signal<>::RegisterHandle m_default_size_handle;
    static void register_handler();
    static void unregister_handler();

    /// Optional handle images.
    ImageGroup m_handles{"handle"};
    DefaultDim m_handle_offset{0};
    DefaultDim m_handle_margin{0};

    DefaultDim m_label_offset{0};

    void deserialize(Serializer::Properties& props);
};

/**
 * This is a slider that can be used to select integer value from a range.
 *
 * @ingroup controls
 */
class EGT_API Slider : public SliderType<int>
{
public:
    using SliderType<int>::SliderType;

    EGT_NODISCARD std::string type() const override
    {
        return "Slider";
    }
};

/**
 * This is a slider that can be used to select floating value from a range.
 *
 * @ingroup controls
 */
class EGT_API SliderF : public SliderType<float>
{
public:
    using SliderType<float>::SliderType;

    EGT_NODISCARD std::string type() const override
    {
        return "SliderF";
    }
};

template <class T>
Size SliderType<T>::m_default_size;

template <class T>
Signal<>::RegisterHandle SliderType<T>::m_default_size_handle = Signal<>::INVALID_HANDLE;

template <class T>
void SliderType<T>::register_handler()
{
    if (m_default_size_handle == Signal<>::INVALID_HANDLE)
    {
        m_default_size_handle = detail::ComposerScreen::register_screen_resize_hook([]()
        {
            m_default_size.clear();
        });
    }
}

template <class T>
void SliderType<T>::unregister_handler()
{
    detail::ComposerScreen::unregister_screen_resize_hook(m_default_size_handle);
    m_default_size_handle = Signal<>::INVALID_HANDLE;
}

template <class T>
Size SliderType<T>::default_size()
{
    if (SliderType<T>::m_default_size.empty())
    {
        register_handler();
        auto ss = egt::Application::instance().screen()->size();
        SliderType<T>::m_default_size = Size(ss.width() * 0.20, ss.height() * 0.10);
    }

    return SliderType<T>::m_default_size;
}

template <class T>
void SliderType<T>::default_size(const Size& size)
{
    if (!size.empty())
        unregister_handler();

    SliderType<T>::m_default_size = size;
}

template <class T>
SliderType<T>::SliderType(const Rect& rect, T start, T end, T value,
                          Orientation orient) noexcept
    : ValueRangeWidget<T>(rect, start, end, value),
      m_orient(orient)
{
    this->name("Slider" + std::to_string(this->m_widgetid));
    this->fill_flags(Theme::FillFlag::blend);
    this->grab_mouse(true);
    this->slider_flags().set(SliderFlag::rectangle_handle);
    this->border_radius(4.0);
}

template <class T>
SliderType<T>::SliderType(Serializer::Properties& props, bool is_derived) noexcept
    : ValueRangeWidget<T>(props, true)
{
    deserialize(props);

    if (!is_derived)
        this->deserialize_leaf(props);
}


template <class T>
void SliderType<T>::draw(Painter& painter, const Rect& rect)
{
    Painter::AutoSaveRestore sr(painter);

    if (this->clip())
    {
        painter.draw(rect);
        painter.clip();
    }

    auto handle = handle_box();

    // line
    draw_line(painter, handle);

    // handle
    draw_handle(painter, handle);

    /**
     * draw labels after since they may overlap both the line and handle
     * depending on the value of 'm_label_offset'.
     */
    if (slider_flags().is_set(SliderFlag::show_label))
    {
        draw_label(painter, this->value());
    }
    else if (slider_flags().is_set(SliderFlag::show_labels))
    {
        draw_label(painter, this->starting());
        draw_label(painter, this->starting() + ((this->ending() - this->starting()) / 2));
        draw_label(painter, this->ending());
    }
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
            return std::min<DefaultDim>(width / 6, height);
        else
            return std::min<DefaultDim>(height / 6, width);
    }
    else
    {
        if (m_orient == Orientation::horizontal)
            return std::min<DefaultDim>(width / 6, height) * 2;
        else
            return std::min<DefaultDim>(height / 6, width);
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
            return std::min<DefaultDim>(width / 6, height);
        else
            return std::min<DefaultDim>(height / 6, width);
    }
    else
    {
        if (m_orient == Orientation::horizontal)
            return std::min<DefaultDim>(width / 6, height);
        else
            return std::min<DefaultDim>(height / 6, width) * 2;
    }
}

template <class T>
Rect SliderType<T>::handle_box(T value) const
{
    const auto b = this->content_area();
    const auto dimw = handle_width();
    const auto dimh = handle_height();
    const auto offset = to_offset(value);
    Rect hndl;

    hndl.width(dimw);
    hndl.height(dimh);

    if (m_orient == Orientation::horizontal)
    {
        if (slider_flags().is_set(SliderFlag::inverted))
            hndl.x(b.x() + b.width() - offset - dimw);
        else
            hndl.x(b.x() + offset);

        if (slider_flags().is_set(SliderFlag::show_labels) ||
            slider_flags().is_set(SliderFlag::show_label))
            hndl.y(b.y() + b.height() * 3 / 4 - dimh / 2 + m_handle_offset);
        else
            hndl.y(b.y() + b.height() / 2 - dimh / 2 + m_handle_offset);
    }
    else
    {
        if (slider_flags().is_set(SliderFlag::inverted))
            hndl.y(b.y() + offset);
        else
            hndl.y(b.y() + b.height() - offset - dimh);

        if (slider_flags().is_set(SliderFlag::show_labels) ||
            slider_flags().is_set(SliderFlag::show_label))
            hndl.x(b.x() + b.width() * 3 / 4 - dimw / 2 + m_handle_offset);
        else
            hndl.x(b.x() + b.width() / 2 - dimw / 2 + m_handle_offset);
    }

    return  hndl;
}

template <class T>
void SliderType<T>::draw_handle(Painter& painter, const Rect& handle_rect)
{
    auto* image = handle_image(this->group(), true);
    if (image)
    {
        this->theme().draw_box(painter,
                               Theme::FillFlag::blend,
                               handle_rect,
                               Palette::transparent,
                               Palette::transparent,
                               0, 0, 0, {},
                               image);
    }
    else if (slider_flags().is_set(SliderFlag::round_handle))
    {
        this->theme().draw_circle(painter,
                                  Theme::FillFlag::blend,
                                  handle_rect,
                                  this->color(Palette::ColorId::border),
                                  this->color(Palette::ColorId::button_bg),
                                  this->border());
    }
    else
    {
        this->theme().draw_box(painter,
                               Theme::FillFlag::blend,
                               handle_rect,
                               this->color(Palette::ColorId::border),
                               this->color(Palette::ColorId::button_bg),
                               this->border(),
                               0,
                               this->border_radius());
    }
}

template <class T>
void SliderType<T>::draw_line(Painter& painter, const Rect& handle_rect)
{
    if (slider_flags().is_set(SliderFlag::hide_line))
        return;

    const auto b = this->content_area();
    const auto center = handle_rect.center();
    const auto xp = center.x();
    const auto yp = center.y();

    Point a1;
    Point a2;
    Point b1;
    Point b2;

    if (m_orient == Orientation::horizontal)
    {
        a1 = Point(b.x() + m_handle_margin, yp);
        a2 = Point(handle_rect.x(), yp);
        b1 = Point(handle_rect.x(), yp);
        b2 = Point(b.x() + b.width() - m_handle_margin, yp);

        painter.line_width(handle_rect.height() / 5.0);
    }
    else
    {
        a1 = Point(xp, b.y() + b.height() - m_handle_margin);
        a2 = Point(xp, handle_rect.y());
        b1 = Point(xp, handle_rect.y());
        b2 = Point(xp, b.y() + m_handle_margin);

        painter.line_width(handle_rect.width() / 5.0);
    }

    if (slider_flags().is_set(SliderFlag::inverted))
        std::swap(a1, b2);

    if (slider_flags().is_set(SliderFlag::consistent_line))
    {
        painter.set(this->color(Palette::ColorId::button_fg,
                                Palette::GroupId::disabled));
        painter.draw(a1, b2);
        painter.stroke();
    }
    else
    {
        painter.set(this->color(Palette::ColorId::button_fg));
        painter.draw(a1, a2);
        painter.stroke();
        painter.set(this->color(Palette::ColorId::button_fg,
                                Palette::GroupId::disabled));
        painter.draw(b1, b2);
        painter.stroke();
    }
}

/// Format the value label specialized for float.
template<>
EGT_API std::string SliderType<float>::format_label(float value);

/// Format the value label specialized for double.
template<>
EGT_API std::string SliderType<double>::format_label(double value);

template <class T>
void SliderType<T>::serialize(Serializer& serializer) const
{
    ValueRangeWidget<T>::serialize(serializer);

    serializer.add_property("sliderflags", m_slider_flags.to_string());
    serializer.add_property("orient", detail::enum_to_string(orient()));
    serializer.add_property("label_offset", label_offset());
    serializer.add_property("handle_offset", handle_offset());
    serializer.add_property("handle_margin", handle_margin());
    m_handles.serialize(serializer);
}

template <class T>
void SliderType<T>::deserialize(Serializer::Properties& props)
{
    props.erase(std::remove_if(props.begin(), props.end(), [&](auto & p)
    {
        if (std::get<0>(p) == "sliderflags")
        {
            m_slider_flags.from_string(std::get<1>(p));
            return true;
        }
        else if (std::get<0>(p) == "orient")
        {
            orient(detail::enum_from_string<Orientation>(std::get<1>(p)));
            return true;
        }
        else if (std::get<0>(p) == "label_offset")
        {
            label_offset(std::stoi(std::get<1>(p)));
            return true;
        }
        else if (std::get<0>(p) == "handle_offset")
        {
            handle_offset(std::stoi(std::get<1>(p)));
            return true;
        }
        else if (std::get<0>(p) == "handle_margin")
        {
            handle_margin(std::stoi(std::get<1>(p)));
            return true;
        }
        else if (m_handles.deserialize(std::get<0>(p), std::get<1>(p)))
        {
            return true;
        }
        return false;
    }), props.end());
}

template <class T>
Image* SliderType<T>::handle_image(Palette::GroupId group, bool allow_fallback) const
{
    return m_handles.get(group, allow_fallback);
}

template <class T>
void SliderType<T>::handle_image(const Image& image, Palette::GroupId group)
{
    m_handles.set(group, image);
    if (group == this->group())
        this->damage(handle_box());
}

template <class T>
void SliderType<T>::reset_handle_image(Palette::GroupId group)
{
    auto changed = m_handles.reset(group);
    if (changed && group == this->group())
        this->damage(handle_box());
}

/// Enum string conversion map
template<>
EGT_API const std::pair<SliderBase::SliderFlag, char const*> detail::EnumStrings<SliderBase::SliderFlag>::data[8];

}
}

#endif
