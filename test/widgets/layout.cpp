/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <gtest/gtest.h>

class Layout : public ::testing::Test
{
protected:

    void SetUp() override
    {
        top.show();
        window.show();
    }

    egt::Application app;
    egt::TopWindow top{};
    egt::Window window{top, egt::Size(400, 400)};
};

TEST_F(Layout, VerticalSizerDefaultPosition)
{
    egt::BoxSizer vsizer(window, egt::Orientation::vertical);

    egt::Button b1(vsizer, "b1", egt::Size(100, 100));

    EXPECT_EQ(b1.display_origin(), egt::DisplayPoint(0, 0));
    EXPECT_EQ(b1.box().size(), egt::Size(100, 100));

    EXPECT_EQ(vsizer.display_origin(), egt::DisplayPoint(0, 0));
    EXPECT_EQ(vsizer.box().size(), egt::Size(100, 100));
}

TEST_F(Layout, ExpandedVerticalSizerDefaultPosition)
{
    egt::BoxSizer vsizer(window, egt::Orientation::vertical);
    egt::expand(vsizer);

    egt::Button b1(vsizer, "b1", egt::Size(100, 100));

    EXPECT_EQ(b1.display_origin(), egt::DisplayPoint(150, 150));
    EXPECT_EQ(b1.box().size(), egt::Size(100, 100));

    EXPECT_EQ(vsizer.display_origin(), egt::DisplayPoint(0, 0));
    EXPECT_EQ(vsizer.box().size(), egt::Size(400, 400));
}

TEST_F(Layout, VerticalSizerAutoGrowth)
{
    /// The goal of this test is to check if the sizer will grow automatically
    /// when adding widgets. The width should be the highest width among the
    /// children and the height the sum of the children's height.
    egt::BoxSizer vsizer(window, egt::Orientation::vertical);

    egt::Button b1(vsizer, "b1", egt::Rect(egt::Point(0, 0), egt::Size(200, 200)));
    egt::Button b2(vsizer, "b2", egt::Rect(egt::Point(0, 0), egt::Size(400, 200)));
    egt::Button b3(vsizer, "b3", egt::Rect(egt::Point(0, 0), egt::Size(600, 200)));

    EXPECT_EQ(b1.display_origin(), egt::DisplayPoint(200, 0));
    EXPECT_EQ(b1.box().size(), egt::Size(200, 200));

    EXPECT_EQ(b2.display_origin(), egt::DisplayPoint(100, 200));
    EXPECT_EQ(b2.box().size(), egt::Size(400, 200));

    EXPECT_EQ(b3.display_origin(), egt::DisplayPoint(0, 400));
    EXPECT_EQ(b3.box().size(), egt::Size(600, 200));

    EXPECT_EQ(vsizer.display_origin(), egt::DisplayPoint(0, 0));
    EXPECT_EQ(vsizer.box().size(), egt::Size(600, 600));
}

TEST_F(Layout, VerticalSizerIgnoreUserPosition)
{
    /// The goal of this test is to check that the position given by the user is
    /// not taken into account and that it doesn't corrupt the layout.
    egt::BoxSizer vsizer(window, egt::Orientation::vertical);

    egt::Button b1(vsizer, "b1", egt::Rect(egt::Point(400, 0), egt::Size(100, 100)));
    egt::Button b2(vsizer, "b2", egt::Rect(egt::Point(300, 300), egt::Size(100, 100)));
    egt::Button b3(vsizer, "b3", egt::Rect(egt::Point(100, 100), egt::Size(100, 100)));

    EXPECT_EQ(b1.display_origin(), egt::DisplayPoint(0, 0));
    EXPECT_EQ(b1.box().size(), egt::Size(100, 100));

    EXPECT_EQ(b2.display_origin(), egt::DisplayPoint(0, 100));
    EXPECT_EQ(b2.box().size(), egt::Size(100, 100));

    EXPECT_EQ(b3.display_origin(), egt::DisplayPoint(0, 200));
    EXPECT_EQ(b3.box().size(), egt::Size(100, 100));

    EXPECT_EQ(vsizer.display_origin(), egt::DisplayPoint(0, 0));
    EXPECT_EQ(vsizer.box().size(), egt::Size(100, 300));

}

