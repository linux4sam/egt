/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <gtest/gtest.h>

using namespace egt;

static float calculate(float start, float decrement, int count)
{
    for (int i = 0; i < count; ++i)
        start -= decrement;
    return start;
}

static double calculate(double start, double decrement, int count)
{
    for (int i = 0; i < count; ++i)
        start -= decrement;
    return start;
}

TEST(Math, CompareFloat)
{
    const auto total = 10000;
    auto count = 0;
    for (auto i = 0; i < total; ++i)
    {
        auto expected = (i / 10.0f);
        auto actual = calculate(9.0f + expected, 0.2f, 45);
        if (detail::float_compare(actual, expected))
            ++count;
    }
    EXPECT_EQ(count, total);
}

TEST(Math, CompareDouble)
{
    const auto total = 10000;
    auto count = 0;
    for (auto i = 0; i < total; ++i)
    {
        auto expected = (i / 10.0);
        auto actual = calculate(9.0 + expected, 0.2, 45);
        if (detail::float_compare(actual, expected))
            ++count;
    }
    EXPECT_EQ(count, total);
}

TEST(Color, Basic)
{
    Color c1(0x11223344);
    EXPECT_EQ(c1.red(), 0x11U);
    EXPECT_EQ(c1.green(), 0x22U);
    EXPECT_EQ(c1.blue(), 0x33U);
    EXPECT_EQ(c1.alpha(), 0x44U);

    Color c2(c1,0xff);
    EXPECT_EQ(c2.red(), 0x11U);
    EXPECT_EQ(c2.green(), 0x22U);
    EXPECT_EQ(c2.blue(), 0x33U);
    EXPECT_EQ(c2.alpha(), 0xffU);

    Color c3(11,22,33,44);
    EXPECT_EQ(c3.red(), 11U);
    EXPECT_EQ(c3.green(), 22U);
    EXPECT_EQ(c3.blue(), 33U);
    EXPECT_EQ(c3.alpha(), 44U);

    Color c4 = Color::rgb(0x112233);
    EXPECT_EQ(c4.red(), 0x11U);
    EXPECT_EQ(c4.green(), 0x22U);
    EXPECT_EQ(c4.blue(), 0x33U);
    EXPECT_EQ(c4.alpha(), 0xffU);

    Color c5 = Color::rgbaf(0.0,1.0,0.5,0.75);
    EXPECT_EQ(c5.red(), 0U);
    EXPECT_EQ(c5.green(), 255U);
    EXPECT_FLOAT_EQ(c5.blue(), 127);
    EXPECT_FLOAT_EQ(c5.alpha(), 191);
}

TEST(TextBox, Basic)
{
    Application app;

    std::string str1 = "hello world\nthis is EGT";
    std::string str2 = "howdy";

    TextBox text1(str1);
    ASSERT_EQ(str1, text1.text());
    text1.text(str2);
    ASSERT_EQ(str2, text1.text());
    ASSERT_EQ(str2.size(), text1.len());
    text1.append(str1);
    ASSERT_EQ(str2 + str1, text1.text());
    text1.cursor_begin();
    text1.insert(str1);
    ASSERT_EQ(str1 + str2 + str1, text1.text());
    text1.cursor_end();
    text1.insert(str1);
    ASSERT_EQ(str1 + str2 + str1 + str1, text1.text());
    text1.selection_all();
    ASSERT_EQ(str1 + str2 + str1 + str1, text1.selected_text());
    text1.selection(str1.size(), 5);
    ASSERT_EQ(str2, text1.selected_text());
    text1.selection_clear();
    ASSERT_EQ("", text1.selected_text());
    text1.selection_all();
    text1.selection_delete();
    ASSERT_EQ("", text1.text());
}

TEST(Screen, DamageAlgorithm)
{
    Screen::DamageArray damage;
    Screen::damage_algorithm(damage,Rect(0,0,100,100));
    EXPECT_EQ(damage.front(),Rect(0,0,100,100));
    Screen::damage_algorithm(damage,Rect(0,0,200,200));
    EXPECT_EQ(damage.size(),1U);
    EXPECT_EQ(damage.front(),Rect(0,0,200,200));
}

