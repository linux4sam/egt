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
#include "egt/imagecache.h"
#include "egt/input.h"
#include "egt/inputevdev.h"
#include "egt/inputlibinput.h"
#include "egt/inputtslib.h"
#include "egt/kmsscreen.h"
#include "egt/painter.h"
#include "egt/utils.h"
#include "egt/version.h"
#include <iostream>
#include <libintl.h>
#include <linux/limits.h>
#include <locale.h>
#include <string>
#include <thread>

#ifdef HAVE_X11
#include "egt/x11screen.h"
#endif

using namespace std;

namespace egt
{
inline namespace v1
{

static Application* the_app = nullptr;
Application& main_app()
{
    assert(the_app);
    return *the_app;
}

Application::Application(int argc, const char** argv, const std::string& name, bool primary)
    : m_argc(argc),
      m_argv(argv),
      m_signals(event().io(), SIGUSR1, SIGUSR2)
{
    INFO("EGT Version " << EGT_VERSION);

    if (the_app)
        throw std::runtime_error("Already an Application instance created.");

    the_app = this;

    set_image_path(detail::exe_pwd() + "/../share/egt/examples/" + name + "/");

    setlocale(LC_ALL, "");
    bindtextdomain(name.c_str(), (detail::exe_pwd() + "/../share/locale/").c_str());
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
    new InputLibInput;
#endif
}

void Application::signal_handler(const asio::error_code& error, int signum)
{
    if (error)
        return;

    if (signum == SIGUSR1)
        dump(cout);
    else if (signum == SIGUSR2)
    {
        if (m_argc)
            paint_to_file(string(m_argv[0]) + ".png");
        else
            paint_to_file();
    }

    m_signals.async_wait(std::bind(&Application::signal_handler, this,
                                   std::placeholders::_1, std::placeholders::_2));
}

int Application::run()
{
    m_signals.async_wait(std::bind(&Application::signal_handler, this,
                                   std::placeholders::_1, std::placeholders::_2));

    return m_event.run();
}

void Application::paint_to_file(const string& filename)
{
    string name = filename;
    if (name.empty())
    {
        name = "screen.png";
    }

    auto surface = shared_cairo_surface_t(
                       cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                               main_screen()->size().w, main_screen()->size().h),
                       cairo_surface_destroy);

    auto cr = shared_cairo_t(cairo_create(surface.get()), cairo_destroy);

    Painter painter(cr);

    for (auto& w : windows())
    {
        if (!w->visible())
            continue;

        // draw top level frames and plane frames
        if (w->top_level() || w->is_flag_set(widgetflag::PLANE_WINDOW))
            w->paint(painter);
    }

    cairo_surface_write_to_png(surface.get(), name.c_str());
}

void Application::dump(std::ostream& out)
{
    for (auto& w : windows())
    {
        // draw top level frames and plane frames
        if (w->top_level())
            w->dump(out);
    }
}

}
}
