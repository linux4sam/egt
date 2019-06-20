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

    TextBox textbox;
    textbox.set_align(alignmask::expand_horizontal | alignmask::top);
    textbox.text_flags().set({TextBox::flag::multiline, TextBox::flag::word_wrap});

    auto keyboard = make_shared<VirtualKeyboard>();
    keyboard->set_align(alignmask::expand);
    PopupVirtualKeyboard popup_keyboard {keyboard};
    popup_keyboard.show();

    VerticalBoxSizer vbox;
    vbox.set_align(alignmask::expand);
    vbox.add(textbox);

    keyboard_focus(&textbox);

    win.add(popup_keyboard);
    win.add(vbox);
    win.show();

    return app.run();
}
