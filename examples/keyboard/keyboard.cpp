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

struct Multichoice_w : public MultichoicePanel
{
    Multichoice_w()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x0),
            make_shared<Keyboard::Key>(0x0),
            make_shared<Keyboard::Key>(0x0),
        }, {
            make_shared<Keyboard::Key>(0x0),
            make_shared<Keyboard::Key>(0x0),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_e : public MultichoicePanel
{
    Multichoice_e()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x0117),
            make_shared<Keyboard::Key>(0x0119),
            make_shared<Keyboard::Key>(0x011b),
            make_shared<Keyboard::Key>(0x0115),
            make_shared<Keyboard::Key>(0x04d9),
        }, {
            make_shared<Keyboard::Key>(0x00e8),
            make_shared<Keyboard::Key>(0x00e9),
            make_shared<Keyboard::Key>(0x00ea),
            make_shared<Keyboard::Key>(0x00eb),
            make_shared<Keyboard::Key>(0x0113),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_E : public MultichoicePanel
{
    Multichoice_E()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x0116),
            make_shared<Keyboard::Key>(0x0118),
            make_shared<Keyboard::Key>(0x011a),
            make_shared<Keyboard::Key>(0x0115),
            make_shared<Keyboard::Key>(0x04d8),
        }, {
            make_shared<Keyboard::Key>(0x00c8),
            make_shared<Keyboard::Key>(0x00c9),
            make_shared<Keyboard::Key>(0x00ca),
            make_shared<Keyboard::Key>(0x00cb),
            make_shared<Keyboard::Key>(0x0112),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_r : public MultichoicePanel
{
    Multichoice_r()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x0155),
            make_shared<Keyboard::Key>(0x0159),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_R : public MultichoicePanel
{
    Multichoice_R()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x0154),
            make_shared<Keyboard::Key>(0x0158),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_t : public MultichoicePanel
{
    Multichoice_t()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x0fe),
            make_shared<Keyboard::Key>(0x0165),
            make_shared<Keyboard::Key>(0x021b),
            make_shared<Keyboard::Key>(0x0163),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_T : public MultichoicePanel
{
    Multichoice_T()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x0fe),
            make_shared<Keyboard::Key>(0x0164),
            make_shared<Keyboard::Key>(0x021a),
            make_shared<Keyboard::Key>(0x0162),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_y : public MultichoicePanel
{
    Multichoice_y()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x0fd),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_Y : public MultichoicePanel
{
    Multichoice_Y()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x0dd),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_u : public MultichoicePanel
{
    Multichoice_u()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x0173),
            make_shared<Keyboard::Key>(0x0171),
            make_shared<Keyboard::Key>(0x016f),
            make_shared<Keyboard::Key>(0x016b),
        }, {
            make_shared<Keyboard::Key>(0x00fc),
            make_shared<Keyboard::Key>(0x00fb),
            make_shared<Keyboard::Key>(0x00fa),
            make_shared<Keyboard::Key>(0x00f9),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_U : public MultichoicePanel
{
    Multichoice_U()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x0172),
            make_shared<Keyboard::Key>(0x0170),
            make_shared<Keyboard::Key>(0x016e),
            make_shared<Keyboard::Key>(0x016a),
        }, {
            make_shared<Keyboard::Key>(0x00dc),
            make_shared<Keyboard::Key>(0x00db),
            make_shared<Keyboard::Key>(0x00da),
            make_shared<Keyboard::Key>(0x00d9),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_i : public MultichoicePanel
{
    Multichoice_i()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x0131),
            make_shared<Keyboard::Key>(0x012e),
            make_shared<Keyboard::Key>(0x012b),
        }, {
            make_shared<Keyboard::Key>(0x00ef),
            make_shared<Keyboard::Key>(0x00ee),
            make_shared<Keyboard::Key>(0x00ed),
            make_shared<Keyboard::Key>(0x00ec),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_I : public MultichoicePanel
{
    Multichoice_I()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x0130),
            make_shared<Keyboard::Key>(0x012d),
            make_shared<Keyboard::Key>(0x012a),
        }, {
            make_shared<Keyboard::Key>(0x00cf),
            make_shared<Keyboard::Key>(0x00ce),
            make_shared<Keyboard::Key>(0x00cd),
            make_shared<Keyboard::Key>(0x00cc),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_o : public MultichoicePanel
{
    Multichoice_o()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x0153),
            make_shared<Keyboard::Key>(0x0151),
            make_shared<Keyboard::Key>(0x00f8),
            make_shared<Keyboard::Key>(0x00f6),
        }, {
            make_shared<Keyboard::Key>(0x00f5),
            make_shared<Keyboard::Key>(0x00f4),
            make_shared<Keyboard::Key>(0x00f3),
            make_shared<Keyboard::Key>(0x00f2),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_O : public MultichoicePanel
{
    Multichoice_O()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x0152),
            make_shared<Keyboard::Key>(0x0150),
            make_shared<Keyboard::Key>(0x00d8),
            make_shared<Keyboard::Key>(0x00d6),
        }, {
            make_shared<Keyboard::Key>(0x00d5),
            make_shared<Keyboard::Key>(0x00d4),
            make_shared<Keyboard::Key>(0x00d3),
            make_shared<Keyboard::Key>(0x00d2),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_a : public MultichoicePanel
{
    Multichoice_a()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x00e5),
            make_shared<Keyboard::Key>(0x00e6),
            make_shared<Keyboard::Key>(0x0101),
            make_shared<Keyboard::Key>(0x0103),
            make_shared<Keyboard::Key>(0x0105),
        }, {
            make_shared<Keyboard::Key>(0x00e0),
            make_shared<Keyboard::Key>(0x00e1),
            make_shared<Keyboard::Key>(0x00e2),
            make_shared<Keyboard::Key>(0x00e3),
            make_shared<Keyboard::Key>(0x00e4),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_A : public MultichoicePanel
{
    Multichoice_A()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x00c5),
            make_shared<Keyboard::Key>(0x00c6),
            make_shared<Keyboard::Key>(0x0100),
            make_shared<Keyboard::Key>(0x0102),
            make_shared<Keyboard::Key>(0x0104),
        }, {
            make_shared<Keyboard::Key>(0x00c0),
            make_shared<Keyboard::Key>(0x00c1),
            make_shared<Keyboard::Key>(0x00c2),
            make_shared<Keyboard::Key>(0x00c3),
            make_shared<Keyboard::Key>(0x00c4),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_s : public MultichoicePanel
{
    Multichoice_s()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x00df),
            make_shared<Keyboard::Key>(0x00a7),
            make_shared<Keyboard::Key>(0x015b),
            make_shared<Keyboard::Key>(0x0161),
            make_shared<Keyboard::Key>(0x015f),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_S : public MultichoicePanel
{
    Multichoice_S()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x00df),
            make_shared<Keyboard::Key>(0x00a7),
            make_shared<Keyboard::Key>(0x015a),
            make_shared<Keyboard::Key>(0x0160),
            make_shared<Keyboard::Key>(0x015e),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_d : public MultichoicePanel
{
    Multichoice_d()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x010f),
            make_shared<Keyboard::Key>(0x0111),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_D : public MultichoicePanel
{
    Multichoice_D()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x010e),
            make_shared<Keyboard::Key>(0x0110),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_g : public MultichoicePanel
{
    Multichoice_g()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x0123),
            make_shared<Keyboard::Key>(0x011f),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_G : public MultichoicePanel
{
    Multichoice_G()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x0122),
            make_shared<Keyboard::Key>(0x011e),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_k : public MultichoicePanel
{
    Multichoice_k()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x0137),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_K : public MultichoicePanel
{
    Multichoice_K()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x0136),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_l : public MultichoicePanel
{
    Multichoice_l()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x0142),
            make_shared<Keyboard::Key>(0x013e),
            make_shared<Keyboard::Key>(0x013c),
            make_shared<Keyboard::Key>(0x013a),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_L : public MultichoicePanel
{
    Multichoice_L()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x0141),
            make_shared<Keyboard::Key>(0x013d),
            make_shared<Keyboard::Key>(0x013b),
            make_shared<Keyboard::Key>(0x0139),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_z : public MultichoicePanel
{
    Multichoice_z()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x017a),
            make_shared<Keyboard::Key>(0x017c),
            make_shared<Keyboard::Key>(0x017e),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_Z : public MultichoicePanel
{
    Multichoice_Z()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x0179),
            make_shared<Keyboard::Key>(0x017b),
            make_shared<Keyboard::Key>(0x017d),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_c : public MultichoicePanel
{
    Multichoice_c()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x00e7),
            make_shared<Keyboard::Key>(0x0107),
            make_shared<Keyboard::Key>(0x010d),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_C : public MultichoicePanel
{
    Multichoice_C()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x00c7),
            make_shared<Keyboard::Key>(0x0106),
            make_shared<Keyboard::Key>(0x010c),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_n : public MultichoicePanel
{
    Multichoice_n()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x0148),
            make_shared<Keyboard::Key>(0x0146),
            make_shared<Keyboard::Key>(0x0144),
            make_shared<Keyboard::Key>(0x00f1),
        }
    }, Size(45, 75))
    {}
};

