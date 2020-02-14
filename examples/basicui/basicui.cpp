/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/// @[Example]
#include <egt/ui>
#include <egt/uiloader.h>

int main(int argc, char** argv)
{
    egt::Application app(argc, argv);
    egt::add_search_path(EXAMPLEDATA);

    egt::experimental::UiLoader loader;
    auto window = loader.load("file:ui.xml");
    window->show();

    return app.run();
}
/// @[Example]
