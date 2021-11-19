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

    auto grid = std::make_shared<egt::StaticGrid>(egt::StaticGrid::GridSize(3, 3));
    grid->margin(5);
    grid->horizontal_space(5);
    grid->vertical_space(5);
    win.add(egt::expand(grid));

    auto slider1 = std::make_shared<egt::Slider>();
    slider1->value(50);
    grid->add(egt::expand(slider1));

    auto slider2 = std::make_shared<egt::Slider>(0, 100, 0, egt::Orientation::vertical);
    slider2->value(75);
    grid->add(egt::expand(slider2));

    auto slider3 = std::make_shared<egt::SliderF>(100, 0);
    slider3->value(50);
    slider3->slider_flags().set({egt::SliderF::SliderFlag::round_handle,
                                 egt::SliderF::SliderFlag::show_label});
    grid->add(egt::expand(slider3));

    auto slider4 = std::make_shared<egt::Slider>(0, 100, 0, egt::Orientation::vertical);
    slider4->value(75);
    slider4->slider_flags().set(egt::Slider::SliderFlag::round_handle);
    slider4->disable();
    grid->add(egt::expand(slider4));

    auto slider5 = std::make_shared<egt::Slider>();
    slider5->orient(egt::Orientation::vertical);
    slider5->live_update(true);
    grid->add(egt::expand(slider5));

    auto slider6 = std::make_shared<egt::Slider>();
    slider6->orient(egt::Orientation::vertical);
    slider6->live_update(true);
    grid->add(egt::expand(slider6));

    auto slider7 = std::make_shared<egt::Slider>(100, 200, 150, egt::Orientation::vertical);
    slider7->value(180);
    slider7->margin(5);
    slider7->slider_flags().set({egt::Slider::SliderFlag::square_handle, egt::Slider::SliderFlag::show_labels});
    grid->add(egt::expand(slider7));

    auto slider8 = std::make_shared<egt::Slider>(100, 200, 150);
    slider8->slider_flags().set({egt::Slider::SliderFlag::rectangle_handle,
                                 egt::Slider::SliderFlag::show_labels});
    grid->add(egt::expand(slider8));

    win.show();

    return app.run();
}
