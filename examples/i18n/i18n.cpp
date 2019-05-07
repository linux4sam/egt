/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <iostream>
#include <string>

using namespace std;
using namespace egt;

int main(int argc, const char** argv)
{
    Application app(argc, argv, "i18n");

    TopWindow window;

    auto logo = make_shared<ImageLabel>(Image("@egt_logo_black.png"));
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
        _("Hello World")
    };

    for (auto& str : variations)
    {
        static const auto face = "Noto Sans CJK SC Regular";
        auto label = make_shared<Label>(str, egt::Label::default_align, egt::Font(face, 36));
        vsizer.add(label);

    }

    window.add(vsizer);

    int minx = 0 - vsizer.w();
    int maxx = window.w();
    int half = (window.w() - vsizer.w()) / 2;

    auto in = new PropertyAnimator(maxx, half, std::chrono::seconds(3), easing_exponential_easeout);
    in->on_change(std::bind(&Label::set_x, std::ref(vsizer), std::placeholders::_1));

    auto out = new PropertyAnimator(half + 1, minx, std::chrono::seconds(3), easing_exponential_easeout);
    out->reverse(true);
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
