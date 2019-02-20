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
    Application app(argc, argv, "keyboard");

    TopWindow win;

    Keyboard<Window> keyboard;
    win.add(&keyboard);
    keyboard.show();

    TextBox textbox(Rect(0, 0, 100, 100), alignmask::LEFT | alignmask::TOP);
    textbox.set_text_flag(TextBox::flag::multiline);
    win.add(textbox);
    textbox.set_align(alignmask::EXPAND_HORIZONTAL | alignmask::BOTTOM);
    keyboard_focus(&textbox);

    win.show();

    return app.run();
}