struct Multichoice_N : public MultichoicePanel
{
    Multichoice_N()
        : MultichoicePanel(
    {
        {
            make_shared<Keyboard::Key>(0x0147),
            make_shared<Keyboard::Key>(0x0145),
            make_shared<Keyboard::Key>(0x0143),
            make_shared<Keyboard::Key>(0x00d1),
        }
    }, Size(45, 75))
    {}
};

struct QwertyLettersLowerCase : public MainPanel
{
    QwertyLettersLowerCase()
        : MainPanel(
    {
        {
            make_shared<Keyboard::Key>(0x0031),
            make_shared<Keyboard::Key>(0x0032),
            make_shared<Keyboard::Key>(0x0033),
            make_shared<Keyboard::Key>(0x0034),
            make_shared<Keyboard::Key>(0x0035),
            make_shared<Keyboard::Key>(0x0036),
            make_shared<Keyboard::Key>(0x0037),
            make_shared<Keyboard::Key>(0x0038),
            make_shared<Keyboard::Key>(0x0039),
            make_shared<Keyboard::Key>(0x0030)
        }, {
            make_shared<Keyboard::Key>(0x0071),
            make_shared<Keyboard::Key>(0x0077),
            make_shared<Keyboard::Key>(0x0065, make_shared<Multichoice_e>()),
            make_shared<Keyboard::Key>(0x0072, make_shared<Multichoice_r>()),
            make_shared<Keyboard::Key>(0x0074, make_shared<Multichoice_t>()),
            make_shared<Keyboard::Key>(0x0079, make_shared<Multichoice_y>()),
            make_shared<Keyboard::Key>(0x0075, make_shared<Multichoice_u>()),
            make_shared<Keyboard::Key>(0x0069, make_shared<Multichoice_i>()),
            make_shared<Keyboard::Key>(0x006f, make_shared<Multichoice_o>()),
            make_shared<Keyboard::Key>(0x0070)
        }, {
            make_shared<Keyboard::Key>(0x0061, make_shared<Multichoice_a>()),
            make_shared<Keyboard::Key>(0x0073, make_shared<Multichoice_s>()),
            make_shared<Keyboard::Key>(0x0064, make_shared<Multichoice_d>()),
            make_shared<Keyboard::Key>(0x0066),
            make_shared<Keyboard::Key>(0x0067, make_shared<Multichoice_g>()),
            make_shared<Keyboard::Key>(0x0068),
            make_shared<Keyboard::Key>(0x006a),
            make_shared<Keyboard::Key>(0x006b, make_shared<Multichoice_k>()),
            make_shared<Keyboard::Key>(0x006c, make_shared<Multichoice_l>())
        }, {
            make_shared<Keyboard::Key>("\u21d1", 1, 1.5),
            make_shared<Keyboard::Key>(0x007a, make_shared<Multichoice_z>()),
            make_shared<Keyboard::Key>(0x0078),
            make_shared<Keyboard::Key>(0x0063, make_shared<Multichoice_c>()),
            make_shared<Keyboard::Key>(0x0076),
            make_shared<Keyboard::Key>(0x0062),
            make_shared<Keyboard::Key>(0x006e, make_shared<Multichoice_n>()),
            make_shared<Keyboard::Key>(0x006d),
            make_shared<Keyboard::Key>("\u2190", EKEY_BACKSPACE, 1.5)
        }, {
            make_shared<Keyboard::Key>("!#\u263a", 2, 1.5),
            make_shared<Keyboard::Key>(0x0),
            make_shared<Keyboard::Key>(0x0020, 5.0),
            make_shared<Keyboard::Key>(0x002e),
            make_shared<Keyboard::Key>("\u21b5", EKEY_ENTER, 1.5)
        }
    }, Size(30, 50), 4)
    {}
};

