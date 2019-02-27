/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <egt/painter.h>
#include <string>
#include <vector>
#include <sstream>

using namespace egt;
using namespace std;

int main(int argc, const char** argv)
{
    Application app(argc, argv, "animations");

    TopWindow win;

    StaticGrid grid(Rect(win.size()), 3, 2, 10);
    win.add(expand(grid));

    AnimationSequence seq;

    PropertyAnimator in(0, win.w(), std::chrono::seconds(1));
    in.on_change(std::bind(&StaticGrid::set_width, std::ref(grid), std::placeholders::_1));
    seq.add(in);

    Button button("hello world");
    grid.add(expand(button), 0, 0);

    PropertyAnimator shrink_in(100, 10, std::chrono::seconds(1));
    shrink_in.on_change(std::bind(&Button::set_width, std::ref(button), std::placeholders::_1));
    PropertyAnimator shrink_out(10, 100, std::chrono::seconds(1));
    shrink_out.on_change(std::bind(&Button::set_width, std::ref(button), std::placeholders::_1));
    seq.add(shrink_in);
    seq.add(shrink_out);

    Label label("hello world");
    label.palette().set(Palette::ColorId::bg, Palette::GroupId::normal, Palette::red);
    grid.add(expand(label), 0, 1);

    auto orig = label.w();
    PropertyAnimator shrinkh_in(100, 10, std::chrono::seconds(1));
    shrinkh_in.on_change(std::bind(&Label::set_width, std::ref(label), std::placeholders::_1));
    PropertyAnimator shrinkh_out(10, orig, std::chrono::seconds(1));
    shrinkh_out.on_change(std::bind(&Label::set_width, std::ref(label), std::placeholders::_1));
    seq.add(shrinkh_in);
    seq.add(shrinkh_out);

    Label label2("hello world");
    label2.palette().set(Palette::ColorId::bg, Palette::GroupId::normal, Palette::green);
    grid.add(expand(label2), 1, 1);

    seq.start();

    win.show();

    return app.run();
}
