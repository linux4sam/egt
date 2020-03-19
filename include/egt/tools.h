/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_TOOLS_H
#define EGT_TOOLS_H

#include <chrono>
#include <cstdint>
#include <egt/detail/meta.h>

/**
 * @file
 * @brief Collection of various tools.
 */

namespace egt
{
inline namespace v1
{
namespace experimental
{
/**
 * Monitor CPU usage of the system.
 */
class EGT_API CPUMonitorUsage
{
public:

    CPUMonitorUsage() = default;

    /**
     * Get the total CPU usage as a percentage.
     */
    inline double usage() const { return m_cpu_usage; }

    /**
     * Update the CPU usage.
     *
     * This must be called periodically to perform the calculation.
     */
    void update();

    virtual ~CPUMonitorUsage() = default;

private:

    size_t m_last_total_time{0};
    size_t m_last_idle_time{0};
    double m_cpu_usage{0};
};

/**
 * Calculate "frame-per-second" of something.
 *
 * @code{.cpp}
 * FramesPerSecond fps;
 * fps.start();
 * while (true)
 * {
 *     std::cout << std::round(fps.fps()) << std::endl;
 *     fps.end_frame();
 * }
 * @endcode
 */
class EGT_API FramesPerSecond
{
public:

    FramesPerSecond() noexcept
    {
        start();
    }

    /**
     * Start/reset the counter.
     */
    void start()
    {
        m_start = std::chrono::steady_clock::now();
        m_frames = 0;
    }

    /**
     * Call at the end of every frame.
     */
    void end_frame()
    {
        m_frames++;

        const auto now = std::chrono::steady_clock::now();
        const auto diff = std::chrono::duration<double>(now - m_start).count();
        if (diff > 1.0)
        {
            m_fps = m_frames / diff;
            m_ready = true;
            start();
        }
    }

    /**
     * Is any calculation ready?
     */
    bool ready() const { return m_ready && m_frames > 0; }

    /**
     * Retrieve the current FPS value.
     */
    float fps()
    {
        m_ready = false;
        return m_fps;
    }

protected:

    /// Start time
    std::chrono::time_point<std::chrono::steady_clock> m_start{};

    /// Number of frames recorded since start time.
    uint64_t m_frames{0};

    /// Calculated FPS
    float m_fps{0.};

    /// Is m_fps valid and ready?
    bool m_ready{false};
};

}
}
}

#endif
