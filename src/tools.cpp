/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "egt/tools.h"
#include <sstream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <cassert>

using namespace std;

namespace egt
{

    CPUMonitorUsage::CPUMonitorUsage()
    {}

    void CPUMonitorUsage::update()
    {
        std::ifstream fp("/proc/stat");

        if (fp.is_open())
        {
            int i = 0;
            std::string line;
            while (std::getline(fp, line) && i < 1)
            {
                istringstream ss(line);
                string field0;
                ss >> field0;

                char cpu_id[5];
                sprintf(cpu_id, "cpu%d", i);

                if (field0.find(cpu_id) != std::string::npos)
                {
                    uint64_t times[7];
                    for (auto& x : times)
                        ss >> x;

                    uint64_t work_time = times[0] + times[1] + times[2];
                    uint64_t total_time = work_time + times[3] + times[4] + times[5] + times[6];

                    // Update CPU Usage
                    m_cpu_usage[i] = ((double)work_time - m_work_cpu_last_time[i]) /
                                     ((double)total_time - m_total_cpu_last_time[i]) * 100.;

                    // Update last values
                    m_work_cpu_last_time[i] = (double)work_time;
                    m_total_cpu_last_time[i] = (double)total_time;

                    i++;
                }
            }

            fp.close();
        }
    }

    CPUMonitorUsage::~CPUMonitorUsage()
    {}

}
