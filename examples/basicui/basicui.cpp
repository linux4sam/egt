/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/// @[Example]
#include <cxxopts.hpp>
#include <egt/ui>
#include <egt/uiloader.h>
#include <iostream>

int main(int argc, char** argv)
{
    egt::Application app(argc, argv);
#ifdef EXAMPLEDATA
    egt::add_search_path(EXAMPLEDATA);
#endif

    cxxopts::Options options(argv[0], "load ui xml");
    options.add_options()
    ("h,help", "Show help")
    ("i,input", "Input file", cxxopts::value<std::string>()->default_value("file:ui.xml"));
    auto args = options.parse(argc, argv);
    if (args.count("help"))
    {
        std::cout << options.help() << std::endl;
        return 0;
    }

    egt::experimental::UiLoader loader;
    auto window = loader.load(args["input"].as<std::string>());

    window->on_show([&]()
    {
        for (const auto& w : app.windows())
        {
            if (w->type() == "VideoWindow")
            {
                egt::VideoWindow* vwin = dynamic_cast<egt::VideoWindow*>(w);
                vwin->play();
            }
            else if (w->type() == "CameraWindow")
            {
                egt::CameraWindow* cwin = dynamic_cast<egt::CameraWindow*>(w);
                cwin->start();
            }
        }
    });

    window->show();
    return app.run();
}
/// @[Example]
