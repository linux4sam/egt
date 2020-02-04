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
     * Get the CPU usage as a percentage.
     *
     * @param index CPU index if more than one CPU.
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
 */
class EGT_API Fps
{
public:

    Fps() noexcept
    {
        start();
    }

    /**
     * Start/reset the counter.
     */
    inline void start()
    {
        m_start = std::chrono::steady_clock::now();
        m_frames = 0;
    }

    /**
     * Call at the end of every frame.
     */
    inline void end_frame()
    {
        m_frames++;

        auto now = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration<double>(now - m_start).count();
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
    inline bool ready() const { return m_ready && m_frames > 0; }

    /**
     * Retrieve the current FPS value.
     */
    inline float fps()
    {
        m_ready = false;
        return m_fps;
    }

protected:
    std::chrono::time_point<std::chrono::steady_clock> m_start{};
    uint64_t m_frames{0};
    float m_fps{0.};
    bool m_ready{false};
};

}
}
}

#endif
