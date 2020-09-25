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

class VideoWidgetTest : public testing::TestWithParam<std::string> {};

TEST_P(VideoWidgetTest, VideoWidget)
{
    egt::Application app;
    egt::TopWindow win;
    std::shared_ptr<egt::VideoWindow> m_player;

    std::string file = GetParam();
    egt::Size size(320, 192);

    if (file.empty())
    {
        EXPECT_NO_THROW(m_player.reset(new egt::VideoWindow(size)));
        EXPECT_THROW(m_player->media(file), std::runtime_error);
        EXPECT_THROW(m_player.reset(new egt::VideoWindow(size, file)), std::runtime_error);
    }
    else
    {
        EXPECT_NO_THROW(m_player.reset(new egt::VideoWindow(size)));
        if (m_player)
        {
            EXPECT_NO_THROW(win.add(m_player));

            bool error_flag = false;
            std::string error_message;
            m_player->on_error([&error_flag, &error_message](std::string err)
            {
                error_flag = true;
                error_message = err;
            });

            bool state_changed = false;
            m_player->on_state_changed([&state_changed]()
            {
                state_changed = true;
            });

            bool on_position = false;
            m_player->on_position_changed([&on_position](int64_t pos)
            {
                on_position = true;
            });

            if (file.find("microchip_corporate_mpeg2") != std::string::npos)
            {
                EXPECT_NO_THROW(m_player->media(file));

                if (error_flag)
                    FAIL() << error_message;

                EXPECT_TRUE(m_player->play());

                if (error_flag)
                    FAIL() << error_message;

                if (m_player->playing())
                {
                    EXPECT_TRUE(m_player->pause());
                }

                if (!m_player->playing())
                {
                    EXPECT_TRUE(m_player->play());
                }

                /**
                 * If Loopback Mode Enabled and receive a
                 * EOS event then report as error.
                 */
                egt::PeriodicTimer cputimer(std::chrono::seconds(2));
                m_player->on_eos([m_player, &app, &cputimer]()
                {
                    EXPECT_FALSE(m_player->loopback());
                    if (cputimer.running())
                        cputimer.stop();
                    EXPECT_NO_THROW(app.quit());
                });

                cputimer.on_timeout([&m_player]()
                {
                    static int count = 0;
                    if (m_player->playing())
                    {
                        static float seek = 0.25;
                        auto dur = m_player->duration();
                        if (count <= 2)
                        {
                            EXPECT_TRUE(m_player->seek(dur * seek));
                            EXPECT_LE(m_player->position(), (dur * (seek + 0.01)));
                            seek += 0.25;
                        }
                        if (count == 3)
                        {
                            EXPECT_TRUE(m_player->seek(dur * 0.98));
                            EXPECT_LE(m_player->position(), dur);
                        }
                        count++;
                        static bool scale = true;
                        if (scale)
                        {
                            EXPECT_NO_THROW(m_player->scale(2.5, 2.5));
                            EXPECT_FLOAT_EQ(m_player->hscale(), 2.5);
                            EXPECT_FLOAT_EQ(m_player->vscale(), 2.5);
                            scale = false;
                        }
                        else
                        {
                            EXPECT_NO_THROW(m_player->scale(1.0, 1.0));
                            EXPECT_FLOAT_EQ(m_player->hscale(), 1.0);
                            EXPECT_FLOAT_EQ(m_player->vscale(), 1.0);
                            scale = true;
                        }
                    }
                });
                cputimer.start();

                EXPECT_NO_THROW(m_player->show());
                EXPECT_NO_THROW(win.show());
                EXPECT_NO_THROW(app.run());

                EXPECT_FALSE(error_flag);
                EXPECT_TRUE(on_position);
                EXPECT_TRUE(state_changed);
            }
            else
            {
                EXPECT_FALSE(m_player->media(file));
                EXPECT_TRUE(error_flag);
            }
        }
    }
}

static const std::string videofiles[] =
{
    "",
    "file:ewfgfewyyewfyfewylfew",
    "file:microchip_corporate_mpeg2.avi"
};
INSTANTIATE_TEST_SUITE_P(DISABLED_VideoWindowTestGroup, VideoWidgetTest, testing::ValuesIn(videofiles));


