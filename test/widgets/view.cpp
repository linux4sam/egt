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

class ViewTest : public testing::TestWithParam<::testing::tuple<int, int>> {};

TEST_P(ViewTest, TestWidget)
{
    egt::Application app;
    egt::TopWindow win;
    std::shared_ptr<egt::ScrolledView> widget;

    egt::ScrolledView::Policy hp = static_cast<egt::ScrolledView::Policy>(::testing::get<0>(GetParam()));
    egt::ScrolledView::Policy vp = static_cast<egt::ScrolledView::Policy>(::testing::get<1>(GetParam()));

    egt::Rect rect = egt::Rect(130, 80, 640, 320);
    EXPECT_NO_THROW(widget.reset(new egt::ScrolledView(rect, hp, vp)));
    EXPECT_NO_THROW(win.add((widget)));

    for (int j = 0; j < 100; j++)
    {
        //place button widget in 1600x800 view
        auto x = 16 * j;
        auto y = 8 * j;
        auto text1 = std::make_shared<egt::Button>("Button " + std::to_string(j), egt::Rect(x, y, 200, 80));
        EXPECT_NO_THROW(widget->add(text1));
    }

    EXPECT_EQ(widget->offset(), egt::Point(0, 0));

    int px;
    int py;
    for (int count = 0; count <= 10; count++)
    {
        px = 30 * count;
        py = 24 * count;
        if ((hp == egt::ScrolledView::Policy::always) &&
            (vp == egt::ScrolledView::Policy::always))
        {
            widget->offset(egt::Point(-px, -py));
            EXPECT_EQ(widget->offset(), egt::Point(-px, -py));
        }
        else if ((hp == egt::ScrolledView::Policy::always) ||
                 (hp == egt::ScrolledView::Policy::as_needed))
        {
            widget->hoffset(-px);
            EXPECT_EQ(widget->offset(), egt::Point(-px, 0));
        }
        else if ((vp == egt::ScrolledView::Policy::always) ||
                 (vp == egt::ScrolledView::Policy::as_needed))
        {
            widget->voffset(-py);
            EXPECT_EQ(widget->offset(), egt::Point(0, -py));
        }

        if (count % 2)
        {
            auto s = egt::Size(320, 160);
            widget->resize(s);
            EXPECT_EQ(widget->size(), s);
        }
        else
        {
            widget->resize(rect.size());
            EXPECT_EQ(widget->size(), rect.size());
        }
    }
}
INSTANTIATE_TEST_SUITE_P(ViewTestGroup, ViewTest, Combine(Range(0, 3), Range(0, 3)));
