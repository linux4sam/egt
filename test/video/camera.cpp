
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <egt/ui>
#include <gtest/gtest.h>
#include <random>

using namespace egt;

using ::testing::TestWithParam;
using ::testing::Values;
using ::testing::AssertionResult;
using ::testing::Combine;
using ::testing::Range;

static int random_item(int start, int end)
{
    std::random_device r;
    std::default_random_engine e {r()};
    std::uniform_int_distribution<int> dist(start, end);
    return dist(e);
}

class CameraWindowTest : public testing::TestWithParam<std::string> {};

TEST_P(CameraWindowTest, CameraWidget)
{
    Application app;
    TopWindow win;
    std::shared_ptr<CameraWindow> m_camera;

    std::string file = GetParam();
    std::cout << "File : " << file << std::endl;

    Size size(320, 240);

    EXPECT_NO_THROW(m_camera.reset(new CameraWindow(size, file)));

    if (m_camera)
    {
        EXPECT_NO_THROW(win.add(m_camera));

        bool result = false;
        bool on_error = false;
        m_camera->on_error([m_camera, &on_error](const std::string & message)
        {
            if (message.empty() ||
                (message.find("device removed:") != std::string::npos))
            {
                on_error = true;
            }
        });

        EXPECT_TRUE(result = m_camera->start());
        if (result)
        {
            egt::experimental::CPUMonitorUsage tools;
            PeriodicTimer cputimer(std::chrono::seconds(5));
            cputimer.on_timeout([m_camera, &tools, &app, &cputimer]()
            {
                auto p = Point(random_item(1, 480), random_item(1, 240));
                EXPECT_NO_THROW(m_camera->move(p));
                EXPECT_EQ(m_camera->point(), p);

                static int quit_count = 0;
                if (quit_count % 2)
                {
                    EXPECT_NO_THROW(m_camera->scale(1.5, 1.5));
                }
                else
                {
                    EXPECT_NO_THROW(m_camera->scale(1.0, 1.0));
                }

                tools.update();
                EXPECT_LE(static_cast<int>(tools.usage()), 20);

                if (++quit_count >= 10)
                {
                    cputimer.stop();
                    m_camera->stop();
                    EXPECT_NO_THROW(app.quit());
                }
            });
            cputimer.start();

            EXPECT_NO_THROW(m_camera->show());
            EXPECT_NO_THROW(win.show());
            EXPECT_NO_THROW(app.run());


        }
    }
}

INSTANTIATE_TEST_SUITE_P(CameraWindowTestGroup, CameraWindowTest, testing::Values("/dev/video0"));
