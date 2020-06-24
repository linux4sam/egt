/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <gtest/gtest.h>

using ::testing::Combine;
using ::testing::Range;
using ::testing::TestWithParam;
using ::testing::Values;

class ScrollwheelTest : public testing::TestWithParam<int> {};

TEST_P(ScrollwheelTest, Test1)
{
    egt::Application app;
    egt::TopWindow win;
    std::shared_ptr<egt::Scrollwheel> widget;

    auto item_count = 10;
    egt::Scrollwheel::ItemArray items;
    for (auto x = 0; x < item_count; x++)
    {
        items.push_back("Testitem " + std::to_string(x));
    }

    auto type = GetParam();
    if (type == 0)
    {
        EXPECT_NO_THROW(widget.reset(new egt::Scrollwheel()));
        EXPECT_NO_THROW(win.add(expand(widget)));
        EXPECT_EQ(static_cast<int>(widget->item_count()), 0);

        for (auto x = 0; x < item_count; x++)
        {
            widget->add_item("Testitem " + std::to_string(x));
        }

        EXPECT_EQ(static_cast<int>(widget->item_count()), item_count);
    }
    else if (type == 1)
    {
        EXPECT_NO_THROW(widget.reset(new egt::Scrollwheel(items)));
        EXPECT_NO_THROW(win.add((widget)));

        EXPECT_EQ(static_cast<int>(widget->item_count()), item_count);
    }
    else if (type == 2)
    {
        EXPECT_NO_THROW(widget.reset(new egt::Scrollwheel(egt::Rect(0, 0, 100, 150), items)));
        EXPECT_NO_THROW(win.add(widget));

        EXPECT_EQ(widget->size(), egt::Size(100, 150));

        EXPECT_EQ(static_cast<int>(widget->item_count()), item_count);
    }
    else if (type == 3)
    {
        EXPECT_NO_THROW(widget.reset(new egt::Scrollwheel(win, items)));

        EXPECT_EQ(static_cast<int>(widget->item_count()), item_count);
    }
    else if (type == 4)
    {
        EXPECT_NO_THROW(widget.reset(new egt::Scrollwheel(win, egt::Rect(0, 0, 100, 150), items)));

        EXPECT_EQ(widget->size(), egt::Size(100, 150));

        EXPECT_EQ(static_cast<int>(widget->item_count()), item_count);
    }
    else if (type == 5)
    {
        EXPECT_NO_THROW(widget.reset(new egt::Scrollwheel(egt::Rect(0, 0, 100, 150), 0, 9, 1)));
        EXPECT_NO_THROW(win.add(expand(widget)));

        EXPECT_EQ(widget->size(), egt::Size(100, 150));

        EXPECT_EQ(static_cast<int>(widget->item_count()), item_count);
    }

    bool value_changed = false;
    widget->on_value_changed([&value_changed]()
    {
        value_changed = true;
    });
    widget->selected(5);
    EXPECT_TRUE(value_changed);
    EXPECT_EQ(static_cast<int>(widget->selected()), 5);

    int i;
    if (type == 5)
    {
        EXPECT_TRUE(widget->value() == "5");

        for (i = 10; i < 20; i++)
            widget->add_item(std::to_string(i));
        EXPECT_EQ(static_cast<int>(widget->item_count()), item_count + 10);

        for (int i = 0; i < 5; i++)
            widget->remove_item(std::to_string(i));
        EXPECT_EQ(static_cast<int>(widget->item_count()), item_count + 5);
    }
    else
    {
        EXPECT_TRUE(widget->value() == "Testitem 5");

        for (i = 10; i < 20; i++)
            widget->add_item("Testitem " + std::to_string(i));
        EXPECT_EQ(static_cast<int>(widget->item_count()), item_count + 10);

        for (int i = 0; i < 5; i++)
            widget->remove_item("Testitem " + std::to_string(i));
        EXPECT_EQ(static_cast<int>(widget->item_count()), item_count + 5);
    }

    widget->clear_items();
    EXPECT_EQ(static_cast<int>(widget->item_count()), 0);

    widget->orient(egt::Orientation::horizontal);
    EXPECT_EQ(widget->orient(), egt::Orientation::horizontal);

    widget->orient(egt::Orientation::vertical);
    EXPECT_EQ(widget->orient(), egt::Orientation::vertical);
}

INSTANTIATE_TEST_SUITE_P(ScrollwheelTestGroup, ScrollwheelTest, Range(0, 6));
