/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>

int main(int argc, char** argv)
{
    egt::Application app(argc, argv);
    egt::TopWindow win;

    auto hsizer1 = std::make_shared<egt::BoxSizer>(egt::Orientation::flex);
    win.add(egt::expand(hsizer1));

    auto circle = std::make_shared<egt::CircleWidget>(egt::Circle(egt::Point(), 50));
    circle->margin(10);
    hsizer1->add(circle);

    auto hline = std::make_shared<egt::LineWidget>(egt::Size(100, 100));
    hline->margin(10);
    hsizer1->add(hline);

    auto vline = std::make_shared<egt::LineWidget>(egt::Size(100, 100));
    vline->margin(10);
    vline->horizontal(false);
    hsizer1->add(vline);

    auto rect = std::make_shared<egt::RectangleWidget>(egt::Size(100, 100));
    rect->margin(10);
    hsizer1->add(rect);

    win.show();

    return app.run();
}
