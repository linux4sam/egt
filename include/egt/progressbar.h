/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_PROGRESSBAR_H
#define EGT_PROGRESSBAR_H

/**
 * @file
 * @brief Working with progress meters.
 */

#include <egt/detail/alignment.h>
#include <egt/detail/math.h>
#include <egt/detail/meta.h>
#include <egt/detail/textwidget.h>
#include <egt/frame.h>
#include <egt/painter.h>
#include <egt/valuewidget.h>
#include <string>

namespace egt
{
inline namespace v1
{

/**
 * Displays a progress bar based on a value.
 *
 * @ingroup controls
 */
template<class T = int>
class EGT_API ProgressBar : public ValueRangeWidget<T>
{
public:

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] start Starting value for the range.
     * @param[in] end Ending value in the range.
     * @param[in] value Current value in the range.
     */
    explicit ProgressBar(const Rect& rect = {},
                         T start = {}, T end = 100, T value = {}) noexcept
        : ValueRangeWidget<T>(rect, start, end, value)
    {
        this->fill_flags(Theme::FillFlag::blend);
        this->border(this->theme().default_border());
    }

    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Rectangle for the widget.
     * @param[in] start Starting value for the range.
     * @param[in] end Ending value in the range.
     * @param[in] value Current value in the range.
     */
    explicit ProgressBar(Frame& parent, const Rect& rect = {},
                         T start = {}, T end = 100, T value = {}) noexcept
        : ProgressBar(rect, start, end, value)
    {
        parent.add(*this);
    }

    virtual void draw(Painter& painter, const Rect& rect) override
    {
        Drawer<ProgressBar<T>>::draw(*this, painter, rect);
    }

    /**
     * Default draw method for the ProgressBar.
     */
    static void default_draw(ProgressBar& widget, Painter& painter, const Rect&)
    {
        widget.draw_box(painter, Palette::ColorId::bg, Palette::ColorId::border);

        auto b = widget.content_area();
        auto width = detail::normalize<float>(widget.value(),
                                              widget.start(),
                                              widget.end(), 0, b.width());

        if (width > 0.f)
        {
            widget.theme().draw_box(painter,
                                    Theme::FillFlag::blend,
                                    Rect(b.x(), b.y(), width, b.height()),
                                    Color(),
                                    widget.color(Palette::ColorId::button_bg));
        }

        if (widget.show_label())
        {
            std::string text = std::to_string(widget.value()) + "%";
            auto f = detail::TextWidget::scale_font(Size(b.width() * 0.75,
                                                    b.height() * 0.75),
                                                    text, widget.font());

            painter.set(widget.color(Palette::ColorId::label_text).color());
            painter.set(f);
            auto size = painter.text_size(text);
            auto target = detail::align_algorithm(size, b, AlignFlag::center);
            painter.draw(target.point());
            painter.draw(text);
        }
    }


    using ValueRangeWidget<T>::min_size_hint;

    static const Size DEFAULT_PROGRESSBAR_SIZE;

    virtual Size min_size_hint() const override
    {
        if (!this->m_min_size.empty())
            return this->m_min_size;

        return DEFAULT_PROGRESSBAR_SIZE + Widget::min_size_hint();
    }

    /**
     * Enable/disable showing the label text.
     *
     * @param[in] value When true, the label text is shown.
     */
    virtual void show_label(bool value)
    {
        if (m_show_label != value)
        {
            m_show_label = value;
            this->damage();
        }
    }

    /**
     * Get the show label state.
     */
    inline bool show_label() const { return m_show_label; }

    virtual ~ProgressBar() = default;

protected:
    /**
     * When true, the label text is shown.
     */
    bool m_show_label{true};
};

template <class T>
const Size ProgressBar<T>::DEFAULT_PROGRESSBAR_SIZE{200, 30};

/**
 * Displays a spinning progress meter.
 *
 * @ingroup controls
 */
template <class T = int>
class EGT_API SpinProgress : public ValueRangeWidget<T>
{
public:

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] start Starting value for the range.
     * @param[in] end Ending value in the range.
     * @param[in] value Current value in the range.
     */
    explicit SpinProgress(const Rect& rect = {},
                          T start = 0, T end = 100, T value = 0) noexcept
        : ValueRangeWidget<T>(rect, start, end, value)
    {
        this->fill_flags(Theme::FillFlag::blend);
    }

    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Rectangle for the widget.
     * @param[in] min Starting value for the range.
     * @param[in] max Ending value in the range.
     * @param[in] value Current value in the range.
     */
    explicit SpinProgress(Frame& parent, const Rect& rect = {},
                          T start = 0, T end = 100, T value = 0) noexcept
        : SpinProgress(rect, start, end, value)
    {
        parent.add(*this);
    }


