/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <math.h>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>

using namespace std;
using namespace egt;

int main(int argc, const char** argv)
{
    Application app(argc, argv);

    set_image_path("../share/egt/examples/keyboard/");

    TopWindow win;

    Keyboard<Window> keyboard;
    keyboard.show();

    MultilineTextBox textbox(Rect(0, 0, 100, 100), alignmask::LEFT | alignmask::TOP);
    win.add(textbox);
    textbox.set_align(alignmask::EXPAND_HORIZONTAL | alignmask::BOTTOM);
    keyboard_focus(&textbox);

#if 0
    // TODO: this is broken with keyboard and focus - get dups
    detail::IInput::global_input().on_event([&](eventid event)
    {
        switch (event)
        {
        case eventid::KEYBOARD_DOWN:
        case eventid::KEYBOARD_UP:
        case eventid::KEYBOARD_REPEAT:
            return textbox.handle(event);
            break;
        default:
            break;
        }
        return 0;
    });
#endif

    win.show();

    app.dump(cout);

    return app.run();
}
