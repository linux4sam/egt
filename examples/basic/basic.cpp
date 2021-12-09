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
    egt::TextBox button(window);
    button.on_gain_focus.invoke();
    egt::center(button);
    window.show();

    return app.run();
}
/// @[Example]
