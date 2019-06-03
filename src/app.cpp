/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/app.h"
#include "egt/detail/filesystem.h"
#include "egt/detail/imagecache.h"
#include "egt/detail/screen/kmsscreen.h"
#include "egt/detail/string.h"
#include "egt/eventloop.h"
#include "egt/painter.h"
#include "egt/timer.h"
#include "egt/utils.h"
#include "egt/version.h"
#include "egt/window.h"
#include <clocale>
#include <iostream>
#include <libintl.h>
#include <regex>
#include <string>
#include <thread>

#ifdef HAVE_X11
#include "egt/detail/screen/x11screen.h"
#endif

#ifdef HAVE_FBDEV
#include "egt/detail/screen/framebuffer.h"
#endif

#ifdef HAVE_LIBINPUT
#include "egt/detail/input/inputlibinput.h"
#endif

#ifdef HAVE_TSLIB
#include "egt/detail/input/inputtslib.h"
#endif

#ifdef HAVE_LINUX_INPUT_H
#include "egt/detail/input/inputevdev.h"
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

    // first search install path
    detail::add_search_path(std::string(DATADIR));
    if (!name.empty())
    {
        // examples install path
        detail::add_search_path(std::string(DATADIR) + "/examples/" + name);
        detail::add_search_path(detail::exe_pwd() + "/../share/egt/examples/" + name);
    }
    // then go to exe pwd
    detail::add_search_path(detail::exe_pwd());
    // then go to icons
    detail::add_search_path(std::string(DATADIR) + "/icons");
    detail::add_search_path(detail::exe_pwd() + "/../share/libegt/icons");
    detail::add_search_path(detail::exe_pwd() + "/../../../icons");
    detail::add_search_path(detail::exe_pwd() + "/../../icons");
    detail::add_search_path(detail::exe_pwd() + "/../share/egt");
    detail::add_search_path(std::string(DATADIR) + "/egt");

    setlocale(LC_ALL, "");
    bindtextdomain(name.c_str(), (detail::exe_pwd() + "/../share/locale/").c_str());
    textdomain(name.c_str());

    static string backend;

    const char* value = getenv("EGT_BACKEND");
    if (value)
        backend = value;

    // try to choose a sane default
    if (backend.empty())
    {
#if defined(HAVE_LIBPLANES)
        backend = "kms";
#elif defined(HAVE_X11)
        backend = "x11";
#elif defined(HAVE_FBDEV)
        backend = "fbdev";
#endif
    }

#ifdef HAVE_X11
    if (backend == "x11")
        new detail::X11Screen(Size(800, 480));
    else
#endif
#ifdef HAVE_LIBPLANES
        if (backend == "kms")
            new detail::KMSScreen(primary);
        else
#endif
#ifdef HAVE_FBDEV
            if (backend == "fbdev")
                new detail::FrameBuffer("/dev/fb0");
            else
#endif
                throw std::runtime_error("backend not available");

    // EGT_INPUT_DEVICES=library:event_device1,event_device2;library:event_device3
    const char* tmp = getenv("EGT_INPUT_DEVICES");

    if (tmp)
    {
        string env_val = string(tmp);
        vector<string> inputs;

        detail::tokenize(env_val, ';', inputs);

        for (auto& input : inputs)
        {
            vector<string> tokens;
            detail::tokenize(input, ':', tokens);

            string library = tokens.front();
            string devices = tokens.back();
            vector<string> device_tokens;
            detail::tokenize(devices, ',', device_tokens);

            for (auto& device : device_tokens)
            {
                // deal with symlink
                string target = detail::readlink(device);
                if (!target.empty())
                {
                    // absolute or relative path
                    if (target[0] == '/')
                        device = target;
                    else
                        device = detail::extract_dirname(device) + "/" + target;
                }

                if (detail::exists(device))
                    m_input_devices.push_back(pair<string, string>(library, device));
            }
        }
    }

    for (auto& device : m_input_devices)
    {
        if (device.first == "tslib")
        {
#ifdef HAVE_TSLIB
            new detail::InputTslib(device.second);
#endif
        }
        else if (device.first == "evdev")
        {
#ifdef HAVE_LINUX_INPUT_H
            new detail::InputEvDev(device.second);
#endif
        }
    }

#ifdef HAVE_LIBINPUT
    new detail::InputLibInput;
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

    Input::global_input().on_event([this](Event & event)
    {
        if (event.key().code == EKEY_SNAPSHOT)
        {
            if (m_argc)
                paint_to_file(string(m_argv[0]) + ".png");
            else
                paint_to_file();
        }

        return 0;
    }, {eventid::keyboard_down});

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
        if (w->top_level() || w->flags().is_set(Widget::flag::plane_window))
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

    dump_timers(out);
}

vector<pair<string, string>> Application::get_input_devices()
{
    return m_input_devices;
}

}
}
