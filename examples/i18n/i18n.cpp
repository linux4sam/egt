/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <iostream>
#include <string>
#include <utf8.h>

using namespace std;
using namespace egt;

int main(int argc, const char** argv)
{
    Application app(argc, argv, "i18n");

    TopWindow window;

    auto logo = make_shared<ImageLabel>(Image("@128px/egt_logo_black.png"));
    logo->set_margin(10);
    logo->set_align(alignmask::center | alignmask::bottom);
    window.add(logo);

    VerticalBoxSizer vsizer;

    vector<string> variations =
    {
        _("EGT supports all languages"),
        "EGT支持所有語言",
        "EGT prend en charge toutes les langues",
        "EGT unterstützt alle Sprachen",
        "EGT soporta todos los idiomas",
        ""
    };

    utf8::append(0x0001f602, std::back_inserter(variations.back()));
    utf8::append(0x0001f412, std::back_inserter(variations.back()));
    utf8::append(0x0001f3d7, std::back_inserter(variations.back()));
    utf8::append(0x0001f6b2, std::back_inserter(variations.back()));
    utf8::append(0x0001f3a3, std::back_inserter(variations.back()));

    int index = 0;
    for (auto& str : variations)
    {
        auto label = make_shared<Label>(str);
        string face;
        if (index == static_cast<int>(variations.size()) - 1)
            label->set_font(egt::Font("Noto Color Emoji", 16));
        else
            label->set_font(egt::Font("Noto Sans CJK SC Regular", 30));

        vsizer.add(label);
        index++;
    }

    window.add(vsizer);

    int minx = 0 - vsizer.width();
    int maxx = window.width();
    int half = (window.width() - vsizer.width()) / 2;

    auto in = new PropertyAnimator(maxx, half, std::chrono::seconds(3), easing_exponential_easeout);
    in->on_change(std::bind(&Label::set_x, std::ref(vsizer), std::placeholders::_1));

    auto out = new PropertyAnimator(half + 1, minx, std::chrono::seconds(3), easing_exponential_easeout);
    out->set_reverse(true);
    out->on_change(std::bind(&Label::set_x, std::ref(vsizer), std::placeholders::_1));

    auto delay = new AnimationDelay(std::chrono::seconds(1));

    auto sequence = new AnimationSequence(true);
    sequence->add(*in);
    sequence->add(*out);
    sequence->add(*delay);
    sequence->start();

    window.show();

    return app.run();
}
