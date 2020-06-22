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

class SizerTest : public testing::TestWithParam<int> {};

TEST_P(SizerTest, TestWidget)
{
    egt::Application app;
    egt::TopWindow win;
    std::shared_ptr<egt::BoxSizer> widget;

    egt::Orientation orient = static_cast<egt::Orientation>(GetParam());

    widget.reset(new egt::BoxSizer(orient));
    win.add(widget);

    auto text1 = std::make_shared<egt::TextBox>("text 1");
    widget->add((text1));

    auto text2 = std::make_shared<egt::TextBox>("text 2", egt::Rect(0, 0, 200, 100));
    widget->add((text2));

    auto text3 = std::make_shared<egt::Window>();
    widget->add((text3));

    EXPECT_EQ(widget->orient(), orient);

    widget->justify(egt::Justification::start);
    EXPECT_EQ(widget->justify(), egt::Justification::start);

    widget->justify(egt::Justification::ending);
    EXPECT_EQ(widget->justify(), egt::Justification::ending);

    widget->justify(egt::Justification::justify);
    EXPECT_EQ(widget->justify(), egt::Justification::justify);

    widget->justify(egt::Justification::none);
    EXPECT_EQ(widget->justify(), egt::Justification::none);

    widget->justify(egt::Justification::middle);
    EXPECT_EQ(widget->justify(), egt::Justification::middle);
}

INSTANTIATE_TEST_SUITE_P(SizerTestGroup, SizerTest, Values(1, 2, 4, 8));