    virtual void draw(Painter& painter, const Rect& rect) override
    {
        Drawer<SpinProgress<T>>::draw(*this, painter, rect);
    }

    /**
     * Default draw method for the SpinProgress.
     */
    static void default_draw(SpinProgress& widget, Painter& painter, const Rect& rect)
    {
        widget.draw_box(painter, Palette::ColorId::bg, Palette::ColorId::border);

        auto b = widget.content_area();

        auto dim = std::min(b.width(), b.height());
        float linew = dim / 10.;
        float radius = dim / 2. - (linew / 2.);
        auto angle1 = detail::to_radians<float>(180, 0);
        auto angle2 = detail::to_radians<float>(180, widget.value() / 100. * 360.);

        painter.line_width(linew);
        painter.set(widget.color(Palette::ColorId::button_fg, Palette::GroupId::disabled).color());
        painter.draw(Arc(widget.center(), radius, 0.0f, 2 * detail::pi<float>()));
        painter.stroke();

        painter.set(widget.color(Palette::ColorId::button_fg).color());
        painter.draw(Arc(widget.center(), radius, angle1, angle2));
        painter.stroke();

        if (widget.show_label())
        {
            std::string text = std::to_string(widget.value());
            auto f = detail::TextWidget::scale_font(Size(dim * 0.75, dim * 0.75), text, widget.font());
            painter.set(f);
            painter.set(widget.color(Palette::ColorId::text).color());
            auto size = painter.text_size(text);
            auto target = detail::align_algorithm(size, b, AlignFlag::center);
            painter.draw(target.point());
            painter.draw(text);
        }
    }
    using ValueRangeWidget<T>::min_size_hint;

    static const Size DEFAULT_SPINPROGRESS_SIZE;

    virtual Size min_size_hint() const override
    {
        if (!this->m_min_size.empty())
            return this->m_min_size;

        return DEFAULT_SPINPROGRESS_SIZE + Widget::min_size_hint();
    }

    /**
    * Enable/disable showing the label text.
    *
    * @param[in] value When true, the label text is shown.
    */
    virtual void show_label(bool value)
    {
        if (m_show_label != value)
        {
            m_show_label = value;
            this->damage();
        }
    }

    /**
     * Get the show label state.
     */
    inline bool show_label() const { return m_show_label; }

    virtual ~SpinProgress() = default;
protected:
    /**
     * When true, the label text is shown.
     */
    bool m_show_label{true};
};

template <class T>
const Size SpinProgress<T>::DEFAULT_SPINPROGRESS_SIZE{100, 100};

/**
 * Displays a level meter based on a value.
 *
 * @image html widget_levelmeter.png
 * @image latex widget_levelmeter.png "widget_levelmeter" width=5cm
 *
 * @ingroup controls
 */
template <class T = int>
class EGT_API LevelMeter : public ValueRangeWidget<T>
{
public:

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] start Starting value for the range.
     * @param[in] end Ending value in the range.
     * @param[in] value Current value in the range.
     */
    explicit LevelMeter(const Rect& rect = {},
                        T start = 0, T end = 100, T value = 0) noexcept
        : ValueRangeWidget<T>(rect, start, end, value)
    {
        this->fill_flags(Theme::FillFlag::blend);
        this->padding(2);
    }


    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Rectangle for the widget.
     * @param[in] start Starting value for the range.
     * @param[in] end Ending value in the range.
     * @param[in] value Current value in the range.
     */
    explicit LevelMeter(Frame& parent, const Rect& rect = {},
                        T start = 0, T end = 100, T value = 0) noexcept
        : LevelMeter(rect, start, end, value)
    {
        parent.add(*this);
    }

    virtual void draw(Painter& painter, const Rect& rect) override
    {
        Drawer<LevelMeter<T>>::draw(*this, painter, rect);
    }

    /**
     * Default draw method for the LevelMeter.
     */
    static void default_draw(LevelMeter<T>& widget, Painter& painter, const Rect& rect)
    {
        widget.draw_box(painter, Palette::ColorId::bg, Palette::ColorId::border);

        const auto b = widget.content_area();

        const auto limit = egt::detail::normalize<float>(widget.value(),
                           widget.start(),
                           widget.end(),
                           widget.num_bars(), 0);
        const auto barheight = b.height() / widget.num_bars();

        for (size_t i = 0; i < widget.num_bars(); i++)
        {
            auto color = widget.color(Palette::ColorId::button_fg, Palette::GroupId::disabled);
            if (i >= limit)
                color = widget.color(Palette::ColorId::button_fg);

            Rect rect(b.x(),  b.y() + i * barheight, b.width(), barheight - widget.padding());

            widget.theme().draw_box(painter,
                                    Theme::FillFlag::blend,
                                    rect,
                                    widget.color(Palette::ColorId::border),
                                    color);
        }
    }

    /**
     * Set the number of bars to show.
     */
    virtual void num_bars(size_t bars)
    {
        m_num_bars = bars;
    }

    /**
     * Get the number of bars to show.
     */
    inline size_t num_bars() const { return m_num_bars; }

    using ValueRangeWidget<T>::min_size_hint;

    static const Size DEFAULT_LEVELMETER_SIZE;

    virtual Size min_size_hint() const override
    {
        if (!this->m_min_size.empty())
            return this->m_min_size;

        return DEFAULT_LEVELMETER_SIZE + Widget::min_size_hint();
    }

    virtual ~LevelMeter() = default;