TEST(Canvas, Basic)
{
    Canvas canvas1(Size(100,100));
    EXPECT_EQ(canvas1.size(), Size(100,100));
    EXPECT_EQ(canvas1.format(), PixelFormat::argb8888);
    Canvas canvas2(canvas1.surface());
    EXPECT_EQ(canvas2.size(), Size(100,100));
    EXPECT_EQ(canvas2.format(), PixelFormat::argb8888);

    Canvas canvas3(Size(100,100), PixelFormat::rgb565);
    EXPECT_EQ(canvas3.size(), Size(100,100));
    EXPECT_EQ(canvas3.format(), PixelFormat::rgb565);

    Canvas canvas4(canvas3.surface());
    EXPECT_EQ(canvas4.size(), Size(100,100));
    EXPECT_EQ(canvas4.format(), PixelFormat::rgb565);
}

TEST(Geometry, Basic)
{
    Point p1(3,4);
    EXPECT_EQ(p1.x(), 3);
    EXPECT_EQ(p1.y(), 4);
    p1 += 1;
    EXPECT_EQ(p1.x(), 4);
    EXPECT_EQ(p1.y(), 5);

    Size s1(3,4);
    EXPECT_EQ(s1.width(), 3);
    EXPECT_EQ(s1.height(), 4);
    s1 += 1;
    EXPECT_EQ(s1.width(), 4);
    EXPECT_EQ(s1.height(), 5);

    Rect r1(3, 4, 5, 6);
    EXPECT_EQ(r1.x(), 3);
    EXPECT_EQ(r1.y(), 4);
    EXPECT_EQ(r1.width(), 5);
    EXPECT_EQ(r1.height(), 6);
}

TEST(Geometry, Points)
{
    Point p1;

    Point p2(10,0);
    EXPECT_EQ(p1.distance_to(p2), 10);
}

TEST(Geometry, Rects)
{
    Rect r1;
    EXPECT_TRUE(r1.empty());
}

TEST(Geometry, Sizes)
{
    Size s1;
    EXPECT_TRUE(s1.empty());
}

TEST(Geometry, Arcs)
{
    Arc a1;
    EXPECT_TRUE(a1.empty());
}

TEST(Geometry, Circless)
{
    Circle c1;
    EXPECT_TRUE(c1.empty());
}

template <class T>
class Widgets : public testing::Test
{
protected:
    Widgets() {}
    ~Widgets() override {}
    Application app;
};

using WidgetTypes = ::testing::Types<AnalogMeter,
				     Button,
				     ImageButton,
				     CheckBox,
				     CircleWidget,
				     Label,
				     ImageLabel,
				     ListBox,
				     RadioBox,
				     TextBox,
				     ComboBox,
				     Slider,
				     BoxSizer,
				     VerticalBoxSizer,
				     HorizontalBoxSizer,
				     Notebook,
				     StaticGrid,
				     Scrollwheel,
				     ProgressBar,
				     SpinProgress,
				     LevelMeter,
				     AnalogMeter,
				     CircleWidget,
				     LineWidget,
				     RectangleWidget>;
TYPED_TEST_SUITE(Widgets, WidgetTypes);
TYPED_TEST(Widgets, Properties)
{
    auto widget = std::make_shared<TypeParam>();

    widget->resize(Size(100,100));
    EXPECT_EQ(widget->size(), Size(100,100));
    widget->move(Point(100,100));
    EXPECT_EQ(widget->point(), Point(100,100));
    widget->width(10);
    widget->height(10);
    EXPECT_EQ(widget->size(), Size(10,10));
    widget->x(10);
    widget->y(10);
    EXPECT_EQ(widget->point(), Point(10,10));
    widget->move_to_center(Point(10,10));
    EXPECT_EQ(widget->box(), Rect(5,5,10,10));
    EXPECT_EQ(widget->center(), Point(10,10));
    widget->box(Rect(50,51,15,16));
    EXPECT_EQ(widget->box(), Rect(50,51,15,16));
    EXPECT_EQ(widget->width(), 15);
    EXPECT_EQ(widget->height(), 16);
    EXPECT_EQ(widget->x(), 50);
    EXPECT_EQ(widget->y(), 51);
    EXPECT_EQ(widget->visible(), true);
    widget->hide();
    EXPECT_EQ(widget->visible(), false);
    widget->show();
    EXPECT_EQ(widget->visible(), true);
    widget->visible_toggle();
    EXPECT_EQ(widget->visible(), false);
    widget->visible_toggle();
    EXPECT_EQ(widget->visible(), true);
    EXPECT_EQ(widget->readonly(), false);
    widget->readonly(true);
    EXPECT_EQ(widget->readonly(), true);
    widget->readonly(false);
    EXPECT_EQ(widget->readonly(), false);
    widget->align(AlignFlag::right | AlignFlag::center);
    EXPECT_EQ(widget->align(), AlignFlag::right | AlignFlag::center);
    widget->padding(10);
    widget->border(11);
    widget->margin(12);
    EXPECT_EQ(widget->moat(), 33U);
}


