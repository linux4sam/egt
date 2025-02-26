/*
 * Copyright (C) 2025 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/// @[Example]
#include <egt/ui>
#include <egt/widgetflags.h>
#include <list>
#include <memory>
#include <iostream>

int main(int argc, char** argv)
{
    egt::Application app(argc, argv);

    egt::TopWindow window;

    auto hsizer = std::make_shared<egt::HorizontalBoxSizer>();
    hsizer->justify(egt::Justification::middle);
    window.add(egt::expand(hsizer));

    auto list = std::make_shared<egt::ListBoxMulti>();
    list->width(150);
    for (auto x = 0; x < 15; x++)
        list->add_item(std::make_shared<egt::StringItem>("item " + std::to_string(x), egt::Image("icon:file.png")));
    list->add_item_at(std::make_shared<egt::StringItem>("item ?", egt::Image("icon:file.png")), 10);
    list->add_item_at(std::make_shared<egt::StringItem>("item X", egt::Image("icon:file.png")), 10);
    list->remove_item_at(10);
    list->on_selected([](std::list<size_t> indexes)
    {
        std::cout << "on_selected event: \n";
        for (const auto index : indexes)
            std::cout << index << " ";
        std::cout << std::endl;
    });
    list->on_deselected([](std::list<size_t> indexes)
    {
        std::cout << "on_deselected event: " << std::endl;
        for (const auto index : indexes)
            std::cout << index << " ";
        std::cout << std::endl;
    });
    list->on_selected_changed([&list]()
    {
        std::cout << "on_selected_changed event\n";
        const auto selected = list->selected();
        std::cout << "selected: ";
        for (const auto index : selected)
            std::cout << index << " ";
        const auto deselected = list->deselected();
        std::cout << "\ndeselected: ";
        for (const auto index : deselected)
            std::cout << index << " ";
        std::cout << std::endl;
    });
    hsizer->add(egt::expand_vertical(list));

    auto grid = std::make_shared<egt::StaticGrid>(egt::StaticGrid::GridSize{4, 5});
    hsizer->add(egt::expand(grid));

    auto buttonSelectAll = std::make_shared<egt::Button>("Select All");
    buttonSelectAll->on_click([&list](egt::Event&)
    {
        list->select_all();
    });
    grid->add_at(buttonSelectAll, 8);

    auto buttonDeselectAll = std::make_shared<egt::Button>("Deselect All");
    buttonDeselectAll->on_click([&list](egt::Event&)
    {
        list->deselect_all();
    });
    grid->add(buttonDeselectAll, 0, 3);
    grid->remove_at(12);
    grid->add_at(buttonDeselectAll, 12);

    auto buttonToggleAll = std::make_shared<egt::Button>("Toggle All");
    buttonToggleAll->on_click([&list](egt::Event&)
    {
        list->toggle_all();
    });
    grid->add(buttonToggleAll, 0, 4);

    auto buttonSelectedList = std::make_shared<egt::Button>("Selected 1,2,5");
    buttonSelectedList->on_click([&list](egt::Event&)
    {
        list->selected(std::list<size_t> {1, 2, 5});
    });
    grid->add(buttonSelectedList, 1, 0);

    auto buttonDeselectedList = std::make_shared<egt::Button>("Deselected 2,6,9");
    buttonDeselectedList->on_click([&list](egt::Event&)
    {
        list->deselected(std::list<size_t> {2, 6, 9});
    });
    grid->add(buttonDeselectedList, 1, 1);

    auto buttonSelectList = std::make_shared<egt::Button>("Select 1,3,6");
    buttonSelectList->on_click([&list](egt::Event&)
    {
        list->select(std::list<size_t> {1, 3, 6});
    });
    grid->add(buttonSelectList, 1, 2);

    auto buttonDeselectList = std::make_shared<egt::Button>("Deselect 3,4,8");
    buttonDeselectList->on_click([&list](egt::Event&)
    {
        list->deselect(std::list<size_t> {3, 4, 8});
    });
    grid->add(buttonDeselectList, 1, 3);

    auto buttonToggleList = std::make_shared<egt::Button>("Toggle 3,4,7");
    buttonToggleList->on_click([&list](egt::Event&)
    {
        list->toggle(std::list<size_t> {3, 4, 7});
    });
    grid->add(buttonToggleList, 1, 4);

    auto buttonSelectedRange = std::make_shared<egt::Button>("Selected 1-5");
    buttonSelectedRange->on_click([&list](egt::Event&)
    {
        list->selected(1, 6);
    });
    grid->add(buttonSelectedRange, 2, 0);

    auto buttonDeselectedRange = std::make_shared<egt::Button>("Deselected 2-9");
    buttonDeselectedRange->on_click([&list](egt::Event&)
    {
        list->deselected(2, 10);
    });
    grid->add(buttonDeselectedRange, 2, 1);

    auto buttonSelectRange = std::make_shared<egt::Button>("Select 1-6");
    buttonSelectRange->on_click([&list](egt::Event&)
    {
        list->select(1, 7);
    });
    grid->add(buttonSelectRange, 2, 2);

    auto buttonDeselectRange = std::make_shared<egt::Button>("Deselect 3-8");
    buttonDeselectRange->on_click([&list](egt::Event&)
    {
        list->deselect(3, 9);
    });
    grid->add(buttonDeselectRange, 2, 3);

    auto buttonToggleRange = std::make_shared<egt::Button>("Toggle 3-7");
    buttonToggleRange->on_click([&list](egt::Event&)
    {
        list->toggle(3, 8);
    });
    grid->add(buttonToggleRange, 2, 4);

    auto buttonSelected = std::make_shared<egt::Button>("Selected 2");
    buttonSelected->on_click([&list](egt::Event&)
    {
        list->selected(2);
    });
    grid->add(buttonSelected, 3, 0);

    auto buttonDeselected = std::make_shared<egt::Button>("Deselected 3");
    buttonDeselected->on_click([&list](egt::Event&)
    {
        list->deselected(3);
    });
    grid->add(buttonDeselected, 3, 1);

    auto buttonSelect = std::make_shared<egt::Button>("Select 4");
    buttonSelect->on_click([&list](egt::Event&)
    {
        list->select(4);
    });
    grid->add(buttonSelect, 3, 2);

    auto buttonDeselect = std::make_shared<egt::Button>("Deselect 5");
    buttonDeselect->on_click([&list](egt::Event&)
    {
        list->deselect(5);
    });
    grid->add(buttonDeselect, 3, 3);

    auto buttonToggle = std::make_shared<egt::Button>("Toggle 6");
    buttonToggle->on_click([&list](egt::Event&)
    {
        list->toggle(6);
    });
    grid->add(buttonToggle, 3, 4);

    window.show();

    return app.run();
}
/// @[Example
