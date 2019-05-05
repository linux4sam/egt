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

    Label label("FPS: ---");
    label.set_color(Palette::ColorId::text, Palette::black);
    label.set_color(Palette::ColorId::bg, Palette::transparent);

    Popup popup(Size(100, 80));
    popup.set_color(Palette::ColorId::bg, FUCHSIA);
    popup.add(label);
    top(right(popup));
    win.add(popup);
    popup.show();

    default_dim_type WIDTH = 100;
    default_dim_type HEIGHT = 100;

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
    std::uniform_real_distribution<float> x_dist {0., static_cast<float>(win.w() - WIDTH)};
    std::uniform_real_distribution<float> y_dist {0., static_cast<float>(win.h() - HEIGHT)};
    std::uniform_int_distribution<int> color_dist {0, static_cast<int>(colors.size()) - 1};

    experimental::Fps fps;
    fps.start();

    PeriodicTimer timer(std::chrono::milliseconds(1));
    timer.on_timeout([&]()
    {
        Rect rect(x_dist(e1), y_dist(e1), WIDTH, HEIGHT);

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

    win.show();

    return app.run();
}
