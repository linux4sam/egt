/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/// @[Example]
#include <codecvt>
#include <egt/ui>
#include <locale>
#include <sstream>
#include <string>

int main(int argc, char** argv)
{
    egt::Application app(argc, argv);

    egt::TopWindow window;

    egt::VerticalBoxSizer vsizer(window);
    expand(vsizer);

    egt::ImageLabel logo(egt::Image("icon:egt_logo_black.png;128"));
    vsizer.add(logo);

    egt::Label title("Keyboard Events");
    vsizer.add(title);

    egt::HorizontalBoxSizer hsizer(vsizer);
    expand(hsizer);

    egt::ListBox list(hsizer);
    expand(list);

    list.on_event([&list](egt::Event & event)
    {
        std::ostringstream ss;
        ss << event.key().keycode;

        if (event.id() == egt::EventId::keyboard_down)
            ss << " down";
        else if (event.id() == egt::EventId::keyboard_up)
            ss << " up";
        else if (event.id() == egt::EventId::keyboard_repeat)
            ss << " repeat";

        if (event.key().unicode)
        {
            // deprecated in C++17
            std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
            std::string u8str = converter.to_bytes(event.key().unicode);
            ss << " (" << u8str << ")";
        }

        auto item = std::make_shared<egt::StringItem>(ss.str());
        list.add_item(item);
        item->zorder_bottom();
        list.selected(0);
        list.scroll_top();

        while (list.item_count() > 100)
            list.remove_item(list.item_at(100).get());

    }, {egt::EventId::keyboard_down,
        egt::EventId::keyboard_up,
        egt::EventId::keyboard_repeat
       });

    window.show();

    egt::Pattern pattern(egt::Pattern::Type::linear,
    {
        {0, egt::Palette::red},
        {1.0, egt::Palette::green},
    });
    pattern.linear(egt::Point(title.x(), title.y() + title.height() / 2),
                   egt::Point(title.x() + title.width(), title.y() + title.height() / 2));

    title.color(egt::Palette::ColorId::label_text, pattern);

    return app.run();
}
/// @[Example]
