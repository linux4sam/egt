
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

class ViewTest : public testing::TestWithParam<::testing::tuple<int, int>> {};

TEST_P(ViewTest, TestWidget)
{
    egt::Application app;
    egt::TopWindow win;
    std::shared_ptr<egt::ScrolledView> widget;

    egt::ScrolledView::Policy hp = static_cast<egt::ScrolledView::Policy>(::testing::get<0>(GetParam()));
    egt::ScrolledView::Policy vp = static_cast<egt::ScrolledView::Policy>(::testing::get<1>(GetParam()));

    Rect rect = Rect(130, 80, 640, 320);
    EXPECT_NO_THROW(widget.reset(new ScrolledView(rect, hp, vp)));
    EXPECT_NO_THROW(win.add((widget)));

    for (int j = 0; j < 100; j++)
    {
        auto x = random_item(0, 1600);
        auto y = random_item(0, 800);
        auto text1 = std::make_shared<egt::Button>("Button " + std::to_string(j), Rect(x, y, 200, 80));
        EXPECT_NO_THROW(widget->add(text1));
    }

    EXPECT_EQ(widget->offset(), Point(0, 0));

    PeriodicTimer cputimer(std::chrono::seconds(1));
    cputimer.on_timeout([widget, &cputimer, &app, &rect, &hp, &vp]()
    {
        int mx = rect.width();
        int my = rect.height();
        auto px = random_item(0, mx);
        auto py = random_item(0, my);
        static int count = 0;

        if ((hp == egt::ScrolledView::Policy::always) &&
            (vp == egt::ScrolledView::Policy::always))
        {
            widget->offset(Point(-px, -py));
            EXPECT_EQ(widget->offset(), Point(-px, -py));
        }
        else if ((hp == egt::ScrolledView::Policy::always) ||
                 (hp == egt::ScrolledView::Policy::as_needed))
        {
            widget->hoffset(-px);
            EXPECT_EQ(widget->offset(), Point(-px, 0));
        }
        else if ((vp == egt::ScrolledView::Policy::always) ||
                 (vp == egt::ScrolledView::Policy::as_needed))
        {
            widget->voffset(-py);
            EXPECT_EQ(widget->offset(), Point(0, -py));
        }
        else
        {
            count = 10;
        }

        if (count % 2)
        {
            Size s = Size(320, 160);
            widget->resize(s);
            mx = s.width();
            my = s.height();
            EXPECT_EQ(widget->size(), s);
        }
        else
        {
            mx = rect.width();
            my = rect.height();
            widget->resize(rect.size());
            EXPECT_EQ(widget->size(), rect.size());
        }

        if (++count >= 10)
        {
            count = 0;
            cputimer.stop();
            app.quit();
        }
    });
    cputimer.start();

    win.show();
    app.run();
}

INSTANTIATE_TEST_SUITE_P(ViewTestGroup, ViewTest, Combine(Range(0, 3), Range(0, 3)));