TEST_F(Layout, VerticalSizerHorizontalAlignment)
{
    /// The goal of this test is to check if the horizontal alignment flags of
    /// the children are respected.
    egt::BoxSizer vsizer(window, egt::Orientation::vertical);

    egt::Button b1(vsizer, "b1", egt::Rect(egt::Point(0, 0), egt::Size(100, 100)));
    b1.align(egt::AlignFlag::left);

    egt::Button b2(vsizer, "b2", egt::Rect(egt::Point(0, 0), egt::Size(200, 100)));
    b2.align(egt::AlignFlag::right);

    egt::Button b3(vsizer, "b3", egt::Rect(egt::Point(0, 0), egt::Size(300, 100)));

    EXPECT_EQ(b1.display_origin(), egt::DisplayPoint(0, 0));
    EXPECT_EQ(b1.box().size(), egt::Size(100, 100));

    EXPECT_EQ(b2.display_origin(), egt::DisplayPoint(100, 100));
    EXPECT_EQ(b2.box().size(), egt::Size(200, 100));

    EXPECT_EQ(b3.display_origin(), egt::DisplayPoint(0, 200));
    EXPECT_EQ(b3.box().size(), egt::Size(300, 100));

    EXPECT_EQ(vsizer.display_origin(), egt::DisplayPoint(0, 0));
    EXPECT_EQ(vsizer.box().size(), egt::Size(300, 300));
}

TEST_F(Layout, VerticalSizerVerticalAlignment)
{
    /// The goal of this test is to check that vertical alignment flags have no
    /// consequence on the layout.
    egt::BoxSizer vsizer(window, egt::Orientation::vertical);

    egt::Button b1(vsizer, "b1", egt::Rect(egt::Point(0, 0), egt::Size(100, 100)));
    b1.align(egt::AlignFlag::bottom);

    egt::Button b2(vsizer, "b2", egt::Rect(egt::Point(0, 0), egt::Size(100, 100)));
    b2.align(egt::AlignFlag::top);

    egt::Button b3(vsizer, "b3", egt::Rect(egt::Point(0, 0), egt::Size(100, 100)));

    EXPECT_EQ(b1.display_origin(), egt::DisplayPoint(0, 0));
    EXPECT_EQ(b1.box().size(), egt::Size(100, 100));

    EXPECT_EQ(b2.display_origin(), egt::DisplayPoint(0, 100));
    EXPECT_EQ(b2.box().size(), egt::Size(100, 100));

    EXPECT_EQ(b3.display_origin(), egt::DisplayPoint(0, 200));
    EXPECT_EQ(b3.box().size(), egt::Size(100, 100));

    EXPECT_EQ(vsizer.display_origin(), egt::DisplayPoint(0, 0));
    EXPECT_EQ(vsizer.box().size(), egt::Size(100, 300));
}

TEST_F(Layout, VerticalSizerExpandedChildren)
{
    /// The goal of this test is to check that expanded children in a non
    /// expanded sizer makes no sense. The sizer can't grow from an unknown
    /// size.
    egt::BoxSizer vsizer(window, egt::Orientation::vertical);

    egt::Button b1(vsizer, "b1", egt::Rect(egt::Point(0, 0), egt::Size(100, 100)));
    egt::expand(b1);

    egt::Button b2(vsizer, "b2", egt::Rect(egt::Point(0, 0), egt::Size(100, 100)));

    egt::Button b3(vsizer, "b3", egt::Rect(egt::Point(0, 0), egt::Size(100, 100)));
    egt::expand(b3);

    EXPECT_EQ(b1.display_origin(), egt::DisplayPoint(0, 0));
    EXPECT_EQ(b1.box().size(), egt::Size(100, 0));

    EXPECT_EQ(b2.display_origin(), egt::DisplayPoint(0, 0));
    EXPECT_EQ(b2.box().size(), egt::Size(100, 100));

    EXPECT_EQ(b3.display_origin(), egt::DisplayPoint(0, 100));
    EXPECT_EQ(b3.box().size(), egt::Size(100, 0));

    EXPECT_EQ(vsizer.display_origin(), egt::DisplayPoint(0, 0));
    EXPECT_EQ(vsizer.box().size(), egt::Size(100, 100));
}

