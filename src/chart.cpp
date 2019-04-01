/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/chart.h"
#include "egt/painter.h"
#include "egt/screen.h"
#include <cmath>
#include <vector>
#ifdef HAVE_KPLOT
#include "kplot.h"
#endif

using namespace std;

namespace egt
{
inline namespace v1
{

#ifdef HAVE_KPLOT
static void xticlabelfmt(double v, char* b, size_t)
{
    sprintf(b, "%.02f", v);
}

LineChart::LineChart(const Rect& rect)
    : Widget(rect)
{
    set_name("LineChart" + std::to_string(m_widgetid));
}

void LineChart::draw(Painter& painter, const Rect& rect)
{
    ignoreparam(rect);

    Painter::AutoSaveRestore sr(painter);

    struct kplot* p;

    p = kplot_alloc(NULL);
    struct kplotcfg* cfg = kplot_get_plotcfg(p);
    cfg->grid = m_grid;
    cfg->xticlabelfmt = xticlabelfmt;
    cfg->yticlabelfmt = xticlabelfmt;
    auto tc = palette().color(Palette::ColorId::border);
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

    auto cr = painter.context();

    struct kdatacfg	 dcfg;
    kdatacfg_defaults(&dcfg);
    dcfg.line.sz = m_linewidth;
    dcfg.line.clr.type = KPLOTCTYPE_PATTERN;
    dcfg.line.clr.pattern =
        cairo_pattern_create_linear(0.0, 0.0, 600.0, 400.0);

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

    auto bg = palette().color(Palette::ColorId::bg);
    painter.set(bg);

    cairo_rectangle(cr.get(), x(), y(), w(), h());
    cairo_fill(cr.get());

    experimental::code_timer(false, "kplot_draw: ", [&]()
    {
        kplot_draw(p, w(), h(), cr.get());
    });

    kplot_free(p);
}
#endif

PieChart::PieChart(const Rect& rect)
    : Widget(rect)
{
    set_name("PieChart" + std::to_string(m_widgetid));

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
    // keep everything square
    int width = std::min(w(), h());
    int height = std::min(w(), h());

    ignoreparam(painter);
    ignoreparam(rect);

    shared_cairo_t cr = screen()->context();

    cairo_save(cr.get());

    float to_angle = M_PI * 3 / 2;
    float from_angle = to_angle;

    auto c = m_colors.begin();

    for (auto& i : m_data)
    {
        to_angle += 2 * M_PI * i.second;

        Color color = *c;
        if (++c == m_colors.end())
            c = m_colors.begin();

        painter.set(color);
        cairo_move_to(cr.get(), x() + width / 2, y() + height / 2);
        cairo_arc(cr.get(), x() + width / 2, y() + height / 2, width * .45, from_angle, to_angle);
        cairo_line_to(cr.get(), x() + width / 2, y() + height / 2);
        cairo_fill(cr.get());
        from_angle = to_angle;
    }

    cairo_set_font_size(cr.get(), width / 30);
    to_angle = M_PI * 3 / 2;
    from_angle = to_angle;
    for (auto& i : m_data)
    {
        if (i.second < 0.01)
            continue;

        to_angle += 2 * M_PI * i.second;
        float label_angle = (from_angle + to_angle) / 2;
        int label_x = width / 2 * (1.0 + 0.7 * std::cos(label_angle));
        int label_y = height / 2 * (1.0 + 0.7 * std::sin(label_angle));
        painter.set(Palette::black);
        cairo_move_to(cr.get(), x() + label_x, y() + label_y);
        cairo_show_text(cr.get(), i.first.c_str());
        cairo_fill(cr.get());
        from_angle = to_angle;
    }

    cairo_restore(cr.get());
}

}
}
