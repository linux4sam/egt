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
#include <egt/frame.h>
#include <egt/painter.h>
#include <egt/serialize.h>
#include <egt/textwidget.h>
#include <egt/valuewidget.h>
#include <string>

namespace egt
{
inline namespace v1
{

/**
 * Displays a progress bar based on a value.
 *
 * Typically @ref ProgressBar, @ref ProgressBarF are used as aliases.
 *
 * @ingroup controls
 */
template<class T>
class EGT_API ProgressBarType : public ValueRangeWidget<T>
{
public:

    /**
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] start Starting value for the range.
     * @param[in] end Ending value in the range.
     * @param[in] value Current value in the range.
     */
    explicit ProgressBarType<T>(const Rect& rect = {},
                                T start = {}, T end = 100, T value = {}) noexcept
        : ValueRangeWidget<T>(rect, start, end, value)
    {
        this->name("ProgressBar" + std::to_string(this->m_widgetid));
        this->fill_flags(Theme::FillFlag::blend);
        this->border(this->theme().default_border());
    }

    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] start Starting value for the range.
     * @param[in] end Ending value in the range.
     * @param[in] value Current value in the range.
     */
    explicit ProgressBarType(Frame& parent, const Rect& rect = {},
                             T start = {}, T end = 100, T value = {}) noexcept
        : ProgressBarType(rect, start, end, value)
    {
        parent.add(*this);
    }

    void draw(Painter& painter, const Rect& rect) override
    {
        Drawer<ProgressBarType<T>>::draw(*this, painter, rect);
    }

    /// Default draw method for the widget.
    static void default_draw(ProgressBarType<T>& widget, Painter& painter, const Rect&)
    {
        widget.draw_box(painter, Palette::ColorId::bg, Palette::ColorId::border);

        auto b = widget.content_area();
        auto width = detail::normalize<float>(widget.value(),
                                              widget.starting(),
                                              widget.ending(), 0, b.width());

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
            auto f = TextWidget::scale_font(Size(b.width() * 0.75,
                                                 b.height() * 0.75),
                                            text, widget.font());

            painter.set(widget.color(Palette::ColorId::label_text));
            painter.set(f);
            auto size = painter.text_size(text);
            auto target = detail::align_algorithm(size, b, AlignFlag::center);
            painter.draw(target.point());
            painter.draw(text);
        }
    }

    using ValueRangeWidget<T>::min_size_hint;

    /// Default ProgressBar size.
    static Size default_size();
    /// Change default ProgressBar size.
    static void default_size(const Size& size);

    EGT_NODISCARD Size min_size_hint() const override
    {
        if (!this->m_min_size.empty())
            return this->m_min_size;

        return default_size() + Widget::min_size_hint();
    }

    /**
     * Enable/disable showing the label text.
     *
     * @param[in] value When true, the label text is shown.
     */
    void show_label(bool value)
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
    EGT_NODISCARD bool show_label() const { return m_show_label; }

protected:
    /**
     * When true, the label text is shown.
     */
    bool m_show_label{true};

private:
    /// Default size.
    static Size m_default_size;
};

/**
 * This is a progess bar that can be used to display integer values.
 *
 * @ingroup controls
 */
class EGT_API ProgressBar : public ProgressBarType<int>
{
public:
    using ProgressBarType<int>::ProgressBarType;

    EGT_NODISCARD std::string type() const override
    {
        return "ProgressBar";
    }
};

/**
 * This is a progress bar that can be used to display floating values.
 *
 * @ingroup controls
 */
class EGT_API ProgressBarF : public ProgressBarType<float>
{
public:
    using ProgressBarType<float>::ProgressBarType;

    EGT_NODISCARD std::string type() const override
    {
        return "ProgressBarF";
    }
};

template <class T>
Size ProgressBarType<T>::m_default_size{200, 30};

template <class T>
Size ProgressBarType<T>::default_size()
{
    return ProgressBarType<T>::m_default_size;
}

template <class T>
void ProgressBarType<T>::default_size(const Size& size)
{
    ProgressBarType<T>::default_progressbar_size_value = size;
}

/**
 * Displays a spinning progress meter.
 *
 * Typically @ref SpinProgress, @ref SpinProgressF are used as aliases.
 *
 * @ingroup controls
 */
