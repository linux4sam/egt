/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <egt/ui>
#include <iostream>

using namespace egt;
using namespace egt::experimental;
using namespace std;

struct ColorMapWidget : public Widget
{
    explicit ColorMapWidget(const std::string& name,
                            const ColorMap::steps_array& steps,
                            ColorMap::interpolation interp = ColorMap::interpolation::rgba)
        : m_map(steps, interp),
          m_name(name)
    {}

    virtual void draw(Painter& painter, const Rect& rect) override
    {
        auto b = content_area();

        auto inc = 1.0 / b.width;

        for (auto x = 0.0; x <= 1.0; x += inc)
        {
            RectF seg(b.x, b.y, b.width, b.height);
            seg.x = x * seg.width;
            painter.set(m_map.interp(x));
            painter.draw(seg);
            painter.fill();
        }

        painter.set(m_map.interp(1.0));
        painter.set(font());
        painter.draw(b.point());
        painter.draw(m_name);
    }

    ColorMap m_map;
    std::string m_name;
};

int main(int argc, const char** argv)
{
    Application app(argc, argv);

    TopWindow window;
    VerticalBoxSizer sizer;
    window.add(expand(sizer));

    struct triple
    {
        std::string first;
        ColorMap::steps_array second;
        ColorMap::interpolation third;
    };

    vector<triple> maps =
    {
        { "RGB", {Palette::white, Palette::black}, ColorMap::interpolation::rgba },
        { "HSV", {Palette::white, Palette::black}, ColorMap::interpolation::hsv },

        { "RGB", {Palette::red, Palette::green}, ColorMap::interpolation::rgba },
        { "HSV", {Palette::red, Palette::green}, ColorMap::interpolation::hsv },

        { "RGB", {Palette::red, Palette::blue}, ColorMap::interpolation::rgba },
        { "HSV", {Palette::red, Palette::blue}, ColorMap::interpolation::hsv },

        { "RGB", {Palette::white, Palette::orange}, ColorMap::interpolation::rgba },
        { "HSV", {Palette::white, Palette::orange}, ColorMap::interpolation::hsv },

        { "RGB", {Palette::purple, Palette::yellow, Palette::maroon}, ColorMap::interpolation::rgba },
        { "HSV", {Palette::purple, Palette::yellow, Palette::maroon}, ColorMap::interpolation::hsv },
    };

    for (auto& m : maps)
    {
        auto inst = make_shared<ColorMapWidget>(m.first, m.second, m.third);
        sizer.add(expand(inst));
    }

    window.show();

    return app.run();
}
