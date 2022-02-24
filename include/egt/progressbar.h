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
#include <egt/app.h>
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

    /**
     * @param[in] props list of widget argument and its properties.
     */
    explicit ProgressBarType(Serializer::Properties& props) noexcept
        : ProgressBarType(props, false)
    {
    }

protected:

    explicit ProgressBarType(Serializer::Properties& props, bool is_derived) noexcept
        : ValueRangeWidget<T>(props, true)
    {
        this->name("ProgressBar" + std::to_string(this->m_widgetid));
        this->fill_flags(Theme::FillFlag::blend);
        this->border(this->theme().default_border());

        deserialize(props);

        if (!is_derived)
            this->deserialize_leaf(props);
    }

public:

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

        if ((width > 0.0f) && b.height())
        {
            widget.theme().draw_box(painter,
                                    Theme::FillFlag::blend,
                                    Rect(b.x(), b.y(), width, b.height()),
                                    Color(),
                                    widget.color(Palette::ColorId::button_bg));
        }

        if (widget.show_label())
        {
            std::string text = std::to_string(widget.value());
            if (widget.show_percentage())
            {
                auto percentage = detail::normalize<float>(widget.value(),
                                  widget.starting(),
                                  widget.ending(), 0, 100);
                text = std::to_string(static_cast<int>(percentage)) + "%";
            }
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

    /**
     * Enable/disable showing the label text in percentage.
     *
     * @param[in] value When true, the label text is shown in percentage
     */
    void show_percentage(bool enable)
    {
        if (m_show_percentage != enable)
        {
            m_show_percentage = enable;
            this->damage();
        }
    }

    /**
     * Get the show label in percentage.
     */
    EGT_NODISCARD bool show_percentage() const { return m_show_percentage; }

    void serialize(Serializer& serializer) const override;

protected:
    /**
     * When true, the label text is shown.
     */
    bool m_show_label{true};

    /**
     * When true, the value is displayed in percentage.
     */
    bool m_show_percentage{false};

private:
    /// Default size.
    static Size m_default_size;

    void deserialize(Serializer::Properties& props);
};

template <class T>
void ProgressBarType<T>::serialize(Serializer& serializer) const
{
    ValueRangeWidget<T>::serialize(serializer);

    serializer.add_property("show_label", detail::to_string(this->m_show_label));
}

template <class T>
void ProgressBarType<T>::deserialize(Serializer::Properties& props)
{
    props.erase(std::remove_if(props.begin(), props.end(), [&](auto & p)
    {
        if (std::get<0>(p) == "show_label")
        {
            m_show_label = detail::from_string(std::get<1>(p));
            return true;
        }
        return false;
    }), props.end());
}

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
Size ProgressBarType<T>::m_default_size;

template <class T>
Size ProgressBarType<T>::default_size()
{
    if (ProgressBarType<T>::m_default_size.empty())
    {
        auto ss = egt::Application::instance().screen()->size();
        ProgressBarType<T>::m_default_size = Size(ss.width() * 0.25, ss.height() * 0.05);
    }

    return ProgressBarType<T>::m_default_size;
}

template <class T>
void ProgressBarType<T>::default_size(const Size& size)
{
    ProgressBarType<T>::m_default_size = size;
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

    /**
     * @param[in] props list of widget argument and its properties.
     */
    explicit SpinProgressType(Serializer::Properties& props) noexcept
        : SpinProgressType(props, false)
    {
    }

protected:

    explicit SpinProgressType(Serializer::Properties& props, bool is_derived) noexcept
        : ValueRangeWidget<T>(props, true)
    {
        this->name("SpinProgress" + std::to_string(this->m_widgetid));
        this->fill_flags(Theme::FillFlag::blend);

        deserialize(props);

        if (!is_derived)
            this->deserialize_leaf(props);
    }

public:

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

protected:
    /// When true, the label text is shown.
    bool m_show_label{true};

private:
    /// Default size.
    static Size m_default_size;

    void deserialize(Serializer::Properties& props);
};

/**
 * This is a spinning progress meter that can be used to display integer values.
 *
 * @ingroup controls
 */
class EGT_API SpinProgress : public SpinProgressType<int>
{
public:
    using SpinProgressType<int>::SpinProgressType;

    EGT_NODISCARD std::string type() const override
    {
        return "SpinProgress";
    }
};

/**
 * This is a spinning progress meter that can be used to display floating values.
 *
 * @ingroup controls
 */
class EGT_API SpinProgressF : public SpinProgressType<float>
{
public:
    using SpinProgressType<float>::SpinProgressType;

    EGT_NODISCARD std::string type() const override
    {
        return "SpinProgressF";
    }
};

template <class T>
Size SpinProgressType<T>::m_default_size;

