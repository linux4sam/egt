/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_TOOLS_H
#define EGT_TOOLS_H

/**
 * @file
 * @brief Collection of various tools.
 */

namespace egt
{
inline namespace v1
{

/**
 * Monitor CPU usage of the system.
 */
class CPUMonitorUsage
{
public:

    CPUMonitorUsage() = default;

    double usage(unsigned int index = 0) const { return m_cpu_usage[index]; }

    /**
     * Update the CPU usage.
     * The value is stored in the public cpu_usage array
     */
    void update();

    virtual ~CPUMonitorUsage() = default;

private:

    double m_total_cpu_last_time[4] = {0};
    double m_work_cpu_last_time[4] = {0};
    double m_cpu_usage[4] = {0};
};

}
}

#endif
