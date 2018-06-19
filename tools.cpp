/*
 * Copyright (C) 2013-2015 Atmel Corporation.
 *
 * This file is licensed under the terms of the Atmel LIMITED License Agreement
 * as written in the COPYING file that is delivered in the project root directory.
 *
 * DISCLAIMER: THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
 * See the COPYING license file for more details.
 */

#include "tools.h"
#include <sstream>
#include <fstream>
#include <cstring>

Tools::Tools()
{
}


Tools::~Tools()
{
}

void Tools::updateCpuUsage()
{
    std::ifstream fp("/proc/stat");
    char cpu_id[5]; // CPU to search
    char* line_buffer;
    int i = 0;      // CPU counter
    int times[7];

    // Time variables
    int total_time;
    int work_time;

    std::string line;
    if (fp.is_open())
    {
        while ( std::getline (fp,line) && i<1 ) // End of file or 4 cpus founded
        {
            sprintf(cpu_id, "cpu%d", i);  // Get CPU ID

            if(line.find(cpu_id) != std::string::npos){

                // Pass line to line buffer
                line_buffer = new char[line.size() + 1];
                strcpy (line_buffer, line.c_str());

                // Split data and get total and work time
                strtok(line_buffer, " ");
                // Get all 7 values
                times[0] = atoi(strtok(NULL, " "));
                times[1] = atoi(strtok(NULL, " "));
                times[2] = atoi(strtok(NULL, " "));
                times[3] = atoi(strtok(NULL, " "));
                times[4] = atoi(strtok(NULL, " "));
                times[5] = atoi(strtok(NULL, " "));
                times[6] = atoi(strtok(NULL, " "));
                work_time = times[0] + times[1] + times[2];
                total_time = work_time + times[3] + times[4] + times[5] + times[6];

                // Update CPU Usage
                cpu_usage[i] = ((float)work_time - work_cpu_last_time[i]) / ((float)total_time -total_cpu_last_time[i]) * 100;

                // Update last values
                work_cpu_last_time[i] = (float)work_time;
                total_cpu_last_time[i] = (float)total_time;

                i++; // Next CPU
            }

        }
        fp.close();
    }
}
