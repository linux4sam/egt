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
                                  std::chrono::milliseconds duration = std::chrono::seconds(10),
                                  const egt::EasingFunc& easingin = egt::easing_circular_easein,
                                  const egt::EasingFunc& easingout = egt::easing_circular_easeout)
{
    auto animationup = std::make_shared<egt::PropertyAnimator>(min, max, duration, easingin);
    animationup->on_change([widget](egt::PropertyAnimator::Value value) { widget->value(value); });

    auto animationdown = std::make_shared<egt::PropertyAnimator>(max, min, duration, easingout);
    animationdown->on_change([widget](egt::PropertyAnimator::Value value) { widget->value(value); });

    auto delay = std::make_shared<egt::AnimationDelay>(std::chrono::milliseconds(500));

    auto sequence = std::make_unique<egt::AnimationSequence>(true);
    sequence->add(animationup);
    sequence->add(animationdown);
    sequence->add(delay);
    sequence->start();
    return sequence;
}

static std::shared_ptr<egt::experimental::NeedleLayer> create_needle(egt::experimental::Gauge& gauge,
        egt::SvgImage& svg,
        const std::string& id, const std::string& point_id,
        int min, int max, int min_angle, int max_angle,
        std::chrono::milliseconds duration,
        std::vector<std::unique_ptr<egt::AnimationSequence>>& animations,
        const egt::EasingFunc& easing = egt::easing_circular_easein)

{
    if (!svg.id_exists(id) || !svg.id_exists(point_id))
        return nullptr;

    auto needle_box = svg.id_box(id);
    auto needle = std::make_shared<egt::experimental::NeedleLayer>(svg.render(id, needle_box),
                  min, max, min_angle, max_angle);
    auto needle_point = svg.id_box(point_id).center();
    needle->needle_point(needle_point);
    needle->needle_center(needle_point - needle_box.point());
    gauge.add(needle);

    animations.push_back(demo_up_down_animator(needle, min, max, duration, easing));

    return needle;
}

static std::shared_ptr<egt::experimental::GaugeLayer>
create_layer(egt::experimental::Gauge& gauge, egt::SvgImage& svg,
             const std::string& id,
             std::chrono::milliseconds duration,
             std::vector<std::unique_ptr<egt::PeriodicTimer>>& timers)
{
    if (!svg.id_exists(id))
        return nullptr;

    auto box = svg.id_box(id);
    auto layer = std::make_shared<egt::experimental::GaugeLayer>(svg.render(id, box));
    layer->box(egt::Rect(std::floor(box.x()),
                         std::floor(box.y()),
                         std::ceil(box.width()),
                         std::ceil(box.height())));
    layer->hide();
    gauge.add(layer);

    timers.emplace_back(std::make_unique<egt::PeriodicTimer>(duration));
    timers.back()->on_timeout([layer]() { layer->visible_toggle(); });
    timers.back()->start();

    return layer;
}

