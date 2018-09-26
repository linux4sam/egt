/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef HAVE_CONFIG_H
#include "config.h"
#endif

#include "app.h"
#include "event_loop.h"
#include "input.h"
#include "kmsscreen.h"
#include <libintl.h>
#include <locale.h>

using namespace std;

namespace mui
{

    Application::Application(bool primary, const std::string& name)
    {
        setlocale(LC_ALL, "");
        textdomain(name.c_str());

        static string backend;

        if (backend.empty())
        {
            const char* value = getenv("MUI_BACKEND");
            if (value)
                backend = value;
        }

        if (backend == "x11")
        {
#ifdef HAVE_X11
            new X11Screen(Size(800, 480));
#endif
        }

        if (backend == "kms")
        {
#ifdef HAVE_LIBPLANES
            new KMSScreen(primary);
#endif
        }

        if (backend == "fbdev")
        {
            new FrameBuffer("/dev/fb0");
        }

        if (backend.empty())
        {
#ifdef HAVE_LIBPLANES
            new KMSScreen(primary);
#elif defined(HAVE_X11)
            new X11Screen(Size(800, 480));
#else
            new FrameBuffer("/dev/fb0");
#endif
        }

#ifdef HAVE_TSLIB
        new InputTslib("/dev/input/touchscreen0");
#endif
        new InputEvDev("/dev/input/event2");
    }

    int Application::run()
    {
        return EventLoop::run();
    }

    Application::~Application()
    {
    }
}