struct QwertyLettersUpperCase : public MainPanel
{
    QwertyLettersUpperCase()
        : MainPanel(
    {
        {
            make_shared<Keyboard::Key>(0x0031),
            make_shared<Keyboard::Key>(0x0032),
            make_shared<Keyboard::Key>(0x0033),
            make_shared<Keyboard::Key>(0x0034),
            make_shared<Keyboard::Key>(0x0035),
            make_shared<Keyboard::Key>(0x0036),
            make_shared<Keyboard::Key>(0x0037),
            make_shared<Keyboard::Key>(0x0038),
            make_shared<Keyboard::Key>(0x0039),
            make_shared<Keyboard::Key>(0x0030)
        }, {
            make_shared<Keyboard::Key>(0x0051),
            make_shared<Keyboard::Key>(0x0057),
            make_shared<Keyboard::Key>(0x0045, make_shared<Multichoice_E>()),
            make_shared<Keyboard::Key>(0x0052, make_shared<Multichoice_R>()),
            make_shared<Keyboard::Key>(0x0054, make_shared<Multichoice_T>()),
            make_shared<Keyboard::Key>(0x0059, make_shared<Multichoice_Y>()),
            make_shared<Keyboard::Key>(0x0055, make_shared<Multichoice_U>()),
            make_shared<Keyboard::Key>(0x0049, make_shared<Multichoice_I>()),
            make_shared<Keyboard::Key>(0x004f, make_shared<Multichoice_O>()),
            make_shared<Keyboard::Key>(0x0050)
        }, {
            make_shared<Keyboard::Key>(0x0041, make_shared<Multichoice_A>()),
            make_shared<Keyboard::Key>(0x0053, make_shared<Multichoice_S>()),
            make_shared<Keyboard::Key>(0x0044, make_shared<Multichoice_D>()),
            make_shared<Keyboard::Key>(0x0046),
            make_shared<Keyboard::Key>(0x0047, make_shared<Multichoice_G>()),
            make_shared<Keyboard::Key>(0x0048),
            make_shared<Keyboard::Key>(0x004a),
            make_shared<Keyboard::Key>(0x004b, make_shared<Multichoice_K>()),
            make_shared<Keyboard::Key>(0x004c, make_shared<Multichoice_L>())
        }, {
            make_shared<Keyboard::Key>("\u21d1", 0, 1.5),
            make_shared<Keyboard::Key>(0x005a, make_shared<Multichoice_Z>()),
            make_shared<Keyboard::Key>(0x0058),
            make_shared<Keyboard::Key>(0x0043, make_shared<Multichoice_C>()),
            make_shared<Keyboard::Key>(0x0056),
            make_shared<Keyboard::Key>(0x0042),
            make_shared<Keyboard::Key>(0x004e, make_shared<Multichoice_N>()),
            make_shared<Keyboard::Key>(0x004d),
            make_shared<Keyboard::Key>("\u2190", EKEY_BACKSPACE, 1.5)
        }, {
            make_shared<Keyboard::Key>("!#\u263a", 2, 1.5),
            make_shared<Keyboard::Key>(0x0),
            make_shared<Keyboard::Key>(0x0020, 5.0),
            make_shared<Keyboard::Key>(0x002e),
            make_shared<Keyboard::Key>("\u21b5", EKEY_ENTER, 1.5)
        }
    }, Size(30, 50), 4)
    {}
};

