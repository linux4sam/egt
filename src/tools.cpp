/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/tools.h"
#include <cassert>
#include <cstring>
#include <fstream>
#include <sstream>

namespace egt
{
inline namespace v1
{
namespace experimental
{

void CPUMonitorUsage::update()
{
    std::ifstream fp("/proc/stat");

    if (fp.is_open())
    {
        int i = 0;
        std::string line;
        while (std::getline(fp, line) && i < 1)
        {
            std::istringstream ss(line);
            std::string field0;
            ss >> field0;

            std::ostringstream cpu_id;
            cpu_id << "cpu" << i;

            if (field0.find(cpu_id.str()) != std::string::npos)
            {
                uint64_t times[7];
                for (auto& x : times)
                    ss >> x;

                double work_time = times[0] + times[1] + times[2];
                double total_time = work_time + times[3] + times[4] + times[5] + times[6];

                // Update CPU Usage
                m_cpu_usage[i] = (work_time - m_work_cpu_last_time[i]) /
                                 (total_time - m_total_cpu_last_time[i]) * 100.;

                // Update last values
                m_work_cpu_last_time[i] = work_time;
                m_total_cpu_last_time[i] = total_time;

                i++;
            }
        }

        fp.close();
    }
}

}
}
}
