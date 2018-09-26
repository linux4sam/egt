/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef HAVE_CONFIG_H
#include "config.h"
#endif

#include "utils.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

namespace mui
{

    static int m_v = 0;

    void LOG_VERBOSE(int verbose)
    {
        m_v = verbose;
    }

    static int envset = -1;

    void LOG(const char* format, ...)
    {
        va_list ap;

        if (envset < 0)
            envset = !!getenv("MUI_DEBUG");

        if (m_v || envset)
        {
            va_start(ap, format);
            vfprintf(stderr, format, ap);
            va_end(ap);
        }
    }

    int globalenvset = -1;
}
