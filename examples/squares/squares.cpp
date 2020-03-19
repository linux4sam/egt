/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <iostream>
#include <random>
#include <sstream>
#include <vector>

template<typename T, typename RandomGenerator>
T random_item(T start, T end, RandomGenerator& e)
{
    std::uniform_int_distribution<> dist(0, std::distance(start, end) - 1);
    std::advance(start, dist(e));
    return start;
}

int main(int argc, char** argv)
{
    egt::Application app(argc, argv, "squares");

    egt::TopWindow win;

    const egt::Color FUCHSIA(egt::Color::css("#F012BE"));

    egt::VerticalBoxSizer sizer;

    egt::Label label("FPS: ---");
    label.color(egt::Palette::ColorId::text, egt::Palette::black);
    label.color(egt::Palette::ColorId::bg, egt::Palette::transparent);
    sizer.add(expand_horizontal(label));

    label.on_text_changed([&label]()
    {
        std::cout << label.text() << std::endl;
    });

    egt::Label label_dims("");
    label_dims.color(egt::Palette::ColorId::text, egt::Palette::black);
    label_dims.color(egt::Palette::ColorId::bg, egt::Palette::transparent);
    sizer.add(expand_horizontal(label_dims));

    label_dims.on_text_changed([&label_dims]()
    {
        std::cout << label_dims.text() << std::endl;
    });

    egt::Popup popup(egt::Size(100, 80));
    popup.color(egt::Palette::ColorId::bg, FUCHSIA);
    popup.add(sizer);
    egt::expand(sizer);
    egt::top(egt::right(popup));
    win.add(popup);
    popup.show();

    const std::vector<egt::Color> colors1 =
    {
        egt::Palette::red,
        egt::Palette::green,
        egt::Palette::blue,
        egt::Palette::yellow,
        egt::Palette::cyan,
        egt::Palette::magenta,
        egt::Palette::silver,
        egt::Palette::gray,
        egt::Palette::lightgray,
        egt::Palette::maroon,
        egt::Palette::olive,
        egt::Palette::purple,
        egt::Palette::teal,
        egt::Palette::navy,
        egt::Palette::orange,
        egt::Palette::aqua,
        egt::Palette::lightblue,
    };

    const std::vector<egt::Color> colors2 =
    {
        egt::Color(egt::Palette::red, 128),
        egt::Color(egt::Palette::green, 128),
        egt::Color(egt::Palette::blue, 128),
        egt::Color(egt::Palette::yellow, 128),
        egt::Color(egt::Palette::cyan, 128),
        egt::Color(egt::Palette::magenta, 128),
        egt::Color(egt::Palette::silver, 128),
        egt::Color(egt::Palette::gray, 128),
        egt::Color(egt::Palette::lightgray, 128),
        egt::Color(egt::Palette::maroon, 128),
        egt::Color(egt::Palette::olive, 128),
        egt::Color(egt::Palette::purple, 128),
        egt::Color(egt::Palette::teal, 128),
        egt::Color(egt::Palette::navy, 128),
        egt::Color(egt::Palette::orange, 128),
        egt::Color(egt::Palette::aqua, 128),
        egt::Color(egt::Palette::lightblue, 128),
    };

    size_t index = 0;
    struct test_data
    {
        test_data(int w, int h, const std::vector<egt::Color>& colors)
            : w(w), h(h), colors(colors)
        {}

        int w;
        int h;
        std::vector<egt::Color> colors;
    };

    std::vector<test_data> sets =
    {
        test_data(100, 100, colors1),
        test_data(200, 200, colors1),
        test_data(800, 480, colors1),
        test_data(100, 100, colors2),
        test_data(200, 200, colors2),
        test_data(800, 480, colors2),
    };

    {
        std::ostringstream ss;
        ss << sets[index].w << "," << sets[index].h;
        label_dims.text(ss.str());
    }

    std::random_device r;
    std::default_random_engine e1 {r()};
    std::uniform_real_distribution<float> x_dist {0., static_cast<float>(win.width() - sets[index].w)};
    std::uniform_real_distribution<float> y_dist {0., static_cast<float>(win.height() - sets[index].h)};

    egt::experimental::FramesPerSecond fps;
    fps.start();

    egt::PeriodicTimer timer(std::chrono::milliseconds(1));
    timer.on_timeout([&]()
    {
        egt::Rect rect(x_dist(e1), y_dist(e1), sets[index].w, sets[index].h);

        egt::Painter painter(win.screen()->context());
        auto color = *random_item(sets[index].colors.begin(), sets[index].colors.end(), e1);
        painter.set(color);
        painter.draw(rect);
        painter.fill();

        std::ostringstream ss;
        ss << "FPS: " << std::round(fps.fps());
        label.text(ss.str());

        egt::Screen::DamageArray damage;
        damage.push_back(rect);
        win.screen()->flip(damage);

        fps.end_frame();
    });
    timer.start();

    egt::PeriodicTimer vtimer(std::chrono::seconds(10));
    vtimer.on_timeout([&]()
    {
        index++;
        if (index >= sets.size())
            index = 0;

        std::ostringstream ss;
        ss << sets[index].w << "," << sets[index].h;
        label_dims.text(ss.str());

        x_dist = std::uniform_real_distribution<float>(0., static_cast<float>(win.width() - sets[index].w));
        y_dist = std::uniform_real_distribution<float>(0., static_cast<float>(win.height() - sets[index].h));
    });
    vtimer.start();

    win.show();

    return app.run();
}
