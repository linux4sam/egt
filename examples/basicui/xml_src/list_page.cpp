/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>

int main(int argc, char** argv)
{
    egt::Application app(argc, argv);
    egt::TopWindow win;

    auto hsizer1 = std::make_shared<egt::BoxSizer>(egt::Orientation::horizontal,
                   egt::Justification::justify);
    hsizer1->padding(20);
    win.add(egt::expand(hsizer1));

    auto list0 = std::make_shared<egt::ListBox>(egt::Rect(0, 0, 200, 0));
    for (auto x = 0; x < 25; x++)
        list0->add_item(std::make_shared<egt::StringItem>("item " + std::to_string(x)));
    list0->align(egt::AlignFlag::expand_vertical);
    hsizer1->add(list0);

    auto list1 = std::make_shared<egt::ListBox>(egt::Rect(0, 0, 200, 300));
    for (auto x = 0; x < 5; x++)
        list1->add_item(std::make_shared<egt::StringItem>("item " + std::to_string(x), egt::Image("icon:ok.png")));
    hsizer1->add(list1);

    auto list2 = std::make_shared<egt::ListBox>(egt::Rect(0, 0, 200, 0));
    list2->add_item(std::make_shared<egt::StringItem>("Help", egt::Image("icon:help.png")));
    list2->add_item(std::make_shared<egt::StringItem>("Info", egt::Image("icon:info.png")));
    list2->add_item(std::make_shared<egt::StringItem>("Warning", egt::Image("icon:warning.png")));
    list2->height((list2->border() * 2) + (40 * list2->item_count()));
    hsizer1->add(list2);

    win.show();

    return app.run();
}