TEST(WidgetFlags, Basic)
{
    Widget::Flags flags1("window|readonly");
    EXPECT_TRUE(flags1.is_set(Widget::Flag::window));
    EXPECT_TRUE(flags1.is_set(Widget::Flag::readonly));
    EXPECT_FALSE(flags1.is_set(Widget::Flag::plane_window));

    flags1.clear(Widget::Flag::readonly);
    EXPECT_TRUE(flags1.is_set(Widget::Flag::window));
    EXPECT_FALSE(flags1.is_set(Widget::Flag::readonly));
    EXPECT_FALSE(flags1.is_set(Widget::Flag::plane_window));

    flags1.set(Widget::Flag::plane_window);
    EXPECT_TRUE(flags1.is_set(Widget::Flag::window));
    EXPECT_FALSE(flags1.is_set(Widget::Flag::readonly));
    EXPECT_TRUE(flags1.is_set(Widget::Flag::plane_window));

    flags1.clear();
    EXPECT_FALSE(flags1.is_set(Widget::Flag::window));
    EXPECT_FALSE(flags1.is_set(Widget::Flag::readonly));
    EXPECT_FALSE(flags1.is_set(Widget::Flag::plane_window));

    flags1.set({Widget::Flag::window,Widget::Flag::readonly});
    EXPECT_TRUE(flags1.is_set(Widget::Flag::window));
    EXPECT_TRUE(flags1.is_set(Widget::Flag::readonly));
    EXPECT_FALSE(flags1.is_set(Widget::Flag::plane_window));
    EXPECT_EQ(flags1.to_string(), "window|readonly");
}

TEST(AlignFlags, Basic)
{
    bool state = false;
    AlignFlags flags1("left|right");
    flags1.on_change([&state](){ state = true; });
    EXPECT_TRUE(flags1.is_set(AlignFlag::left));
    EXPECT_TRUE(flags1.is_set(AlignFlag::right));
    EXPECT_FALSE(flags1.is_set(AlignFlag::bottom));

    flags1.clear(AlignFlag::right);
    EXPECT_TRUE(state); state = false;
    EXPECT_TRUE(flags1.is_set(AlignFlag::left));
    EXPECT_FALSE(flags1.is_set(AlignFlag::right));
    EXPECT_FALSE(flags1.is_set(AlignFlag::bottom));

    flags1.set(AlignFlag::bottom);
    EXPECT_TRUE(state); state = false;
    flags1.set(AlignFlag::bottom);
    EXPECT_FALSE(state);
    EXPECT_TRUE(flags1.is_set(AlignFlag::left));
    EXPECT_FALSE(flags1.is_set(AlignFlag::right));
    EXPECT_TRUE(flags1.is_set(AlignFlag::bottom));

    flags1.clear();
    EXPECT_TRUE(state); state = false;
    EXPECT_FALSE(flags1.is_set(AlignFlag::left));
    EXPECT_FALSE(flags1.is_set(AlignFlag::right));
    EXPECT_FALSE(flags1.is_set(AlignFlag::bottom));

    flags1.set({AlignFlag::left,AlignFlag::right});
    EXPECT_TRUE(state); state = false;
    flags1.set({AlignFlag::left,AlignFlag::right});
    EXPECT_FALSE(state);
    EXPECT_TRUE(flags1.is_set(AlignFlag::left));
    EXPECT_TRUE(flags1.is_set(AlignFlag::right));
    EXPECT_FALSE(flags1.is_set(AlignFlag::bottom));
    EXPECT_EQ(flags1.to_string(), "left|right");

    Button b;
    b.align().on_change([&state](){ state = true; });
    b.align(flags1);
    EXPECT_TRUE(state); state = false;
    EXPECT_TRUE(b.align().is_set(AlignFlag::left));
    EXPECT_TRUE(b.align().is_set(AlignFlag::right));
    EXPECT_FALSE(b.align().is_set(AlignFlag::bottom));

    b.align(b.align() | AlignFlag::bottom);
    EXPECT_TRUE(state); state = false;
    EXPECT_TRUE(b.align().is_set(AlignFlag::left));
    EXPECT_TRUE(b.align().is_set(AlignFlag::right));
    EXPECT_TRUE(b.align().is_set(AlignFlag::bottom));
}

