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
#include <egt/canvas.h>
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
            return 0;

        const auto b = this->content_area();
        if (m_orient == Orientation::horizontal)
            return egt::detail::normalize<float>(value, this->m_start, this->m_end,
                                                 0, b.width() - handle_width());
        else
            return egt::detail::normalize<float>(value, this->m_start, this->m_end,
                                                 0, b.height() - handle_height());
    }

    /// Convert an offset to value.
    EGT_NODISCARD T to_value(int offset) const
    {
        const auto b = this->content_area();
        if (m_orient == Orientation::horizontal)
            return egt::detail::normalize<float>(offset, 0, b.width() - handle_width(),
                                                 this->m_start, this->m_end);
        else
            return egt::detail::normalize<float>(offset, 0, b.height() - handle_height(),
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
                Canvas canvas(Size(100, 100));
                Painter painter(canvas.context());

                auto label_rect = label_box(painter, prev_value, text);
                this->damage(label_rect);

                label_rect = label_box(painter, this->m_value, text);
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
    EGT_NODISCARD Rect label_box(Painter& painter, T value, std::string& text) const
    {
        const auto b = this->content_area();
        auto handle_rect = handle_box(value);

        if (m_orient == Orientation::horizontal)
            handle_rect -= Point(0, b.height() / 2.);
        else
            handle_rect -= Point(b.width() / 2., 0);

        text = format_label(value);

        painter.set(this->color(Palette::ColorId::label_text));
        painter.set(this->font());

        const auto text_size = painter.text_size(text);
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
        const auto target = label_box(painter, value, text);
        painter.draw(target.point());
        painter.draw(text);
    }

    /// Draw the handle.
    void draw_handle(Painter& painter);

    /// Draw the line.
    void draw_line(Painter& painter, float xp, float yp);

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
void SliderType<T>::draw(Painter& painter, const Rect& /*rect*/)
{
    auto b = this->content_area();
    auto yp = b.y() + b.height() / 2.;
    auto xp = b.x() + b.width() / 2.;

    if (slider_flags().is_set(SliderFlag::show_labels) ||
        slider_flags().is_set(SliderFlag::show_label))
    {
        if (m_orient == Orientation::horizontal)
            yp += b.height() / 4.;
        else
            xp += b.width() / 4.;

        if (slider_flags().is_set(SliderFlag::show_label))
        {
            draw_label(painter, this->value());
        }
        else
        {
            draw_label(painter, this->starting());
            draw_label(painter, this->starting() + ((this->ending() - this->starting()) / 2));
            draw_label(painter, this->ending());
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

    if (m_orient == Orientation::horizontal)
    {
        auto xv = b.x() + to_offset(value);
        if (slider_flags().is_set(SliderFlag::inverted))
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
        if (slider_flags().is_set(SliderFlag::inverted))
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
    const auto handle_rect = handle_box();

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
void SliderType<T>::draw_line(Painter& painter, float xp, float yp)
{
    const auto b = this->content_area();
    const auto handle_rect = handle_box();

    Point a1;
    Point a2;
    Point b1;
    Point b2;

    if (m_orient == Orientation::horizontal)
    {
        a1 = Point(b.x(), yp);
        a2 = Point(handle_rect.x(), yp);
        b1 = Point(handle_rect.x(), yp);
        b2 = Point(b.x() + b.width(), yp);

        painter.line_width(handle_rect.height() / 5.0);
    }
    else
    {
        a1 = Point(xp, b.y() + b.height());
        a2 = Point(xp, handle_rect.y());
        b1 = Point(xp, handle_rect.y());
        b2 = Point(xp, b.y());

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
EGT_API const std::pair<SliderBase::SliderFlag, char const*> detail::EnumStrings<SliderBase::SliderFlag>::data[7];

}
}

#endif
