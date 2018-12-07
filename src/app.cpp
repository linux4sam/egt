/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/app.h"
#include "egt/eventloop.h"
#include "egt/framebuffer.h"
#include "egt/input.h"
#include "egt/kmsscreen.h"
#include "egt/libinput.h"
#include "egt/version.h"
#include <libintl.h>
#include <locale.h>
#include <thread>
#include <string>

#ifdef HAVE_X11
#include "egt/x11screen.h"
#endif

using namespace std;

namespace egt
{
    static Application* the_app = nullptr;
    Application& main_app()
    {
        assert(the_app);
        return *the_app;
    }

    Application::Application(int argc, const char** argv, bool primary, const std::string& name)
        : m_argc(argc),
          m_argv(argv)
    {
        INFO("EGT Version " << EGT_VERSION);

        the_app = this;

        setlocale(LC_ALL, "");
        textdomain(name.c_str());

        static string backend;

        if (backend.empty())
        {
            const char* value = getenv("EGT_BACKEND");
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
            //new X11Screen(Size(1280, 1024));
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
        if (m_argc)
        {
            // catch SIGQUIT (Ctrl + \) and screenshot
            asio::signal_set signals(event().io(), SIGQUIT);
            signals.async_wait([this](const asio::error_code & error, int)
            {
                if (error)
                    return;
                event().paint_to_file(string(m_argv[0]) + ".png");
            });

            return m_event.run();
        }

        return m_event.run();
    }

    Application::~Application()
    {
    }
}
