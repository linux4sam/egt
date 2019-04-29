/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <string>

using namespace std;
using namespace egt;

int main(int argc, const char** argv)
{
    Application app(argc, argv, "i18n");

    TopWindow window;

    VerticalBoxSizer vsizer;
    expand(vsizer);

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
    window.show();

    return app.run();
}
