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

class ComboBoxWidgetTest : public testing::TestWithParam<int> {};

TEST_P(ComboBoxWidgetTest, TestWidget)
{
    egt::Application app;
    egt::TopWindow win;
    std::shared_ptr<egt::ComboBox> widget;

    auto item_count = 200;
    egt::ComboBox::ItemArray items;
    for (auto x = 0; x < item_count; x++)
        items.push_back("Testitem " + std::to_string(x));

    auto type = GetParam();
    if (type == 0)
    {
        EXPECT_NO_THROW(widget.reset(new egt::ComboBox()));
        EXPECT_NO_THROW(win.add((widget)));
        for (auto x = 0; x < item_count; x++)
            widget->add_item("Testitem " + std::to_string(x));
    }
    else if (type == 1)
    {
        EXPECT_NO_THROW(widget.reset(new egt::ComboBox(egt::Rect(0, 0, 200, 100))));
        EXPECT_NO_THROW(win.add(widget));
        for (auto x = 0; x < item_count; x++)
            widget->add_item("Testitem " + std::to_string(x));
    }
    else if (type == 2)
    {
        EXPECT_NO_THROW(widget.reset(new egt::ComboBox(items)));
        EXPECT_NO_THROW(win.add(widget));
    }
    else if (type == 3)
    {
        EXPECT_NO_THROW(widget.reset(new egt::ComboBox(items, egt::Rect(0, 0, 200, 100))));
        EXPECT_NO_THROW(win.add(widget));
    }
    else if (type == 4)
    {
        EXPECT_NO_THROW(widget.reset(new egt::ComboBox(win, items, egt::Rect(0, 0, 200, 100))));
    }

    EXPECT_EQ(static_cast<int>(widget->item_count()), item_count);

    EXPECT_TRUE(widget->item_at(5) == "Testitem 5");

    EXPECT_TRUE(widget->remove("Testitem 5"));
    item_count -= 1;
    EXPECT_EQ(static_cast<int>(widget->item_count()), item_count);

    bool state = false;
    widget->on_selected_changed([&state]()
    {
        state = true;
    });
    widget->selected(4);
    EXPECT_EQ(widget->selected(), 4);
    EXPECT_TRUE(state);

    egt::PeriodicTimer cputimer(std::chrono::milliseconds(1));
    cputimer.on_timeout([widget, &item_count, &cputimer, &app]()
    {
        static int flag = 0;
        if (flag % 2)
        {
            for (auto x = 0; x < 5; x++)
                widget->add_item("Testitem " + std::to_string(x));
            item_count += 5;
            EXPECT_EQ(static_cast<int>(widget->item_count()), item_count);
        }
        else
        {
            for (auto x = 0; x < 3; x++)
                widget->remove("Testitem " + std::to_string(x));
            item_count -= 3;
            EXPECT_EQ(static_cast<int>(widget->item_count()), item_count);
        }
        flag++;

        if (flag >= 5)
        {
            flag = 0;
            cputimer.stop();
            app.quit();
        }

    });
    cputimer.start();

    //win.show();
    app.run();

}
INSTANTIATE_TEST_SUITE_P(ComboBoxWidgetTestGroup, ComboBoxWidgetTest, Range(0, 4));
