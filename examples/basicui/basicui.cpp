/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <egt/uiloader.h>

#define SHARED_PATH "../share/egt/examples/basicui/"

int main(int argc, const char** argv)
{
    egt::Application app(argc, argv);

    egt::experimental::UiLoader loader;
    auto window = loader.load(SHARED_PATH "ui.xml");
    window->show();

    return app.run();
}