int main(int argc, char** argv)
{
    egt::Application app(argc, argv);
#ifdef EXAMPLEDATA
    egt::add_search_path(EXAMPLEDATA);
#endif

    egt::TopWindow win;
    win.padding(10);
    win.color(egt::Palette::ColorId::bg, egt::Color::css("#1b1d43"));

    auto logo = std::make_shared<egt::ImageLabel>(egt::Image("icon:egt_logo_white.png;128"));
    win.add(top(left(logo)));

    // the gauge
    egt::experimental::Gauge gauge;
    center(gauge);

    auto dash_background = std::make_unique<egt::SvgImage>("file:dash_background.svg", egt::SizeF(win.content_area().width(), 0));

    // create a background layer
    auto gauge_background = std::make_shared<egt::experimental::GaugeLayer>(dash_background->render("#background"));
    gauge.add(gauge_background);

    std::vector<std::unique_ptr<egt::PeriodicTimer>> timers;

    // pick out some other layers
    auto right_blink = create_layer(gauge, *dash_background, "#right_blink", std::chrono::milliseconds(1500), timers);
    auto left_blink = create_layer(gauge, *dash_background, "#left_blink", std::chrono::seconds(1), timers);
    auto brights = create_layer(gauge, *dash_background, "#brights", std::chrono::seconds(5), timers);
    auto high_brights = create_layer(gauge, *dash_background, "#highbrights", std::chrono::seconds(4), timers);
    auto hazards = create_layer(gauge, *dash_background, "#hazards", std::chrono::seconds(2), timers);
    auto heat = create_layer(gauge, *dash_background, "#heat", std::chrono::seconds(3), timers);

    std::vector<std::unique_ptr<egt::AnimationSequence>> animations;

    // pick out the needles
    auto rpm_needle = create_needle(gauge, *dash_background, "#rpmneedle", "#rpmpoint",
                                    0, 6000, -20, 190, std::chrono::seconds(8), animations, egt::easing_bounce);
    auto mph_needle = create_needle(gauge, *dash_background, "#mphneedle", "#mphpoint",
                                    0, 220, -20, 190, std::chrono::seconds(8), animations);
    auto fuel_needle = create_needle(gauge, *dash_background, "#fuelneedle", "#fuelpoint",
                                     0, 100, 0, 90, std::chrono::seconds(3), animations);

    win.add(gauge);

    // add some labels to show updating text at specific places

    auto rpm_box = dash_background->id_box("#rpm");
    auto rpm_text = std::make_shared<egt::Label>();
    rpm_text->text_align(egt::AlignFlag::center);
    rpm_text->box(egt::Rect(rpm_box.x(), rpm_box.y(), rpm_box.width(), rpm_box.height()));
    rpm_text->color(egt::Palette::ColorId::label_text, egt::Palette::cyan);
    rpm_text->text("Trip 1: 100.5 miles");
    gauge.add(rpm_text);

    auto speed_box = dash_background->id_box("#speed");
    auto speed_text = std::make_shared<egt::Label>();
    speed_text->text_align(egt::AlignFlag::center);
    speed_text->box(egt::Rect(speed_box.x(), speed_box.y(), speed_box.width(), speed_box.height()));
    speed_text->color(egt::Palette::ColorId::label_text, egt::Palette::white);
    speed_text->font(egt::Font(28, egt::Font::Weight::bold));
    speed_text->text("0 mph");
    gauge.add(speed_text);

    mph_needle->on_value_changed([speed_text, mph_needle]()
    {
        speed_text->text(std::to_string(static_cast<int>(mph_needle->value())) + " mph");
    });

    auto middle_box = dash_background->id_box("#middle");
    auto middle_text = std::make_shared<egt::Label>();
    middle_text->text_align(egt::AlignFlag::center);
    middle_text->box(egt::Rect(middle_box.x(), middle_box.y(), middle_box.width(), middle_box.height()));
    middle_text->color(egt::Palette::ColorId::label_text, egt::Palette::aquamarine);
    middle_text->text("98.7 FM");
    gauge.add(middle_text);

    auto console_box = dash_background->id_box("#console");
    auto console_text = std::make_shared<egt::Label>();
    console_text->text_align(egt::AlignFlag::center);
    console_text->box(egt::Rect(console_box.x(), console_box.y(), console_box.width(), console_box.height()));
    console_text->font(egt::Font(75, egt::Font::Weight::bold));
    gauge.add(console_text);

    auto change_state = [&console_text]()
    {
        static size_t state = 0;
        const std::pair<const char*, egt::Color> states[] =
        {
            {"P", egt::Palette::white},
            {"R", egt::Palette::red},
            {"N", egt::Palette::orange},
            {"D", egt::Palette::green},
            {"2", egt::Palette::green},
            {"1", egt::Palette::green},
        };

        console_text->color(egt::Palette::ColorId::label_text, states[state].second);
        console_text->text(states[state].first);
        if (++state >= 6)
            state = 0;
    };
    change_state();

    timers.emplace_back(std::make_unique<egt::PeriodicTimer>(std::chrono::seconds(2)));
    timers.back()->on_timeout(change_state);
    timers.back()->start();

    dash_background.reset(nullptr);

    win.show();

    return app.run();
}
