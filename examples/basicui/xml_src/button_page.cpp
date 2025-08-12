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

    auto grid0 = std::make_shared<egt::StaticGrid>(egt::StaticGrid::GridSize(3, 6));
    grid0->margin(5);
    grid0->horizontal_space(5);
    grid0->vertical_space(5);
    win.add(expand(grid0));

    grid0->add(egt::expand(std::make_shared<egt::Button>("Button", egt::Size(100, 40))));

    auto btn2 = std::make_shared<egt::Button>("Disabled");
    btn2->disable();
    grid0->add(egt::expand(btn2));

    auto image = egt::Image("icon:calculator.png");
    grid0->add(expand(std::make_shared<egt::ImageButton>(image, "Calculator")));

    auto imagebutton1 = std::make_shared<egt::ImageButton>(image, "Calculator");
    grid0->add(expand(imagebutton1));
    imagebutton1->text_align(egt::AlignFlag::center_vertical | egt::AlignFlag::left);

    auto imagebutton2 = std::make_shared<egt::ImageButton>(image, "Calculator");
    grid0->add(expand(imagebutton2));
    imagebutton2->text_align(egt::AlignFlag::center_vertical | egt::AlignFlag::right);

    auto imagebutton3 = std::make_shared<egt::ImageButton>(image, "Calculator");
    grid0->add(expand(imagebutton3));
    imagebutton3->text_align(egt::AlignFlag::center_horizontal | egt::AlignFlag::top);

    auto imagebutton4 = std::make_shared<egt::ImageButton>(image, "Calculator");
    grid0->add(expand(imagebutton4));
    imagebutton4->text_align(egt::AlignFlag::center_horizontal | egt::AlignFlag::bottom);
    imagebutton4->image_align(egt::AlignFlag::right);

    auto imagebutton5 = std::make_shared<egt::ImageButton>(image);
    grid0->add(expand(imagebutton5));

    auto button6 = std::make_shared<egt::Button>("Multiline\nButton Text");
    grid0->add(egt::expand(button6));

    win.show();

    return app.run();
}
