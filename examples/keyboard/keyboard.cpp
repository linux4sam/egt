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
using MainPanel = Keyboard::MainPanel;
using MultichoicePanel = Keyboard::MultichoicePanel;

struct Multichoice_q : public MultichoicePanel
{
    Multichoice_q()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>("q1"),
            make_shared<Keyboard::Key>("q2"),
            make_shared<Keyboard::Key>("q3"),
        }, {
            make_shared<Keyboard::Key>("q4"),
            make_shared<Keyboard::Key>("q5"),
            make_shared<Keyboard::Key>("q6"),
        }
    })
    {}
};

struct Multichoice_w : public MultichoicePanel
{
    Multichoice_w()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>("w1"),
            make_shared<Keyboard::Key>("w2"),
            make_shared<Keyboard::Key>("w3"),
        }, {
            make_shared<Keyboard::Key>("w4"),
            make_shared<Keyboard::Key>("w5"),
        }
    }, Size(30, 50))
    {}
};

struct Multichoice_e : public MultichoicePanel
{
    Multichoice_e()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>("e1"),
            make_shared<Keyboard::Key>("e2"),
            make_shared<Keyboard::Key>("e3"),
        }
    }, Size(30, 50))
    {}
};

struct Multichoice_s : public MultichoicePanel
{
    Multichoice_s()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>("s1"),
            make_shared<Keyboard::Key>("s2"),
            make_shared<Keyboard::Key>("s3"),
        }, {
            make_shared<Keyboard::Key>("s4"),
            make_shared<Keyboard::Key>("s5"),
            make_shared<Keyboard::Key>("s6"),
        }
    }, Size(30, 50))
    {}
};

struct Multichoice_Q : public MultichoicePanel
{
    Multichoice_Q()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>("Q1"),
            make_shared<Keyboard::Key>("Q2"),
        }
    }, Size(30, 50))
    {}
};

struct QwertyLettersLowerCase : public MainPanel
{
    QwertyLettersLowerCase()
        : MainPanel(
    {
        {
            make_shared<Keyboard::Key>("q", make_shared<Multichoice_q>()),
            make_shared<Keyboard::Key>("w", make_shared<Multichoice_w>()),
            make_shared<Keyboard::Key>("e", make_shared<Multichoice_e>()),
            make_shared<Keyboard::Key>("r"),
            make_shared<Keyboard::Key>("t"),
            make_shared<Keyboard::Key>("y"),
            make_shared<Keyboard::Key>("u"),
            make_shared<Keyboard::Key>("i"),
            make_shared<Keyboard::Key>("o"),
            make_shared<Keyboard::Key>("p")
        }, {
            make_shared<Keyboard::Key>("a"),
            make_shared<Keyboard::Key>("s", make_shared<Multichoice_s>()),
            make_shared<Keyboard::Key>("d"),
            make_shared<Keyboard::Key>("f"),
            make_shared<Keyboard::Key>("g"),
            make_shared<Keyboard::Key>("h"),
            make_shared<Keyboard::Key>("j"),
            make_shared<Keyboard::Key>("k"),
            make_shared<Keyboard::Key>("l")
        }, {
            make_shared<Keyboard::Key>("Shift", 1, 1.5),
            make_shared<Keyboard::Key>("z"),
            make_shared<Keyboard::Key>("x"),
            make_shared<Keyboard::Key>("c"),
            make_shared<Keyboard::Key>("v"),
            make_shared<Keyboard::Key>("b"),
            make_shared<Keyboard::Key>("n"),
            make_shared<Keyboard::Key>("m"),
            make_shared<Keyboard::Key>("<-", 1.5)
        }, {
            make_shared<Keyboard::Key>("?123", 2, 1.5),
            make_shared<Keyboard::Key>(","),
            make_shared<Keyboard::Key>(" ", 5.0),
            make_shared<Keyboard::Key>("."),
            make_shared<Keyboard::Key>("Enter", 1.5)
        }
    }, Size(30, 50))
    {}
};

