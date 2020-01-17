/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <chrono>
#include <memory>

using namespace std;
using namespace egt;
using namespace egt::experimental;

template<class T>
static auto demo_up_down_animator(std::shared_ptr<T> widget, int min, int max, std::chrono::seconds length = std::chrono::seconds(10))
{
    auto animationup = std::make_shared<PropertyAnimator>(min, max, length, easing_circular_easein);
    animationup->on_change([widget](PropertyAnimator::Value v) { widget->value(v); });

    auto animationdown = std::make_shared<PropertyAnimator>(max, min, length, easing_circular_easeout);
    animationdown->on_change([widget](PropertyAnimator::Value v) { widget->value(v); });

    auto sequence = std::make_unique<AnimationSequence>(true);
    sequence->add(animationup);
    sequence->add(animationdown);
    sequence->start();
    return sequence;
}

int main(int argc, const char** argv)
{
    Application app(argc, argv, "gauge");

    TopWindow win;
    win.color(Palette::ColorId::bg, Palette::black);

    auto logo = std::make_shared<ImageLabel>(Image("@128px/egt_logo_white.png"));
    logo->margin(10);
    win.add(top(center(logo)));

    StaticGrid grid(std::make_tuple(3, 2));
    win.add(expand(grid));

    vector<std::unique_ptr<AnimationSequence>> animations;

    Gauge gauge;
    auto gauge_background = make_shared<GaugeLayer>(SvgImage("gauge_background.svg", SizeF(192, 192)));
    gauge.add_layer(gauge_background);

    auto gauge_needle1 = make_shared<NeedleLayer>(SvgImage("gauge_needle2.svg", SizeF(91, 14)),
                         0, 100, 135, 45);
    gauge_needle1->needle_point(PointF(192, 192) / PointF(2.0, 2.0));
    gauge_needle1->needle_center(PointF(6.8725, 6.87253));
    gauge.add_layer(gauge_needle1);
    animations.push_back(demo_up_down_animator(gauge_needle1, 0, 100, std::chrono::seconds(5)));

    auto gauge_needle2 = make_shared<NeedleLayer>(SvgImage("gauge_needle1.svg", SizeF(91, 14)),
                         0, 100, 135, 45);
    gauge_needle2->needle_point(PointF(192, 192) / PointF(2.0, 2.0));
    gauge_needle2->needle_center(PointF(6.8725, 6.87253));
    gauge.add_layer(gauge_needle2);
    animations.push_back(demo_up_down_animator(gauge_needle2, 0, 100, std::chrono::seconds(8)));
    grid.add(center(gauge));

    Gauge fuel;
    SvgImage fuelsvg("fuel_background.svg", SizeF(0, 150));

    auto fuel_background = make_shared<GaugeLayer>(fuelsvg.render("#background"));
    fuel.add_layer(fuel_background);

    auto fuel_needle_box = fuelsvg.id_box("#needle");
    auto fuel_needle = make_shared<NeedleLayer>(fuelsvg.render("#needle", fuel_needle_box),
                       0, 100, -63, 63, true);

    auto needle_point = fuelsvg.id_box("#needlepoint").center();
    fuel_needle->needle_point(needle_point);
    fuel_needle->needle_center(needle_point - fuel_needle_box.point());
    fuel.add_layer(fuel_needle);
    animations.push_back(demo_up_down_animator(fuel_needle, 0, 100, std::chrono::seconds(10)));
    grid.add(center(fuel));

    Gauge custom1;
    SvgImage custom1svg("custom1_gauge.svg", SizeF(200, 0));

    auto custom1_background = make_shared<GaugeLayer>(custom1svg.render("#g82"));
    custom1.add_layer(custom1_background);

    auto custom1_needle_box = custom1svg.id_box("#needle");
    auto custom1_needle = make_shared<NeedleLayer>(custom1svg.render("#needle", custom1_needle_box),
                          0, 100, 8, 150, true);
    auto custom1_needle_point = custom1svg.id_box("#needlepoint").center();
    custom1_needle->needle_point(custom1_needle_point);
    custom1_needle->needle_center(custom1_needle_point - custom1_needle_box.point());
    custom1.add_layer(custom1_needle);
    animations.push_back(demo_up_down_animator(custom1_needle, 0, 100, std::chrono::seconds(10)));
    grid.add(center(custom1));

    Gauge airspeed;
    auto airspeed_background = make_shared<GaugeLayer>(SvgImage("airspeed_background.svg", SizeF(400, 400) / SizeF(2, 2)));
    airspeed.add_layer(airspeed_background);
    auto airspeed_needle = make_shared<NeedleLayer>(SvgImage("airspeed_needle.svg", SizeF(168, 17) / SizeF(2, 2)),
                           0, 180, 270, 222);
    airspeed_needle->needle_point(PointF(200, 200) / PointF(2, 2));
    airspeed_needle->needle_center(PointF(8, 8.3) / PointF(2, 2));
    airspeed.add_layer(airspeed_needle);
    animations.push_back(demo_up_down_animator(airspeed_needle, 0, 180, std::chrono::seconds(7)));
    grid.add(center(airspeed));

    Gauge speedometer;
    auto speedometer_background = make_shared<GaugeLayer>(SvgImage("speedometer_background.svg", SizeF(913, 908) / SizeF(4.6, 4.6)));
    speedometer.add_layer(speedometer_background);
    auto speedometer_needle = make_shared<NeedleLayer>(SvgImage("speedometer_needle.svg", SizeF(467, 376) / SizeF(4.6, 4.6)),
                              0, 110, 136.52, 41.80);
    speedometer_needle->needle_point(PointF(913, 908) / PointF(4.6, 4.6) / PointF(2, 2));
    speedometer_needle->needle_center(PointF(85.88, 185.44) / PointF(4.6, 4.6));
    speedometer.add_layer(speedometer_needle);
    animations.push_back(demo_up_down_animator(speedometer_needle, 0, 110, std::chrono::seconds(12)));
    grid.add(center(speedometer));

    win.show();

    return app.run();
}
