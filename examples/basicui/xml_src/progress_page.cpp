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

    auto vsizer = std::make_shared<egt::VerticalBoxSizer>();
    win.add(egt::expand(vsizer));

    auto grid0 = std::make_shared<egt::StaticGrid>(egt::StaticGrid::GridSize(2, 8));
    grid0->margin(5);
    grid0->horizontal_space(5);
    grid0->vertical_space(5);
    vsizer->add(egt::expand(grid0));

    auto spinprogress = std::make_shared<egt::SpinProgress>();
    grid0->add(egt::expand(spinprogress));

    auto progressbar = std::make_shared<egt::ProgressBar>();
    grid0->add(egt::expand(progressbar));

    auto progressbar1 = std::make_shared<egt::ProgressBar>();
    progressbar1->show_label(false);
    grid0->add(egt::expand(progressbar1));

    auto hsizer = std::make_shared<egt::HorizontalBoxSizer>();
    vsizer->add(egt::expand_horizontal(hsizer));

    auto start = std::make_shared<egt::Button>("Start");
    hsizer->add(start);

    auto stop = std::make_shared<egt::Button>("Stop");
    hsizer->add(stop);

    auto resume = std::make_shared<egt::Button>("Resume");
    hsizer->add(resume);

    win.show();

    return app.run();
}
