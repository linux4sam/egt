/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mui/chart.h"
#include <cmath>
#include <vector>
#ifdef HAVE_KPLOT
#include "kplot.h"
#endif

using namespace std;

namespace mui
{
#ifdef HAVE_KPLOT
    static void xticlabelfmt(double v, char* b, size_t)
    {
        sprintf(b, "%.02f", v);
    }

    LineChart::LineChart(const Rect& rect)
        : Widget(rect)
    {}

    void LineChart::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(painter);
        ignoreparam(rect);

        struct kdata* d1;
        struct kplot* p;

        d1 = kdata_array_alloc(reinterpret_cast<kpair*>(&m_data[0]), m_data.size());
        p = kplot_alloc(NULL);
        struct kplotcfg* cfg = kplot_get_plotcfg(p);
        cfg->xticlabelfmt = xticlabelfmt;
        cfg->yticlabelfmt = xticlabelfmt;
        auto tc = palette().color(Palette::BORDER);
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

        auto cr = screen()->context();

        cairo_save(cr.get());

        struct kdatacfg	 dcfg;
        kdatacfg_defaults(&dcfg);
        dcfg.line.clr.type = KPLOTCTYPE_PATTERN;
        dcfg.line.clr.pattern =
            cairo_pattern_create_linear(0.0, 0.0, 600.0, 400.0);

        cairo_pattern_add_color_stop_rgb
        (dcfg.line.clr.pattern, 0.25, 1.0, 0.0, 0.0);
        cairo_pattern_add_color_stop_rgb
        (dcfg.line.clr.pattern, 0.5, 0.0, 1.0, 0.0);
        cairo_pattern_add_color_stop_rgb
        (dcfg.line.clr.pattern, 0.75, 0.0, 0.0, 1.0);

        kplot_attach_data(p, d1, KPLOT_LINES, &dcfg);

        auto bg = palette().color(Palette::BG);
        cairo_set_source_rgba(cr.get(),
                              bg.redf(),
                              bg.greenf(),
                              bg.bluef(),
                              bg.alphaf());

        cairo_rectangle(cr.get(), x(), y(), w(), h());
        cairo_fill(cr.get());
        kplot_draw(p, w(), h(), cr.get());

        kdata_destroy(d1);
        kplot_free(p);
        cairo_restore(cr.get());
    }
#endif

    PieChart::PieChart(const Rect& rect)
        : Widget(rect)
    {}

    void PieChart::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(painter);
        ignoreparam(rect);

        static const vector<Color> piecolors =
        {
            Color::RED,
            Color::GREEN,
            Color::BLUE,
            Color::YELLOW,
            Color::CYAN,
            Color::MAGENTA,
            Color::SILVER,
            Color::GRAY,
            Color::LIGHTGRAY,
            Color::MAROON,
            Color::OLIVE,
            Color::PURPLE,
            Color::TEAL,
            Color::NAVY,
            Color::ORANGE,
            Color::AQUA,
            Color::LIGHTBLUE,
        };

        shared_cairo_t cr = screen()->context();

        cairo_save(cr.get());

        float to_angle = M_PI * 3 / 2;
        float from_angle = to_angle;

        auto c = piecolors.begin();

        for (auto& i : m_data)
        {
            to_angle += 2 * M_PI * i.second;

            Color color = *c;
            if (++c == piecolors.end())
                c = piecolors.begin();

            cairo_set_source_rgba(cr.get(),
                                  color.redf(),
                                  color.greenf(),
                                  color.bluef(),
                                  color.alphaf());
            cairo_move_to(cr.get(), x() + w() / 2, y() + h() / 2);
            cairo_arc(cr.get(), x() + w() / 2, y() + h() / 2, w()*.45, from_angle, to_angle);
            cairo_line_to(cr.get(), x() + w() / 2, y() + h() / 2);
            cairo_fill(cr.get());
            from_angle = to_angle;
        }

        cairo_set_font_size(cr.get(), w() / 30);
        to_angle = M_PI * 3 / 2;
        from_angle = to_angle;
        for (auto& i : m_data)
        {
            if (i.second < 0.01)
                continue;

            to_angle += 2 * M_PI * i.second;
            float label_angle = (from_angle + to_angle) / 2;
            int label_x = w() / 2 * (1.0 + 0.7 * cosf(label_angle));
            int label_y = h() / 2 * (1.0 + 0.7 * sinf(label_angle));
            cairo_set_source_rgb(cr.get(), 0, 0, 0);
            cairo_move_to(cr.get(), x() + label_x, y() + label_y);
            cairo_show_text(cr.get(), i.first.c_str());
            cairo_fill(cr.get());
            from_angle = to_angle;
        }

        cairo_restore(cr.get());
    }

}
