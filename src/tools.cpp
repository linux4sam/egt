/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/tools.h"
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <vector>

namespace egt
{
inline namespace v1
{
namespace experimental
{

static void get_cpu_times(std::vector<size_t>& times)
{
    std::ifstream in("/proc/stat");
    if (in.is_open())
    {
        in.ignore(5, ' ');
        size_t time{};
        while (in >> time)
            times.push_back(time);
    }
}

static bool get_cpu_times(size_t& idle_time, size_t& total_time)
{
    static std::vector<size_t> times;
    times.clear();
    get_cpu_times(times);
    if (times.size() < 4)
        return false;
    idle_time = times[3];
    total_time = std::accumulate(times.begin(), times.end(), static_cast<size_t>(0));
    return true;
}

void CPUMonitorUsage::update()
{
    size_t idle_time{};
    size_t total_time{};
    if (get_cpu_times(idle_time, total_time))
    {
        const double idle_time_diff = idle_time - m_last_idle_time;
        const double total_time_diff = total_time - m_last_total_time;
        m_cpu_usage = 100.0 * (1.0 - idle_time_diff / total_time_diff);
        m_last_idle_time = idle_time;
        m_last_total_time = total_time;
    }
}

struct PerfMonitor::Impl
{
    // Empty for now, reserved for future extensions
};

bool PerfMonitor::show_fps_enabled()
{
    static const bool value = (getenv("EGT_SHOW_FPS") != nullptr);
    return value;
}

bool PerfMonitor::show_cpu_enabled()
{
    static const bool value = (getenv("EGT_SHOW_CPU") != nullptr);
    return value;
}

PerfMonitor::PerfMonitor()
    : m_impl(std::make_unique<Impl>())
{
}

PerfMonitor::~PerfMonitor()
{
    stop();
}

void PerfMonitor::set_update_interval(std::chrono::milliseconds interval)
{
    m_update_interval = interval;
}

void PerfMonitor::start()
{
    if (running())
        return;

    m_stop_requested = false;
    m_running = true;
    m_thread = std::make_unique<std::thread>(&PerfMonitor::monitor_loop, this);
}

void PerfMonitor::stop()
{
    if (!running())
        return;

    m_stop_requested = true;

    if (m_thread && m_thread->joinable())
    {
        m_thread->join();
        m_thread.reset();
    }
    m_running = false;
}

bool PerfMonitor::running() const
{
    return m_running;
}

void PerfMonitor::enable_fps_tracking(bool enable)
{
    m_track_fps = enable;
}

void PerfMonitor::enable_cpu_tracking(bool enable)
{
    m_track_cpu = enable;
}

bool PerfMonitor::fps_tracking_enabled() const
{
    return m_track_fps;
}

bool PerfMonitor::cpu_tracking_enabled() const
{
    return m_track_cpu;
}

void PerfMonitor::notify_frame()
{
    if (fps_tracking_enabled())
        m_fps_monitor.end_frame();
}

void PerfMonitor::monitor_loop()
{
    while (!m_stop_requested)
    {
        // Schedule next update based on the desired interval
        const auto next_update = std::chrono::steady_clock::now() + m_update_interval;

        const bool show_fps = show_fps_enabled();
        const bool show_cpu = show_cpu_enabled();

        const bool show_any = show_fps || show_cpu;
        if (show_any)
            std::cout << "PerfMonitor:";

        if (fps_tracking_enabled())
        {
            const float fps = m_fps_monitor.fps();
            if (show_fps)
                std::cout << " FPS: " << std::fixed << std::setprecision(0) << std::round(fps);
        }

        if (cpu_tracking_enabled())
        {
            m_cpu_monitor.update();
            const double cpu = m_cpu_monitor.usage();
            if (show_cpu)
                std::cout << " CPU: " << std::fixed << std::setprecision(0) << cpu << "%";
        }

        if (show_any)
            std::cout << std::endl;

        // Compute remaining time until the next update and sleep at the end of the loop
        const auto after_work = std::chrono::steady_clock::now();
        const auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(next_update - after_work);
        if (remaining.count() > 0)
            std::this_thread::sleep_for(remaining);
    }
}

}
}
}