TEST(Network, URI)
{
    egt::uri uri1("http://www.example.com/glynos/?key=value#frag");
    EXPECT_TRUE(uri1.is_valid());
    EXPECT_EQ(uri1.scheme(), "http");
    EXPECT_EQ(uri1.host() , "www.example.com");
    EXPECT_EQ(uri1.path() , "/glynos/");
    EXPECT_EQ(uri1.query() , "key=value");
    EXPECT_EQ(uri1.fragment() , "frag");

    uri1.set("res:name");
    EXPECT_TRUE(uri1.is_valid());
    EXPECT_EQ(uri1.scheme(), "res");
    EXPECT_EQ(uri1.host() , "");
    EXPECT_EQ(uri1.path() , "name");
    EXPECT_EQ(uri1.query() , "");
    EXPECT_EQ(uri1.fragment() , "");

    uri1.set("file:///absolute/path");
    EXPECT_TRUE(uri1.is_valid());
    EXPECT_EQ(uri1.scheme(), "file");
    EXPECT_EQ(uri1.host() , "");
    EXPECT_EQ(uri1.path() , "/absolute/path");
    EXPECT_EQ(uri1.query() , "");
    EXPECT_EQ(uri1.fragment() , "");

    uri1.set("file:relative/path");
    EXPECT_TRUE(uri1.is_valid());
    EXPECT_EQ(uri1.scheme(), "file");
    EXPECT_EQ(uri1.host() , "");
    EXPECT_EQ(uri1.path() , "relative/path");
    EXPECT_EQ(uri1.query() , "");
    EXPECT_EQ(uri1.fragment() , "");


    uri1.set("icon:relative/path");
    EXPECT_TRUE(uri1.is_valid());
    EXPECT_EQ(uri1.scheme(), "icon");
    EXPECT_EQ(uri1.host() , "");
    EXPECT_EQ(uri1.path() , "relative/path");
    EXPECT_EQ(uri1.query() , "");
    EXPECT_EQ(uri1.fragment() , "");

    uri1.set("icon:/absolute/path");
    EXPECT_TRUE(uri1.is_valid());
    EXPECT_EQ(uri1.scheme(), "icon");
    EXPECT_EQ(uri1.host() , "");
    EXPECT_EQ(uri1.path() , "/absolute/path");
    EXPECT_EQ(uri1.query() , "");
    EXPECT_EQ(uri1.fragment() , "");

    uri1.set("icon:/absolute/path;32");
    EXPECT_TRUE(uri1.is_valid());
    EXPECT_EQ(uri1.scheme(), "icon");
    EXPECT_EQ(uri1.host() , "");
    EXPECT_EQ(uri1.path() , "/absolute/path");
    EXPECT_EQ(uri1.query() , "");
    EXPECT_EQ(uri1.fragment() , "");
    EXPECT_EQ(uri1.icon_size() , "32");

    uri1.set("icon:/absolute/path;SMALL");
    EXPECT_TRUE(uri1.is_valid());
    EXPECT_EQ(uri1.scheme(), "icon");
    EXPECT_EQ(uri1.host() , "");
    EXPECT_EQ(uri1.path() , "/absolute/path");
    EXPECT_EQ(uri1.query() , "");
    EXPECT_EQ(uri1.fragment() , "");
    EXPECT_EQ(uri1.icon_size() , "16");
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
