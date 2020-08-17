/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <gtest/gtest.h>

using ::testing::TestWithParam;
using ::testing::Values;
using ::testing::Range;

class ListBoxWidgetTest : public testing::TestWithParam<int> {};


TEST_P(ListBoxWidgetTest, TestWidget)
{
    egt::Application app;
    egt::TopWindow win;
    std::shared_ptr<egt::ListBox> widget;

    auto type = GetParam();

    egt::ListBox::ItemArray items;
    for (auto x = 0; x <= 6; x++)
        items.push_back(std::make_shared<egt::StringItem>("Testitem " + std::to_string(x)));

    if (type == 0)
    {
        EXPECT_NO_THROW(widget.reset(new egt::ListBox()));
        EXPECT_NO_THROW(win.add(expand(widget)));

        for (auto x = 0; x <= 6; x++)
            widget->add_item(std::make_shared<egt::StringItem>("Testitem " + std::to_string(x)));
    }
    else if (type == 1)
    {
        EXPECT_NO_THROW(widget.reset(new egt::ListBox(egt::Rect(0, 0, 200, 100))));
        EXPECT_NO_THROW(win.add(widget));

        for (auto x = 0; x <= 6; x++)
            widget->add_item(std::make_shared<egt::StringItem>("Testitem " + std::to_string(x)));
    }
    else if (type == 2)
    {
        EXPECT_NO_THROW(widget.reset(new egt::ListBox(items)));
        EXPECT_NO_THROW(win.add(expand(widget)));
    }
    else if (type == 3)
    {
        EXPECT_NO_THROW(widget.reset(new egt::ListBox(items, egt::Rect(0, 0, 400, 200))));
        EXPECT_NO_THROW(win.add(widget));
    }
    else if (type == 4)
    {
        EXPECT_NO_THROW(widget.reset(new egt::ListBox(win, items, egt::Rect(0, 0, 300, 600))));
    }

    EXPECT_EQ(static_cast<int>(widget->item_count()), 7);

    EXPECT_NO_THROW(widget->remove_item(widget->item_at(5).get()));
    EXPECT_EQ(static_cast<int>(widget->item_count()), 6);

    bool state = false;
    widget->on_selected_changed([&state]()
    {
        state = true;
    });
    widget->selected(4);
    EXPECT_EQ(widget->selected(), 4);
    EXPECT_TRUE(state);
}

INSTANTIATE_TEST_SUITE_P(ListBoxWidgetTestGroup, ListBoxWidgetTest, Range(0, 4));
