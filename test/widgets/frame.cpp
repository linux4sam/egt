
#include <egt/ui>
#include <gtest/gtest.h>
#include <random>

using namespace egt;

using ::testing::Combine;
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

class FrameTest : public testing::TestWithParam<int> {};

TEST_P(FrameTest, TestWidget)
{
    egt::Application app;
    egt::TopWindow win;
    std::shared_ptr<egt::Frame> widget;

    Widget::Flags flag = static_cast<Widget::Flag>(GetParam());

    std::cout << " flag : " << flag << std::endl;
    EXPECT_NO_THROW(widget.reset(new Frame(Rect(0, 0, 800, 480), flag)));
    EXPECT_NO_THROW(win.add(widget));
    int wcount = 0;
    for (int i = 0; i < 10; i++)
    {
        auto w = random_item(40, 150);
        auto h = random_item(40, 150);
        auto text1 = std::make_shared<egt::TextBox>("textBox " + std::to_string(i), Rect(0, 0, w, h), AlignFlag::center);
        EXPECT_NO_THROW(widget->add(text1));
        wcount++;
    }
    auto c = widget->count_children();
    EXPECT_EQ(static_cast<int>(c), wcount);

    PeriodicTimer cputimer(std::chrono::seconds(2));
    cputimer.on_timeout([widget, &app, &cputimer, &c]()
    {
        static bool remove_all = false;
        c = widget->count_children();
        std::cout << " count : " << c << std::endl;
        if (c == 0)
        {
            for (int j = 10; j < 20; j++)
            {
                auto text2 = std::make_shared<egt::TextBox>("textBox " + std::to_string(j), Rect(random_item(0, 720), random_item(0, 340), 80, 100));
                EXPECT_NO_THROW(widget->add(text2));
            }
            remove_all = true;
        }
        else
        {
            if (!remove_all)
            {
                auto w = widget->child_at(c - 1);
                EXPECT_TRUE(w != nullptr);
                if (w != nullptr)
                {
                    auto n = w->name();
                    //auto f = widget->find_child(n);
                    std::cout << " name  : " << n  << std::endl;
                    EXPECT_NO_THROW(widget->remove(w.get()));
                }
            }
            else
            {
                EXPECT_NO_THROW(widget->remove_all());
                cputimer.stop();
                app.quit();
            }
        }
    });
    cputimer.start();

    widget->show();
    win.show();
    app.run();

}

INSTANTIATE_TEST_SUITE_P(FrameTestGroup, FrameTest, Values(1, 2, 4));
