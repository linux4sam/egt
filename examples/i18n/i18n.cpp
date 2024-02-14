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

class Language : public egt::StringItem
{
public:
    Language(egt::ListBox& list,
             const std::string& text = {})
        : StringItem(text),
          m_list(list)
    {
        m_list.add_item(*this);
    }

    egt::Signal<> on_checked_changed;

    EGT_NODISCARD egt::Size min_size_hint() const override
    {
        if (!m_min_size.empty())
            return m_min_size;

        const auto carea = m_list.content_area();
        const auto item_count = std::max<egt::DefaultDim>(1, m_list.item_count());
        const auto height = carea.height() / item_count;
        const auto width = carea.width();
        return egt::Size(width, height);
    }

    void resize(const egt::Size& size) override
    {
        if (this->size() != size)
        {
            auto f = font();
            // 45 is the height of each of the 6 Languages in 'm_list' for a
            // 800x480 screen.
            f.size(24 * size.height() / 45);
            font(f);
            StringItem::resize(size);
        }
    }

protected:
    egt::ListBox& m_list;
};

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

    const auto screen_size = app.screen()->size();
    const auto landscape = screen_size.width() >= screen_size.height();

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
    next->align(egt::AlignFlag::right | egt::AlignFlag::bottom);
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

    // If align flags are not empty(), Widget::layout() and egt::detail_algorithm()
    // prevents vsizer.point() from being lower than its parent content_area()),
    // which breaks the animation.
    // Therefore, vsizer is centered manually instead of using something like
    // 'vsizer.align(egt::AlignFlag::center*)'.
    if (landscape)
        vsizer.y((window.height() - vsizer.height()) / 2);
    else
        vsizer.x((window.width() - vsizer.width()) / 2);
    window.add(vsizer);

    int min, max, half;
    if (landscape)
    {
        min = 0 - vsizer.width();
        max = window.width();
        half = (window.width() - vsizer.width()) / 2;
    }
    else
    {
        min = 0 - vsizer.height();
        max = window.height();
        half = (window.height() - vsizer.height()) / 2;
    }

    auto in = std::make_shared<egt::PropertyAnimator>(max, half,
              std::chrono::seconds(3),
              egt::easing_exponential_easeout);
    in->on_change([&vsizer, landscape](int value)
    {
        if (landscape)
            vsizer.x(value);
        else
            vsizer.y(value);
    });

    auto out = std::make_shared<egt::PropertyAnimator>(half + 1, min,
               std::chrono::seconds(3),
               egt::easing_exponential_easeout);
    out->reverse(true);
    out->on_change([&vsizer, landscape](int value)
    {
        if (landscape)
            vsizer.x(value);
        else
            vsizer.y(value);
    });

    auto delay = std::make_shared<egt::AnimationDelay>(std::chrono::seconds(1));

    egt::AnimationSequence sequence(true);
    sequence.add(in);
    sequence.add(out);
    sequence.add(delay);
    sequence.start();

    egt::Frame page2;
    page2.hide();
    window.add(egt::expand(page2));

    auto label = std::make_shared<egt::Label>(_("Hello World"));
    label->font(egt::Font("Free Sans", 40, egt::Font::Weight::bold));
    egt::Rect label_box;
    label_box.width(window.width());
    label_box.height(window.height() * 30 / 100);
    label->box(label_box);
    page2.add(label);

    egt::ListBox languages;
    languages.on_selected([&languages](size_t index)
    {
        auto language = std::static_pointer_cast<Language>(languages.item_at(index));
        language->on_checked_changed.invoke();
    });
    page2.add(languages);

    auto english = std::make_shared<Language>(languages, "English");
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

    auto french = std::make_shared<Language>(languages, "French");
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

    auto german = std::make_shared<Language>(languages, "German");
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

    auto spanish = std::make_shared<Language>(languages, "Spanish");
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

    auto hindi = std::make_shared<Language>(languages, "Hindi");
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

    auto chinese = std::make_shared<Language>(languages, "Chinese");
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

    egt::Rect languages_boundaries;
    languages_boundaries.y(window.height() * 30 / 100);
    languages_boundaries.width(window.width());
    languages_boundaries.height((window.height() * 70 / 100) - logo->height());
    languages.width(languages_boundaries.width() * 60 / 100);
    languages.height(languages_boundaries.height());
    auto languages_box = egt::detail::align_algorithm(languages.box(),
                                                      languages_boundaries,
                                                      {egt::AlignFlag::center});
    languages.box(languages_box);

    static bool flag = true;
    next->on_click([&](egt::Event&)
    {
        if (flag)
        {
            sequence.stop();
            vsizer.hide();
            page2.show();
            next->image(egt::Image(egt::Image("icon:arrow_left.png;64")));
            next->align(egt::AlignFlag::left | egt::AlignFlag::bottom);
            flag = false;
            auto env = std::getenv("LANG");
            std::string lang = env ? env : "";
            if (!lang.empty())
            {
                if (lang.compare(0, 3, "fr_") == 0)
                {
                    languages.selected(1);
                }
                else if (lang.compare(0, 3, "de_") == 0)
                {
                    languages.selected(2);
                }
                else if (lang.compare(0, 3, "es_") == 0)
                {
                    languages.selected(3);
                }
                else if (lang.compare(0, 5, "hi_IN") == 0)
                {
                    languages.selected(4);
                }
                else if (lang.compare(0, 3, "zh_") == 0)
                {
                    languages.selected(5);
                }
                else
                {
                    languages.selected(0);
                }
            }

        }
        else
        {
            page2.hide();
            next->image(egt::Image(egt::Image("icon:arrow_right.png;64")));
            next->align(egt::AlignFlag::right | egt::AlignFlag::bottom);
            flag = true;
            vsizer.show();
            sequence.start();
        }
    });

    next->zorder_top();
    window.show();

    return app.run();
}
