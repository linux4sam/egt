/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <gtest/gtest.h>
#include <random>

using ::testing::Combine;
using ::testing::TestWithParam;
using ::testing::Values;
using ::testing::Range;

static int random_item(int start, int end)
{
    std::random_device r;
    std::default_random_engine e {r()};
    std::uniform_int_distribution<int> dist(start, end);
    return dist(e);
}

class StaticGridTest : public testing::TestWithParam<::testing::tuple<int, int>> {};

TEST_P(StaticGridTest, TestWidget)
{
    egt::Application app;
    egt::TopWindow win;
    std::shared_ptr<egt::StaticGrid> widget;

    auto rows = ::testing::get<0>(GetParam());
    auto columns = ::testing::get<1>(GetParam());

    widget.reset(new egt::StaticGrid(std::make_tuple(columns, rows), 5));
    win.add(egt::expand(widget));

    EXPECT_TRUE(widget->last_add_column() == -1);
    EXPECT_TRUE(widget->last_add_row() == -1);

    egt::PeriodicTimer cputimer(std::chrono::milliseconds(1));
    if ((rows == 0) || (columns == 0))
    {
        ASSERT_EXIT((widget->get(egt::Point(0, 0)), exit(0)), ::testing::KilledBySignal(SIGSEGV), ".*");

        auto text1 = std::make_shared<egt::TextBox>("text 1", egt::Rect(0, 0, 200, 100));
        ASSERT_EXIT((widget->add(egt::expand(text1)), exit(0)), ::testing::KilledBySignal(SIGSEGV), ".*");
    }
    else
    {
        EXPECT_TRUE(widget->get(egt::Point(0, 0)) == nullptr);

        for (int i = 0; i <  rows;  i++)
        {
            for (int j = 0; j < columns; j++)
            {
                auto text1 = std::make_shared<egt::TextBox>("text " + std::to_string(j) + " " + std::to_string(i));
                widget->add(egt::expand(text1));
                EXPECT_EQ(widget->last_add_row(), i);
                EXPECT_EQ(widget->last_add_column(), j);
            }
        }
        EXPECT_TRUE(static_cast<int>(widget->count_children()) == (rows * columns));

        cputimer.on_timeout([widget, &cputimer, &app]()
        {
            int r = widget->last_add_row();
            int c = widget->last_add_column();
            auto rm = widget->get(egt::Point(c, r));
            if (rm != nullptr)
            {
                widget->remove(rm);
            }
            else
            {
                cputimer.stop();
                app.quit();
            }
        });
        cputimer.start();
    }

    win.show();
    app.run();
}

INSTANTIATE_TEST_SUITE_P(StaticGridTestGroup, StaticGridTest, Combine(Range(1, 3), Range(1, 3)));


class SelectableGridTest : public testing::TestWithParam<::testing::tuple<int, int>> {};

TEST_P(SelectableGridTest, TestWidget)
{
    egt::Application app;
    egt::TopWindow win;
    std::shared_ptr<egt::SelectableGrid> widget;

    auto rows = ::testing::get<0>(GetParam());
    auto columns = ::testing::get<1>(GetParam());

    widget.reset(new egt::SelectableGrid(std::make_tuple(columns, rows), 5));
    win.add(egt::expand(widget));

    EXPECT_TRUE(widget->last_add_column() == -1);
    EXPECT_TRUE(widget->last_add_row() == -1);

    egt::PeriodicTimer cputimer(std::chrono::milliseconds(1));
    if ((rows == 0) || (columns == 0))
    {
        ASSERT_EXIT((widget->get(egt::Point(0, 0)), exit(0)), ::testing::KilledBySignal(SIGSEGV), ".*");

        auto text1 = std::make_shared<egt::TextBox>("text 1", egt::Rect(0, 0, 200, 100));
        ASSERT_EXIT((widget->add(egt::expand(text1)), exit(0)), ::testing::KilledBySignal(SIGSEGV), ".*");
    }
    else
    {
        EXPECT_TRUE(widget->get(egt::Point(0, 0)) == nullptr);

        for (int i = 0; i <  rows;  i++)
        {
            for (int j = 0; j < columns; j++)
            {
                auto text1 = std::make_shared<egt::Button>("Button " + std::to_string(j) + " " + std::to_string(i));
                widget->add(egt::expand(text1));
                EXPECT_EQ(widget->last_add_row(), i);
                EXPECT_EQ(widget->last_add_column(), j);
            }
        }
        EXPECT_TRUE(static_cast<int>(widget->count_children()) == (rows * columns));

        cputimer.on_timeout([widget, &cputimer, &app]()
        {
            static int count = 8;
            int r = random_item(0, widget->last_add_row());
            int c = random_item(0, widget->last_add_column());
            widget->selected(c, r);
            if (++count >= 8)
            {
                count = 0;
                cputimer.stop();
                app.quit();
            }
        });
        cputimer.start();
    }

    win.show();
    app.run();
}

INSTANTIATE_TEST_SUITE_P(SelectableGridTestGroup, SelectableGridTest, Combine(Range(1, 5), Range(1, 5)));
