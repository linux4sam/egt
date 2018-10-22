/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mui/app.h"
#include "mui/event_loop.h"
#include "mui/input.h"
#include "mui/libinput.h"

#ifdef HAVE_X11
#include "mui/x11screen.h"
#endif

#include "mui/framebuffer.h"
#include "mui/kmsscreen.h"
#include <libintl.h>
#include <locale.h>
#include <thread>

using namespace std;

namespace mui
{
    static Application* the_app = nullptr;
    Application& main_app()
    {
        assert(the_app);
        return *the_app;
    }

    Application::Application(bool primary, const std::string& name)
    {
        the_app = this;

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
#else
            ignoreparam(primary);
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
        //new InputEvDev("/dev/input/event2");
#ifdef HAVE_LIBINPUT
        new LibInput;
#endif
    }

    int Application::run()
    {
        return m_event.run();
    }

    Application::~Application()
    {
    }
}
