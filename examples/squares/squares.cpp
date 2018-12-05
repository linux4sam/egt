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
    Application app(argc, argv);

    TopWindow win;

    const Color FUCHSIA(Color::CSS("#F012BE"));

    Label label("FPS: -",
                Rect(Point(0, 40), Size(100, 40)),
                alignmask::CENTER);
    label.palette()
    .set(Palette::TEXT, Palette::GROUP_NORMAL, Color::BLACK)
    .set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);

    Popup<Window> popup(Size(100, 80));
    popup.move(Point(win.w() - 100 - 10, 10));
    popup.palette().set(Palette::BG, Palette::GROUP_NORMAL, FUCHSIA);
    popup.set_name("popup");

    popup.add(label);
    popup.show();

    auto WIDTH = 100;
    auto HEIGHT = 100;

    const vector<Color> colors =
    {
        Color::RED,
        Color::GREEN,
        Color::BLUE,
        Color::YELLOW,
        Color::CYAN,
        Color::MAGENTA,
        Color::SILVER,
        Color::GRAY,
        Color::LIGHTGRAY,
        Color::MAROON,
        Color::OLIVE,
        Color::PURPLE,
        Color::TEAL,
        Color::NAVY,
        Color::ORANGE,
        Color::AQUA,
        Color::LIGHTBLUE,
    };

    std::random_device r;
    std::default_random_engine e1 {r()};
    std::uniform_int_distribution<int> x_dist {0, win.w() - WIDTH};
    std::uniform_int_distribution<int> y_dist {0, win.h() - HEIGHT};
    std::uniform_int_distribution<int> color_dist {0, static_cast<int>(colors.size()) - 1};

    experimental::Fps fps;
    fps.start();

    PeriodicTimer timer(std::chrono::milliseconds(1));
    timer.on_timeout([&]()
    {
        Rect rect(x_dist(e1), y_dist(e1), WIDTH, HEIGHT);

        Painter painter(win.screen()->context());
        painter.set_color(colors[color_dist(e1)]);
        painter.draw_fill(rect);

        ostringstream ss;
        ss << "FPS: " << std::round(fps.fps());
        label.set_text(ss.str());

        IScreen::damage_array damage;
        damage.push_back(rect);
        win.screen()->flip(damage);

        fps.end_frame();

    });
    timer.start();

    win.show();

    return app.run();
}