struct QwertyLettersUpperCase : public MainPanel
{
    QwertyLettersUpperCase()
        : MainPanel(
    {
        {
            make_shared<Keyboard::Key>("Q", make_shared<Multichoice_Q>()),
            make_shared<Keyboard::Key>("W"),
            make_shared<Keyboard::Key>("E"),
            make_shared<Keyboard::Key>("R"),
            make_shared<Keyboard::Key>("T"),
            make_shared<Keyboard::Key>("Y"),
            make_shared<Keyboard::Key>("U"),
            make_shared<Keyboard::Key>("I"),
            make_shared<Keyboard::Key>("O"),
            make_shared<Keyboard::Key>("P")
        }, {
            make_shared<Keyboard::Key>("A"),
            make_shared<Keyboard::Key>("S"),
            make_shared<Keyboard::Key>("D"),
            make_shared<Keyboard::Key>("F"),
            make_shared<Keyboard::Key>("G"),
            make_shared<Keyboard::Key>("H"),
            make_shared<Keyboard::Key>("J"),
            make_shared<Keyboard::Key>("K"),
            make_shared<Keyboard::Key>("L")
        }, {
            make_shared<Keyboard::Key>("SHIFT", 0, 1.5),
            make_shared<Keyboard::Key>("Z"),
            make_shared<Keyboard::Key>("X"),
            make_shared<Keyboard::Key>("C"),
            make_shared<Keyboard::Key>("V"),
            make_shared<Keyboard::Key>("B"),
            make_shared<Keyboard::Key>("N"),
            make_shared<Keyboard::Key>("M"),
            make_shared<Keyboard::Key>("<-", 1.5)
        }, {
            make_shared<Keyboard::Key>("?123", 2, 1.5),
            make_shared<Keyboard::Key>(","),
            make_shared<Keyboard::Key>(" ", 5.0),
            make_shared<Keyboard::Key>("."),
            make_shared<Keyboard::Key>("Enter", 1.5)
        }
    }, Size(30, 50))
    {}
};

struct NumbersSymbols : public MainPanel
{
    NumbersSymbols()
        : MainPanel(
    {
        {
            make_shared<Keyboard::Key>("1"),
            make_shared<Keyboard::Key>("2"),
            make_shared<Keyboard::Key>("3"),
            make_shared<Keyboard::Key>("4"),
            make_shared<Keyboard::Key>("5"),
            make_shared<Keyboard::Key>("6"),
            make_shared<Keyboard::Key>("7"),
            make_shared<Keyboard::Key>("8"),
            make_shared<Keyboard::Key>("9"),
            make_shared<Keyboard::Key>("0")
        }, {
            make_shared<Keyboard::Key>("+"),
            make_shared<Keyboard::Key>("-"),
            make_shared<Keyboard::Key>("*"),
            make_shared<Keyboard::Key>("/"),
            make_shared<Keyboard::Key>("("),
            make_shared<Keyboard::Key>(")"),
            make_shared<Keyboard::Key>(""),
            make_shared<Keyboard::Key>(""),
            make_shared<Keyboard::Key>("")
        }, {
            make_shared<Keyboard::Key>("1/2"),
            make_shared<Keyboard::Key>("!"),
            make_shared<Keyboard::Key>("@"),
            make_shared<Keyboard::Key>("#"),
            make_shared<Keyboard::Key>("$"),
            make_shared<Keyboard::Key>("%"),
            make_shared<Keyboard::Key>("^"),
            make_shared<Keyboard::Key>("&"),
            make_shared<Keyboard::Key>("*"),
            make_shared<Keyboard::Key>("<-", 1.5)
        }, {
            make_shared<Keyboard::Key>("ABC", 0, 1.5),
            make_shared<Keyboard::Key>(","),
            make_shared<Keyboard::Key>(" ", 5.0),
            make_shared<Keyboard::Key>("."),
            make_shared<Keyboard::Key>("Enter", 1.5)
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
