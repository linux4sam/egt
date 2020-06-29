/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/tools.h"
#include <fstream>
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

}
}
}