TEST_F(Layout, ExpandedVerticalSizerExpandedChildren)
{
    /// The goal of this test is to check the behavior of expanded children in
    /// an expanded sizer. The size of the non expanded child must be the one
    /// given by the user. Others children must share the remaining space.
    egt::BoxSizer vsizer(window, egt::Orientation::vertical);
    egt::expand(vsizer);

    egt::Button b1(vsizer, "b1", egt::Rect(egt::Point(0, 0), egt::Size(100, 100)));
    egt::expand(b1);

    egt::Button b2(vsizer, "b2", egt::Rect(egt::Point(0, 0), egt::Size(100, 100)));

    egt::Button b3(vsizer, "b3", egt::Rect(egt::Point(0, 0), egt::Size(100, 100)));
    egt::expand(b3);

    EXPECT_EQ(b1.display_origin(), egt::DisplayPoint(0, 0));
    EXPECT_EQ(b1.box().size(), egt::Size(400, 150));

    EXPECT_EQ(b2.display_origin(), egt::DisplayPoint(150, 150));
    EXPECT_EQ(b2.box().size(), egt::Size(100, 100));

    EXPECT_EQ(b3.display_origin(), egt::DisplayPoint(0, 250));
    EXPECT_EQ(b3.box().size(), egt::Size(400, 150));

    EXPECT_EQ(vsizer.display_origin(), egt::DisplayPoint(0, 0));
    EXPECT_EQ(vsizer.box().size(), egt::Size(400, 400));
}

TEST_F(Layout, FlexSizerStepByStep)
{
    /// The goal ot this test is to check we have the expected behavior when we
    /// set the properties of the widget one by one instead of doing it when
    /// it is constructed.
    egt::BoxSizer fsizer(window, egt::Orientation::flex);
    expand(fsizer);

    egt::Button b1(fsizer, "b1");
    b1.x(150);
    b1.y(0);
    b1.width(100);
    b1.height(80);

    egt::Button b2(fsizer, "b2");
    b2.x(350);
    b2.y(0);
    b2.width(200);
    b2.height(80);

    egt::Button b3(fsizer, "b3");
    b3.x(200);
    b3.y(80);
    b3.width(300);
    b3.height(80);

    EXPECT_EQ(b1.display_origin(), egt::DisplayPoint(50, 0));
    EXPECT_EQ(b1.box().size(), egt::Size(100, 80));

    EXPECT_EQ(b2.display_origin(), egt::DisplayPoint(150, 0));
    EXPECT_EQ(b2.box().size(), egt::Size(200, 80));

    EXPECT_EQ(b3.display_origin(), egt::DisplayPoint(50, 80));
    EXPECT_EQ(b3.box().size(), egt::Size(300, 80));
}

TEST_F(Layout, FlexSizerAddOrder)
{
    /// The goal of this test is to check that adding a widget before or after
    /// setting its properties doesn't impact the layout.
    egt::BoxSizer fsizer(window, egt::Orientation::flex);
    expand(fsizer);

    egt::Button b1(fsizer, "b1");
    b1.x(150);
    b1.y(0);
    b1.width(100);
    b1.height(80);

    egt::Button b2("b2");
    fsizer.add(b2);
    b2.x(350);
    b2.y(0);
    b2.width(200);
    b2.height(80);

    egt::Button b3("b3");
    b3.x(200);
    b3.y(80);
    b3.width(300);
    b3.height(80);
    fsizer.add(b3);

    EXPECT_EQ(b1.display_origin(), egt::DisplayPoint(50, 0));
    EXPECT_EQ(b1.box().size(), egt::Size(100, 80));

    EXPECT_EQ(b2.display_origin(), egt::DisplayPoint(150, 0));
    EXPECT_EQ(b2.box().size(), egt::Size(200, 80));

    EXPECT_EQ(b3.display_origin(), egt::DisplayPoint(50, 80));
    EXPECT_EQ(b3.box().size(), egt::Size(300, 80));
}

