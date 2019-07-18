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
#include <algorithm>

using namespace std;
using namespace egt;

int main(int argc, const char** argv)
{
    Application app(argc, argv, "keyboard");

    vector<shared_ptr<VirtualKeyboard::Key>> code_keyboard_row1 =
    {
        make_shared<VirtualKeyboard::Key>(0x0037),
        make_shared<VirtualKeyboard::Key>(0x0038),
        make_shared<VirtualKeyboard::Key>(0x0039),
    };
    random_shuffle(code_keyboard_row1.begin(), code_keyboard_row1.end());

    vector<shared_ptr<VirtualKeyboard::Key>> code_keyboard_row2 =
    {
        make_shared<VirtualKeyboard::Key>(0x0034),
        make_shared<VirtualKeyboard::Key>(0x0035),
        make_shared<VirtualKeyboard::Key>(0x0036),
    };
    random_shuffle(code_keyboard_row2.begin(), code_keyboard_row2.end());

    vector<shared_ptr<VirtualKeyboard::Key>> code_keyboard_row3 =
    {
        make_shared<VirtualKeyboard::Key>(0x0031),
        make_shared<VirtualKeyboard::Key>(0x0032),
        make_shared<VirtualKeyboard::Key>(0x0033),
    };
    random_shuffle(code_keyboard_row3.begin(), code_keyboard_row3.end());

    vector<shared_ptr<VirtualKeyboard::Key>> code_keyboard_row4 =
    {
        make_shared<VirtualKeyboard::Key>(Image("@cancel.png"), EKEY_BACKSPACE),
        make_shared<VirtualKeyboard::Key>(0x0030),
        make_shared<VirtualKeyboard::Key>(Image("@ok.png"), EKEY_ENTER),
    };

    VirtualKeyboard::panel_keys code_keyboard_panel_keys =
    {
        code_keyboard_row1, code_keyboard_row2, code_keyboard_row3, code_keyboard_row4
    };
    random_shuffle(code_keyboard_panel_keys.begin(), code_keyboard_panel_keys.end() - 1);

    VirtualKeyboard code_keyboard(
    { code_keyboard_panel_keys },
    Rect(Point(), Size(200, 200))
    );

    auto key_a = make_shared<VirtualKeyboard::Key>(0x0061);
    auto key_b = make_shared<VirtualKeyboard::Key>(0x0062);
    auto key_c = make_shared<VirtualKeyboard::Key>(0x0063);
    auto key_d = make_shared<VirtualKeyboard::Key>(0x0064);
    auto key_e = make_shared<VirtualKeyboard::Key>(0x0065);
    auto key_f = make_shared<VirtualKeyboard::Key>(0x0066);
    auto key_g = make_shared<VirtualKeyboard::Key>(0x0067);
    auto key_h = make_shared<VirtualKeyboard::Key>(0x0068);
    auto key_i = make_shared<VirtualKeyboard::Key>(0x0069);
    auto key_j = make_shared<VirtualKeyboard::Key>(0x006a);
    auto key_k = make_shared<VirtualKeyboard::Key>(0x006b);
    auto key_l = make_shared<VirtualKeyboard::Key>(0x006c);
    auto key_m = make_shared<VirtualKeyboard::Key>(0x006d);
    auto key_n = make_shared<VirtualKeyboard::Key>(0x006e);
    auto key_o = make_shared<VirtualKeyboard::Key>(0x006f);
    auto key_p = make_shared<VirtualKeyboard::Key>(0x0070);
    auto key_q = make_shared<VirtualKeyboard::Key>(0x0071);
    auto key_r = make_shared<VirtualKeyboard::Key>(0x0072);
    auto key_s = make_shared<VirtualKeyboard::Key>(0x0073);
    auto key_t = make_shared<VirtualKeyboard::Key>(0x0074);
    auto key_u = make_shared<VirtualKeyboard::Key>(0x0075);
    auto key_v = make_shared<VirtualKeyboard::Key>(0x0076);
    auto key_w = make_shared<VirtualKeyboard::Key>(0x0077);
    auto key_x = make_shared<VirtualKeyboard::Key>(0x0078);
    auto key_y = make_shared<VirtualKeyboard::Key>(0x0079);
    auto key_z = make_shared<VirtualKeyboard::Key>(0x007a);

    vector<shared_ptr<VirtualKeyboard::Key>> abc_keyboard_row1 =
    {
        key_a, key_b, key_c, key_d, key_e, key_f, key_g, key_h, key_i
    };
    for (auto& key : abc_keyboard_row1)
        key->set_color(Palette::ColorId::button_bg, Palette::red);

    vector<shared_ptr<VirtualKeyboard::Key>> abc_keyboard_row2 =
    {
        key_j, key_k, key_l, key_m, key_n, key_o, key_p, key_q, key_r
    };
    for (auto& key : abc_keyboard_row2)
        key->set_color(Palette::ColorId::button_bg, Palette::green);

    vector<shared_ptr<VirtualKeyboard::Key>> abc_keyboard_row3 =
    {
        key_s, key_t, key_u, key_v, key_w, key_x, key_y, key_z
    };
    for (auto& key : abc_keyboard_row3)
        key->set_color(Palette::ColorId::button_bg, Palette::blue);

    VirtualKeyboard abc_keyboard(
    {
        {
            abc_keyboard_row1,
            abc_keyboard_row2,
            abc_keyboard_row3,
        },
    },
    Rect(Point(), Size(400, 100))
    );
    abc_keyboard.set_align(alignmask::center);

    HorizontalBoxSizer hbox;
    hbox.set_align(alignmask::expand);
    hbox.add(code_keyboard);
    hbox.add(expand_horizontal(abc_keyboard));

    TopWindow win;
    win.add(hbox);
    win.show();

    return app.run();
}
