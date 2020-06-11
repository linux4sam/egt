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
            egt::PeriodicTimer cputimer(std::chrono::seconds(2));
            bool on_error = false;
            std::string error_message;
            m_player->on_error([&cputimer, &on_error, &error_message](std::string err)
            {
                on_error = true;
                error_message = err;
                if (cputimer.running())
                    cputimer.stop();
            });

            if (file.find("concerto.mp3") != std::string::npos)
            {
                m_player->on_eos([&cputimer, &app]()
                {
                    if (cputimer.running())
                        cputimer.stop();
                    EXPECT_NO_THROW(app.quit());
                });

                bool state_changed = false;
                m_player->on_state_changed([&state_changed]()
                {
                    state_changed = true;
                });

                bool position = false;
                m_player->on_position_changed([&position](int64_t pos)
                {
                    position = true;
                });

                EXPECT_TRUE(m_player->play());
                if (on_error)
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
                 * This testcase seek audio in steps of 25%.
                 */
                cputimer.on_timeout([&m_player]()
                {
                    if (m_player->playing())
                    {
                        static int count = 0;
                        static float seek = 0.25;
                        auto dur = m_player->duration();
                        if (count <= 2)
                        {
                            std::cout << " seeking to " << seek << std::endl;
                            EXPECT_TRUE(m_player->seek(dur * seek));
                            EXPECT_LE(m_player->position(), (dur * (seek + 0.01)));
                            seek += 0.25;
                        }
                        if (count == 3)
                        {
                            EXPECT_TRUE(m_player->seek(dur * 0.998));
                            EXPECT_LE(m_player->position(), dur);
                        }
                        count++;
                    }
                });
                cputimer.start();
                EXPECT_NO_THROW(app.run());

                EXPECT_FALSE(on_error);
                EXPECT_TRUE(state_changed);
                EXPECT_TRUE(position);
            }
            else
            {
                EXPECT_FALSE(m_player->play());
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