struct QwertySymbols1 : public MainPanel
{
    QwertySymbols1()
        : MainPanel(
    {
        {
            make_shared<Keyboard::Key>(0x0031),
            make_shared<Keyboard::Key>(0x0032),
            make_shared<Keyboard::Key>(0x0033),
            make_shared<Keyboard::Key>(0x0034),
            make_shared<Keyboard::Key>(0x0035),
            make_shared<Keyboard::Key>(0x0036),
            make_shared<Keyboard::Key>(0x0037),
            make_shared<Keyboard::Key>(0x0038),
            make_shared<Keyboard::Key>(0x0039),
            make_shared<Keyboard::Key>(0x0030)
        }, {
            make_shared<Keyboard::Key>(0x002b),
            make_shared<Keyboard::Key>(0x0078),
            make_shared<Keyboard::Key>(0x00f7),
            make_shared<Keyboard::Key>(0x003d),
            make_shared<Keyboard::Key>(0x002f),
            make_shared<Keyboard::Key>(0x005f),
            make_shared<Keyboard::Key>(0x20ac),
            make_shared<Keyboard::Key>(0x00a3),
            make_shared<Keyboard::Key>(0x00a5),
            make_shared<Keyboard::Key>(0x20a9)
        }, {
            make_shared<Keyboard::Key>(0x0021),
            make_shared<Keyboard::Key>(0x0040),
            make_shared<Keyboard::Key>(0x0023),
            make_shared<Keyboard::Key>(0x0024),
            make_shared<Keyboard::Key>(0x0025),
            make_shared<Keyboard::Key>(0x005e),
            make_shared<Keyboard::Key>(0x0026),
            make_shared<Keyboard::Key>(0x002a),
            make_shared<Keyboard::Key>(0x0028),
            make_shared<Keyboard::Key>(0x0029)
        }, {
            make_shared<Keyboard::Key>("1/2", 3, 1.5),
            make_shared<Keyboard::Key>(0x002d),
            make_shared<Keyboard::Key>(0x0027),
            make_shared<Keyboard::Key>(0x0022),
            make_shared<Keyboard::Key>(0x003a),
            make_shared<Keyboard::Key>(0x003b),
            make_shared<Keyboard::Key>(0x002c),
            make_shared<Keyboard::Key>(0x003f),
            make_shared<Keyboard::Key>("\u2190", EKEY_BACKSPACE, 1.5)
        }, {
            make_shared<Keyboard::Key>("ABC", 2, 1.5),
            make_shared<Keyboard::Key>(0x0),
            make_shared<Keyboard::Key>(0x0020, 5.0),
            make_shared<Keyboard::Key>(0x002e),
            make_shared<Keyboard::Key>("\u21b5", EKEY_ENTER, 1.5)
        }
    }, Size(30, 50), 4)
    {}
};

