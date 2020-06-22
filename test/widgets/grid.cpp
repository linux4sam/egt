/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <gtest/gtest.h>

using ::testing::Combine;
using ::testing::TestWithParam;
using ::testing::Values;
using ::testing::Range;

class StaticGridTest : public testing::TestWithParam<::testing::tuple<int, int>> {};

TEST_P(StaticGridTest, TestWidget)
{
    egt::Application app;
    egt::TopWindow win;
    std::shared_ptr<egt::StaticGrid> widget;

    auto rows = ::testing::get<0>(GetParam());
    auto columns = ::testing::get<1>(GetParam());

    widget.reset(new egt::StaticGrid(egt::StaticGrid::GridSize(columns, rows), 5));
    win.add(egt::expand(widget));

    EXPECT_TRUE(widget->last_add_column() == -1);
    EXPECT_TRUE(widget->last_add_row() == -1);

    if ((rows == 0) || (columns == 0))
    {
        ASSERT_EXIT((widget->get(egt::StaticGrid::GridPoint(0, 0)), exit(0)), ::testing::KilledBySignal(SIGSEGV), ".*");

        auto text1 = std::make_shared<egt::TextBox>("text 1", egt::Rect(0, 0, 200, 100));
        ASSERT_EXIT((widget->add(egt::expand(text1)), exit(0)), ::testing::KilledBySignal(SIGSEGV), ".*");
    }
    else
    {
        EXPECT_TRUE(widget->get(egt::StaticGrid::GridPoint(0, 0)) == nullptr);

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
        int tcount = static_cast<int>(widget->count_children());
        EXPECT_EQ(tcount, (rows * columns));

        for (int r = widget->last_add_row(); r >= 0; r--)
        {
            for (int c = widget->last_add_column(); c >= 0; c--)
            {
                auto rm = widget->get(egt::StaticGrid::GridPoint(c, r));
                if (rm != nullptr)
                {
                    widget->remove(rm);
                    EXPECT_EQ(static_cast<int>(widget->count_children()), --tcount);
                }
            }
        }
    }
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

    widget.reset(new egt::SelectableGrid(egt::StaticGrid::GridSize(columns, rows), 5));
    win.add(egt::expand(widget));

    EXPECT_TRUE(widget->last_add_column() == -1);
    EXPECT_TRUE(widget->last_add_row() == -1);

    if ((rows == 0) || (columns == 0))
    {
        ASSERT_EXIT((widget->get(egt::StaticGrid::GridPoint(0, 0)), exit(0)), ::testing::KilledBySignal(SIGSEGV), ".*");

        auto text1 = std::make_shared<egt::TextBox>("text 1", egt::Rect(0, 0, 200, 100));
        ASSERT_EXIT((widget->add(egt::expand(text1)), exit(0)), ::testing::KilledBySignal(SIGSEGV), ".*");
    }
    else
    {
        EXPECT_TRUE(widget->get(egt::StaticGrid::GridPoint(0, 0)) == nullptr);
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

        int col = 0;
        int row = 0;
        widget->on_selected_changed([&widget, &row, &col]()
        {
            EXPECT_EQ(widget->selected(), egt::StaticGrid::GridPoint(col, row));
        });

        for (row = 0; row < widget->last_add_column();  row++)
        {
            for (col = 0; col < widget->last_add_row(); col++)
            {
                widget->selected(col, row);
            }
        }
    }
}

INSTANTIATE_TEST_SUITE_P(SelectableGridTestGroup, SelectableGridTest, Combine(Range(1, 5), Range(1, 5)));
