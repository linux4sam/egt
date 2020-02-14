/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/detail/meta.h>
#include <egt/ui>
#include <iostream>
#include <libintl.h>
#include <string>
#include <utf8.h>

/**
 * This is a wrapper around gettext().
 */
#define _(String) gettext(String)

int main(int argc, char** argv)
{
    egt::Application app(argc, argv, "i18n");

    egt::Drawer<egt::Label>::draw([](egt::Label & widget, egt::Painter & painter, const egt::Rect & rect)
    {
        egt::detail::ignoreparam(rect);

        widget.draw_box(painter,
                        egt::Palette::ColorId::label_bg,
                        egt::Palette::ColorId::border);

        const auto b = widget.content_area();


        painter.set(widget.font());
        const auto size = painter.text_size(widget.text());
        const auto target = egt::detail::align_algorithm(size,
                            b,
                            widget.text_align());

        // draw a shadow to an offset, but tint the real color and drop the alpha
        auto color = widget.color(egt::Palette::ColorId::label_text).color();
        auto shadow = color.tint(0.5);
        shadow.alphaf(0.3);
        painter.set(shadow);
        painter.draw(target.point() + egt::Point(4, 4));
        painter.draw(widget.text());
        painter.set(color);
        painter.draw(target.point());
        painter.draw(widget.text());
    });

    egt::TopWindow window;

    auto logo = std::make_shared<egt::ImageLabel>(egt::Image("icon:egt_logo_black.png;128"));
    logo->margin(10);
    logo->align(egt::AlignFlag::center | egt::AlignFlag::bottom);
    window.add(logo);

    egt::VerticalBoxSizer vsizer;

    std::vector<std::string> variations =
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
        auto label = std::make_shared<egt::Label>(str);
        std::string face;
        if (index == static_cast<int>(variations.size()) - 1)
            label->font(egt::Font("Noto Color Emoji", 16));
        else
            label->font(egt::Font("Noto Sans CJK SC Regular", 30));

        vsizer.add(label);
        index++;
    }

    window.add(vsizer);

    int minx = 0 - vsizer.width();
    int maxx = window.width();
    int half = (window.width() - vsizer.width()) / 2;

    auto in = std::make_shared<egt::PropertyAnimator>(maxx, half,
              std::chrono::seconds(3),
              egt::easing_exponential_easeout);
    in->on_change([&vsizer](int value)
    {
        vsizer.x(value);
    });

    auto out = std::make_shared<egt::PropertyAnimator>(half + 1, minx,
               std::chrono::seconds(3),
               egt::easing_exponential_easeout);
    out->reverse(true);
    out->on_change([&vsizer](int value)
    {
        vsizer.x(value);
    });

    auto delay = std::make_shared<egt::AnimationDelay>(std::chrono::seconds(1));

    egt::AnimationSequence sequence(true);
    sequence.add(in);
    sequence.add(out);
    sequence.add(delay);
    sequence.start();

    window.show();

    return app.run();
}
