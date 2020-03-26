
#include <egt/ui>
#include <gtest/gtest.h>
#include <random>

using namespace egt;

using ::testing::TestWithParam;
using ::testing::Values;
using ::testing::Range;

static int random_item(int start, int end)
{
    std::random_device r;
    std::default_random_engine e {r()};
    std::uniform_int_distribution<int> dist(start, end);
    return dist(e);
}

class ComboBoxWidgetTest : public testing::TestWithParam<int> {};


TEST_P(ComboBoxWidgetTest, TestWidget)
{
    egt::Application app;
    egt::TopWindow win;
    std::shared_ptr<egt::ComboBox> widget;

    auto item_count = random_item(300, 700);
    egt::ComboBox::ItemArray items;
    for (auto x = 0; x < item_count; x++)
        items.push_back("Testitem " + std::to_string(x));

    auto type = GetParam();
    if (type == 0)
    {
        EXPECT_NO_THROW(widget.reset(new ComboBox()));
        EXPECT_NO_THROW(win.add((widget)));
        for (auto x = 0; x < item_count; x++)
            widget->add_item("Testitem " + std::to_string(x));
    }
    else if (type == 1)
    {
        EXPECT_NO_THROW(widget.reset(new ComboBox(Rect(0, 0, 200, 100))));
        EXPECT_NO_THROW(win.add(widget));
        for (auto x = 0; x < item_count; x++)
            widget->add_item("Testitem " + std::to_string(x));
    }
    else if (type == 2)
    {
        EXPECT_NO_THROW(widget.reset(new ComboBox(items)));
        EXPECT_NO_THROW(win.add(widget));
    }
    else if (type == 3)
    {
        EXPECT_NO_THROW(widget.reset(new ComboBox(items, Rect(0, 0, 200, 100))));
        EXPECT_NO_THROW(win.add(widget));
    }
    else if (type == 4)
    {
        EXPECT_NO_THROW(widget.reset(new ComboBox(win, items, Rect(0, 0, 200, 100))));
    }

    EXPECT_EQ(static_cast<int>(widget->item_count()), item_count);

    std::cout << widget->item_at(5) << std::endl;
    EXPECT_TRUE(widget->item_at(5) == "Testitem 5");

    EXPECT_TRUE(widget->remove("Testitem 5"));
    EXPECT_EQ(static_cast<int>(widget->item_count()), (item_count - 1));

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
    cputimer.on_timeout([widget, &item_count, &cputimer, &app]()
    {
        static int flag = 0;
        if (flag % 2)
        {
            item_count = random_item(1, 10);
            for (auto x = 0; x <= item_count; x++)
                widget->add_item("Testitem " + std::to_string(x));
        }
        else
        {
            item_count = random_item(1, 10);
            for (auto x = 0; x <= item_count; x++)
                widget->remove("Testitem " + std::to_string(x));
        }
        flag++;

        if (flag >= 5)
        {
            flag = 0;
            cputimer.stop();
            app.quit();
        }

    });
    cputimer.start();

    //   win.show();
    app.run();

}
INSTANTIATE_TEST_SUITE_P(ComboBoxWidgetTestGroup, ComboBoxWidgetTest, Range(0, 4));
