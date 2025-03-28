/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/button.h"
#include "egt/checkbox.h"
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
    : Switch(text, rect)
{
    name("CheckBox" + std::to_string(m_widgetid));
}

CheckBox::CheckBox(Frame& parent,
                   const std::string& text,
                   const Rect& rect) noexcept
    : CheckBox(text, rect)
{
    parent.add(*this);
}

void CheckBox::draw(Painter& painter, const Rect& rect)
{
    Drawer<CheckBox>::draw(*this, painter, rect);
}

void CheckBox::draw_switch(Painter& painter, const Rect& handle) const
{
    if (switch_image(checked()))
    {
        Switch::draw_switch(painter, handle);
        return;
    }

    auto border = theme().default_border();

    theme().draw_box(painter, Theme::FillFlag::blend, handle,
                     color(Palette::ColorId::button_fg),
                     Palette::transparent,
                     border);

    if (checked())
    {
        // draw an "X"
        painter.line_cap(Painter::LineCap::round);
        painter.set(color(Palette::ColorId::button_fg));
        painter.draw(handle.top_left() + Point(border, border),
                     handle.bottom_right() - Point(border, border));
        painter.draw(handle.top_right() + Point(-border, border),
                     handle.bottom_left() + Point(border, -border));
        painter.line_width(border);
        painter.stroke();
    }
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