template <class T>
class EGT_API SpinProgressType : public ValueRangeWidget<T>
{
public:

    /**
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] start Starting value for the range.
     * @param[in] end Ending value in the range.
     * @param[in] value Current value in the range.
     */
    explicit SpinProgressType(const Rect& rect = {},
                              T start = 0, T end = 100, T value = 0) noexcept
        : ValueRangeWidget<T>(rect, start, end, value)
    {
        this->name("SpinProgress" + std::to_string(this->m_widgetid));
        this->fill_flags(Theme::FillFlag::blend);
    }

    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] start Starting value for the range.
     * @param[in] end Ending value in the range.
     * @param[in] value Current value in the range.
     */
    explicit SpinProgressType(Frame& parent, const Rect& rect = {},
                              T start = 0, T end = 100, T value = 0) noexcept
        : SpinProgressType(rect, start, end, value)
    {
        parent.add(*this);
    }


    void draw(Painter& painter, const Rect& rect) override
    {
        Drawer<SpinProgressType<T>>::draw(*this, painter, rect);
    }

    /// Default draw method for the widget.
    static void default_draw(SpinProgressType<T>& widget, Painter& painter, const Rect& rect)
    {
        detail::ignoreparam(rect);

        widget.draw_box(painter, Palette::ColorId::bg, Palette::ColorId::border);

        auto b = widget.content_area();

        auto dim = std::min(b.width(), b.height());
        float linew = dim / 10.0f;
        float radius = dim / 2.0f - (linew / 2.0f);
        auto angle1 = detail::to_radians<float>(180, 0);
        auto angle2 = detail::to_radians<float>(180, widget.value() / 100.0f * 360.0f);

        painter.line_width(linew);
        painter.set(widget.color(Palette::ColorId::button_fg, Palette::GroupId::disabled));
        painter.draw(Arc(widget.center(), radius, 0.0f, 2 * detail::pi<float>()));
        painter.stroke();

        painter.set(widget.color(Palette::ColorId::button_fg));
        painter.draw(Arc(widget.center(), radius, angle1, angle2));
        painter.stroke();

        if (widget.show_label())
        {
            std::string text = std::to_string(widget.value());
            auto f = TextWidget::scale_font(Size(dim * 0.75f, dim * 0.75f), text, widget.font());
            painter.set(f);
            painter.set(widget.color(Palette::ColorId::text));
            auto size = painter.text_size(text);
            auto target = detail::align_algorithm(size, b, AlignFlag::center);
            painter.draw(target.point());
            painter.draw(text);
        }
    }

    /// Default SpinProgress size.
    static Size default_size();
    /// Change default SpinProgress size.
    static void default_size(const Size& size);

    using ValueRangeWidget<T>::min_size_hint;

    EGT_NODISCARD Size min_size_hint() const override
    {
        if (!this->m_min_size.empty())
            return this->m_min_size;

        return default_size() + Widget::min_size_hint();
    }

    /**
     * Enable/disable showing the label text.
     *
     * @param[in] value When true, the label text is shown.
     */
    void show_label(bool value)
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
    EGT_NODISCARD bool show_label() const { return m_show_label; }

    void serialize(Serializer& serializer) const override;

    void deserialize(const std::string& name, const std::string& value,
                     const Serializer::Attributes& attrs) override;

protected:
    /// When true, the label text is shown.
    bool m_show_label{true};

private:
    /// Default size.
    static Size m_default_size;
};

/**
 * Helper type alias.
 * @copybrief SpinProgressType
 * @ingroup controls
 */
using SpinProgress = SpinProgressType<int>;

/**
 * Helper type alias.
 * @copybrief SpinProgressType
 * @ingroup controls
 */
using SpinProgressF = SpinProgressType<float>;

template <class T>
Size SpinProgressType<T>::m_default_size{100, 100};

template <class T>
Size SpinProgressType<T>::default_size()
{
    return SpinProgressType<T>::m_default_size;
}

template <class T>
void SpinProgressType<T>::default_size(const Size& size)
{
    SpinProgressType<T>::m_default_size = size;
}

template <class T>
void SpinProgressType<T>::serialize(Serializer& serializer) const
{
    ValueRangeWidget<T>::serialize(serializer);

    serializer.add_property("show_label", detail::to_string(this->m_show_label));
}

