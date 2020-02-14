/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <egt/detail/string.h>

using namespace std;
using namespace egt;
using namespace egt::experimental;

static vector<pair<EasingFunc, string>> easing_functions =
{
    {easing_linear, "linear"},
    {easing_easy, "easy"},
    {easing_easy_slow, "easy slow"},
    {easing_extend, "extend"},
    {easing_drop, "drop"},
    {easing_drop_slow, "drop slow"},
    {easing_snap, "snap"},
    {easing_bounce, "bounce"},
    {easing_bouncy, "bouncy"},
    {easing_rubber, "rubber"},
    {easing_spring, "spring"},
    {easing_boing, "boing"},
    {easing_quadratic_easein, "quadratic easein"},
    {easing_quadratic_easeout, "quadratic easeout"},
    {easing_quadratic_easeinout, "quadratic easeinout"},
    {easing_cubic_easein, "cubic easein"},
    {easing_cubic_easeout, "cubic easeout"},
    {easing_cubic_easeinout, "cubic easeinout"},
    {easing_quartic_easein, "quartic easein"},
    {easing_quartic_easeout, "quartic easeout"},
    {easing_quartic_easeinout, "quartic easeinout"},
    {easing_quintic_easein, "quintic easein"},
    {easing_quintic_easeout, "quintic easeout"},
    {easing_quintic_easeinout, "quintic easeinout"},
    {easing_sine_easein, "sine easein"},
    {easing_sine_easeout, "sine easeout"},
    {easing_sine_easeinout, "sine easeinout"},
    {easing_circular_easein, "circular easein"},
    {easing_circular_easeout, "circular easeout"},
    {easing_circular_easeinout, "circular easeinout"},
    {easing_exponential_easein, "exponential easein"},
    {easing_exponential_easeout, "exponential easeout"},
    {easing_exponential_easeinout, "exponential easeinout"},
    {easing_cubic_bezier(), "cubic bezier"}
};

static LineChart::DataArray create_data(EasingFunc easing)
{
    LineChart::DataArray data;
    for (float i = 0.; i <= 1.; i += .001)
        data.emplace_back(i, detail::interpolate(easing, i, 0.f, 100.f));
    return data;
}

int main(int argc, char** argv)
{
    Application app(argc, argv);

    TopWindow window;
    LineChart line;
    //line.set_color(Palette::ColorId::bg, Palette::black);
    //line.set_color(Palette::ColorId::border, Palette::white);
    window.add(expand(line));
    window.show();

    for (auto& e : easing_functions)
    {
        line.clear();
        line.data(create_data(e.first));

        window.begin_draw();
        app.paint_to_file(detail::replace_all(e.second, " ", "_") + ".png");
    }

    return app.run();
}
