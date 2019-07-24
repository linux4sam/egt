/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <egt/detail/svg.h>

using namespace std;
using namespace egt;
using namespace egt::experimental;

template<class T>
static void demo_up_down_animator(std::shared_ptr<T> widget, int min, int max,
                                  std::chrono::seconds length = std::chrono::seconds(10))
{
    auto animationup = std::make_shared<PropertyAnimator>(min, max, length, easing_circular_easein);
    animationup->on_change(std::bind(&T::set_value, std::ref(*widget), std::placeholders::_1));

    auto animationdown = std::make_shared<PropertyAnimator>(max, min, length, easing_circular_easeout);
    animationdown->on_change(std::bind(&T::set_value, std::ref(*widget), std::placeholders::_1));

    /// @todo leak
    auto sequence = new AnimationSequence(true);
    sequence->add(animationup);
    sequence->add(animationdown);
    sequence->start();
}

int main(int argc, const char** argv)
{
    Application app(argc, argv, "dash");

    TopWindow win;
    win.set_color(Palette::ColorId::bg, Color::css("#1b1d43"));

    auto logo = std::make_shared<ImageLabel>(Image("@128px/egt_logo_white.png"));
    logo->set_margin(10);
    win.add(top(left(logo)));

    auto f = 1.50375;

    // the gauge
    Gauge gauge;
    center(gauge);

    // create a background layer
    auto gauge_background = make_shared<GaugeLayer>(Image(detail::load_svg("dash_background.svg",
                            SizeF(1203, 593) / SizeF(f, f),
                            "#background")));
    gauge_background->set_gauge(&gauge);
    gauge.add_layer(gauge_background);

    // create the right blink layer
    auto right_blink = make_shared<GaugeLayer>(Image(detail::load_svg("dash_background.svg",
                       SizeF(1203, 593) / SizeF(f, f),
                       "#right_blink")));
    right_blink->set_gauge(&gauge);
    right_blink->set_visible(false);
    gauge.add_layer(right_blink);

    PeriodicTimer right_timer(std::chrono::milliseconds(1500));
    right_timer.on_timeout([right_blink]()
    {
        right_blink->visible_toggle();
    });
    right_timer.start();

    // create the left blink layer
    auto left_blink = make_shared<GaugeLayer>(Image(detail::load_svg("dash_background.svg",
                      SizeF(1203, 593) / SizeF(f, f),
                      "#left_blink")));
    left_blink->set_gauge(&gauge);
    left_blink->set_visible(false);
    gauge.add_layer(left_blink);

    PeriodicTimer left_timer(std::chrono::milliseconds(1000));
    left_timer.on_timeout([left_blink]()
    {
        left_blink->visible_toggle();
    });
    left_timer.start();

    // create the brights layer
    auto brights = make_shared<GaugeLayer>(Image(detail::load_svg("dash_background.svg",
                                           SizeF(1203, 593) / SizeF(f, f),
                                           "#brights")));
    brights->set_gauge(&gauge);
    brights->set_visible(false);
    gauge.add_layer(brights);

    PeriodicTimer brights_timer(std::chrono::seconds(5));
    brights_timer.on_timeout([brights]()
    {
        brights->visible_toggle();
    });
    brights_timer.start();

    // create the hazards layer
    auto hazards = make_shared<GaugeLayer>(Image(detail::load_svg("dash_background.svg",
                                           SizeF(1203, 593) / SizeF(f, f),
                                           "#hazards")));
    hazards->set_gauge(&gauge);
    hazards->set_visible(false);
    gauge.add_layer(hazards);

    PeriodicTimer hazards_timer(std::chrono::seconds(2));
    hazards_timer.on_timeout([hazards]()
    {
        hazards->set_visible(!hazards->visible());
    });
    hazards_timer.start();

    auto rpm_needle = make_shared<NeedleLayer>(Image(detail::load_svg("dash_needle.svg", SizeF(102, 8) / SizeF(f, f))),
                      1000, 6000, 171.34, 8.6);
    rpm_needle->set_gauge(&gauge);
    rpm_needle->set_needle_point(PointF(284.49, 339.51) / PointF(f, f));
    rpm_needle->set_needle_center(PointF(4.8, 3.9) / PointF(f, f));
    gauge.add_layer(rpm_needle);
    demo_up_down_animator(rpm_needle, 1000, 6000, std::chrono::seconds(8));

    auto mph_needle = make_shared<NeedleLayer>(Image(detail::load_svg("dash_needle.svg", SizeF(102, 8) / SizeF(f, f))),
                      20, 220, 171.34, 8.6);
    mph_needle->set_gauge(&gauge);
    mph_needle->set_needle_point(PointF(918.40, 339.51) / PointF(f, f));
    mph_needle->set_needle_center(PointF(4.8, 3.9) / PointF(f, f));
    gauge.add_layer(mph_needle);
    demo_up_down_animator(mph_needle, 20, 220, std::chrono::seconds(8));

    auto fuel_needle = make_shared<NeedleLayer>(Image(detail::load_svg("dash_needle.svg", SizeF(102, 8) / SizeF(f, f))),
                       0, 100, 180, 270);
    fuel_needle->set_gauge(&gauge);
    fuel_needle->set_needle_point(PointF(641.29, 359.43) / PointF(f, f));
    fuel_needle->set_needle_center(PointF(4.8, 3.9) / PointF(f, f));
    gauge.add_layer(fuel_needle);
    demo_up_down_animator(fuel_needle, 0, 100, std::chrono::seconds(30));

    win.add(gauge);

    win.show();

    return app.run();
}
