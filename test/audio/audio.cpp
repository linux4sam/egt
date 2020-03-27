/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <gtest/gtest.h>
#include <fstream>
#include <random>
#include <string>

using ::testing::TestWithParam;
using ::testing::Values;

static int random_item(int start, int end)
{
    std::random_device r;
    std::default_random_engine e {r()};
    std::uniform_int_distribution<int> dist(start, end);
    return dist(e);
}

static bool audio_device()
{
    std::ifstream infile("/proc/asound/devices");
    if (infile.is_open())
    {
        std::string line;
        while (getline(infile, line))
        {
            if (line.find("audio playback") != std::string::npos)
            {
                infile.close();
                return true;
            }
        }
        infile.close();
    }
    return false;
}

class AudioPlayerTest : public testing::TestWithParam<std::string> {};

TEST_P(AudioPlayerTest, AudioPlayer)
{
    egt::Application app;
    std::shared_ptr<egt::AudioPlayer> m_player;

    if (!audio_device())
    {
        FAIL() << "No Sound device";
        return;
    }

    std::string file = GetParam();

    bool on_error = false;
    bool play = false;
    if (file.empty())
    {
        /**
         * Note if below test does not quit after fail.
         * then g_mainthread is still running.
         */
        EXPECT_NO_THROW(m_player.reset(new egt::AudioPlayer()));
        EXPECT_THROW(m_player->media(file), std::runtime_error);
        EXPECT_THROW(m_player.reset(new egt::AudioPlayer(file)), std::runtime_error);
    }
    else
    {
        EXPECT_NO_THROW(m_player.reset(new egt::AudioPlayer(file)));

        if (m_player)
        {
            egt::PeriodicTimer cputimer(std::chrono::seconds(5));
            m_player->on_error([&cputimer, &app, &on_error](std::string err)
            {
                on_error = true;
                if (cputimer.running())
                    cputimer.stop();
                EXPECT_NO_THROW(app.quit());
            });

            if (file.find("concerto.mp3") != std::string::npos)
            {
                EXPECT_TRUE(play = m_player->play());
                EXPECT_FALSE(on_error);
            }
            else
            {
                EXPECT_FALSE(play = m_player->play());
                EXPECT_TRUE(on_error);
            }

            m_player->on_eos([&cputimer, &app, &on_error]()
            {
                EXPECT_FALSE(on_error);
                cputimer.stop();
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

            m_player->on_position_changed([&m_player](uint64_t position)
            {
                auto d = static_cast<int>(m_player->duration());
                EXPECT_EQ(d, 865);
                EXPECT_LE(static_cast<int>(position), d);
            });

            /**
             * This TestCase test below functionality
             * 1. play and pause
             * 2. seek audio randomly to 90% of duration.
             */
            cputimer.on_timeout([m_player, &play]()
            {
                if (m_player->playing())
                {
                    EXPECT_TRUE(m_player->pause());
                    play = false;
                }
                else
                {
                    EXPECT_TRUE(m_player->play());
                    play = true;
                }

                if (m_player->playing())
                {
                    static int count = 0;
                    float seek = random_item(1, 90) / 100.0;
                    auto dur = m_player->duration();
                    if (count <= 3)
                    {
                        EXPECT_TRUE(m_player->seek(dur * seek));
                        count++;
                    }

                    if (count == 4)
                    {
                        EXPECT_TRUE(m_player->seek(dur * 0.98));
                        count++;
                    }

                    int volume = random_item(1, 5);
                    EXPECT_TRUE(m_player->volume(volume));
                    EXPECT_LE(m_player->volume(), (volume + 1));
                }
            });
            cputimer.start();
            EXPECT_NO_THROW(app.run());

            if (file.find("concerto.mp3") != std::string::npos)
            {
                EXPECT_FALSE(on_error);
            }
            else
            {
                EXPECT_TRUE(on_error);
            }
        }
    }
}

static const std::string audiofiles[] =
{
    "",
    "file:/vgfweyquwfudqwfufcdqwcudqw.kc",
    "file:concerto.mp3"
};
INSTANTIATE_TEST_SUITE_P(DISABLED_AudioPlayerGroup, AudioPlayerTest, testing::ValuesIn(audiofiles));
