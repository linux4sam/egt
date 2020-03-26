
#include <egt/ui>
#include <gtest/gtest.h>

using namespace egt;

template <class T>
class ButtonWidgets : public testing::Test
{
protected:
    ButtonWidgets() {}
    ~ButtonWidgets() override {}
};

std::string DefaultText = "Button Text";

using WidgetTypes = ::testing::Types<Button,
      RadioBox,
      ImageButton,
      CheckButton,
      CheckBox>;
TYPED_TEST_SUITE(ButtonWidgets, WidgetTypes);

TYPED_TEST(ButtonWidgets, TestValueRange)
{
    egt::Application app;
    egt::TopWindow win;

    auto widget = std::make_shared<TypeParam>(DefaultText);

    win.add(widget);

    EXPECT_TRUE(widget->text() == DefaultText);

    bool text_changed = false;
    widget->on_text_changed([&]()
    {
        text_changed = true;
    });
    widget->text("New Button Text");
    EXPECT_TRUE(widget->text() == "New Button Text");
    EXPECT_TRUE(text_changed);

    bool on_checked = false;
    widget->on_checked_changed([&on_checked]()
    {
        on_checked = true;
    });
    widget->checked(true);
    EXPECT_TRUE(on_checked);

    bool on_hide = false;
    widget->on_hide([&on_hide]()
    {
        on_hide = true;
    });
    widget->hide();
    EXPECT_TRUE(on_hide);

    bool on_show = false;
    widget->on_show([&on_show]()
    {
        on_show = true;
    });
    widget->show();
    EXPECT_TRUE(on_show);

}

TEST(ImageButtonTest, ImageButton)
{
    egt::Application app;
    egt::TopWindow win;

    auto image = egt::Image("icon:calculator.png");
    auto imgbtn = std::make_shared<egt::ImageButton>(image, "Calculator");
    win.add(imgbtn);

    auto img1 = egt::Image("icon:cursor_hand.png");
    imgbtn->image(img1);

    EXPECT_TRUE(imgbtn->show_label());

    imgbtn->show_label(false);
    EXPECT_FALSE(imgbtn->show_label());
}