template <class T>
void SpinProgressType<T>::deserialize(const std::string& name, const std::string& value,
                                      const Serializer::Attributes& attrs)
{
    if (name == "show_label")
        m_show_label = detail::from_string(value);
    else
        ValueRangeWidget<T>::deserialize(name, value, attrs);
}

/**
 * Displays a level meter based on a value.
 *
 * Typically @ref LevelMeter, @ref LevelMeterF are used as aliases.
 *
 * @image html widget_levelmeter.png
 * @image latex widget_levelmeter.png "widget_levelmeter" width=5cm
 *
 * @ingroup controls
 */
template <class T>
class EGT_API LevelMeterType : public ValueRangeWidget<T>
{
public:

    /**
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] start Starting value for the range.
     * @param[in] end Ending value in the range.
     * @param[in] value Current value in the range.
     */
    explicit LevelMeterType(const Rect& rect = {},
                            T start = 0, T end = 100, T value = 0) noexcept
        : ValueRangeWidget<T>(rect, start, end, value)
    {
        this->name("LevelMeter" + std::to_string(this->m_widgetid));
        this->fill_flags(Theme::FillFlag::blend);
        this->padding(2);
    }


    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] start Starting value for the range.
     * @param[in] end Ending value in the range.
     * @param[in] value Current value in the range.
     */
    explicit LevelMeterType(Frame& parent, const Rect& rect = {},
                            T start = 0, T end = 100, T value = 0) noexcept
        : LevelMeterType(rect, start, end, value)
    {
        parent.add(*this);
    }

    void draw(Painter& painter, const Rect& rect) override
    {
        Drawer<LevelMeterType<T>>::draw(*this, painter, rect);
    }

    /// Default draw method for the widget.
    static void default_draw(LevelMeterType<T>& widget, Painter& painter, const Rect&)
    {
        widget.draw_box(painter, Palette::ColorId::bg, Palette::ColorId::border);

        const auto b = widget.content_area();

        const auto limit = egt::detail::normalize<float>(widget.value(),
                           widget.starting(),
                           widget.ending(),
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
    void num_bars(size_t bars)
    {
        m_num_bars = bars;
    }

    /**
     * Get the number of bars to show.
     */
    EGT_NODISCARD size_t num_bars() const { return m_num_bars; }

    /// Default LevelMeter size.
    static Size default_size();
    /// Change default LevelMeter size.
    static void default_size(const Size& size);

    using ValueRangeWidget<T>::min_size_hint;

    EGT_NODISCARD Size min_size_hint() const override
    {
        if (!this->m_min_size.empty())
            return this->m_min_size;

        return default_size() + Widget::min_size_hint();
    }

    void serialize(Serializer& serializer) const override;

    void deserialize(const std::string& name, const std::string& value,
                     const Serializer::Attributes& attrs) override;

protected:

    /// The number of bars to display.
    size_t m_num_bars{10};

private:
    /// Default size.
    static Size m_default_size;
};

/**
 * Helper type alias.
 * @copybrief LevelMeterType
 * @ingroup controls
 */
using LevelMeter = LevelMeterType<int>;

/**
 * Helper type alias.
 * @copybrief LevelMeterType
 * @ingroup controls
 */
using LevelMeterF = LevelMeterType<float>;

template <class T>
Size LevelMeterType<T>::m_default_size{40, 100};

template <class T>
Size LevelMeterType<T>::default_size()
{
    return LevelMeterType<T>::m_default_size;
}

template <class T>
void LevelMeterType<T>::default_size(const Size& size)
{
    LevelMeterType<T>::m_default_size = size;
}

template <class T>
void LevelMeterType<T>::serialize(Serializer& serializer) const
{
    ValueRangeWidget<T>::serialize(serializer);

    serializer.add_property("num_bars", std::to_string(this->m_num_bars));
}

template <class T>
void LevelMeterType<T>::deserialize(const std::string& name, const std::string& value,
                                    const Serializer::Attributes& attrs)
{
    if (name == "num_bars")
        m_num_bars = std::stoi(value);
    else
        ValueRangeWidget<T>::deserialize(name, value, attrs);
}

/**
 * Displays an analog meter based on a percentage value.
 *
 * Typically @ref AnalogMeter, @ref AnalogMeterF are used as aliases.
 *
 * @image html widget_analogmeter.png
 * @image latex widget_analogmeter.png "widget_analogmeter" width=5cm
 *
 * @ingroup controls
 */
template <class T>
class EGT_API AnalogMeterType : public ValueRangeWidget<T>
{
public:

    /**
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit AnalogMeterType(const Rect& rect = {}) noexcept
        : ValueRangeWidget<T>(rect, 0, 100, 0)
    {
        this->name("AnalogMeter" + std::to_string(this->m_widgetid));
        this->fill_flags(Theme::FillFlag::blend);
    }

    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit AnalogMeterType(Frame& parent, const Rect& rect = {}) noexcept
        : AnalogMeterType(rect)
    {
        parent.add(*this);
    }

    void draw(Painter& painter, const Rect& rect) override
    {
        Drawer<AnalogMeterType<T>>::draw(*this, painter, rect);
    }

    /// Default draw method for the widget.
    static void default_draw(AnalogMeterType<T>& widget, Painter& painter, const Rect& rect)
    {
        detail::ignoreparam(rect);

        static const auto tick_width = 1.0f;

        widget.draw_box(painter, Palette::ColorId::bg, Palette::ColorId::border);

        const auto b = widget.content_area();
        painter.line_width(tick_width);
        painter.set(widget.font());
        const auto text_size = painter.text_size("999");

        auto cr = painter.context().get();
        cairo_translate(cr, b.center().x(), b.y() + b.height() - text_size.height());

        const auto dim = std::min<DefaultDim>(b.width() / 2, b.height());
        const float hw = dim - (text_size.width() * 2.0);

        // ticks and labels
        for (auto tick = 0; tick <= 100; tick += 10)
        {
            const auto xangle = std::cos(detail::pi<float>() * tick * 0.01f);
            const auto yangle = std::sin(detail::pi<float>() * tick * 0.01f);
            painter.set(widget.color(Palette::ColorId::button_fg,
                                     Palette::GroupId::disabled));
            painter.draw(Point(hw * xangle,
                               -hw * yangle),
                         Point((hw + 10.0f) * xangle,
                               -(hw + 10.0f) * yangle));
            painter.stroke();

            const auto text = std::to_string(tick);
            painter.set(widget.color(Palette::ColorId::text));
            const auto size = painter.text_size(text);
            painter.draw(Point(-(hw + 30.0f) * xangle - size.width() / 2.0f,
                               -(hw + 30.0f) * yangle - size.height() / 2.0f));
            painter.draw(text);
            painter.stroke();
        }

        // needle
        const auto dest = Point((-hw - 15.0f) * std::cos(detail::pi<float>() * widget.value() * 0.01f),
                                (-hw - 15.0f) * std::sin(detail::pi<float>() * widget.value() * 0.01f));

        painter.set(widget.color(Palette::ColorId::button_fg));
        painter.line_width(tick_width * 2.0f);
        painter.draw(Point(), dest);
        painter.stroke();
        painter.draw(Circle(Point(), 5));
        painter.draw(Circle(dest, 2));
        painter.fill();
    }

    /// Default ProgressBar size.
    static Size default_size();
    /// Change default ProgressBar size.
    static void default_size(const Size& size);

    using ValueRangeWidget<T>::min_size_hint;

    EGT_NODISCARD Size min_size_hint() const override
    {
        if (!this->m_min_size.empty())
            return this->m_min_size;

        return default_size() + Widget::min_size_hint();
    }

private:

    /// Default size.
    static Size m_default_size;
};

/**
 * Helper type alias.
 * @copybrief AnalogMeterType
 * @ingroup controls
 */
using AnalogMeter = AnalogMeterType<int>;

/**
 * Helper type alias.
 * @copybrief AnalogMeterType
 * @ingroup controls
 */
using AnalogMeterF = AnalogMeterType<float>;

template <class T>
Size AnalogMeterType<T>::m_default_size{200, 100};

template <class T>
Size AnalogMeterType<T>::default_size()
{
    return AnalogMeterType<T>::m_default_size;
}

template <class T>
void AnalogMeterType<T>::default_size(const Size& size)
{
    AnalogMeterType<T>::default_analogmeter_size_value = size;
}

}
}

#endif
