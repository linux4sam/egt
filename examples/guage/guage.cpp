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
static void demo_up_down_animator(std::shared_ptr<T> widget, int min, int max, std::chrono::seconds length = std::chrono::seconds(10))
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
    Application app(argc, argv, "guage");

    TopWindow win;
    win.set_color(Palette::ColorId::bg, Palette::gray);

    StaticGrid grid(Tuple(2, 2));
    grid.set_color(Palette::ColorId::border, Palette::transparent);
    win.add(expand(grid));

    Guage guage;
    center(guage);
    auto guage_background = make_shared<GuageLayer>(Image(detail::load_svg("guage_background.svg", SizeF(192, 192))));
    guage_background->set_guage(&guage);
    guage.add_layer(guage_background);

    auto guage_needle1 = make_shared<NeedleLayer>(Image(detail::load_svg("guage_needle2.svg", SizeF(91, 14))),
                         0, 100, 135, 45);
    guage_needle1->set_needle_point(PointF(192, 192) / PointF(2.0, 2.0));
    guage_needle1->set_needle_center(PointF(6.8725, 6.87253));
    guage_needle1->set_guage(&guage);
    guage.add_layer(guage_needle1);
    demo_up_down_animator(guage_needle1, 0, 100, std::chrono::seconds(5));

    auto guage_needle2 = make_shared<NeedleLayer>(Image(detail::load_svg("guage_needle1.svg", SizeF(91, 14))),
                         0, 100, 135, 45);
    guage_needle2->set_needle_point(PointF(192, 192) / PointF(2.0, 2.0));
    guage_needle2->set_needle_center(PointF(6.8725, 6.87253));
    guage_needle2->set_guage(&guage);
    guage.add_layer(guage_needle2);
    demo_up_down_animator(guage_needle2, 0, 100, std::chrono::seconds(8));

    grid.add(guage);

    Guage fuel;
    center(fuel);
    auto fuel_background = make_shared<GuageLayer>(Image(detail::load_svg("fuel_background.svg", SizeF(198, 153))));
    fuel_background->set_guage(&fuel);
    fuel.add_layer(fuel_background);
    auto fuel_needle = make_shared<NeedleLayer>(Image(detail::load_svg("fuel_needle.svg", SizeF(72, 6))),
                       0, 100, 60, 300, false);
    fuel_needle->set_needle_point(PointF(415.73, 382.09) / PointF(5.0, 5.0));
    fuel_needle->set_needle_center(PointF(3, 3));
    fuel_needle->set_guage(&fuel);
    fuel.add_layer(fuel_needle);
    demo_up_down_animator(fuel_needle, 0, 100, std::chrono::seconds(10));
    grid.add(fuel);

    Guage airspeed;
    center(airspeed);
    auto airspeed_background = make_shared<GuageLayer>(Image(detail::load_svg("airspeed_background.svg", SizeF(400, 400) / SizeF(2, 2))));
    airspeed_background->set_guage(&airspeed);
    airspeed.add_layer(airspeed_background);
    auto airspeed_needle = make_shared<NeedleLayer>(Image(detail::load_svg("airspeed_needle.svg", SizeF(168, 17) / SizeF(2, 2))),
                           0, 180, 270, 222);
    airspeed_needle->set_needle_point(PointF(200, 200) / PointF(2, 2));
    airspeed_needle->set_needle_center(PointF(8, 8.3) / PointF(2, 2));
    airspeed_needle->set_guage(&airspeed);
    airspeed.add_layer(airspeed_needle);
    demo_up_down_animator(airspeed_needle, 0, 180, std::chrono::seconds(7));
    grid.add(airspeed);

    Guage speedometer;
    center(speedometer);
    auto speedometer_background = make_shared<GuageLayer>(Image(detail::load_svg("speedometer_background.svg", SizeF(913, 908) / SizeF(4.6, 4.6))));
    speedometer_background->set_guage(&speedometer);
    speedometer.add_layer(speedometer_background);
    auto speedometer_needle = make_shared<NeedleLayer>(Image(detail::load_svg("speedometer_needle.svg", SizeF(467, 376) / SizeF(4.6, 4.6))),
                              0, 110, 136.52, 41.80);
    speedometer_needle->set_needle_point(PointF(913, 908) / PointF(4.6, 4.6) / PointF(2, 2));
    speedometer_needle->set_needle_center(PointF(85.88, 185.44) / PointF(4.6, 4.6));
    speedometer_needle->set_guage(&speedometer);
    speedometer.add_layer(speedometer_needle);
    demo_up_down_animator(speedometer_needle, 0, 110, std::chrono::seconds(12));
    grid.add(speedometer);

    win.show();

    return app.run();
}
