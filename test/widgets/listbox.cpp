
#include <egt/ui>
#include <gtest/gtest.h>

using namespace egt;

using ::testing::TestWithParam;
using ::testing::Values;
using ::testing::Range;

class ListBoxWidgetTest : public testing::TestWithParam<int> {};


TEST_P(ListBoxWidgetTest, TestWidget)
{
    egt::Application app;
    egt::TopWindow win;
    std::shared_ptr<egt::ListBox> widget;

    auto type = GetParam();

    egt::ListBox::ItemArray items;
    for (auto x = 0; x <= 6; x++)
        items.push_back(std::make_shared<egt::StringItem>("Testitem " + std::to_string(x)));

    if (type == 0)
    {
        EXPECT_NO_THROW(widget.reset(new egt::ListBox()));
        EXPECT_NO_THROW(win.add(expand(widget)));

        for (auto x = 0; x <= 6; x++)
            widget->add_item(std::make_shared<egt::StringItem>("Testitem " + std::to_string(x)));
    }
    else if (type == 1)
    {
        EXPECT_NO_THROW(widget.reset(new egt::ListBox(Rect(0, 0, 200, 100))));
        EXPECT_NO_THROW(win.add(widget));

        for (auto x = 0; x <= 6; x++)
            widget->add_item(std::make_shared<egt::StringItem>("Testitem " + std::to_string(x)));
    }
    else if (type == 2)
    {
        EXPECT_NO_THROW(widget.reset(new egt::ListBox(items)));
        EXPECT_NO_THROW(win.add(expand(widget)));
    }
    else if (type == 3)
    {
        EXPECT_NO_THROW(widget.reset(new egt::ListBox(items, Rect(0, 0, 400, 200))));
        EXPECT_NO_THROW(win.add(widget));
    }
    else if (type == 4)
    {
        EXPECT_NO_THROW(widget.reset(new egt::ListBox(win, items, Rect(0, 0, 300, 600))));
    }

    EXPECT_EQ(static_cast<int>(widget->item_count()), 7);

    EXPECT_NO_THROW(widget->remove_item(widget->item_at(5).get()));
    EXPECT_EQ(static_cast<int>(widget->item_count()), 6);

    bool state = false;
    widget->on_selected_changed([&state]()
    {
        std::cout << "on_selected_changed" << std::endl;
        state = true;
    });
    widget->selected(4);
    EXPECT_EQ(widget->selected(), 4);
    EXPECT_TRUE(state);


    PeriodicTimer cputimer(std::chrono::seconds(1));
    cputimer.on_timeout([widget, &cputimer, &app]()
    {
        static int count = 0;
        auto btn3 = std::make_shared<egt::Button>("Disabled1");
        widget->add_item(egt::expand(btn3));

        EXPECT_NO_THROW(widget->remove_item(btn3.get()));

        if (count++ >= 20)
        {
            static bool flag = false;
            if (flag)
            {
                count = 0;
                cputimer.stop();
                app.quit();
            }
            widget->clear();
            flag = true;
        }

    });
    cputimer.start();

    win.show();
    app.run();

}

INSTANTIATE_TEST_SUITE_P(ListBoxWidgetTestGroup, ListBoxWidgetTest, Range(0, 4));
