/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>

int main(int argc, const char** argv)
{
    egt::Application app(argc, argv);

    egt::TopWindow win;

    auto create_label = [](const std::string & text)
    {
        auto label = std::make_shared<egt::Label>(text);
        label->font(egt::Font(30));
        label->fill_flags().clear();
        label->align(egt::AlignFlag::center);
        return label;
    };

    auto label = std::make_shared<egt::ImageLabel>(
                     egt::Image("icon:egt_logo_black.png;128"),
                     "SideBoard Widget");
    label->font(egt::Font(28));
    label->fill_flags().clear();
    label->align(egt::AlignFlag::center);
    label->image_align(egt::AlignFlag::top);
    win.add(label);

    egt::SideBoard board0;
    board0.color(egt::Palette::ColorId::bg, egt::Palette::red);
    board0.add(create_label("LEFT"));
    win.add(board0);
    board0.show();

    egt::SideBoard board1(egt::SideBoard::PositionFlag::bottom);
    board1.color(egt::Palette::ColorId::bg, egt::Palette::blue);
    board1.add(create_label("BOTTOM"));
    win.add(board1);
    board1.show();

    egt::SideBoard board2(egt::SideBoard::PositionFlag::right);
    board2.color(egt::Palette::ColorId::bg, egt::Palette::green);
    board2.add(create_label("RIGHT"));
    win.add(board2);
    board2.show();

    egt::SideBoard board3(egt::SideBoard::PositionFlag::top);
    board3.color(egt::Palette::ColorId::bg, egt::Palette::gray);
    board3.add(create_label("TOP"));
    win.add(board3);
    board3.show();

    win.show();

    return app.run();
}
