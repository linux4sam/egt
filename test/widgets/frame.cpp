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

class FrameTest : public testing::TestWithParam<int> {};

TEST_P(FrameTest, TestWidget)
{
    egt::Application app;
    egt::TopWindow win;
    std::shared_ptr<egt::Frame> widget;

    egt::Widget::Flags flag = static_cast<egt::Widget::Flag>(GetParam());

    EXPECT_NO_THROW(widget.reset(new egt::Frame(egt::Rect(0, 0, 800, 480), flag)));
    EXPECT_NO_THROW(win.add(widget));
    int wcount = 0;
    for (int i = 0; i < 10; i++)
    {
        auto w = 800 - (i * 10);
        auto h = 200 - (i * 10);
        auto text1 = std::make_shared<egt::TextBox>("textBox " + std::to_string(i), egt::Rect(0, 0, w, h), egt::AlignFlag::center);
        EXPECT_NO_THROW(widget->add(text1));
        wcount++;
    }
    auto c = widget->count_children();
    EXPECT_EQ(static_cast<int>(c), wcount);

    while (c)
    {
        auto w = widget->child_at(--c);
        EXPECT_TRUE(w != nullptr);
        if (w != nullptr)
        {
            EXPECT_NO_THROW(widget->remove(w.get()));
        }
    }

    EXPECT_EQ(widget->count_children(), c);

    if (c == 0)
    {
        size_t j = 0;
        for (j = 0; j < 10; j++)
        {
            auto r = egt::Rect((720 - (j * 30)), (380 - (j * 17)), 80, 100);
            auto text2 = std::make_shared<egt::TextBox>("textBox " + std::to_string(j), r);
            EXPECT_NO_THROW(widget->add(text2));
        }
        EXPECT_EQ(widget->count_children(), j);

        widget->remove_all();
        EXPECT_EQ(static_cast<int>(widget->count_children()), 0);
    }
}

INSTANTIATE_TEST_SUITE_P(FrameTestGroup, FrameTest, testing::Values(1, 2, 4));