TEST_F(Layout, FlexSizerConstructor)
{
    egt::BoxSizer fsizer(window, egt::Orientation::flex);
    expand(fsizer);

    egt::Button b1(fsizer, "b1", egt::Rect(150, 0, 100, 80));
    egt::Button b2(fsizer, "b2", egt::Rect(350, 0, 200, 80));
    egt::Button b3(fsizer, "b3", egt::Rect(200, 80, 300, 80));

    EXPECT_EQ(b1.display_origin(), egt::DisplayPoint(50, 0));
    EXPECT_EQ(b1.box().size(), egt::Size(100, 80));

    EXPECT_EQ(b2.display_origin(), egt::DisplayPoint(150, 0));
    EXPECT_EQ(b2.box().size(), egt::Size(200, 80));

    EXPECT_EQ(b3.display_origin(), egt::DisplayPoint(50, 80));
    EXPECT_EQ(b3.box().size(), egt::Size(300, 80));
}

TEST_F(Layout, FlexSizerDefaultPosition)
{
    egt::BoxSizer fsizer(window, egt::Orientation::flex);
    expand(fsizer);

    egt::Button b1(fsizer, "b1", egt::Rect(0, 0, 100, 80));
    egt::Button b2(fsizer, "b2", egt::Rect(0, 0, 200, 80));
    egt::Button b3(fsizer, "b3", egt::Rect(0, 0, 300, 80));

    EXPECT_EQ(b1.display_origin(), egt::DisplayPoint(50, 0));
    EXPECT_EQ(b1.box().size(), egt::Size(100, 80));

    EXPECT_EQ(b2.display_origin(), egt::DisplayPoint(150, 0));
    EXPECT_EQ(b2.box().size(), egt::Size(200, 80));

    EXPECT_EQ(b3.display_origin(), egt::DisplayPoint(50, 80));
    EXPECT_EQ(b3.box().size(), egt::Size(300, 80));
}

TEST_F(Layout, FlexSizerEmptyBox)
{
    /// The goal of this test is to check that an empty box for a flex sizer
    /// leads to a width resize to the maximum witdh of its children.
    egt::BoxSizer fsizer(window, egt::Orientation::flex);

    egt::Button b1(fsizer, "b1", egt::Rect(0, 0, 100, 80));
    egt::Button b2(fsizer, "b2", egt::Rect(0, 0, 200, 80));
    egt::Button b3(fsizer, "b3", egt::Rect(0, 0, 300, 80));
    egt::Button b4(fsizer, "b4", egt::Rect(0, 0, 100, 80));
    egt::Button b5(fsizer, "b5", egt::Rect(0, 0, 100, 300));

    EXPECT_EQ(b1.display_origin(), egt::DisplayPoint(0, 0));
    EXPECT_EQ(b1.box().size(), egt::Size(100, 80));

    EXPECT_EQ(b2.display_origin(), egt::DisplayPoint(100, 0));
    EXPECT_EQ(b2.box().size(), egt::Size(200, 80));

    EXPECT_EQ(b3.display_origin(), egt::DisplayPoint(0, 80));
    EXPECT_EQ(b3.box().size(), egt::Size(300, 80));

    EXPECT_EQ(b4.display_origin(), egt::DisplayPoint(50, 270));
    EXPECT_EQ(b4.box().size(), egt::Size(100, 80));

    EXPECT_EQ(b5.display_origin(), egt::DisplayPoint(150, 160));
    EXPECT_EQ(b5.box().size(), egt::Size(100, 300));

    EXPECT_EQ(fsizer.display_origin(), egt::DisplayPoint(0, 0));
    EXPECT_EQ(fsizer.box().size(), egt::Size(300, 460));
}

