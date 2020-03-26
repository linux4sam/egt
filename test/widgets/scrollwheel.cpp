
#include <egt/ui>
#include <gtest/gtest.h>

using namespace egt;

using ::testing::Combine;
using ::testing::Range;
using ::testing::TestWithParam;
using ::testing::Values;


class ScrollwheelTest : public testing::TestWithParam<::testing::tuple<int, int>> {};

TEST_P(ScrollwheelTest, Test1)
{
    egt::Application app;
    egt::TopWindow win;
    std::shared_ptr<egt::Scrollwheel> widget;

    auto item_count = 10;
    egt::Scrollwheel::ItemArray items;
    for (auto x = 0; x < item_count; x++)
    {
        items.push_back("Testitem " + std::to_string(x));
    }

    bool reversed = ::testing::get<0>(GetParam());
    auto type = ::testing::get<1>(GetParam());
    if (type == 0)
    {
        EXPECT_NO_THROW(widget.reset(new Scrollwheel()));
        EXPECT_NO_THROW(win.add(expand(widget)));
        EXPECT_EQ(static_cast<int>(widget->item_count()), 0);

        for (auto x = 0; x < item_count; x++)
        {
            widget->add_item("Testitem " + std::to_string(x));
        }

        EXPECT_EQ(static_cast<int>(widget->item_count()), item_count);

        std::cout << " value = " << widget->value() << std::endl;
        EXPECT_TRUE(widget->value() == "Testitem 0");
    }
    else if (type == 1)
    {
        EXPECT_NO_THROW(widget.reset(new Scrollwheel(items)));
        EXPECT_NO_THROW(win.add((widget)));

        EXPECT_EQ(static_cast<int>(widget->item_count()), item_count);

        std::cout << " value = " << widget->value() << std::endl;

        widget->reversed(reversed);
        if (reversed)
            EXPECT_TRUE(widget->value() == "Testitem 9");
        else
            EXPECT_TRUE(widget->value() == "Testitem 0");
    }
    else if (type == 2)
    {
        EXPECT_NO_THROW(widget.reset(new Scrollwheel(Rect(0, 0, 100, 150), items)));
        EXPECT_NO_THROW(win.add(widget));

        EXPECT_EQ(widget->size(), Size(100, 150));

        EXPECT_EQ(static_cast<int>(widget->item_count()), item_count);

        std::cout << " value = " << widget->value() << std::endl;

        widget->reversed(reversed);
        if (reversed)
            EXPECT_TRUE(widget->value() == "Testitem 9");
        else
            EXPECT_TRUE(widget->value() == "Testitem 0");
    }
    else if (type == 3)
    {
        EXPECT_NO_THROW(widget.reset(new Scrollwheel(win, items)));

        EXPECT_EQ(static_cast<int>(widget->item_count()), item_count);

        std::cout << " value = " << widget->value() << std::endl;
        if (reversed)
            EXPECT_TRUE(widget->value() == "Testitem 9");
        else
            EXPECT_TRUE(widget->value() == "Testitem 0");
    }
    else if (type == 4)
    {
        EXPECT_NO_THROW(widget.reset(new Scrollwheel(win, Rect(0, 0, 100, 150), items)));

        EXPECT_EQ(widget->size(), Size(100, 150));

        EXPECT_EQ(static_cast<int>(widget->item_count()), item_count);

        std::cout << " value = " << widget->value() << std::endl;

        widget->reversed(reversed);
        if (reversed)
            EXPECT_TRUE(widget->value() == "Testitem 9");
        else
            EXPECT_TRUE(widget->value() == "Testitem 0");
    }
    else if (type == 5)
    {
        EXPECT_NO_THROW(widget.reset(new Scrollwheel(Rect(0, 0, 100, 150), 0, 9, 1)));

        EXPECT_EQ(widget->size(), Size(100, 150));

        EXPECT_EQ(static_cast<int>(widget->item_count()), item_count);

        std::cout << " value = " << widget->value() << std::endl;

        widget->reversed(reversed);
        if (reversed)
            EXPECT_TRUE(widget->value() == "9");
        else
            EXPECT_TRUE(widget->value() == "0");
    }

    bool value_changed = false;
    widget->on_value_changed([&value_changed]()
    {
        value_changed = true;
    });
    widget->selected(5);
    EXPECT_TRUE(value_changed);


    widget->orient(egt::Orientation::horizontal);
    EXPECT_EQ(widget->orient(), egt::Orientation::horizontal);

    widget->orient(egt::Orientation::vertical);
    EXPECT_EQ(widget->orient(), egt::Orientation::vertical);

}

INSTANTIATE_TEST_SUITE_P(ScrollwheelTestGroup, ScrollwheelTest, Combine(Range(0, 6), Values(0, 1)));
