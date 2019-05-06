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

struct QwertyLettersLowerCase : public Panel
{
    QwertyLettersLowerCase()
        : Panel(
    {
        {
            make_shared<Key>("q"),
            make_shared<Key>("w"),
            make_shared<Key>("e"),
            make_shared<Key>("r"),
            make_shared<Key>("t"),
            make_shared<Key>("y"),
            make_shared<Key>("u"),
            make_shared<Key>("i"),
            make_shared<Key>("o"),
            make_shared<Key>("p")
        }, {
            make_shared<Key>("a"),
            make_shared<Key>("s"),
            make_shared<Key>("d"),
            make_shared<Key>("f"),
            make_shared<Key>("g"),
            make_shared<Key>("h"),
            make_shared<Key>("j"),
            make_shared<Key>("k"),
            make_shared<Key>("l")
        }, {
            make_shared<Key>("Shift", 1.5),
            make_shared<Key>("z"),
            make_shared<Key>("x"),
            make_shared<Key>("c"),
            make_shared<Key>("v"),
            make_shared<Key>("b"),
            make_shared<Key>("n"),
            make_shared<Key>("m"),
            make_shared<Key>("<-", 1.5)
        }, {
            make_shared<Key>("?123", 1.5),
            make_shared<Key>(","),
            make_shared<Key>(" ", 5.0),
            make_shared<Key>("."),
            make_shared<Key>("Enter", 1.5)
        }
    }, Size(30, 50))
    {}
};

int main(int argc, const char** argv)
{
    Application app(argc, argv, "keyboard");

    TopWindow win;

    TextBox textbox;
    textbox.set_align(alignmask::expand_horizontal | alignmask::top);
    textbox.text_flags().set({TextBox::flag::multiline, TextBox::flag::word_wrap});

    Keyboard keyboard({make_shared<QwertyLettersLowerCase>()});
    keyboard.set_align(alignmask::expand);

    VerticalBoxSizer vbox;
    vbox.set_align(alignmask::expand);
    vbox.add(textbox);
    vbox.add(keyboard);

    keyboard_focus(&textbox);

    win.add(vbox);
    win.show();

    return app.run();
}
