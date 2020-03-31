/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/detail/string.h>
#include <egt/ui>
#include <string>
#include <vector>

static const std::vector<std::pair<egt::EasingFunc, std::string>> easing_functions =
{
    {egt::easing_linear, "linear"},
    {egt::easing_easy, "easy"},
    {egt::easing_easy_slow, "easy slow"},
    {egt::easing_extend, "extend"},
    {egt::easing_drop, "drop"},
    {egt::easing_drop_slow, "drop slow"},
    {egt::easing_snap, "snap"},
    {egt::easing_bounce, "bounce"},
    {egt::easing_bouncy, "bouncy"},
    {egt::easing_rubber, "rubber"},
    {egt::easing_spring, "spring"},
    {egt::easing_boing, "boing"},
    {egt::easing_quadratic_easein, "quadratic easein"},
    {egt::easing_quadratic_easeout, "quadratic easeout"},
    {egt::easing_quadratic_easeinout, "quadratic easeinout"},
    {egt::easing_cubic_easein, "cubic easein"},
    {egt::easing_cubic_easeout, "cubic easeout"},
    {egt::easing_cubic_easeinout, "cubic easeinout"},
    {egt::easing_quartic_easein, "quartic easein"},
    {egt::easing_quartic_easeout, "quartic easeout"},
    {egt::easing_quartic_easeinout, "quartic easeinout"},
    {egt::easing_quintic_easein, "quintic easein"},
    {egt::easing_quintic_easeout, "quintic easeout"},
    {egt::easing_quintic_easeinout, "quintic easeinout"},
    {egt::easing_sine_easein, "sine easein"},
    {egt::easing_sine_easeout, "sine easeout"},
    {egt::easing_sine_easeinout, "sine easeinout"},
    {egt::easing_circular_easein, "circular easein"},
    {egt::easing_circular_easeout, "circular easeout"},
    {egt::easing_circular_easeinout, "circular easeinout"},
    {egt::easing_exponential_easein, "exponential easein"},
    {egt::easing_exponential_easeout, "exponential easeout"},
    {egt::easing_exponential_easeinout, "exponential easeinout"},
    {egt::easing_cubic_bezier(), "cubic bezier"}
};

static egt::LineChart::DataArray create_data(const egt::EasingFunc& easing)
{
    egt::LineChart::DataArray data;
    for (float i = 0.; i <= 1.; i += .001)
        data.emplace_back(i, egt::detail::interpolate(easing, i, 0.f, 100.f));
    return data;
}

int main(int argc, char** argv)
{
    egt::Application app(argc, argv);

    egt::TopWindow window;
    egt::LineChart line;
    window.add(expand(line));
    window.show();

    for (auto& e : easing_functions)
    {
        line.clear();
        line.data(create_data(e.first));

        window.begin_draw();
        app.paint_to_file(egt::detail::replace_all(e.second, " ", "_") + ".png");
    }

    return app.run();
}
