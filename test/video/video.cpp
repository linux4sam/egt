/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <gtest/gtest.h>
#include <random>

using ::testing::TestWithParam;
using ::testing::Values;
using ::testing::AssertionResult;
using ::testing::Combine;
using ::testing::Range;

class VideoWidgetTest : public testing::TestWithParam<std::string> {};

static int random_item(int start, int end)
{
    std::random_device r;
    std::default_random_engine e {r()};
    std::uniform_int_distribution<int> dist(start, end);
    return dist(e);
}

TEST_P(VideoWidgetTest, VideoWidget)
{
    egt::Application app;
    egt::TopWindow win;
    std::shared_ptr<egt::VideoWindow> m_player;

    std::string file = GetParam();

    bool play = false;
    bool error_flag = false;
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
            m_player->on_error([m_player, &app, &error_flag](std::string err)
            {
                error_flag = true;
                EXPECT_NO_THROW(app.quit());
            });

            m_player->on_state_changed([m_player, &play]()
            {
                if (play)
                {
                    EXPECT_TRUE(m_player->playing());
                }
                else
                {
                    EXPECT_FALSE(m_player->playing());
                }
            });

            if (file.find("microchip_corporate_mpeg2") != std::string::npos)
            {
                EXPECT_NO_THROW(m_player->media(file));
                EXPECT_TRUE(play = m_player->play());
                EXPECT_TRUE(play = m_player->playing());
            }
            else
            {
                EXPECT_FALSE(m_player->media(file));
            }
        }

        /**
         * This TestCase test below functionality
         * 1. play and pause
         * 2. scale
         * 3. move (center, rightm left, top & bottom
         * 4. seek video up to 60% of duration.
         * 5. CPU utilization between 0 to 60%
         */
        if (play)
        {
            /**
             * If Loopback Mode Enabled and receive a
             * EOS event then report as error.
             */
            m_player->on_eos([m_player, &app]()
            {
                EXPECT_FALSE(m_player->loopback());
                EXPECT_NO_THROW(app.quit());
            });

            m_player->on_position_changed([&m_player]()
            {
                auto d = static_cast<int>(m_player->duration() / 1000000000UL);
                EXPECT_EQ(d, 119);

                auto p = static_cast<int>(m_player->position() / 1000000000UL);
                EXPECT_LE(p, d);
            });

            egt::experimental::CPUMonitorUsage tools;
            egt::PeriodicTimer cputimer(std::chrono::seconds(3));
            cputimer.on_timeout([m_player, &play, &tools]()
            {
                if (m_player->playing())
                {
                    EXPECT_TRUE(m_player->pause());
                    play = false;

                    auto p = egt::Point(random_item(0, 480), random_item(0, 240));
                    m_player->move(p);
                    EXPECT_EQ(m_player->point(), p);

                    static int count = 0;
                    if (count % 2)
                    {
                        EXPECT_NO_THROW(m_player->scale(1.5, 1.5));
                        EXPECT_FLOAT_EQ(m_player->hscale(), 1.5);
                        EXPECT_FLOAT_EQ(m_player->vscale(), 1.5);
                    }
                    else
                    {
                        EXPECT_NO_THROW(m_player->scale(1.0, 1.0));
                        EXPECT_FLOAT_EQ(m_player->hscale(), 1.0);
                        EXPECT_FLOAT_EQ(m_player->vscale(), 1.0);
                    }

                    float seek = random_item(1, 90) / 100.0;
                    if (count <= 10)
                    {
                        EXPECT_TRUE(m_player->seek(m_player->duration() * seek));
                        count++;
                    }

                    tools.update();
                    EXPECT_LE(static_cast<int>(tools.usage()), 60);
                }
                else
                {
                    EXPECT_TRUE(m_player->play());
                    play = true;
                }
            });
            cputimer.start();

            EXPECT_NO_THROW(m_player->show());
            EXPECT_NO_THROW(win.show());
            EXPECT_NO_THROW(app.run());
        }

        if (file.find("microchip_corporate_mpeg2") != std::string::npos)
        {
            EXPECT_FALSE(error_flag);
        }
        else
        {
            EXPECT_TRUE(error_flag);
        }
    }
}

static const std::string videofiles[] =
{
    "file:microchip_corporate_mpeg2.avi"
    "",
    "file:/ewfgfewyyewfyfewylfew",
};
INSTANTIATE_TEST_SUITE_P(DISABLED_VideoWindowTestGroup, VideoWidgetTest, testing::ValuesIn(videofiles));

class CreateVideoWindowTest : public testing::TestWithParam<::testing::tuple<int, int>>
{
};

TEST_P(CreateVideoWindowTest, DefaultVideoWindow)
{
    egt::Application app;
    egt::TopWindow win;
    std::shared_ptr<egt::VideoWindow> m_player;

    egt::PixelFormat format = static_cast<egt::PixelFormat>(::testing::get<0>(GetParam()));
    egt::WindowHint hint = static_cast<egt::WindowHint>(::testing::get<1>(GetParam()));

    EXPECT_NO_THROW(m_player.reset(new egt::VideoWindow(egt::Size(320, 240), format, hint)));

    if (m_player)
    {
        EXPECT_NO_THROW(win.add(m_player));

        bool res = false;
        std::string file;
        file = "file:microchip_corporate_mpeg2.avi";
        EXPECT_EQ(res = m_player->media(file), true);
        if (res)
        {
            EXPECT_TRUE(res = m_player->play());
            EXPECT_EQ(m_player->format(), format);
            EXPECT_NO_THROW(m_player->show());
            EXPECT_NO_THROW(win.show());
        }
    }
}

INSTANTIATE_TEST_SUITE_P(DISABLED_CreateVideoWindowGroup, CreateVideoWindowTest, Combine(Range(1, 11), Values(1, 2, 4, 8, 16)));
