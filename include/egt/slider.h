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

    enum class SliderLineTransition
    {
        to_end,
        to_begin,
        to_center,
    };
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
        {
            this->damage();
            this->layout();
        }
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
        {
            this->damage();
            this->layout();
        }
    }

    /**
     * Get the line transition.
     */
    EGT_NODISCARD SliderLineTransition line_transition() const { return m_line_transition; }

    /**
     * Set the line transition.
     *
     * @param[in] transition The line transition.
     */
    void line_transition(SliderLineTransition transition)
    {
        if (detail::change_if_diff<>(m_line_transition, transition))
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

        auto* image = this->background(Palette::GroupId::normal);
        Rect rect(image ? image->size() : default_size());

        image = handle_image(Palette::GroupId::normal);
        if (image)
        {
            Rect r(image->size());
            r.move_to_center(rect.center());
            if (m_orient == Orientation::horizontal)
                r.y(r.y() + m_handle_offset);
            else
                r.x(r.x() + m_handle_offset);
            rect = Rect::merge(rect, r);
        }

        return rect.size() + Widget::min_size_hint();
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

        auto* image = handle_image(Palette::GroupId::normal);
        if (m_orient == Orientation::horizontal)
        {
            auto shift = image ? 0 : (b.height() / 2);
            handle_rect += Point(0, m_label_offset - shift);
        }
        else
        {
            auto shift = image ? 0 : (b.width() / 2);
            handle_rect += Point(m_label_offset - shift, 0);
        }

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

    void compute_boxes(T value, Rect* background, Rect* handle) const;

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

    /// Line transition.
    SliderLineTransition m_line_transition{SliderLineTransition::to_end};

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

    painter.alpha_blending(true);

    Rect background, handle;
    compute_boxes(this->value(), &background, &handle);

    auto* bg = this->background(true);
    if (bg)
        painter.draw(*bg, background.point(), background);

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
    auto* image = handle_image(Palette::GroupId::normal);
    if (image)
        return image->width();

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
    auto* image = handle_image(Palette::GroupId::normal);
    if (image)
        return image->height();

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
void SliderType<T>::compute_boxes(T value,
                                  Rect* background,
                                  Rect* handle) const
{
    const auto b = this->content_area();
    const auto dimw = handle_width();
    const auto dimh = handle_height();
    const auto offset = to_offset(value);
    Rect bg, hndl;

    auto* image_bg = this->background(Palette::GroupId::normal);
    bg.size(image_bg ? image_bg->size() : b.size());

    auto* image_hndl = handle_image(Palette::GroupId::normal);
    hndl.width(dimw);
    hndl.height(dimh);

    if (m_orient == Orientation::horizontal)
    {
        auto deltah = (bg.height() - dimh) / 2 + m_handle_offset;

        bg.x(b.x() + (b.width() - bg.width()) / 2);
        bg.y(b.y() - std::min(0, deltah));

        if (slider_flags().is_set(SliderFlag::inverted))
            hndl.x(b.x() + b.width() - offset - dimw);
        else
            hndl.x(b.x() + offset);

        if (image_hndl)
            hndl.y(b.y() + std::max(0, deltah));
        else if (slider_flags().is_set(SliderFlag::show_labels) ||
                 slider_flags().is_set(SliderFlag::show_label))
            hndl.y(b.y() + b.height() * 3 / 4 - dimh / 2 + m_handle_offset);
        else
            hndl.y(b.y() + b.height() / 2 - dimh / 2 + m_handle_offset);
    }
    else
    {
        auto deltaw = (bg.width() - dimw) / 2 + m_handle_offset;

        bg.y(b.y() + (b.height() - bg.height()) / 2);
        bg.x(b.x() - std::min(0, deltaw));

        if (slider_flags().is_set(SliderFlag::inverted))
            hndl.y(b.y() + offset);
        else
            hndl.y(b.y() + b.height() - offset - dimh);

        if (image_hndl)
            hndl.x(b.x() + std::max(0, deltaw));
        else if (slider_flags().is_set(SliderFlag::show_labels) ||
                 slider_flags().is_set(SliderFlag::show_label))
            hndl.x(b.x() + b.width() * 3 / 4 - dimw / 2 + m_handle_offset);
        else
            hndl.x(b.x() + b.width() / 2 - dimw / 2 + m_handle_offset);
    }

    if (background)
        *background = bg;

    if (handle)
        *handle = hndl;
}

template <class T>
Rect SliderType<T>::handle_box(T value) const
{
    Rect handle;
    compute_boxes(value, nullptr, &handle);
    return handle;
}

template <class T>
void SliderType<T>::draw_handle(Painter& painter, const Rect& handle_rect)
{
    auto* image = handle_image(this->group(), true);
    if (image)
    {
        painter.draw(*image, handle_rect.point(), handle_rect);
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

    const auto inverted = !!slider_flags().is_set(SliderFlag::inverted);
    const auto b = this->content_area();
    const auto center = handle_rect.center();
    Point p1, p2;
    Size s1, s2, s;

    if (m_orient == Orientation::horizontal)
    {
        const auto w = handle_rect.width() / 2;
        auto transition_x = center.x();
        switch (line_transition())
        {
        case SliderLineTransition::to_end:
            transition_x += inverted ? -w : w;
            break;
        case SliderLineTransition::to_begin:
            transition_x -= inverted ? -w : w;
            break;
        default:
            break;
        }

        p1.x(b.x() + m_handle_margin);
        s1.width(transition_x - p1.x());
        s1.height(handle_rect.height() / 5);
        p1.y(center.y() - s1.height() / 2);

        p2.x(transition_x);
        s2.width(b.x() + b.width() - m_handle_margin - p2.x());
        s2.height(s1.height());
        p2.y(p1.y());

        s.width(s1.width() + s2.width());
        s.height(s1.height());
    }
    else
    {
        const auto h = handle_rect.height() / 2;
        auto transition_y = center.y();
        switch (line_transition())
        {
        case SliderLineTransition::to_end:
            transition_y -= inverted ? -h : h;
            break;
        case SliderLineTransition::to_begin:
            transition_y += inverted ? -h : h;
            break;
        default:
            break;
        }

        p1.y(b.y() + m_handle_margin);
        s1.height(transition_y - p1.y());
        s1.width(handle_rect.width() / 5);
        p1.x(center.x() - s1.width() / 2);

        p2.y(transition_y);
        s2.height(b.y() + b.height() - m_handle_margin - p2.y());
        s2.width(s1.width());
        p2.x(p1.x());

        s.height(s1.height() + s2.height());
        s.width(s1.width());
    }

    if (slider_flags().is_set(SliderFlag::consistent_line))
    {
        painter.draw(this->color(Palette::ColorId::button_fg,
                                 Palette::GroupId::disabled),
                     Rect(p1, s));
    }
    else
    {
        auto c1 = this->color(Palette::ColorId::button_fg);
        auto c2 = this->color(Palette::ColorId::button_fg,
                              Palette::GroupId::disabled);

        if (inverted ^ (m_orient == Orientation::vertical))
            std::swap(c1, c2);

        const Rect r1(p1, s1);
        if (!r1.empty())
            painter.draw(c1, r1);

        const Rect r2(p2, s2);
        if (!r2.empty())
            painter.draw(c2, r2);
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
    serializer.add_property("line_transition", detail::enum_to_string(line_transition()));
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
        else if (std::get<0>(p) == "line_transition")
        {
            line_transition(detail::enum_from_string<SliderLineTransition>(std::get<1>(p)));
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
    this->damage();
    this->layout();
}

template <class T>
void SliderType<T>::reset_handle_image(Palette::GroupId group)
{
    if (m_handles.reset(group))
    {
        this->damage();
        this->layout();
    }
}

/// Enum string conversion map
template<>
EGT_API const std::pair<SliderBase::SliderFlag, char const*> detail::EnumStrings<SliderBase::SliderFlag>::data[8];

template<>
EGT_API const std::pair<SliderBase::SliderLineTransition, char const*> detail::EnumStrings<SliderBase::SliderLineTransition>::data[3];

}
}

#endif
