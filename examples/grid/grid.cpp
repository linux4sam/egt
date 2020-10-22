/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/// @[Example]
#include <egt/ui>

int main(int argc, char** argv)
{
    egt::Application app(argc, argv);

    egt::TopWindow window;

    egt::HorizontalBoxSizer hsizer;
    window.add(egt::expand(hsizer));

    egt::SelectableGrid grid(egt::StaticGrid::GridSize(4, 3));
    grid.fill_flags(egt::Theme::FillFlag::blend);
    grid.color(egt::Palette::ColorId::bg, egt::Palette::grey);
    grid.color(egt::Palette::ColorId::border, egt::Palette::blue);
    grid.selection_highlight(10);
    hsizer.add(egt::expand(grid));

    egt::Button b1("");
    grid.add(egt::expand(b1), egt::StaticGrid::GridPoint(0, 0));
    egt::Button b2("");
    grid.add(egt::expand(b2), egt::StaticGrid::GridPoint(1, 0));
    egt::Button b3("");
    grid.add(egt::expand(b3), egt::StaticGrid::GridPoint(1, 1));
    egt::Button b4("");
    grid.add(egt::expand(b4), egt::StaticGrid::GridPoint(2, 1));
    egt::Button b5("");
    grid.add(egt::expand(b5), egt::StaticGrid::GridPoint(2, 2));
    egt::Button b6("");
    grid.add(egt::expand(b6), egt::StaticGrid::GridPoint(3, 2));

    egt::VerticalBoxSizer vsizer;
    hsizer.add(vsizer);

    egt::CheckBox grid_margin(vsizer, "grid margin");
    egt::left(grid_margin);
    grid_margin.on_checked_changed([&]()
    {
        if (grid_margin.checked())
            grid.margin(10);
        else
            grid.margin(0);
    });

    egt::CheckBox grid_border(vsizer, "grid border");
    egt::left(grid_border);
    grid_border.on_checked_changed([&]()
    {
        if (grid_border.checked())
            grid.border(10);
        else
            grid.border(0);
    });

    egt::CheckBox grid_padding(vsizer, "grid padding");
    egt::left(grid_padding);
    grid_padding.on_checked_changed([&]()
    {
        if (grid_padding.checked())
            grid.padding(10);
        else
            grid.padding(0);
    });

    egt::CheckBox grid_horizontal_space(vsizer, "grid horizontal space");
    egt::left(grid_horizontal_space);
    grid_horizontal_space.on_checked_changed([&]()
    {
        if (grid_horizontal_space.checked())
            grid.horizontal_space(10);
        else
            grid.horizontal_space(0);
    });

    egt::CheckBox grid_vertical_space(vsizer, "grid vertical space");
    egt::left(grid_vertical_space);
    grid_vertical_space.on_checked_changed([&]()
    {
        if (grid_vertical_space.checked())
            grid.vertical_space(10);
        else
            grid.vertical_space(0);
    });

    egt::CheckBox buttons_margin(vsizer, "buttons margin");
    egt::left(buttons_margin);
    buttons_margin.on_checked_changed([&]()
    {
        if (buttons_margin.checked())
        {
            b1.margin(10);
            b2.margin(10);
            b3.margin(10);
            b4.margin(10);
            b5.margin(10);
            b6.margin(10);
        }
        else
        {
            b1.margin(0);
            b2.margin(0);
            b3.margin(0);
            b4.margin(0);
            b5.margin(0);
            b6.margin(0);
        }
    });

    window.show();

    return app.run();
}
/// @[Example]
