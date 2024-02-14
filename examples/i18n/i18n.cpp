/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/detail/meta.h>
#include <egt/ui>
#include <iostream>
#include <locale>
#include <libintl.h>
#include <string>
#include <utf8.h>

#define _(String) gettext(String)

static std::string get_translated_string(const std::locale& loc)
{
    std::string ts;
    std::cout.imbue(loc);
    auto& facet = std::use_facet<std::messages<char>>(loc);
    auto cat = facet.open("i18n", loc);
    if (cat < 0)
    {
        std::cout << "Could not open \"i18n\" message catalog\n";
    }
    else
    {
        ts = facet.get(cat, 0, 0, "Hello World");
        std::cout << "\"Hello World\" in "
                  << "\"" << loc.name().c_str() << "\"" << " : "
                  << ts << '\n';
    }
    facet.close(cat);
    return ts;
}

int main(int argc, char** argv)
{
    egt::Application app(argc, argv, "i18n");
#ifdef EXAMPLEDATA
    egt::add_search_path(EXAMPLEDATA);
#endif

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
        auto color = widget.color(egt::Palette::ColorId::label_text).solid();
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
    egt::VerticalBoxSizer vsizer;
    vsizer.margin(10);

    auto logo = std::make_shared<egt::ImageLabel>(egt::Image("icon:egt_logo_black.png;128"));
    logo->margin(10);
    // The size needs to be set again since the margin has been modified.
    const auto m = logo->moat();
    logo->resize(logo->image().size_orig() + egt::Size(2 * m, 2 * m));
    logo->align(egt::AlignFlag::center_horizontal | egt::AlignFlag::bottom);
    window.add(logo);

    auto next = std::make_shared<egt::ImageButton>(egt::Image("icon:arrow_right.png;64"));
    next->fill_flags().clear();
    next->align(egt::AlignFlag::right | egt::AlignFlag::center_vertical);
    window.add(next);

    std::vector<std::string> variations =
    {
        _("EGT supports all languages"),
        "EGT支持所有語言",
        "EGT prend en charge toutes les langues",
        "EGT unterstützt alle Sprachen",
        "ईजीटी सभी भाषाओं का समर्थन करता है",
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
        else if (index == 4)
            label->font(egt::Font("Lohit Devanagari", 30));
        else if (index == 1)
            label->font(egt::Font("Noto Sans CJK SC", 30));
        else
            label->font(egt::Font("Free Sans", 30));

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

    egt::HorizontalBoxSizer hsizer;
    hsizer.hide();
    window.add(egt::expand(hsizer));

    auto label = std::make_shared<egt::Label>(_("Hello World"));
    label->font(egt::Font("Free Sans", 40, egt::Font::Weight::bold));
    hsizer.add(egt::expand(label));

    egt::VerticalBoxSizer ts_sizer;
    hsizer.add(ts_sizer);

    auto english = std::make_shared<egt::RadioBox>(ts_sizer, "English");
    egt::left(english);
    english->on_checked_changed([&]()
    {
        if (english->checked())
        {
            try
            {
                std::locale loc_en("en_US.utf8");
                label->font(egt::Font("Free Sans", 40, egt::Font::Weight::bold));
                label->text(get_translated_string(loc_en));
            }
            catch (std::runtime_error&)
            {
                std::cout << "can't create locale for en_US.utf8" << std::endl;
            }
        }
    });

    auto french = std::make_shared<egt::RadioBox>(ts_sizer, "French");
    egt::left(french);
    french->on_checked_changed([&]()
    {
        if (french->checked())
        {
            try
            {
                std::locale loc_fr("fr_FR.utf8");
                label->font(egt::Font("Free Sans", 40, egt::Font::Weight::bold));
                label->text(get_translated_string(loc_fr));
            }
            catch (std::runtime_error&)
            {
                std::cout << "can't create locale for fr_FR.utf8" << std::endl;
            }
        }
    });

    auto german = std::make_shared<egt::RadioBox>(ts_sizer, "German");
    egt::left(german);
    german->on_checked_changed([&]()
    {
        if (german->checked())
        {
            try
            {
                std::locale loc_de("de_DE.utf8");
                label->font(egt::Font("Free Sans", 40, egt::Font::Weight::bold));
                label->text(get_translated_string(loc_de));
            }
            catch (std::runtime_error&)
            {
                std::cout << "can't create locale for de_DE.utf8" << std::endl;
            }
        }
    });

    auto spanish = std::make_shared<egt::RadioBox>(ts_sizer, "Spanish");
    egt::left(spanish);
    spanish->on_checked_changed([&]()
    {
        if (spanish->checked())
        {
            try
            {
                std::locale loc_es("es_ES.utf8");
                label->font(egt::Font("Free Sans", 40, egt::Font::Weight::bold));
                label->text(get_translated_string(loc_es));
            }
            catch (std::runtime_error&)
            {
                std::cout << "can't create locale for es_ES.utf8" << std::endl;
            }
        }
    });

    auto hindi = std::make_shared<egt::RadioBox>(ts_sizer, "Hindi");
    egt::left(hindi);
    hindi->on_checked_changed([&]()
    {
        if (hindi->checked())
        {
            try
            {
                std::locale loc_hi("hi_IN.utf8");
                label->font(egt::Font("Lohit Devanagari", 40, egt::Font::Weight::bold));
                label->text(get_translated_string(loc_hi));
            }
            catch (std::runtime_error&)
            {
                std::cout << "can't create locale for hi_IN.utf8" << std::endl;
            }
        }
    });

    auto chinese = std::make_shared<egt::RadioBox>(ts_sizer, "Chinese");
    egt::left(chinese);
    chinese->on_checked_changed([&]()
    {
        if (chinese->checked())
        {
            try
            {
                std::locale loc_zh("zh_CN.utf8");
                label->font(egt::Font("Noto Sans CJK SC", 40, egt::Font::Weight::bold));
                label->text(get_translated_string(loc_zh));
            }
            catch (std::runtime_error&)
            {
                std::cout << "can't create locale for zh_CN.utf8" << std::endl;
            }
        }
    });

    auto radiobox_group = std::make_unique<egt::ButtonGroup>(true);
    radiobox_group->add(english);
    radiobox_group->add(french);
    radiobox_group->add(german);
    radiobox_group->add(spanish);
    radiobox_group->add(hindi);
    radiobox_group->add(chinese);

    static bool flag = true;
    next->on_click([&](egt::Event&)
    {
        if (flag)
        {
            sequence.stop();
            vsizer.hide();
            hsizer.show();
            next->image(egt::Image(egt::Image("icon:arrow_left.png;64")));
            next->align(egt::AlignFlag::left | egt::AlignFlag::center_vertical);
            flag = false;
            auto env = std::getenv("LANG");
            std::string lang = env ? env : "";
            if (!lang.empty())
            {
                if (lang.compare(0, 3, "fr_") == 0)
                {
                    french->checked(true);
                }
                else if (lang.compare(0, 3, "de_") == 0)
                {
                    german->checked(true);
                }
                else if (lang.compare(0, 3, "es_") == 0)
                {
                    spanish->checked(true);
                }
                else if (lang.compare(0, 5, "hi_IN") == 0)
                {
                    hindi->checked(true);
                }
                else if (lang.compare(0, 3, "zh_") == 0)
                {
                    chinese->checked(true);
                }
                else
                {
                    english->checked(true);
                }
            }

        }
        else
        {
            hsizer.hide();
            next->image(egt::Image(egt::Image("icon:arrow_right.png;64")));
            next->align(egt::AlignFlag::right | egt::AlignFlag::center_vertical);
            flag = true;
            vsizer.show();
            sequence.start();
        }
        next->zorder_top();
    });

    window.show();

    return app.run();
}
