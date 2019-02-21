/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>

using namespace egt;

int main(int argc, const char** argv)
{
    Application app(argc, argv);

    TopWindow win;

    auto create_label = [](const std::string & text)
    {
        auto label = new Label(text);
        label->set_font(Font(30));
        label->set_boxtype(Theme::boxtype::none);
        label->set_align(alignmask::center);
        return label;
    };

    win.add(create_label("EGT SideBoards"));

    SideBoard board0;
    board0.palette().set(Palette::ColorId::bg, Palette::GroupId::normal, Palette::red);
    board0.add(create_label("LEFT"));
    win.add(&board0);
    board0.show();

    SideBoard board1(SideBoard::flags::bottom);
    board1.palette().set(Palette::ColorId::bg, Palette::GroupId::normal, Palette::blue);
    board1.add(create_label("BOTTOM"));
    win.add(&board1);
    board1.show();

    SideBoard board2(SideBoard::flags::right);
    board2.palette().set(Palette::ColorId::bg, Palette::GroupId::normal, Palette::green);
    board2.add(create_label("RIGHT"));
    win.add(&board2);
    board2.show();

    SideBoard board3(SideBoard::flags::top);
    board3.palette().set(Palette::ColorId::bg, Palette::GroupId::normal, Palette::gray);
    board3.add(create_label("TOP"));
    win.add(&board3);
    board3.show();

    win.show();

    return app.run();
}
