/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/// @[snippet0]
#include <egt/ui>

int main(int argc, const char** argv)
{
    egt::Application app(argc, argv);

    egt::TopWindow window;
    egt::Button button(window, "Press Me");
    center(button);
    window.show();

    return app.run();
}
/// @[snippet0]
