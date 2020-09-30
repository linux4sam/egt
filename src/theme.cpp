/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/checkbox.h"
#include "egt/detail/enum.h"
#include "egt/detail/math.h"
#include "egt/painter.h"
#include "egt/theme.h"
#include "egt/widget.h"
#include <cassert>

namespace egt
{
inline namespace v1
{

std::vector<DrawerReset::ResetFunction> DrawerReset::m_reset_list;

static std::shared_ptr<Theme> the_global_theme;

Theme& global_theme()
{
    if (!the_global_theme)
    {
        the_global_theme = std::make_shared<Theme>();
        the_global_theme->apply();
    }

    return *the_global_theme;
}

void global_theme(std::shared_ptr<Theme> theme)
{
    assert(theme);
    if (theme)
    {
        the_global_theme = std::move(theme);

        if (the_global_theme)
            the_global_theme->apply();
    }
}

static Pattern pattern(const Color& color)
{
    return Pattern(Pattern::Type::linear,
    {
        {0, color},
        {0.43, color.shade(.1)},
        {0.5, color.shade(.15)},
        {1.0, color.shade(.18)},
    });
}

Theme::Theme()
    : m_palette
{
    {
        {
            Palette::GroupId::normal, {
                {Palette::ColorId::cursor, Palette::red},
                {Palette::ColorId::bg, Palette::white},
                {Palette::ColorId::text, Palette::black},
                {Palette::ColorId::text_highlight, Palette::hotpink},
                {Palette::ColorId::border, Color(0xed192dff)},
                {Palette::ColorId::button_bg, pattern(Color(0xed192dff))},
                {Palette::ColorId::button_fg, Color(0xed192dff)},
                {Palette::ColorId::button_text, Palette::white},
                {Palette::ColorId::label_bg, Palette::white},
                {Palette::ColorId::label_text, Palette::black},
            }
        },
        {
            Palette::GroupId::disabled, {
                {Palette::ColorId::bg, Palette::white},
                {Palette::ColorId::text, Color(0x989a9aff)},
                {Palette::ColorId::text_highlight, Palette::hotpink},
                {Palette::ColorId::border, Color(0xcbcecfff)},
                {Palette::ColorId::button_bg, pattern(Color(0xcbcecfff))},
                {Palette::ColorId::button_fg, Color(0xcbcecfff)},
                {Palette::ColorId::button_text, Palette::black},
                {Palette::ColorId::label_bg, Palette::white},
                {Palette::ColorId::label_text, Palette::black},
            }
        },
        {
            Palette::GroupId::active, {
                {Palette::ColorId::bg, Palette::white},
                {Palette::ColorId::text, Palette::black},
                {Palette::ColorId::text_highlight, Palette::hotpink},
                {Palette::ColorId::border, Color(0xed192dff)},
                {Palette::ColorId::button_bg, pattern(Color(0xc01f2aff))},
                {Palette::ColorId::button_fg, Color(0xed192dff)},
                {Palette::ColorId::button_text, Palette::white},
                {Palette::ColorId::label_bg, Palette::white},
                {Palette::ColorId::label_text, Palette::gray},
            }
        },
        {
            Palette::GroupId::checked, {
                {Palette::ColorId::bg, Palette::white},
                {Palette::ColorId::text, Palette::black},
                {Palette::ColorId::text_highlight, Palette::hotpink},
                {Palette::ColorId::border, Color(0xed192dff)},
                {Palette::ColorId::button_bg, pattern(Color(0xed192dff))},
                {Palette::ColorId::button_fg, Color(0xed192dff)},
                {Palette::ColorId::button_text, Palette::white},
                {Palette::ColorId::label_bg, pattern(Color(0xed192dff))},
                {Palette::ColorId::label_text, Palette::black},
            }
        }
    }
}
{}

void Theme::init_palette()
{

}

void Theme::init_draw()
{
    DrawerReset::reset();
}

void Theme::init_font()
{
    m_font = Font();
}

void Theme::rounded_box(Painter& painter, const RectF& box, float border_radius) const
{
    auto cr = painter.context().get();

    if (!detail::float_equal(border_radius, 0) && border_radius > 0)
    {
        const double rx = box.x();
        const double ry = box.y();
        const double width = box.width();
        const double height = box.height();
        const double aspect = 1.0;
        const double corner_radius = border_radius;
        const double radius = corner_radius / aspect;
        const double degrees = detail::pi<double>() / 180.0;

        cairo_new_path(cr);
        cairo_arc(cr, rx + width - radius, ry + radius, radius, -90. * degrees, 0. * degrees);
        cairo_arc(cr, rx + width - radius, ry + height - radius, radius, 0 * degrees, 90. * degrees);
        cairo_arc(cr, rx + radius, ry + height - radius, radius, 90. * degrees, 180. * degrees);
        cairo_arc(cr, rx + radius, ry + radius, radius, 180. * degrees, 270. * degrees);
        cairo_close_path(cr);
    }
    else
    {
        painter.draw(box);
    }
}

void Theme::draw_box(Painter& painter, const Widget& widget,
                     Palette::ColorId bg,
                     Palette::ColorId border) const
{
    const auto& type = widget.fill_flags();

    if (type.empty() && !widget.border())
        return;

    Palette::GroupId group = Palette::GroupId::normal;
    if (widget.disabled())
        group = Palette::GroupId::disabled;
    else if (widget.active())
        group = Palette::GroupId::active;
    else if (widget.checked())
        group = Palette::GroupId::checked;

    draw_box(painter,
             type,
             widget.box(),
             widget.color(border, group),
             widget.color(bg, group),
             widget.border(),
             widget.margin(),
             widget.border_radius(),
             widget.border_flags());
}

void Theme::draw_box(Painter& painter,
                     const FillFlags& type,
                     const Rect& rect,
                     const Pattern& border,
                     const Pattern& bg,
                     DefaultDim border_width,
                     DefaultDim margin_width,
                     float border_radius,
                     const BorderFlags& border_flags) const
{
    if (type.empty() && !border_width)
        return;

    auto box = rect;

    // adjust for margin
    if (margin_width)
    {
        box += Point(margin_width, margin_width);
        box -= Size(margin_width * 2., margin_width * 2.);
    }

    // adjust for border
    if (border_width)
    {
        box += Point(border_width / 2., border_width / 2.);
        box -= Size(border_width, border_width);
    }

    if (box.empty())
        return;

    Painter::AutoSaveRestore sr(painter);
    auto cr = painter.context().get();

    if (type.is_set(FillFlag::solid))
    {
        cairo_set_operator(painter.context().get(), CAIRO_OPERATOR_SOURCE);
    }

    if (border_width && border_flags.is_set(BorderFlag::drop_shadow))
    {
        auto sbox = box;
        sbox += Point(border_width, border_width);
        sbox -= Size(border_width, border_width);
        painter.set(border);

        rounded_box(painter, sbox, border_radius);

        painter.fill();

        box += Point(border_width / 2., border_width / 2.);
        box -= Size(border_width, border_width);
    }

    rounded_box(painter, box, border_radius);

    if (type.is_set(FillFlag::blend) || type.is_set(FillFlag::solid))
    {
        // force the pattern on the center of the widget box vertically
        if (bg.type() == Pattern::Type::linear)
        {
            auto pattern = bg;
            pattern.linear(Point(box.x() + box.width() / 2., box.y()),
                           Point(box.x() + box.width() / 2., box.y() + box.height()));
            painter.set(pattern);
        }
        else
        {
            painter.set(bg);
        }

        cairo_fill_preserve(cr);
    }

    if (!border_flags.is_set(BorderFlag::drop_shadow))
    {
        if (border_width)
        {
            if (!border_flags.empty())
                cairo_new_path(cr);

            if (border_flags.is_set(BorderFlag::top))
                painter.draw(box.top_left(), box.top_right());

            if (border_flags.is_set(BorderFlag::right))
                painter.draw(box.top_right(), box.bottom_right());

            if (border_flags.is_set(BorderFlag::bottom))
                painter.draw(box.bottom_left(), box.bottom_right());

            if (border_flags.is_set(BorderFlag::left))
                painter.draw(box.top_left(), box.bottom_left());

            painter.set(border);
            painter.line_width(border_width);
            painter.stroke();
        }
    }

    cairo_new_path(cr);
}

void Theme::draw_circle(Painter& painter, const Widget& widget,
                        Palette::ColorId bg,
                        Palette::ColorId border) const
{
    const auto& type = widget.fill_flags();

    if (type.empty())
        return;

    Palette::GroupId group = Palette::GroupId::normal;
    if (widget.disabled())
        group = Palette::GroupId::disabled;
    else if (widget.active())
        group = Palette::GroupId::active;
    else if (widget.checked())
        group = Palette::GroupId::checked;

    draw_circle(painter,
                type,
                widget.box(),
                widget.color(border, group),
                widget.color(bg, group),
                widget.border(),
                widget.margin());
}

void Theme::draw_circle(Painter& painter,
                        const FillFlags& type,
                        const Rect& rect,
                        const Pattern& border,
                        const Pattern& bg,
                        DefaultDim border_width,
                        DefaultDim margin_width) const
{
    if (type.empty())
        return;

    auto box = rect;

    // adjust for margin
    if (margin_width)
    {
        box += Point(margin_width, margin_width);
        box -= Size(margin_width * 2., margin_width * 2.);
    }

    // adjust for border
    if (border_width)
    {
        box += Point(border_width / 2., border_width / 2.);
        box -= Size(border_width, border_width);
    }

    assert(!box.empty());
    if (box.empty())
        return;

    Circle circle(box.center(), std::min(box.width(), box.height()) / 2.);

    Painter::AutoSaveRestore sr(painter);
    auto cr = painter.context().get();

    if (type.is_set(FillFlag::solid))
    {
        cairo_set_operator(painter.context().get(), CAIRO_OPERATOR_SOURCE);
    }

    cairo_new_path(cr);
    painter.draw(circle);

    if (type.is_set(FillFlag::blend) || type.is_set(FillFlag::solid))
    {
        // force the pattern on the center of the widget box
        if (bg.type() == Pattern::Type::linear ||
            bg.type() == Pattern::Type::radial)
        {
            auto pattern = bg;
            pattern.radial(circle.center(), circle.radius(),
                           circle.center(), 0);
            painter.set(pattern);
        }
        else
        {
            painter.set(bg);
        }

        cairo_fill_preserve(cr);
    }

    if (border_width)
    {
        painter.set(border);
        painter.line_width(border_width);
        cairo_stroke(cr);
    }

    cairo_new_path(cr);
}

template<>
const std::pair<Theme::FillFlag, char const*> detail::EnumStrings<Theme::FillFlag>::data[] =
{
    {Theme::FillFlag::solid, "solid"},
    {Theme::FillFlag::blend, "blend"},
};

template<>
const std::pair<Theme::BorderFlag, char const*> detail::EnumStrings<Theme::BorderFlag>::data[] =
{
    {Theme::BorderFlag::top, "top"},
    {Theme::BorderFlag::right, "right"},
    {Theme::BorderFlag::bottom, "bottom"},
    {Theme::BorderFlag::left, "left"},
    {Theme::BorderFlag::drop_shadow, "drop_shadow"},
};

}
}
