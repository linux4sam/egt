/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/// @[snippet1]
#include <egt/ui>

using namespace egt;

int main(int argc, const char** argv)
{
    Application app(argc, argv);

    TopWindow window;
    Button button(window, "Press Me");
    center(button);
    window.show();

    return app.run();
}
/// @[snippet1]
