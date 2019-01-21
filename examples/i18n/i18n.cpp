/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>

int main(int argc, const char** argv)
{
    egt::Application app(argc, argv, "i18n");

    egt::TopWindow window;
    egt::Button button(window, gettext("Hello World"));
    button.set_align(egt::alignmask::CENTER);
    window.show();

    return app.run();
}
