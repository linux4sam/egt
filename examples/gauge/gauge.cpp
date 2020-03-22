/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <chrono>
#include <egt/ui>
#include <memory>

template<class T>
static auto demo_up_down_animator(std::shared_ptr<T> widget, int min, int max,
                                  std::chrono::seconds length = std::chrono::seconds(10))
{
    auto animationup =
        std::make_shared<egt::PropertyAnimator>(min, max, length,
                egt::easing_circular_easein);
    animationup->on_change([widget](egt::PropertyAnimator::Value v)
    {
        widget->value(v);
    });

    auto animationdown =
        std::make_shared<egt::PropertyAnimator>(max, min, length,
                egt::easing_circular_easeout);
    animationdown->on_change([widget](egt::PropertyAnimator::Value v)
    {
        widget->value(v);
    });

    auto sequence = std::make_unique<egt::AnimationSequence>(true);
    sequence->add(animationup);
    sequence->add(animationdown);
    sequence->start();
    return sequence;
}

int main(int argc, char** argv)
{
    egt::Application app(argc, argv, "gauge");
#ifdef EXAMPLEDATA
    egt::add_search_path(EXAMPLEDATA);
#endif

    egt::TopWindow win;
    win.color(egt::Palette::ColorId::bg, egt::Palette::black);

    auto logo = std::make_shared<egt::ImageLabel>(egt::Image("icon:egt_logo_white.png;128"));
    logo->margin(10);
    win.add(top(center(logo)));

    egt::StaticGrid grid(std::make_tuple(3, 2));
    win.add(expand(grid));

    std::vector<std::unique_ptr<egt::AnimationSequence>> animations;

    egt::experimental::Gauge gauge;
    auto gauge_background = std::make_shared<egt::experimental::GaugeLayer>(egt::SvgImage("file:gauge_background.svg", egt::SizeF(192, 192)));
    gauge.add(gauge_background);

    auto gauge_needle1 = std::make_shared<egt::experimental::NeedleLayer>(egt::SvgImage("file:gauge_needle2.svg", egt::SizeF(91, 14)),
                         0, 100, 135, 45);
    gauge_needle1->needle_point(egt::PointF(192, 192) / egt::PointF(2.0, 2.0));
    gauge_needle1->needle_center(egt::PointF(6.8725, 6.87253));
    gauge.add(gauge_needle1);
    animations.push_back(demo_up_down_animator(gauge_needle1, 0, 100, std::chrono::seconds(5)));

    auto gauge_needle2 = std::make_shared<egt::experimental::NeedleLayer>(egt::SvgImage("file:gauge_needle1.svg", egt::SizeF(91, 14)),
                         0, 100, 135, 45);
    gauge_needle2->needle_point(egt::PointF(192, 192) / egt::PointF(2.0, 2.0));
    gauge_needle2->needle_center(egt::PointF(6.8725, 6.87253));
    gauge.add(gauge_needle2);
    animations.push_back(demo_up_down_animator(gauge_needle2, 0, 100, std::chrono::seconds(8)));
    grid.add(center(gauge));

    egt::experimental::Gauge fuel;
    egt::SvgImage fuelsvg("file:fuel_background.svg", egt::SizeF(0, 150));

    auto fuel_background = std::make_shared<egt::experimental::GaugeLayer>(fuelsvg.render("#background"));
    fuel.add(fuel_background);

    auto fuel_needle_box = fuelsvg.id_box("#needle");
    auto fuel_needle = std::make_shared<egt::experimental::NeedleLayer>(fuelsvg.render("#needle", fuel_needle_box),
                       0, 100, -63, 63, true);

    auto needle_point = fuelsvg.id_box("#needlepoint").center();
    fuel_needle->needle_point(needle_point);
    fuel_needle->needle_center(needle_point - fuel_needle_box.point());
    fuel.add(fuel_needle);
    animations.push_back(demo_up_down_animator(fuel_needle, 0, 100, std::chrono::seconds(10)));
    grid.add(center(fuel));

    egt::experimental::Gauge custom1;
    egt::SvgImage custom1svg("file:custom1_gauge.svg", egt::SizeF(200, 0));

    auto custom1_background = std::make_shared<egt::experimental::GaugeLayer>(custom1svg.render("#g82"));
    custom1.add(custom1_background);

    auto custom1_needle_box = custom1svg.id_box("#needle");
    auto custom1_needle = std::make_shared<egt::experimental::NeedleLayer>(custom1svg.render("#needle", custom1_needle_box),
                          0, 100, 8, 150, true);
    auto custom1_needle_point = custom1svg.id_box("#needlepoint").center();
    custom1_needle->needle_point(custom1_needle_point);
    custom1_needle->needle_center(custom1_needle_point - custom1_needle_box.point());
    custom1.add(custom1_needle);
    animations.push_back(demo_up_down_animator(custom1_needle, 0, 100, std::chrono::seconds(10)));
    grid.add(center(custom1));

    egt::experimental::Gauge airspeed;
    auto airspeed_background = std::make_shared<egt::experimental::GaugeLayer>(egt::SvgImage("file:airspeed_background.svg", egt::SizeF(400, 400) / egt::SizeF(2, 2)));
    airspeed.add(airspeed_background);
    auto airspeed_needle = std::make_shared<egt::experimental::NeedleLayer>(egt::SvgImage("file:airspeed_needle.svg", egt::SizeF(168, 17) / egt::SizeF(2, 2)),
                           0, 180, 270, 222);
    airspeed_needle->needle_point(egt::PointF(200, 200) / egt::PointF(2, 2));
    airspeed_needle->needle_center(egt::PointF(8, 8.3) / egt::PointF(2, 2));
    airspeed.add(airspeed_needle);
    animations.push_back(demo_up_down_animator(airspeed_needle, 0, 180, std::chrono::seconds(7)));
    grid.add(center(airspeed));

    egt::experimental::Gauge speedometer;
    auto speedometer_background = std::make_shared<egt::experimental::GaugeLayer>(egt::SvgImage("file:speedometer_background.svg", egt::SizeF(913, 908) / egt::SizeF(4.6, 4.6)));
    speedometer.add(speedometer_background);
    auto speedometer_needle = std::make_shared<egt::experimental::NeedleLayer>(egt::SvgImage("file:speedometer_needle.svg", egt::SizeF(467, 376) / egt::SizeF(4.6, 4.6)),
                              0, 110, 136.52, 41.80);
    speedometer_needle->needle_point(egt::PointF(913, 908) / egt::PointF(4.6, 4.6) / egt::PointF(2, 2));
    speedometer_needle->needle_center(egt::PointF(85.88, 185.44) / egt::PointF(4.6, 4.6));
    speedometer.add(speedometer_needle);
    animations.push_back(demo_up_down_animator(speedometer_needle, 0, 110, std::chrono::seconds(12)));
    grid.add(center(speedometer));

    win.show();

    return app.run();
}
