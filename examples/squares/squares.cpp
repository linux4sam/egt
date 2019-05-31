/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <egt/ui>
#include <random>

using namespace std;
using namespace egt;

int main(int argc, const char** argv)
{
    Application app(argc, argv, "squares");

    TopWindow win;

    const Color FUCHSIA(Color::CSS("#F012BE"));

    VerticalBoxSizer sizer;

    Label label("FPS: ---");
    label.set_color(Palette::ColorId::text, Palette::black);
    label.set_color(Palette::ColorId::bg, Palette::transparent);
    sizer.add(expand_horizontal(label));

    Label label_dims("");
    label_dims.set_color(Palette::ColorId::text, Palette::black);
    label_dims.set_color(Palette::ColorId::bg, Palette::transparent);
    sizer.add(expand_horizontal(label_dims));

    Popup popup(Size(100, 80));
    popup.set_color(Palette::ColorId::bg, FUCHSIA);
    popup.add(sizer);
    expand(sizer);
    top(right(popup));
    win.add(popup);
    popup.show();

    default_dim_type width = 100;
    default_dim_type height = 100;

    {
        ostringstream ss;
        ss << width << "," << height;
        label_dims.set_text(ss.str());
    }

    const vector<Color> colors =
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

    std::random_device r;
    std::default_random_engine e1 {r()};
    std::uniform_real_distribution<float> x_dist {0., static_cast<float>(win.w() - width)};
    std::uniform_real_distribution<float> y_dist {0., static_cast<float>(win.h() - height)};
    std::uniform_int_distribution<int> color_dist {0, static_cast<int>(colors.size()) - 1};

    experimental::Fps fps;
    fps.start();

    PeriodicTimer timer(std::chrono::milliseconds(1));
    timer.on_timeout([&]()
    {
        Rect rect(x_dist(e1), y_dist(e1), width, height);

        Painter painter(win.screen()->context());
        painter.set(colors[color_dist(e1)]);
        painter.draw(rect);
        painter.fill();

        ostringstream ss;
        ss << "FPS: " << std::round(fps.fps());
        label.set_text(ss.str());

        Screen::damage_array damage;
        damage.push_back(rect);
        win.screen()->flip(damage);

        fps.end_frame();

    });
    timer.start();

    size_t index = 0;
    vector<std::pair<int, int>> sets =
    {
        {100, 100},
        {200, 200},
        {800, 480}
    };

    PeriodicTimer vtimer(std::chrono::seconds(10));
    vtimer.on_timeout([&]()
    {
        index++;
        if (index >= sets.size())
            index = 0;
        width = sets[index].first;
        height = sets[index].second;

        ostringstream ss;
        ss << width << "," << height;
        label_dims.set_text(ss.str());

        x_dist = std::uniform_real_distribution<float>(0., static_cast<float>(win.w() - width));
        y_dist = std::uniform_real_distribution<float>(0., static_cast<float>(win.h() - height));
    });
    vtimer.start();

    win.show();

    return app.run();
}
