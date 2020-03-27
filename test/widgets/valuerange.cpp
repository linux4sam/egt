/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <gtest/gtest.h>

template <class T>
class ValueRangeWidgets : public testing::Test
{
protected:
    ValueRangeWidgets() {}
    ~ValueRangeWidgets() override {}
    egt::Application app;
};

using ValueRangeWidgetTypes = ::testing::Types<egt::AnalogMeter,
      egt::ProgressBar,
      egt::SpinProgress,
      egt::LevelMeter,
      egt::Slider>;
TYPED_TEST_SUITE(ValueRangeWidgets, ValueRangeWidgetTypes);
TYPED_TEST(ValueRangeWidgets, TestValueRange)
{
    auto widget = std::make_shared<TypeParam>();

    widget->start(1);
    EXPECT_EQ(widget->start(), 1);

    widget->end(100);
    EXPECT_EQ(widget->end(), 100);

    widget->value(10);
    EXPECT_EQ(widget->value(), 10);

    bool value_change = false;
    widget->on_value_changed([&]()
    {
        value_change = true;
    });
    widget->value(20);
    EXPECT_EQ(widget->value(), 20);

    widget->enable();
    widget->value(40);
    EXPECT_TRUE(widget->value() == 40);

    EXPECT_TRUE(value_change);

}

template <class T>
class FloatWidgets : public testing::Test
{
protected:
    FloatWidgets() {}
    ~FloatWidgets() override {}
    egt::Application app;
};

using WidgetFloatTypes = ::testing::Types<egt::AnalogMeterF,
      egt::ProgressBarF,
      egt::SpinProgressF,
      egt::LevelMeterF>;
TYPED_TEST_SUITE(FloatWidgets, WidgetFloatTypes);

TYPED_TEST(FloatWidgets, TestValueRangeF)
{
    auto widget = std::make_shared<TypeParam>();

    widget->start(0.0);
    EXPECT_FLOAT_EQ(widget->start(), 0.0);

    widget->end(1.0);
    EXPECT_FLOAT_EQ(widget->end(), 1.0);

    widget->value(0.4);
    EXPECT_FLOAT_EQ(widget->value(), 0.4);

    bool value_change = false;
    widget->on_value_changed([&]()
    {
        value_change = true;
    });
    widget->value(0.8);
    EXPECT_FLOAT_EQ(widget->value(), 0.8);
    EXPECT_TRUE(value_change);

}
