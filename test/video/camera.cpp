/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <gtest/gtest.h>

using ::testing::TestWithParam;
using ::testing::Values;
using ::testing::AssertionResult;
using ::testing::Combine;
using ::testing::Range;

class CameraWindowTest : public testing::TestWithParam<std::string> {};

TEST_P(CameraWindowTest, CameraWidget)
{
    egt::Application app;
    egt::TopWindow win;
    std::shared_ptr<egt::CameraWindow> m_camera;
    std::string file = GetParam();
    egt::Size size(320, 240);

    EXPECT_NO_THROW(m_camera.reset(new egt::CameraWindow(size, file)));
    if (m_camera)
    {
        EXPECT_NO_THROW(win.add(m_camera));

        bool on_error = false;
        std::string error_message;
        m_camera->on_error([&on_error, &error_message](const std::string & message)
        {
            error_message = message;
            on_error = true;
        });

        if (m_camera->start())
        {
            egt::PeriodicTimer cputimer(std::chrono::seconds(1));
            cputimer.on_timeout([m_camera, &app, &cputimer, &win]()
            {
                static bool scale = true;
                if (scale)
                {
                    EXPECT_NO_THROW(m_camera->scale(2.5, 2.0));
                    EXPECT_FLOAT_EQ(m_camera->hscale(), 2.5);
                    EXPECT_FLOAT_EQ(m_camera->vscale(), 2.0);
                    scale = false;
                }
                else
                {
                    EXPECT_NO_THROW(m_camera->scale(1.0, 1.0));
                    EXPECT_FLOAT_EQ(m_camera->hscale(), 1.0);
                    EXPECT_FLOAT_EQ(m_camera->vscale(), 1.0);
                    m_camera->move_to_center(win.center());
                    scale = true;
                }

                static int quit_count = 0;
                if (quit_count > 3)
                {
                    cputimer.stop();
                    m_camera->stop();
                    EXPECT_NO_THROW(app.quit());
                }
                quit_count++;
            });
            cputimer.start();

            EXPECT_NO_THROW(m_camera->show());
            EXPECT_NO_THROW(win.show());
            EXPECT_NO_THROW(app.run());

            EXPECT_FALSE(on_error);
        }
        else
        {
            if (on_error)
                FAIL() << error_message;
        }
    }
}

INSTANTIATE_TEST_SUITE_P(DISABLED_CameraWindowTestGroup, CameraWindowTest, testing::Values("/dev/video0"));
