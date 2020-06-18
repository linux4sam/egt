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

class FormTest : public testing::TestWithParam<int> {};

TEST_P(FormTest, TestWidget)
{
    egt::Application app;
    egt::TopWindow win;
    egt::Frame frame;
    std::shared_ptr<egt::experimental::Form> widget;

    int type = GetParam();

    if (type == 0)
    {
        EXPECT_NO_THROW(widget.reset(new egt::experimental::Form("Form Title ")));
        EXPECT_NO_THROW(win.add(expand(widget)));
    }
    else if (type == 1)
    {
        EXPECT_NO_THROW(widget.reset(new egt::experimental::Form(frame, "Form Title ")));
    }

    widget->add_group("Group Tiltle");

    for (int i = 0; i < 5; i++)
    {
        auto text1 = std::make_shared<egt::TextBox>("textBox " + std::to_string(i));
        widget->add_option(text1);
        widget->group_align(egt::AlignFlag::center);
        widget->name_align(egt::AlignFlag::right);
        widget->remove_options();
    }
}

INSTANTIATE_TEST_SUITE_P(FormTestGroup, FormTest, Values(0, 1));
