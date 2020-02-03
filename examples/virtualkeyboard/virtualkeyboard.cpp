/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <algorithm>
#include <egt/ui>
#include <random>

int main(int argc, const char** argv)
{
    egt::Application app(argc, argv, "keyboard");

    std::random_device rd;
    std::mt19937 g(rd());

    std::vector<std::shared_ptr<egt::VirtualKeyboard::Key>> code_keyboard_row1 =
    {
        std::make_shared<egt::VirtualKeyboard::Key>(0x0037),
        std::make_shared<egt::VirtualKeyboard::Key>(0x0038),
        std::make_shared<egt::VirtualKeyboard::Key>(0x0039),
    };
    shuffle(code_keyboard_row1.begin(), code_keyboard_row1.end(), g);

    std::vector<std::shared_ptr<egt::VirtualKeyboard::Key>> code_keyboard_row2 =
    {
        std::make_shared<egt::VirtualKeyboard::Key>(0x0034),
        std::make_shared<egt::VirtualKeyboard::Key>(0x0035),
        std::make_shared<egt::VirtualKeyboard::Key>(0x0036),
    };
    shuffle(code_keyboard_row2.begin(), code_keyboard_row2.end(), g);

    std::vector<std::shared_ptr<egt::VirtualKeyboard::Key>> code_keyboard_row3 =
    {
        std::make_shared<egt::VirtualKeyboard::Key>(0x0031),
        std::make_shared<egt::VirtualKeyboard::Key>(0x0032),
        std::make_shared<egt::VirtualKeyboard::Key>(0x0033),
    };
    shuffle(code_keyboard_row3.begin(), code_keyboard_row3.end(), g);

    std::vector<std::shared_ptr<egt::VirtualKeyboard::Key>> code_keyboard_row4 =
    {
        std::make_shared<egt::VirtualKeyboard::Key>(egt::Image("icon:cancel.png"), egt::EKEY_BACKSPACE),
        std::make_shared<egt::VirtualKeyboard::Key>(0x0030),
        std::make_shared<egt::VirtualKeyboard::Key>(egt::Image("icon:ok.png"), egt::EKEY_ENTER),
    };

    egt::VirtualKeyboard::PanelKeys code_keyboard_panel_keys =
    {
        code_keyboard_row1, code_keyboard_row2, code_keyboard_row3, code_keyboard_row4
    };
    shuffle(code_keyboard_panel_keys.begin(), code_keyboard_panel_keys.end() - 1, g);

    egt::VirtualKeyboard code_keyboard(
    { code_keyboard_panel_keys },
    egt::Rect(egt::Point(), egt::Size(200, 200))
    );

    auto key_a = std::make_shared<egt::VirtualKeyboard::Key>(0x0061);
    auto key_b = std::make_shared<egt::VirtualKeyboard::Key>(0x0062);
    auto key_c = std::make_shared<egt::VirtualKeyboard::Key>(0x0063);
    auto key_d = std::make_shared<egt::VirtualKeyboard::Key>(0x0064);
    auto key_e = std::make_shared<egt::VirtualKeyboard::Key>(0x0065);
    auto key_f = std::make_shared<egt::VirtualKeyboard::Key>(0x0066);
    auto key_g = std::make_shared<egt::VirtualKeyboard::Key>(0x0067);
    auto key_h = std::make_shared<egt::VirtualKeyboard::Key>(0x0068);
    auto key_i = std::make_shared<egt::VirtualKeyboard::Key>(0x0069);
    auto key_j = std::make_shared<egt::VirtualKeyboard::Key>(0x006a);
    auto key_k = std::make_shared<egt::VirtualKeyboard::Key>(0x006b);
    auto key_l = std::make_shared<egt::VirtualKeyboard::Key>(0x006c);
    auto key_m = std::make_shared<egt::VirtualKeyboard::Key>(0x006d);
    auto key_n = std::make_shared<egt::VirtualKeyboard::Key>(0x006e);
    auto key_o = std::make_shared<egt::VirtualKeyboard::Key>(0x006f);
    auto key_p = std::make_shared<egt::VirtualKeyboard::Key>(0x0070);
    auto key_q = std::make_shared<egt::VirtualKeyboard::Key>(0x0071);
    auto key_r = std::make_shared<egt::VirtualKeyboard::Key>(0x0072);
    auto key_s = std::make_shared<egt::VirtualKeyboard::Key>(0x0073);
    auto key_t = std::make_shared<egt::VirtualKeyboard::Key>(0x0074);
    auto key_u = std::make_shared<egt::VirtualKeyboard::Key>(0x0075);
    auto key_v = std::make_shared<egt::VirtualKeyboard::Key>(0x0076);
    auto key_w = std::make_shared<egt::VirtualKeyboard::Key>(0x0077);
    auto key_x = std::make_shared<egt::VirtualKeyboard::Key>(0x0078);
    auto key_y = std::make_shared<egt::VirtualKeyboard::Key>(0x0079);
    auto key_z = std::make_shared<egt::VirtualKeyboard::Key>(0x007a);

    std::vector<std::shared_ptr<egt::VirtualKeyboard::Key>> abc_keyboard_row1 =
    {
        key_a, key_b, key_c, key_d, key_e, key_f, key_g, key_h, key_i
    };
    for (auto& key : abc_keyboard_row1)
        key->color(egt::Palette::ColorId::button_bg, egt::Palette::red);

    std::vector<std::shared_ptr<egt::VirtualKeyboard::Key>> abc_keyboard_row2 =
    {
        key_j, key_k, key_l, key_m, key_n, key_o, key_p, key_q, key_r
    };
    for (auto& key : abc_keyboard_row2)
        key->color(egt::Palette::ColorId::button_bg, egt::Palette::green);

    std::vector<std::shared_ptr<egt::VirtualKeyboard::Key>> abc_keyboard_row3 =
    {
        key_s, key_t, key_u, key_v, key_w, key_x, key_y, key_z
    };
    for (auto& key : abc_keyboard_row3)
        key->color(egt::Palette::ColorId::button_bg, egt::Palette::blue);

    egt::VirtualKeyboard abc_keyboard(
    {
        {
            abc_keyboard_row1,
            abc_keyboard_row2,
            abc_keyboard_row3,
        },
    },
    egt::Rect(egt::Point(), egt::Size(400, 100))
    );
    abc_keyboard.align(egt::AlignFlag::center);

    egt::VerticalBoxSizer vbox;
    vbox.align(egt::AlignFlag::expand);
    vbox.add(egt::expand_horizontal(abc_keyboard));
    vbox.add(code_keyboard);

    egt::TopWindow win;
    win.add(vbox);
    win.show();

    return app.run();
}