struct QwertySymbols2 : public MainPanel
{
    QwertySymbols2()
        : MainPanel(
    {
        {
            make_shared<Keyboard::Key>(0x0031),
            make_shared<Keyboard::Key>(0x0032),
            make_shared<Keyboard::Key>(0x0033),
            make_shared<Keyboard::Key>(0x0034),
            make_shared<Keyboard::Key>(0x0035),
            make_shared<Keyboard::Key>(0x0036),
            make_shared<Keyboard::Key>(0x0037),
            make_shared<Keyboard::Key>(0x0038),
            make_shared<Keyboard::Key>(0x0039),
            make_shared<Keyboard::Key>(0x0030)
        }, {
            make_shared<Keyboard::Key>(0x0060),
            make_shared<Keyboard::Key>(0x007e),
            make_shared<Keyboard::Key>(0x005c),
            make_shared<Keyboard::Key>(0x007c),
            make_shared<Keyboard::Key>(0x003c),
            make_shared<Keyboard::Key>(0x003e),
            make_shared<Keyboard::Key>(0x007b),
            make_shared<Keyboard::Key>(0x007d),
            make_shared<Keyboard::Key>(0x005b),
            make_shared<Keyboard::Key>(0x005d)
        }, {
            make_shared<Keyboard::Key>(0x00b0),
            make_shared<Keyboard::Key>(0x2022),
            make_shared<Keyboard::Key>(0x25cb),
            make_shared<Keyboard::Key>(0x25cf),
            make_shared<Keyboard::Key>(0x25a1),
            make_shared<Keyboard::Key>(0x25a0),
            make_shared<Keyboard::Key>(0x2664),
            make_shared<Keyboard::Key>(0x2661),
            make_shared<Keyboard::Key>(0x2662),
            make_shared<Keyboard::Key>(0x2667)
        }, {
            make_shared<Keyboard::Key>("2/2", 2, 1.5),
            make_shared<Keyboard::Key>(0x2606),
            make_shared<Keyboard::Key>(0x25aa),
            make_shared<Keyboard::Key>(0x0), //
            make_shared<Keyboard::Key>(0x00ab),
            make_shared<Keyboard::Key>(0x00bb),
            make_shared<Keyboard::Key>(0x00a1),
            make_shared<Keyboard::Key>(0x00bf),
            make_shared<Keyboard::Key>("\u2190", EKEY_BACKSPACE, 1.5)
        }, {
            make_shared<Keyboard::Key>("ABC", 2, 1.5),
            make_shared<Keyboard::Key>(0x0),
            make_shared<Keyboard::Key>(0x0020, 5.0),
            make_shared<Keyboard::Key>(0x002e),
            make_shared<Keyboard::Key>("\u21b5", EKEY_ENTER, 1.5)
        }
    }, Size(30, 50), 4)
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
        make_shared<QwertySymbols1>(),
        make_shared<QwertySymbols2>()
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