TEST_F(Layout, FlexSizerExpanded)
{
    /// The goal of this test is to check that an expanded box for the flex sizer
    /// leads to a different layout than the previous case as the width is
    /// higher.
    egt::BoxSizer fsizer(window, egt::Orientation::flex);
    egt::expand(fsizer);

    egt::Button b1(fsizer, "b1", egt::Rect(0, 0, 100, 80));
    egt::Button b2(fsizer, "b2", egt::Rect(0, 0, 200, 80));
    egt::Button b3(fsizer, "b3", egt::Rect(0, 0, 300, 80));
    egt::Button b4(fsizer, "b4", egt::Rect(0, 0, 100, 80));
    egt::Button b5(fsizer, "b5", egt::Rect(0, 0, 100, 300));

    EXPECT_EQ(b1.display_origin(), egt::DisplayPoint(50, 0));
    EXPECT_EQ(b1.box().size(), egt::Size(100, 80));

    EXPECT_EQ(b2.display_origin(), egt::DisplayPoint(150, 0));
    EXPECT_EQ(b2.box().size(), egt::Size(200, 80));

    EXPECT_EQ(b3.display_origin(), egt::DisplayPoint(0, 80));
    EXPECT_EQ(b3.box().size(), egt::Size(300, 80));

    EXPECT_EQ(b4.display_origin(), egt::DisplayPoint(300, 80));
    EXPECT_EQ(b4.box().size(), egt::Size(100, 80));

    EXPECT_EQ(b5.display_origin(), egt::DisplayPoint(150, 160));
    EXPECT_EQ(b5.box().size(), egt::Size(100, 300));

    EXPECT_EQ(fsizer.display_origin(), egt::DisplayPoint(0, 0));
    EXPECT_EQ(fsizer.box().size(), egt::Size(400, 400));
}

TEST_F(Layout, FlexSizerExpandedAndExpendedChild)
{
    /// The goal of this test is to check the behavior of expanded child.
    /// Expansion is about using the size available. If two children fit in a
    /// row, there is no remaing space for other children so they will have an
    /// empty size. Check also when there is available space that the child is
    /// expanded. Expansion is done at the row level. The height of the expanded
    /// widget is the height of the row.
    egt::BoxSizer fsizer(window, egt::Orientation::flex);
    egt::expand(fsizer);

    egt::Button b1(fsizer, "b1");
    egt::expand(b1);

    egt::Button b2(fsizer, "b2", egt::Rect(0, 0, 200, 100));

    egt::Button b3(fsizer, "b3");
    egt::expand(b3);

    egt::Button b4(fsizer, "b4", egt::Rect(0, 0, 200, 100));

    egt::Button b5(fsizer, "b5", egt::Rect(0, 0, 100, 200));

    egt::Button b6(fsizer, "b6");
    egt::expand(b6);

    EXPECT_EQ(b1.display_origin(), egt::DisplayPoint(0, 0));
    EXPECT_EQ(b1.box().size(), egt::Size(0, 100));

    EXPECT_EQ(b2.display_origin(), egt::DisplayPoint(0, 0));
    EXPECT_EQ(b2.box().size(), egt::Size(200, 100));

    EXPECT_EQ(b3.display_origin(), egt::DisplayPoint(200, 0));
    EXPECT_EQ(b3.box().size(), egt::Size(0, 100));

    EXPECT_EQ(b4.display_origin(), egt::DisplayPoint(200, 0));
    EXPECT_EQ(b4.box().size(), egt::Size(200, 100));

    EXPECT_EQ(b5.display_origin(), egt::DisplayPoint(0, 100));
    EXPECT_EQ(b5.box().size(), egt::Size(100, 200));

    EXPECT_EQ(fsizer.display_origin(), egt::DisplayPoint(0, 0));
    EXPECT_EQ(fsizer.box().size(), egt::Size(400, 400));
}
