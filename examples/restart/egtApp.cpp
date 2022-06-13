/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * Modified by Owen O'Hehir
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/// @[Example]
#include <egt/ui>

int main(int argc, char** argv)
{
    egt::Application app(argc, argv);

    egt::TopWindow window;

    egt::Button button_quit(window, "Quit");
    egt::center(button_quit);
    egt::top(button_quit);

    egt::Button button_restart(window, "Restart");
    egt::center(button_restart);

    button_quit.on_click([&app](egt::Event&){
    	app.instance().quit(0);
    });

    button_restart.on_click([&app](egt::Event&){
    	app.instance().quit(1);
    });

    window.show();


    return app.run();
}
/// @[Example]
