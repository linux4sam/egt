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
    win.add(keyboard);
    keyboard.show();

    TextBox textbox(Rect(0, 0, 100, 100), alignmask::left | alignmask::top);
    textbox.text_flags().set({TextBox::flag::multiline, TextBox::flag::word_wrap});
    win.add(expand_horizontal(bottom(textbox)));
    keyboard_focus(&textbox);

    win.show();

    return app.run();
}
