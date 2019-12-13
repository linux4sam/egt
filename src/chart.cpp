/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "config.h"
#include "compat.h"
#include "egt/chart.h"
#include "egt/detail/meta.h"
#include "egt/painter.h"
#include "egt/screen.h"
#include "kplot.h"
#include <cmath>
#include <vector>

using namespace std;

namespace egt
{
inline namespace v1
{
namespace experimental
{

static void xticlabelfmt(double v, char* b, size_t len)
{
    snprintf(b, len, "%.02f", v);
}

LineChart::LineChart(const Rect& rect)
    : Widget(rect)
{
    set_name("LineChart" + std::to_string(m_widgetid));

    set_boxtype(Theme::boxtype::fill);
}

void LineChart::draw(Painter& painter, const Rect& rect)
{
    detail::ignoreparam(rect);

    draw_box(painter, Palette::ColorId::bg, Palette::ColorId::border);

    Painter::AutoSaveRestore sr(painter);

    const auto b = content_area();

    auto cr = painter.context();
    cairo_translate(cr.get(),
                    b.x(),
                    b.y());

    struct kplot* p = kplot_alloc(nullptr);
    struct kplotcfg* cfg = kplot_get_plotcfg(p);
    cfg->grid = m_grid;
    cfg->xticlabelfmt = xticlabelfmt;
    cfg->yticlabelfmt = xticlabelfmt;
    auto tc = color(Palette::ColorId::border).color();
    cfg->borderline.clr.type = KPLOTCTYPE_RGBA;
    cfg->borderline.clr.rgba[0] = tc.redf();
    cfg->borderline.clr.rgba[1] = tc.bluef();
    cfg->borderline.clr.rgba[2] = tc.greenf();
    cfg->borderline.clr.rgba[3] = tc.alphaf();

    cfg->ticline.clr.type = KPLOTCTYPE_RGBA;
    cfg->ticline.clr.rgba[0] = tc.redf();
    cfg->ticline.clr.rgba[1] = tc.bluef();
    cfg->ticline.clr.rgba[2] = tc.greenf();
    cfg->ticline.clr.rgba[3] = tc.alphaf();

    struct kdatacfg dcfg {};
    kdatacfg_defaults(&dcfg);
    dcfg.line.sz = m_linewidth;
    dcfg.line.clr.type = KPLOTCTYPE_PATTERN;
    dcfg.line.clr.pattern =
        cairo_pattern_create_linear(0.0, 0.0, b.width(), b.height());

    cairo_pattern_add_color_stop_rgb
    (dcfg.line.clr.pattern, 0.25, 1.0, 0.0, 0.0);
    cairo_pattern_add_color_stop_rgb
    (dcfg.line.clr.pattern, 0.5, 0.0, 1.0, 0.0);
    cairo_pattern_add_color_stop_rgb
    (dcfg.line.clr.pattern, 0.75, 0.0, 0.0, 1.0);

    for (auto& d : m_data)
    {
        struct kdata* d1 = kdata_array_alloc(reinterpret_cast<kpair*>(&d.data[0]), d.data.size());
        switch (d.type)
        {
        case chart_type::points:
            kplot_attach_data(p, d1, KPLOT_POINTS, &dcfg);
            break;
        case chart_type::marks:
            kplot_attach_data(p, d1, KPLOT_MARKS, &dcfg);
            break;
        case chart_type::lines:
            kplot_attach_data(p, d1, KPLOT_LINES, &dcfg);
            break;
        case chart_type::linespoints:
            kplot_attach_data(p, d1, KPLOT_LINESPOINTS, &dcfg);
            break;
        case chart_type::linesmarks:
            kplot_attach_data(p, d1, KPLOT_LINESMARKS, &dcfg);
            break;
        }
        kdata_destroy(d1);
    }

    detail::code_timer(false, "kplot_draw: ", [&]()
    {
        kplot_draw(p, b.width(), b.height(), cr.get());
    });

    kplot_free(p);
}

PieChart::PieChart(const Rect& rect)
    : Widget(rect)
{
    set_name("PieChart" + std::to_string(m_widgetid));

    set_boxtype(Theme::boxtype::fill);

    static const vector<Color> default_colors =
    {
        Palette::red,
        Palette::green,
        Palette::blue,
        Palette::yellow,
        Palette::cyan,
        Palette::magenta,
        Palette::silver,
        Palette::gray,
        Palette::lightgray,
        Palette::maroon,
        Palette::olive,
        Palette::purple,
        Palette::teal,
        Palette::navy,
        Palette::orange,
        Palette::aqua,
        Palette::lightblue,
    };

    m_colors = default_colors;
}

void PieChart::draw(Painter& painter, const Rect& rect)
{
    detail::ignoreparam(rect);

    draw_box(painter, Palette::ColorId::bg, Palette::ColorId::border);

    const auto b = content_area();

    Painter::AutoSaveRestore sr(painter);
    auto cr = painter.context();

    float to_angle = M_PI * 3 / 2;
    float from_angle = to_angle;

    // keep everything square
    auto width = std::min(b.width(), b.height());
    auto height = std::min(b.width(), b.height());

    auto c = b.center();

    auto colorit = m_colors.begin();

    for (auto& i : m_data)
    {
        to_angle += 2 * M_PI * i.second;

        Color color = *colorit;
        if (++colorit == m_colors.end())
            colorit = m_colors.begin();

        painter.set(color);
        cairo_move_to(cr.get(), c.x(), c.y());
        cairo_arc(cr.get(), c.x(), c.y(), width * .45, from_angle, to_angle);
        cairo_line_to(cr.get(), c.x(), c.y());
        cairo_fill(cr.get());
        from_angle = to_angle;
    }

    cairo_set_font_size(cr.get(), width / 30.);
    to_angle = M_PI * 3. / 2.;
    from_angle = to_angle;
    for (auto& i : m_data)
    {
        if (i.second < 0.01)
            continue;

        to_angle += 2 * M_PI * i.second;
        auto label_angle = (from_angle + to_angle) / 2.;
        auto label_x = width / 2. * (1.0 + 0.7 * std::cos(label_angle));
        auto label_y = height / 2. * (1.0 + 0.7 * std::sin(label_angle));
        painter.set(Palette::black);
        cairo_move_to(cr.get(), c.x() - width / 2. + label_x, c.y() - height / 2. + label_y);
        cairo_show_text(cr.get(), i.first.c_str());
        cairo_fill(cr.get());
        from_angle = to_angle;
    }
}

}

}
}
