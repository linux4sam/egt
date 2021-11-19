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

    auto grid0 = std::make_shared<egt::StaticGrid>(egt::StaticGrid::GridSize(3, 10));
    grid0->margin(5);
    grid0->horizontal_space(5);
    grid0->vertical_space(5);
    win.add(egt::expand(grid0));

    auto toggle1 = std::make_shared<egt::ToggleBox>();
    toggle1->toggle_text("Off", "On");
    grid0->add(egt::expand(toggle1));

    auto toggle2 = std::make_shared<egt::ToggleBox>();
    toggle2->checked(true);
    grid0->add(egt::expand(toggle2));

    auto toggle3 = std::make_shared<egt::ToggleBox>();
    toggle3->disable();
    grid0->add(egt::expand(toggle3));

    auto checkbox1 = std::make_shared<egt::CheckBox>("checkbox 1");
    grid0->add(egt::expand(checkbox1));

    auto checkbox2 = std::make_shared<egt::CheckBox>("checkbox 2");
    grid0->add(egt::expand(checkbox2));

    auto checkbox3 = std::make_shared<egt::CheckBox>("checkbox 3");
    grid0->add(egt::expand(checkbox3));

    auto checkbox4 = std::make_shared<egt::CheckBox>("Disabled");
    checkbox4->disable();
    grid0->add(egt::expand(checkbox4));

    auto radiobox1 = std::make_shared<egt::RadioBox>("radiobox 1");
    grid0->add(egt::expand(radiobox1));

    auto radiobox2 = std::make_shared<egt::RadioBox>("radiobox 2");
    grid0->add(egt::expand(radiobox2));
    radiobox2->checked(true);

    auto radiobox3 = std::make_shared<egt::RadioBox>("Disabled");
    grid0->add(egt::expand(radiobox3));
    radiobox3->disable();

    win.show();

    return app.run();
}
