/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_TOOLS_H
#define MUI_TOOLS_H

/**
 * @file tools.h
 * @brief Collection of various tools.
 */

namespace mui
{

    /**
     * Monitor CPU usage of the system.
     */
    class CPUMonitorUsage
    {
    public:

        CPUMonitorUsage();

        virtual ~CPUMonitorUsage();

        float cpu_usage[4];

        /**
         * Update the CPU usage.
         * The value is stored in the public cpu_usage array
         */
        void update();

    private:

        float total_cpu_last_time[4];
        float work_cpu_last_time[4];
    };

}

#endif
