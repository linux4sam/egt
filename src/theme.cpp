/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/theme.h"
#include "egt/painter.h"

using namespace std;

namespace egt
{
inline namespace v1
{

static Theme* g_theme = nullptr;

Theme& global_theme()
{
    if (!g_theme)
        g_theme = new Theme();

    return *g_theme;
}

void set_global_theme(Theme* theme)
{
    if (g_theme)
        delete g_theme;

    g_theme = theme;
}

float Theme::DEFAULT_ROUNDED_RADIUS = 6.0;
float Theme::DEFAULT_BORDER_WIDTH = 2.0;

void Theme::draw_box(Painter& painter,
                     Widget& widget,
                     boxtype type,
                     const Rect& rect)
{
    Rect box = rect.empty() ? widget.box() : rect;

    Palette::ColorGroup group = Palette::GROUP_NORMAL;
    if (widget.disabled())
        group = Palette::GROUP_DISABLED;
    else if (widget.active())
        group = Palette::GROUP_ACTIVE;

    switch (type)
    {
    case boxtype::none:
        break;
    case boxtype::fill:
        draw_fill_box(painter,
                      box,
                      widget.palette().color(Palette::BG, group));
        break;
    case boxtype::fillsolid:
        draw_fill_box(painter,
                      box,
                      widget.palette().color(Palette::BG, group),
                      true);
        break;
    case boxtype::border:
        draw_border_box(painter,
                        box,
                        widget.palette().color(Palette::BORDER, group));
        break;
    case boxtype::bottom_border:
        draw_bottom_border_box(painter,
                               box,
                               widget.palette().color(Palette::BORDER, group));
        break;
    case boxtype::rounded_border:
        draw_rounded_border_box(painter,
                                box,
                                widget.palette().color(Palette::BORDER, group));
        break;
    case boxtype::borderfill:
        draw_border_fill_box(painter,
                             box,
                             widget.palette().color(Palette::BORDER, group),
                             widget.palette().color(Palette::BG, group));
        break;
    case boxtype::rounded_fill:
        draw_rounded_fill_box(painter,
                              box,
                              widget.palette().color(Palette::BG, group));
        break;
    case boxtype::rounded_borderfill:
        draw_rounded_borderfill_box(painter,
                                    box,
                                    widget.palette().color(Palette::BORDER, group),
                                    widget.palette().color(Palette::BG, group));
        break;
    case boxtype::rounded_gradient:
        draw_rounded_gradient_box(painter,
                                  box,
                                  widget.palette().color(Palette::BG, group));
        break;
    case boxtype::rounded_border_gradient:
        draw_rounded_border_gradient_box(painter,
                                         box,
                                         widget.palette().color(Palette::BORDER, group),
                                         widget.palette().color(Palette::BG, group));
        break;
    }
}

void Theme::draw_fill_box(Painter& painter,
                          const Rect& rect,
                          const Color& bg,
                          bool solid)
{
    painter.set_color(bg);
    if (solid)
    {
        Painter::AutoSaveRestore sr(painter);
        cairo_set_operator(painter.context().get(), CAIRO_OPERATOR_SOURCE);
        painter.draw_fill(rect);
    }
    else
    {
        painter.draw_fill(rect);
    }
}

void Theme::draw_border_box(Painter& painter,
                            const Rect& rect,
                            const Color& border)
{
    double rx = rect.x + DEFAULT_BORDER_WIDTH / 2.,
           ry = rect.y + DEFAULT_BORDER_WIDTH / 2.,
           width = rect.w - DEFAULT_BORDER_WIDTH,
           height = rect.h - DEFAULT_BORDER_WIDTH;

    if (width < 0 || height < 0)
        return;

    auto r = Rect(rx, ry, width, height);

    painter.rectangle(r);
    painter.set_color(border);
    painter.set_line_width(DEFAULT_BORDER_WIDTH);
    painter.stroke();
}

void Theme::draw_bottom_border_box(Painter& painter,
                                   const Rect& rect,
                                   const Color& border)
{
    double rx = rect.x + DEFAULT_BORDER_WIDTH / 2.,
           ry = rect.y + DEFAULT_BORDER_WIDTH / 2.,
           width = rect.w - DEFAULT_BORDER_WIDTH,
           height = rect.h - DEFAULT_BORDER_WIDTH;

    if (width < 0 || height < 0)
        return;

    auto r = Rect(rx, ry, width, height);

    painter.line(r.bottom_left(), r.bottom_right());
    painter.set_color(border);
    painter.set_line_width(DEFAULT_BORDER_WIDTH);
    painter.stroke();
}

void Theme::draw_border_fill_box(Painter& painter,
                                 const Rect& rect,
                                 const Color& border,
                                 const Color& bg)
{
    double rx = rect.x + DEFAULT_BORDER_WIDTH / 2.,
           ry = rect.y + DEFAULT_BORDER_WIDTH / 2.,
           width = rect.w - DEFAULT_BORDER_WIDTH,
           height = rect.h - DEFAULT_BORDER_WIDTH;

    if (width < 0 || height < 0)
        return;

    auto cr = painter.context();
    painter.set_color(bg);
    painter.rectangle(Rect(rx, ry, width, height));
    cairo_fill_preserve(cr.get());

    painter.set_color(border);
    painter.set_line_width(DEFAULT_BORDER_WIDTH);
    painter.stroke();
}

void Theme::draw_rounded_border_box(Painter& painter,
                                    const Rect& rect,
                                    const Color& border)
{
    Painter::AutoSaveRestore sr(painter);

    double rx = rect.x + DEFAULT_BORDER_WIDTH / 2.,
           ry = rect.y + DEFAULT_BORDER_WIDTH / 2.,
           width = rect.w - DEFAULT_BORDER_WIDTH,
           height = rect.h - DEFAULT_BORDER_WIDTH,
           aspect = 1.0,
           corner_radius = DEFAULT_ROUNDED_RADIUS;

    if (width < 0 || height < 0)
        return;

    double radius = corner_radius / aspect;
    double degrees = M_PI / 180.0;

    auto cr = painter.context();

    cairo_new_sub_path(cr.get());
    cairo_arc(cr.get(), rx + width - radius, ry + radius, radius, -90. * degrees, 0. * degrees);
    cairo_arc(cr.get(), rx + width - radius, ry + height - radius, radius, 0 * degrees, 90. * degrees);
    cairo_arc(cr.get(), rx + radius, ry + height - radius, radius, 90. * degrees, 180. * degrees);
    cairo_arc(cr.get(), rx + radius, ry + radius, radius, 180. * degrees, 270. * degrees);
    cairo_close_path(cr.get());

    painter.set_color(border);
    painter.set_line_width(DEFAULT_BORDER_WIDTH);
    painter.stroke();
}

void Theme::draw_rounded_borderfill_box(Painter& painter,
                                        const Rect& rect,
                                        const Color& border,
                                        const Color& bg)
{
    Painter::AutoSaveRestore sr(painter);

    double rx = rect.x + DEFAULT_BORDER_WIDTH / 2.,
           ry = rect.y + DEFAULT_BORDER_WIDTH / 2.,
           width = rect.w - DEFAULT_BORDER_WIDTH,
           height = rect.h - DEFAULT_BORDER_WIDTH,
           aspect = 1.0,
           corner_radius = DEFAULT_ROUNDED_RADIUS;

    if (width < 0 || height < 0)
        return;

    if (rect.w <= 10)
        corner_radius = rect.w / 2;

    if (rect.h <= 10)
        corner_radius = rect.h / 2;

    double radius = corner_radius / aspect;
    double degrees = M_PI / 180.0;

    auto cr = painter.context();

    cairo_new_sub_path(cr.get());
    cairo_arc(cr.get(), rx + width - radius, ry + radius, radius, -90. * degrees, 0. * degrees);
    cairo_arc(cr.get(), rx + width - radius, ry + height - radius, radius, 0 * degrees, 90. * degrees);
    cairo_arc(cr.get(), rx + radius, ry + height - radius, radius, 90. * degrees, 180. * degrees);
    cairo_arc(cr.get(), rx + radius, ry + radius, radius, 180. * degrees, 270. * degrees);
    cairo_close_path(cr.get());

    painter.set_color(bg);
    cairo_fill_preserve(cr.get());

    painter.set_color(border);
    painter.set_line_width(DEFAULT_BORDER_WIDTH);
    cairo_stroke(cr.get());
}

void Theme::draw_rounded_fill_box(Painter& painter,
                                  const Rect& rect,
                                  const Color& bg)
{
    Painter::AutoSaveRestore sr(painter);

    double rx = rect.x,
           ry = rect.y,
           width = rect.w,
           height = rect.h,
           aspect = 1.0,
           corner_radius = DEFAULT_ROUNDED_RADIUS;

    if (width < 0 || height < 0)
        return;

    if (rect.w <= 10)
        corner_radius = rect.w / 2;

    if (rect.h <= 10)
        corner_radius = rect.h / 2;

    double radius = corner_radius / aspect;
    double degrees = M_PI / 180.0;

    auto cr = painter.context();

    cairo_new_sub_path(cr.get());
    cairo_arc(cr.get(), rx + width - radius, ry + radius, radius, -90. * degrees, 0. * degrees);
    cairo_arc(cr.get(), rx + width - radius, ry + height - radius, radius, 0 * degrees, 90. * degrees);
    cairo_arc(cr.get(), rx + radius, ry + height - radius, radius, 90. * degrees, 180. * degrees);
    cairo_arc(cr.get(), rx + radius, ry + radius, radius, 180. * degrees, 270. * degrees);
    cairo_close_path(cr.get());

    painter.set_color(bg);
    cairo_fill(cr.get());
}

void Theme::draw_rounded_border_gradient_box(Painter& painter,
        const Rect& rect,
        const Color& border,
        const Color& bg)
{
    Painter::AutoSaveRestore sr(painter);

    double rx = rect.x + DEFAULT_BORDER_WIDTH / 2.,
           ry = rect.y + DEFAULT_BORDER_WIDTH / 2.,
           width = rect.w - DEFAULT_BORDER_WIDTH,
           height = rect.h - DEFAULT_BORDER_WIDTH,
           aspect = 1.0,
           corner_radius = DEFAULT_ROUNDED_RADIUS;

    if (width < 0 || height < 0)
        return;

    double radius = corner_radius / aspect;
    double degrees = M_PI / 180.0;

    auto cr = painter.context();

    cairo_new_sub_path(cr.get());
    cairo_arc(cr.get(), rx + width - radius, ry + radius, radius, -90. * degrees, 0. * degrees);
    cairo_arc(cr.get(), rx + width - radius, ry + height - radius, radius, 0 * degrees, 90. * degrees);
    cairo_arc(cr.get(), rx + radius, ry + height - radius, radius, 90. * degrees, 180. * degrees);
    cairo_arc(cr.get(), rx + radius, ry + radius, radius, 180. * degrees, 270. * degrees);
    cairo_close_path(cr.get());

    auto pat = shared_cairo_pattern_t(cairo_pattern_create_linear(rx + width / 2, ry,
                                      rx + width / 2, ry + height),
                                      cairo_pattern_destroy);

    Color step = bg;
    cairo_pattern_add_color_stop_rgb(pat.get(), 0, step.redf(), step.greenf(), step.bluef());
    step = bg.shade(.1);
    cairo_pattern_add_color_stop_rgb(pat.get(), 0.43, step.redf(), step.greenf(), step.bluef());
    step = bg.shade(.15);
    cairo_pattern_add_color_stop_rgb(pat.get(), 0.5, step.redf(), step.greenf(), step.bluef());
    step = bg.shade(.18);
    cairo_pattern_add_color_stop_rgb(pat.get(), 1.0, step.redf(), step.greenf(), step.bluef());

    cairo_set_source(cr.get(), pat.get());
    cairo_fill_preserve(cr.get());

    painter.set_color(border);
    painter.set_line_width(DEFAULT_BORDER_WIDTH);
    cairo_stroke(cr.get());
}


void Theme::draw_rounded_gradient_box(Painter& painter,
                                      const Rect& rect,
                                      const Color& bg)
{
    Painter::AutoSaveRestore sr(painter);

    double rx = rect.x,
           ry = rect.y,
           width = rect.w,
           height = rect.h,
           aspect = 1.0,
           corner_radius = DEFAULT_ROUNDED_RADIUS;

    if (width < 0 || height < 0)
        return;

    double radius = corner_radius / aspect;
    double degrees = M_PI / 180.0;

    auto cr = painter.context();

    cairo_new_sub_path(cr.get());
    cairo_arc(cr.get(), rx + width - radius, ry + radius, radius, -90. * degrees, 0. * degrees);
    cairo_arc(cr.get(), rx + width - radius, ry + height - radius, radius, 0 * degrees, 90. * degrees);
    cairo_arc(cr.get(), rx + radius, ry + height - radius, radius, 90. * degrees, 180. * degrees);
    cairo_arc(cr.get(), rx + radius, ry + radius, radius, 180. * degrees, 270. * degrees);
    cairo_close_path(cr.get());

    auto pat = shared_cairo_pattern_t(cairo_pattern_create_linear(rx + width / 2, ry,
                                      rx + width / 2, ry + height),
                                      cairo_pattern_destroy);

    Color step = bg;
    cairo_pattern_add_color_stop_rgb(pat.get(), 0, step.redf(), step.greenf(), step.bluef());
    step = bg.shade(.1);
    cairo_pattern_add_color_stop_rgb(pat.get(), 0.43, step.redf(), step.greenf(), step.bluef());
    step = bg.shade(.15);
    cairo_pattern_add_color_stop_rgb(pat.get(), 0.5, step.redf(), step.greenf(), step.bluef());
    step = bg;
    cairo_pattern_add_color_stop_rgb(pat.get(), 1.0, step.redf(), step.greenf(), step.bluef());

    cairo_set_source(cr.get(), pat.get());
    cairo_fill(cr.get());
}

}
}