protected:

    /**
     * The number of bars to display.
     */
    size_t m_num_bars{10};
};

template <class T>
const Size LevelMeter<T>::DEFAULT_LEVELMETER_SIZE{40, 100};

/**
 * Displays an analog meter based on a percentage value.
 *
 * @image html widget_analogmeter.png
 * @image latex widget_analogmeter.png "widget_analogmeter" width=5cm
 *
 * @ingroup controls
 */
template <class T = int>
class EGT_API AnalogMeter : public ValueRangeWidget<T>
{
public:

    /**
     * @param[in] rect Rectangle for the widget.
     */
    explicit AnalogMeter(const Rect& rect = {}) noexcept
        : ValueRangeWidget<T>(rect, 0, 100, 0)
    {
        this->fill_flags(Theme::FillFlag::blend);
    }

    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Rectangle for the widget.
     */
    explicit AnalogMeter(Frame& parent, const Rect& rect = {}) noexcept
        : AnalogMeter(rect)
    {
        parent.add(*this);
    }

    virtual void draw(Painter& painter, const Rect& rect) override
    {
        Drawer<AnalogMeter<T>>::draw(*this, painter, rect);
    }

    /**
     * Default draw method for the AnalogMeter.
     */
    static void default_draw(AnalogMeter<T>& widget, Painter& painter, const Rect& rect)
    {
        static const auto tick_width = 1.0;

        widget.draw_box(painter, Palette::ColorId::bg, Palette::ColorId::border);

        const auto b = widget.content_area();
        painter.line_width(tick_width);
        painter.set(widget.font());
        const auto text_size = painter.text_size("999");

        auto cr = painter.context().get();
        cairo_translate(cr, b.center().x(), b.y() + b.height() - text_size.height());

        const auto dim = std::min(b.width() / 2, b.height());
        const float hw = dim - (text_size.width() * 2.0);

        // ticks and labels
        for (auto tick = 0; tick <= 100; tick += 10)
        {
            const auto xangle = std::cos(detail::pi<float>() * tick * 0.01);
            const auto yangle = std::sin(detail::pi<float>() * tick * 0.01);
            painter.set(widget.color(Palette::ColorId::button_fg,
                                     Palette::GroupId::disabled).color());
            painter.draw(Point(hw * xangle,
                               -hw * yangle),
                         Point((hw + 10.0) * xangle,
                               -(hw + 10.0) * yangle));
            painter.stroke();

            const auto text = std::to_string(tick);
            painter.set(widget.color(Palette::ColorId::text).color());
            const auto size = painter.text_size(text);
            painter.draw(Point(-(hw + 30.0) * xangle - size.width() / 2.0,
                               -(hw + 30.0) * yangle - size.height() / 2.0));
            painter.draw(text);
            painter.stroke();
        }

        // needle
        const auto dest = Point((-hw - 15.0) * std::cos(detail::pi<float>() * widget.value() * 0.01),
                                (-hw - 15.0) * std::sin(detail::pi<float>() * widget.value() * 0.01));

        painter.set(widget.color(Palette::ColorId::button_fg).color());
        painter.line_width(tick_width * 2.0);
        painter.draw(Point(), dest);
        painter.stroke();
        painter.draw(Circle(Point(), 5));
        painter.draw(Circle(dest, 2));
        painter.fill();
    }

    using ValueRangeWidget<T>::min_size_hint;

    static const Size DEFAULT_ANALOGMETER_SIZE;

    virtual Size min_size_hint() const override
    {
        if (!this->m_min_size.empty())
            return this->m_min_size;

        return DEFAULT_ANALOGMETER_SIZE + Widget::min_size_hint();
    }

    virtual ~AnalogMeter() = default;
};

template <class T>
const Size AnalogMeter<T>::DEFAULT_ANALOGMETER_SIZE{200, 100};

}
}

#endif
