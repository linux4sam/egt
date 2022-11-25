/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/button.h"
#include "egt/checkbox.h"
#include "egt/detail/alignment.h"
#include "egt/detail/layout.h"
#include "egt/frame.h"
#include "egt/painter.h"
#include "egt/serialize.h"
#include "egt/theme.h"

namespace egt
{
inline namespace v1
{

CheckBox::CheckBox(const std::string& text,
                   const Rect& rect) noexcept
    : Button(text, rect)
{
    name("CheckBox" + std::to_string(m_widgetid));

    fill_flags().clear();
    padding(5);
    text_align(AlignFlag::left | AlignFlag::center_vertical);

    grab_mouse(true);
}

CheckBox::CheckBox(Frame& parent,
                   const std::string& text,
                   const Rect& rect) noexcept
    : CheckBox(text, rect)
{
    parent.add(*this);
}

CheckBox::CheckBox(Serializer::Properties& props, bool is_derived) noexcept
    : Button(props, true)
{
    deserialize(props);

    if (!is_derived)
        deserialize_leaf(props);
}

void CheckBox::handle(Event& event)
{
    // NOLINTNEXTLINE(bugprone-parent-virtual-call)
    Widget::handle(event);

    switch (event.id())
    {
    case EventId::pointer_click:
        checked(!checked());
    default:
        break;
    }
}

void CheckBox::text(const std::string& text)
{
    if (m_text != text)
    {
        if (text.empty())
            show_label(false);
        else
            show_label(true);
    }

    Button::text(text);
}

void CheckBox::draw(Painter& painter, const Rect& rect)
{
    Drawer<CheckBox>::draw(*this, painter, rect);
}

void CheckBox::default_draw(const CheckBox& widget, Painter& painter, const Rect& /*rect*/)
{
    widget.draw_box(painter, Palette::ColorId::label_bg, Palette::ColorId::border);

    auto b = widget.content_area();
    auto border = widget.theme().default_border();
    Rect handle;
    std::vector<detail::LayoutRect> rects;

    if (widget.show_label())
    {
        painter.set(widget.font());
        auto text_size = painter.text_size(widget.text());

        Rect text;
        if (widget.checkbox_align().is_set(AlignFlag::bottom) ||
            widget.checkbox_align().is_set(AlignFlag::top))
        {
            auto w = std::min<DefaultDim>(b.height() - text_size.height(), b.width());
            if (w < 0)
                w = b.height() * 0.15;

            if (widget.checkbox_align().is_set(AlignFlag::bottom))
            {
                rects.emplace_back(0,
                                   Rect(0, 0, b.width(), text_size.height()),
                                   widget.padding() / 2);
                rects.emplace_back(0,
                                   Rect(0, 0, w, w),
                                   0, 0, widget.padding() / 2);

                detail::flex_layout(b, rects, Justification::start, Orientation::vertical);

                text = rects[0].rect + b.point();
                handle = rects[1].rect + b.point();
            }
            else
            {
                rects.emplace_back(0,
                                   Rect(0, 0, w, w),
                                   0, 0, widget.padding() / 2);
                rects.emplace_back(0,
                                   Rect(0, 0, b.width(), text_size.height()),
                                   widget.padding() / 2);

                detail::flex_layout(b, rects, Justification::start, Orientation::vertical);

                handle = rects[0].rect + b.point();
                text = rects[1].rect + b.point();
            }
        }
        else
        {
            auto w = std::min<DefaultDim>(b.width() - text_size.width(), b.height());
            if (w < 0)
                w = b.width() * 0.15;

            if (widget.checkbox_align().is_set(AlignFlag::left))
            {
                rects.emplace_back(0,
                                   Rect(0, 0, w, w),
                                   0, 0, widget.padding() / 2);
                rects.emplace_back(0,
                                   Rect(0, 0, b.width() - w, b.height()),
                                   widget.padding() / 2);

                detail::flex_layout(b, rects, Justification::start, Orientation::horizontal);

                handle = rects[0].rect + b.point();
                text = rects[1].rect + b.point();
            }
            else
            {
                rects.emplace_back(0,
                                   Rect(0, 0, b.width() - w, b.height()),
                                   widget.padding() / 2);
                rects.emplace_back(0,
                                   Rect(0, 0, w, w),
                                   0, 0, widget.padding() / 2);

                detail::flex_layout(b, rects, Justification::start, Orientation::horizontal);

                text = rects[0].rect + b.point();
                handle = rects[1].rect + b.point();
            }
        }

        widget.theme().draw_box(painter, Theme::FillFlag::blend, handle,
                                widget.color(Palette::ColorId::button_fg),
                                Palette::transparent,
                                border);
        // text
        painter.set(widget.color(Palette::ColorId::label_text));
        auto size = painter.text_size(widget.text());
        Rect target = detail::align_algorithm(size,
                                              text,
                                              widget.text_align());
        painter.draw(target.point());
        painter.draw(widget.text());
    }
    else
    {
        auto w = std::min<DefaultDim>(b.width(), b.height());
        if (w < 0)
            w = b.width() * 0.15;

        rects.emplace_back(0,
                           Rect(0, 0, w, w),
                           0, 0, widget.padding() / 2);

        detail::flex_layout(b, rects, Justification::middle, Orientation::horizontal);

        handle = rects[0].rect + b.point();

        widget.theme().draw_box(painter, Theme::FillFlag::blend, handle,
                                widget.color(Palette::ColorId::button_fg),
                                Palette::transparent,
                                border);
    }

    if (widget.checked())
    {
        // draw an "X"
        auto cr = painter.context().get();
        cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
        painter.set(widget.color(Palette::ColorId::button_fg));
        painter.draw(handle.top_left() + Point(border, border),
                     handle.bottom_right() - Point(border, border));
        painter.draw(handle.top_right() + Point(-border, border),
                     handle.bottom_left() + Point(border, -border));
        painter.line_width(border);
        painter.stroke();
    }
}

Size CheckBox::min_size_hint() const
{
    if (!m_min_size.empty())
        return m_min_size;

    // NOLINTNEXTLINE(bugprone-parent-virtual-call)
    const auto min_size = default_size() + Widget::min_size_hint();

    if (!m_text.empty())
    {
        auto s = text_size(m_text);
        s += Size(s.width() / 2 + 5, 0);
        // NOLINTNEXTLINE(bugprone-parent-virtual-call)
        s += Widget::min_size_hint();
        if (s.width() < min_size.width())
            s.width(min_size.width());
        if (s.height() < min_size.height())
            s.height(min_size.height());
        return s;
    }

    /* if text is empty, use only 10% of min_size to
     * draw checkbox alone.
     */
    return min_size * 0.10;
}

void CheckBox::serialize(Serializer& serializer) const
{
    Button::serialize(serializer);

    serializer.add_property("show_label", show_label());
    if (!checkbox_align().empty())
        serializer.add_property("checkbox_align", checkbox_align());
}

void CheckBox::deserialize(Serializer::Properties& props)
{
    props.erase(std::remove_if(props.begin(), props.end(), [&](auto & p)
    {
        switch (detail::hash(std::get<0>(p)))
        {
        case detail::hash("show_label"):
            show_label(egt::detail::from_string(std::get<1>(p)));
            break;
        case detail::hash("checkbox_align"):
            checkbox_align(AlignFlags(std::get<1>(p)));
            break;
        default:
            return false;
        }
        return true;
    }), props.end());
}

ToggleBox::ToggleBox(const Rect& rect) noexcept
    : CheckBox( {}, rect)
{
    name("ToggleBox" + std::to_string(m_widgetid));

    fill_flags(Theme::FillFlag::blend);
    border(theme().default_border());
    border_radius(4.0);
}

ToggleBox::ToggleBox(Frame& parent, const Rect& rect) noexcept
    : ToggleBox(rect)
{
    parent.add(*this);
}

ToggleBox::ToggleBox(Serializer::Properties& props, bool is_derived) noexcept
    : CheckBox(props, true)
{
    deserialize(props);

    if (!is_derived)
        deserialize_leaf(props);
}
void ToggleBox::draw(Painter& painter, const Rect& rect)
{
    Drawer<ToggleBox>::draw(*this, painter, rect);
}

void ToggleBox::default_draw(ToggleBox& widget, Painter& painter, const Rect& rect)
{
    detail::ignoreparam(rect);

    /// @[ToggleBoxDraw]
    widget.draw_box(painter, Palette::ColorId::bg, Palette::ColorId::border);

    auto b = widget.content_area();

    if (widget.checked())
    {
        Rect rect = b;
        rect.width(rect.width() / 2);
        rect.x(rect.x() + rect.width());
        widget.theme().draw_box(painter,
                                Theme::FillFlag::blend,
                                rect,
                                widget.color(Palette::ColorId::border),
                                widget.color(Palette::ColorId::button_bg),
                                0,
                                0,
                                widget.border_radius());
    }
    else
    {
        Rect rect = b;
        rect.width(rect.width() / 2);

        if (widget.enable_disable())
        {
            widget.theme().draw_box(painter,
                                    Theme::FillFlag::blend,
                                    rect,
                                    widget.color(Palette::ColorId::border, Palette::GroupId::disabled),
                                    widget.color(Palette::ColorId::button_bg, Palette::GroupId::disabled),
                                    0,
                                    0,
                                    widget.border_radius());
        }
        else
        {
            widget.theme().draw_box(painter,
                                    Theme::FillFlag::blend,
                                    rect,
                                    widget.color(Palette::ColorId::border),
                                    widget.color(Palette::ColorId::button_bg),
                                    0,
                                    0,
                                    widget.border_radius());
        }
    }

    if (!widget.on_text().empty())
    {
        Rect rect = b;
        rect.width(rect.width() / 2);
        rect.x(rect.x() + rect.width());

        if (widget.checked())
            painter.set(widget.color(Palette::ColorId::button_text));
        else
            painter.set(widget.color(Palette::ColorId::button_text, Palette::GroupId::disabled));
        painter.set(widget.font());
        auto size = painter.text_size(widget.on_text());
        Rect target = detail::align_algorithm(size,
                                              rect,
                                              AlignFlag::center);
        painter.draw(target.point());
        painter.draw(widget.on_text());
    }

    if (!widget.off_text().empty())
    {
        Rect rect = b;
        rect.width(rect.width() / 2);

        painter.set(widget.color(Palette::ColorId::button_text, Palette::GroupId::disabled));
        painter.set(widget.font());
        auto size = painter.text_size(widget.off_text());
        Rect target = detail::align_algorithm(size,
                                              rect,
                                              AlignFlag::center);
        painter.draw(target.point());
        painter.draw(widget.off_text());
    }
    /// @[ToggleBoxDraw]
}

Size ToggleBox::min_size_hint() const
{
    if (!m_min_size.empty())
        return m_min_size;

    // NOLINTNEXTLINE(bugprone-parent-virtual-call)
    return Size(100, 30) + Widget::min_size_hint();
}

void ToggleBox::serialize(Serializer& serializer) const
{
    CheckBox::serialize(serializer);

    serializer.add_property("off_text", off_text());
    serializer.add_property("on_text", on_text());
    serializer.add_property("enable_disable", static_cast<int>(enable_disable()));
}

void ToggleBox::deserialize(Serializer::Properties& props)
{
    props.erase(std::remove_if(props.begin(), props.end(), [&](auto & p)
    {
        auto name = std::get<0>(p);
        if (name == "off_text")
            toggle_text(std::get<1>(p), on_text());
        else if (name == "on_text")
            toggle_text(off_text(), std::get<1>(p));
        else if (name == "enable_disable")
            enable_disable(std::stoi(std::get<1>(p)));
        else
            return false;
        return true;
    }), props.end());
}

}
}