template <class T>
Size SpinProgressType<T>::default_size()
{
    if (SpinProgressType<T>::m_default_size.empty())
    {
        auto ss = egt::Application::instance().screen()->size();
        SpinProgressType<T>::m_default_size = Size(ss.width() * 0.12, ss.height() * 0.20);
    }
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
void SpinProgressType<T>::deserialize(Serializer::Properties& props)
{
    props.erase(std::remove_if(props.begin(), props.end(), [&](auto & p)
    {
        if (std::get<0>(p) == "show_label")
        {
            m_show_label = detail::from_string(std::get<1>(p));
            return true;
        }
        return false;
    }), props.end());
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

    /**
     * @param[in] props list of widget argument and its properties.
     */
    explicit LevelMeterType(Serializer::Properties& props) noexcept
        : LevelMeterType(props, false)
    {
    }

protected:

    explicit LevelMeterType(Serializer::Properties& props, bool is_derived) noexcept
        : ValueRangeWidget<T>(props, true)
    {
        this->name("LevelMeter" + std::to_string(this->m_widgetid));
        this->fill_flags(Theme::FillFlag::blend);
        this->padding(2);

        deserialize(props);

        if (!is_derived)
            this->deserialize_leaf(props);
    }

public:

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

protected:

    /// The number of bars to display.
    size_t m_num_bars{10};

private:
    /// Default size.
    static Size m_default_size;

    void deserialize(Serializer::Properties& props);
};

/**
 * This is a level meter that can be used to display integer values.
 *
 * @ingroup controls
 */
class EGT_API LevelMeter : public LevelMeterType<int>
{
public:
    using LevelMeterType<int>::LevelMeterType;

    EGT_NODISCARD std::string type() const override
    {
        return "LevelMeter";
    }
};

/**
 * This is a level meter that can be used to display floating values.
 *
 * @ingroup controls
 */
class EGT_API LevelMeterF : public LevelMeterType<float>
{
public:
    using LevelMeterType<float>::LevelMeterType;

    EGT_NODISCARD std::string type() const override
    {
        return "LevelMeterF";
    }
};

template <class T>
Size LevelMeterType<T>::m_default_size;

template <class T>
Size LevelMeterType<T>::default_size()
{
    if (LevelMeterType<T>::m_default_size.empty())
    {
        auto ss = egt::Application::instance().screen()->size();
        LevelMeterType<T>::m_default_size = Size(ss.width() * 0.05, ss.height() * 0.20);
    }
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
void LevelMeterType<T>::deserialize(Serializer::Properties& props)
{
    props.erase(std::remove_if(props.begin(), props.end(), [&](auto & p)
    {
        if (std::get<0>(p) == "num_bars")
        {
            m_num_bars = std::stoi(std::get<1>(p));
            return true;
        }
        return false;
    }), props.end());
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

    /**
     * @param[in] props list of widget argument and its properties.
     */
    explicit AnalogMeterType(Serializer::Properties& props) noexcept
        : AnalogMeterType(props, false)
    {
    }

protected:

    explicit AnalogMeterType(Serializer::Properties& props, bool is_derived) noexcept
        : ValueRangeWidget<T>(props, true)
    {
        this->name("AnalogMeter" + std::to_string(this->m_widgetid));
        this->fill_flags(Theme::FillFlag::blend);

        if (!is_derived)
            this->deserialize_leaf(props);
    }

public:

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

            int text = static_cast<int>(detail::normalize<float>(tick, 0, 100, widget.starting(), widget.ending()));
            painter.set(widget.color(Palette::ColorId::text));
            const auto size = painter.text_size(std::to_string(text));
            painter.draw(Point(-(hw + 30.0f) * xangle - size.width() / 2.0f,
                               -(hw + 30.0f) * yangle - size.height() / 2.0f));
            painter.draw(std::to_string(text));
            painter.stroke();
        }

        // needle
        auto avalue = detail::normalize<float>(widget.value(), widget.starting(), widget.ending(), 0, 100);
        const auto dest = Point((-hw - 15.0f) * std::cos(detail::pi<float>() * avalue * 0.01f),
                                (-hw - 15.0f) * std::sin(detail::pi<float>() * avalue * 0.01f));

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
 * This is a level meter that can be used to display integer values.
 *
 * @ingroup controls
 */
class EGT_API AnalogMeter : public AnalogMeterType<int>
{
public:
    using AnalogMeterType<int>::AnalogMeterType;

    EGT_NODISCARD std::string type() const override
    {
        return "AnalogMeter";
    }
};

/**
 * This is a level meter that can be used to display floating values.
 *
 * @ingroup controls
 */
class EGT_API AnalogMeterF : public AnalogMeterType<float>
{
public:
    using AnalogMeterType<float>::AnalogMeterType;

    EGT_NODISCARD std::string type() const override
    {
        return "AnalogMeterF";
    }
};

template <class T>
Size AnalogMeterType<T>::m_default_size;

template <class T>
Size AnalogMeterType<T>::default_size()
{

    if (AnalogMeterType<T>::m_default_size.empty())
    {
        auto ss = egt::Application::instance().screen()->size();
        AnalogMeterType<T>::m_default_size = Size(ss.width() * 0.25, ss.height() * 0.20);
    }
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
