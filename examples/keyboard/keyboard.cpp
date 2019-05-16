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
using Key = Keyboard::Key;
using Panel = Keyboard::Panel;

struct Multichoice_q : public Panel
{
    Multichoice_q()
        : Panel(
    {
        {
            make_shared<Key>("q1"),
            make_shared<Key>("q2"),
            make_shared<Key>("q3"),
        }, {
            make_shared<Key>("q4"),
            make_shared<Key>("q5"),
            make_shared<Key>("q6"),
        }
    })
    {}
};

struct Multichoice_Q : public Panel
{
    Multichoice_Q()
        : Panel(
    {
        {
            make_shared<Key>("Q1"),
            make_shared<Key>("Q2"),
        }
    })
    {}
};

struct QwertyLettersLowerCase : public Panel
{
    QwertyLettersLowerCase()
        : Panel(
    {
        {
            make_shared<Key>("q", make_shared<Multichoice_q>()),
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
            make_shared<Key>("Shift", 1, 1.5),
            make_shared<Key>("z"),
            make_shared<Key>("x"),
            make_shared<Key>("c"),
            make_shared<Key>("v"),
            make_shared<Key>("b"),
            make_shared<Key>("n"),
            make_shared<Key>("m"),
            make_shared<Key>("<-", 1.5)
        }, {
            make_shared<Key>("?123", 2, 1.5),
            make_shared<Key>(","),
            make_shared<Key>(" ", 5.0),
            make_shared<Key>("."),
            make_shared<Key>("Enter", 1.5)
        }
    }, Size(30, 50))
    {}
};

struct QwertyLettersUpperCase : public Panel
{
    QwertyLettersUpperCase()
        : Panel(
    {
        {
            make_shared<Key>("Q", make_shared<Multichoice_Q>()),
            make_shared<Key>("W"),
            make_shared<Key>("E"),
            make_shared<Key>("R"),
            make_shared<Key>("T"),
            make_shared<Key>("Y"),
            make_shared<Key>("U"),
            make_shared<Key>("I"),
            make_shared<Key>("O"),
            make_shared<Key>("P")
        }, {
            make_shared<Key>("A"),
            make_shared<Key>("S"),
            make_shared<Key>("D"),
            make_shared<Key>("F"),
            make_shared<Key>("G"),
            make_shared<Key>("H"),
            make_shared<Key>("J"),
            make_shared<Key>("K"),
            make_shared<Key>("L")
        }, {
            make_shared<Key>("SHIFT", 0, 1.5),
            make_shared<Key>("Z"),
            make_shared<Key>("X"),
            make_shared<Key>("C"),
            make_shared<Key>("V"),
            make_shared<Key>("B"),
            make_shared<Key>("N"),
            make_shared<Key>("M"),
            make_shared<Key>("<-", 1.5)
        }, {
            make_shared<Key>("?123", 2, 1.5),
            make_shared<Key>(","),
            make_shared<Key>(" ", 5.0),
            make_shared<Key>("."),
            make_shared<Key>("Enter", 1.5)
        }
    }, Size(30, 50))
    {}
};

struct NumbersSymbols : public Panel
{
    NumbersSymbols()
        : Panel(
    {
        {
            make_shared<Key>("1"),
            make_shared<Key>("2"),
            make_shared<Key>("3"),
            make_shared<Key>("4"),
            make_shared<Key>("5"),
            make_shared<Key>("6"),
            make_shared<Key>("7"),
            make_shared<Key>("8"),
            make_shared<Key>("9"),
            make_shared<Key>("0")
        }, {
            make_shared<Key>("+"),
            make_shared<Key>("-"),
            make_shared<Key>("*"),
            make_shared<Key>("/"),
            make_shared<Key>("("),
            make_shared<Key>(")"),
            make_shared<Key>(""),
            make_shared<Key>(""),
            make_shared<Key>("")
        }, {
            make_shared<Key>("1/2"),
            make_shared<Key>("!"),
            make_shared<Key>("@"),
            make_shared<Key>("#"),
            make_shared<Key>("$"),
            make_shared<Key>("%"),
            make_shared<Key>("^"),
            make_shared<Key>("&"),
            make_shared<Key>("*"),
            make_shared<Key>("<-", 1.5)
        }, {
            make_shared<Key>("ABC", 0, 1.5),
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

    Keyboard keyboard(
    {
        make_shared<QwertyLettersLowerCase>(),
        make_shared<QwertyLettersUpperCase>(),
        make_shared<NumbersSymbols>()
    });
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
