/*
 * Copyright (C) 2013-2015 Atmel Corporation.
 *
 * This file is licensed under the terms of the Atmel LIMITED License Agreement
 * as written in the COPYING file that is delivered in the project root directory.
 *
 * DISCLAIMER: THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
 * See the COPYING license file for more details.
 */

#ifndef TOOLS_H
#define TOOLS_H

/**
 * @file tools.h
 * @brief Colection of functions to perform common tasks
 */

/**
 *  @brief
 *  Tools
 * */
class Tools
{
public:
    Tools();

    virtual ~Tools();

    // Stores CPU usage
    float  cpu_usage[4];

    /**
      * Updates the CPU Usage
      * The value is stored in the public cpu_usage array
      **/
    void updateCpuUsage();

private:

    // For CPU usage
    float total_cpu_last_time[4];
    float work_cpu_last_time[4];
};

#endif // TOOLS_H
