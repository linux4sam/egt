/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "mui/tools.h"
#include <sstream>
#include <fstream>
#include <cstring>
#include <sstream>

using namespace std;

namespace mui
{

    CPUMonitorUsage::CPUMonitorUsage()
    {}

    void CPUMonitorUsage::update()
    {
        std::ifstream fp("/proc/stat");

        if (fp.is_open())
        {
            int i = 0;      // CPU counter
            std::string line;
            while (std::getline(fp, line) && i < 1) // End of file or 4 cpus founded
            {
                istringstream ss(line);
                string field0;
                ss >> field0;

                char cpu_id[5]; // CPU to search
                sprintf(cpu_id, "cpu%d", i);  // Get CPU ID

                if (field0.find(cpu_id) != std::string::npos)
                {
                    int times[7];
                    for (auto& x : times)
                        ss >> x;

                    int work_time = times[0] + times[1] + times[2];
                    int total_time = work_time + times[3] + times[4] + times[5] + times[6];

                    // Update CPU Usage
                    m_cpu_usage[i] = ((float)work_time - m_work_cpu_last_time[i]) /
                                     ((float)total_time - m_total_cpu_last_time[i]) * 100;

                    // Update last values
                    m_work_cpu_last_time[i] = (float)work_time;
                    m_total_cpu_last_time[i] = (float)total_time;

                    i++; // Next CPU
                }
            }

            fp.close();
        }
    }

    CPUMonitorUsage::~CPUMonitorUsage()
    {}

}
