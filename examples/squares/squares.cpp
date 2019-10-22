/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <egt/ui>
#include <random>

using namespace std;
using namespace egt;

template<typename T, typename RandomGenerator>
T random_item(T start, T end, RandomGenerator& e)
{
    std::uniform_int_distribution<> dist(0, std::distance(start, end) - 1);
    std::advance(start, dist(e));
    return start;
}

int main(int argc, const char** argv)
{
    Application app(argc, argv, "squares");

    TopWindow win;

    const Color FUCHSIA(Color::css("#F012BE"));

    VerticalBoxSizer sizer;

    Label label("FPS: ---");
    label.color(Palette::ColorId::text, Palette::black);
    label.color(Palette::ColorId::bg, Palette::transparent);
    sizer.add(expand_horizontal(label));

    label.on_event([&label](Event&)
    {
        cout << label.text() << endl;
    }, {eventid::property_changed});

    Label label_dims("");
    label_dims.color(Palette::ColorId::text, Palette::black);
    label_dims.color(Palette::ColorId::bg, Palette::transparent);
    sizer.add(expand_horizontal(label_dims));

    label_dims.on_event([&label_dims](Event&)
    {
        cout << label_dims.text() << endl;
    }, {eventid::property_changed});


    Popup popup(Size(100, 80));
    popup.color(Palette::ColorId::bg, FUCHSIA);
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
        label_dims.text(ss.str());
    }

    const vector<Color> colors1 =
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

    const vector<Color> colors2 =
    {
        Color(Palette::red, 128),
        Color(Palette::green, 128),
        Color(Palette::blue, 128),
        Color(Palette::yellow, 128),
        Color(Palette::cyan, 128),
        Color(Palette::magenta, 128),
        Color(Palette::silver, 128),
        Color(Palette::gray, 128),
        Color(Palette::lightgray, 128),
        Color(Palette::maroon, 128),
        Color(Palette::olive, 128),
        Color(Palette::purple, 128),
        Color(Palette::teal, 128),
        Color(Palette::navy, 128),
        Color(Palette::orange, 128),
        Color(Palette::aqua, 128),
        Color(Palette::lightblue, 128),
    };

    size_t index = 0;
    struct test_data
    {
        test_data(int w, int h, const vector<Color>& colors)
            : w(w), h(h), colors(colors)
        {}

        int w;
        int h;
        vector<Color> colors;
    };

    vector<test_data> sets =
    {
        test_data(100, 100, colors1),
        test_data(200, 200, colors1),
        test_data(800, 480, colors1),
        test_data(100, 100, colors2),
        test_data(200, 200, colors2),
        test_data(800, 480, colors2),
    };

    std::random_device r;
    std::default_random_engine e1 {r()};
    std::uniform_real_distribution<float> x_dist {0., static_cast<float>(win.width() - width)};
    std::uniform_real_distribution<float> y_dist {0., static_cast<float>(win.height() - height)};

    egt::experimental::Fps fps;
    fps.start();

    PeriodicTimer timer(std::chrono::milliseconds(1));
    timer.on_timeout([&]()
    {
        Rect rect(x_dist(e1), y_dist(e1), width, height);

        Painter painter(win.screen()->context());
        auto color = *random_item(sets[index].colors.begin(), sets[index].colors.end(), e1);
        painter.set(color);
        painter.draw(rect);
        painter.fill();

        ostringstream ss;
        ss << "FPS: " << std::round(fps.fps());
        label.text(ss.str());

        Screen::damage_array damage;
        damage.push_back(rect);
        win.screen()->flip(damage);

        fps.end_frame();
    });
    timer.start();

    PeriodicTimer vtimer(std::chrono::seconds(10));
    vtimer.on_timeout([&]()
    {
        index++;
        if (index >= sets.size())
            index = 0;
        width = sets[index].w;
        height = sets[index].h;

        ostringstream ss;
        ss << width << "," << height;
        label_dims.text(ss.str());

        x_dist = std::uniform_real_distribution<float>(0., static_cast<float>(win.width() - width));
        y_dist = std::uniform_real_distribution<float>(0., static_cast<float>(win.height() - height));
    });
    vtimer.start();

    win.show();

    return app.run();
}
