/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <gtest/gtest.h>

using ::testing::TestWithParam;
using ::testing::Values;

class SliderTest : public testing::TestWithParam<int>
{};

TEST_P(SliderTest, CreateSlider)
{
    egt::Application app;

    std::shared_ptr<egt::Slider> Slider;

    egt::Slider::SliderFlag sflags = static_cast<egt::Slider::SliderFlag>(GetParam());

    Slider.reset(new egt::Slider());

    Slider->starting(25);
    EXPECT_EQ(Slider->starting(), 25);

    Slider->ending(1000);
    EXPECT_EQ(Slider->ending(), 1000);

    Slider->slider_flags().set({sflags});
    EXPECT_TRUE(Slider->slider_flags().is_set(sflags));

    Slider->value(200);
    EXPECT_EQ(Slider->value(), 200);

    Slider->orient(egt::Orientation::horizontal);
    EXPECT_EQ(Slider->orient(), egt::Orientation::horizontal);

    Slider->orient(egt::Orientation::vertical);
    EXPECT_EQ(Slider->orient(), egt::Orientation::vertical);

    bool value_change = false;
    Slider->on_value_changed([&]()
    {
        value_change = true;
    });
    Slider->value(150);
    EXPECT_EQ(Slider->value(), 150);
    EXPECT_TRUE(value_change);

}

INSTANTIATE_TEST_SUITE_P(SliderTestGroup, SliderTest, Values(1, 2, 4, 8, 16, 32, 64));
