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

    auto grid0 = std::make_shared<egt::StaticGrid>(egt::StaticGrid::GridSize(2, 2));
    grid0->margin(10);
    grid0->horizontal_space(10);
    grid0->vertical_space(10);
    win.add(egt::expand(grid0));

    auto lp1 = std::make_shared<egt::LevelMeter>();
    lp1->num_bars(10);
    grid0->add(egt::expand(lp1));

    auto am1 = std::make_shared<egt::AnalogMeter>();
    grid0->add(egt::expand(am1));

    win.show();

    return app.run();
}
