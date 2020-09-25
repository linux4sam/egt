/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <gtest/gtest.h>

using ::testing::AssertionResult;
using ::testing::Range;
using ::testing::TestWithParam;

class CreateWindowTest : public testing::TestWithParam<int> {};

TEST_P(CreateWindowTest, DefaultWindow)
{
    egt::Application app;
    egt::TopWindow win;
    std::shared_ptr<egt::Window> test_window;

    egt::PixelFormat format = static_cast<egt::PixelFormat>(GetParam());

    EXPECT_NO_THROW(test_window.reset(new egt::Window(egt::Size(320, 240), format, egt::WindowHint::automatic)));
    EXPECT_NO_THROW(win.add(test_window));

    if (test_window->plane_window())
    {
        if (format == egt::PixelFormat::yuy2)
            EXPECT_EQ(test_window->format(), egt::PixelFormat::yuyv);
        else
            EXPECT_EQ(test_window->format(), format);
    }
    else
    {
        EXPECT_EQ(test_window->format(), egt::PixelFormat::rgb565);
    }

    test_window->resize(egt::Size(480, 240));
    EXPECT_EQ(test_window->size(), egt::Size(480, 240));

}

INSTANTIATE_TEST_SUITE_P(CreateWindowTestGroup, CreateWindowTest, Range(1, 11));

