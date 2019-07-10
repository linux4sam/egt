/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/checkbox.h"
#include "egt/detail/math.h"
#include "egt/painter.h"
#include "egt/theme.h"
#include "egt/widget.h"

namespace egt
{
inline namespace v1
{

static Theme* g_theme = nullptr;

Theme& global_theme()
{
    if (!g_theme)
    {
        g_theme = new Theme();
        g_theme->apply();
    }

    assert(g_theme);

    return *g_theme;
}

void set_global_theme(Theme* theme)
{
    assert(theme);

    delete g_theme;
    g_theme = theme;

    if (g_theme)
        g_theme->apply();
}

float Theme::DEFAULT_ROUNDED_RADIUS = 4.0;

void Theme::init_palette()
{
    m_palette->set(Palette::ColorId::cursor, Palette::GroupId::normal, Palette::red);

    auto pattern = [](const Color & color)
    {
        Pattern pattern1;
        pattern1.step(0, color);
        pattern1.step(0.43, color.shade(.1));
        pattern1.step(0.5, color.shade(.15));
        pattern1.step(1.0, color.shade(.18));
        return pattern1;
    };

    m_palette->set(Palette::ColorId::bg, Palette::GroupId::normal, Palette::white);
    m_palette->set(Palette::ColorId::text, Palette::GroupId::normal, Palette::black);
    m_palette->set(Palette::ColorId::text_highlight, Palette::GroupId::normal, Palette::hotpink);
    m_palette->set(Palette::ColorId::border, Palette::GroupId::normal, Color(0xed192dff));
    m_palette->set(Palette::ColorId::button_bg, Palette::GroupId::normal, pattern(Color(0xed192dff)));
    m_palette->set(Palette::ColorId::button_fg, Palette::GroupId::normal, Color(0xed192dff));
    m_palette->set(Palette::ColorId::button_text, Palette::GroupId::normal, Palette::white);
    m_palette->set(Palette::ColorId::label_bg, Palette::GroupId::normal, Palette::white);
    m_palette->set(Palette::ColorId::label_text, Palette::GroupId::normal, Palette::black);

    m_palette->set(Palette::ColorId::bg, Palette::GroupId::disabled, Palette::white);
    m_palette->set(Palette::ColorId::text, Palette::GroupId::disabled, Color(0x989a9aff));
    m_palette->set(Palette::ColorId::text_highlight, Palette::GroupId::disabled, Palette::hotpink);
    m_palette->set(Palette::ColorId::border, Palette::GroupId::disabled, Color(0xcbcecfff));
    m_palette->set(Palette::ColorId::button_bg, Palette::GroupId::disabled, pattern(Color(0xcbcecfff)));
    m_palette->set(Palette::ColorId::button_fg, Palette::GroupId::disabled, Color(0xcbcecfff));
    m_palette->set(Palette::ColorId::button_text, Palette::GroupId::disabled, Palette::black);
    m_palette->set(Palette::ColorId::label_bg, Palette::GroupId::disabled, Palette::white);
    m_palette->set(Palette::ColorId::label_text, Palette::GroupId::disabled, Palette::black);

    m_palette->set(Palette::ColorId::bg, Palette::GroupId::active, Palette::white);
    m_palette->set(Palette::ColorId::text, Palette::GroupId::active, Palette::black);
    m_palette->set(Palette::ColorId::text_highlight, Palette::GroupId::active, Palette::hotpink);
    m_palette->set(Palette::ColorId::border, Palette::GroupId::active, Color(0xed192dff));
    m_palette->set(Palette::ColorId::button_bg, Palette::GroupId::active, pattern(Color(0xc01f2aff)));
    m_palette->set(Palette::ColorId::button_fg, Palette::GroupId::active, Color(0xed192dff));
    m_palette->set(Palette::ColorId::button_text, Palette::GroupId::active, Palette::white);
    m_palette->set(Palette::ColorId::label_bg, Palette::GroupId::active, Color(0xed192dff));
    m_palette->set(Palette::ColorId::label_text, Palette::GroupId::active, Palette::white);
}

void Theme::init_draw()
{
    /**
     * @todo When switching themes, a theme has to reset every single widget
     * draw method, which is obviously not being done here.  It would be nice if
     * there was a global method to reset back to default.  Maybe this function
     * could be it if it listed all widget types.
     */
    Drawer<CheckBox>::set_draw(CheckBox::default_draw);
}

void Theme::init_font()
{
}

void Theme::draw_box(Painter& painter, const Widget& widget,
                     Palette::ColorId bg,
                     Palette::ColorId border) const
{
    auto type = widget.boxtype();

    if (type == boxtype::none)
        return;

    Palette::GroupId group = Palette::GroupId::normal;
    if (widget.disabled())
        group = Palette::GroupId::disabled;
    else if (widget.active())
        group = Palette::GroupId::active;

    draw_box(painter,
             type,
             widget.box(),
             widget.color(border, group),
             widget.color(bg, group),
             widget.border(),
             widget.margin());
}

void Theme::draw_box(Painter& painter,
                     boxtype type,
                     const Rect& rect,
                     const pattern_type& border,
                     const pattern_type& bg,
                     default_dim_type border_width,
                     default_dim_type margin_width) const
{
    if (type == boxtype::none)
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

    //assert(!box.empty());
    if (box.empty())
        return;

    double rx = box.x(),
           ry = box.y(),
           width = box.width(),
           height = box.height(),
           aspect = 1.0,
           corner_radius = DEFAULT_ROUNDED_RADIUS;

    double radius = corner_radius / aspect;
    double degrees = detail::pi<double>() / 180.0;

    Painter::AutoSaveRestore sr(painter);
    auto cr = painter.context().get();

    if ((type & boxtype::solid) == boxtype::solid)
    {
        cairo_set_operator(painter.context().get(), CAIRO_OPERATOR_SOURCE);
    }

    cairo_new_path(cr);

    if ((type & boxtype::border_rounded) == boxtype::border_rounded)
    {
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

    if ((type & boxtype::fill) == boxtype::fill)
    {
        const auto& steps = bg.steps();
        if (steps.size() == 1)
        {
            painter.set(steps.begin()->second);
        }
        else if (steps.size() > 1)
        {
            auto pat = shared_cairo_pattern_t(cairo_pattern_create_linear(rx + width / 2., ry,
                                              rx + width / 2., ry + height),
                                              cairo_pattern_destroy);

            for (const auto& step : steps)
            {
                cairo_pattern_add_color_stop_rgba(pat.get(),
                                                  step.first,
                                                  step.second.redf(),
                                                  step.second.greenf(),
                                                  step.second.bluef(),
                                                  step.second.alphaf());
            }

            cairo_set_source(cr, pat.get());
        }

        cairo_fill_preserve(cr);
    }

    if (border_width)
    {
        if ((type & boxtype::border_bottom) == boxtype::border_bottom)
        {
            cairo_new_path(cr);
            painter.draw(box.bottom_left(), box.bottom_right());
        }

        painter.set(border.color());
        painter.set_line_width(border_width);
        cairo_stroke(cr);
    }

    cairo_new_path(cr);
}

void Theme::draw_circle(Painter& painter,
                        boxtype type,
                        const Rect& rect,
                        const pattern_type& border,
                        const pattern_type& bg,
                        default_dim_type border_width,
                        default_dim_type margin_width) const
{
    if (type == boxtype::none)
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

    if ((type & boxtype::solid) == boxtype::solid)
    {
        cairo_set_operator(painter.context().get(), CAIRO_OPERATOR_SOURCE);
    }

    cairo_new_path(cr);
    painter.draw(circle);

    if ((type & boxtype::fill) == boxtype::fill)
    {
        const auto& steps = bg.steps();
        if (steps.size() == 1)
        {
            painter.set(steps.begin()->second);
        }
        else if (steps.size() > 1)
        {
            auto pat = shared_cairo_pattern_t(cairo_pattern_create_radial(
                                                  circle.center.x(),
                                                  circle.center.y(),
                                                  circle.radius,
                                                  circle.center.x(),
                                                  circle.center.y(),
                                                  0),
                                              cairo_pattern_destroy);

            for (const auto& step : steps)
            {
                cairo_pattern_add_color_stop_rgba(pat.get(),
                                                  step.first,
                                                  step.second.redf(),
                                                  step.second.greenf(),
                                                  step.second.bluef(),
                                                  step.second.alphaf());
            }

            cairo_set_source(cr, pat.get());
        }

        cairo_fill_preserve(cr);
    }

    if (border_width)
    {
        painter.set(border.color());
        painter.set_line_width(border_width);
        cairo_stroke(cr);
    }

    cairo_new_path(cr);
}

}
}
