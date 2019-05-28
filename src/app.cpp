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
#include "egt/input.h"
#include "egt/painter.h"
#include "egt/timer.h"
#include "egt/utils.h"
#include "egt/version.h"
#include "egt/window.h"
#include <clocale>
#include <iostream>
#include <libintl.h>
#include <regex>
#include <spdlog/spdlog.h>
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

#ifdef HAVE_SIMD
#include "Simd/SimdLib.hpp"
#endif

using namespace std;

namespace egt
{
inline namespace v1
{

static Application* the_app = nullptr;
Application& Application::instance()
{
    if (!the_app)
        throw std::runtime_error("no application instance");
    return *the_app;
}

static std::once_flag env_flag;

Application::Application(int argc, const char** argv, const std::string& name, bool primary)
    : m_argc(argc),
      m_argv(argv),
      m_signals(event().io(), SIGUSR1, SIGUSR2)
{
    std::call_once(env_flag, []()
    {
        if (getenv("EGT_DEBUG"))
        {
            auto loglevel = static_cast<spdlog::level::level_enum>(
                                std::atoi(getenv("EGT_DEBUG")));
            spdlog::set_level(loglevel);
        }
        else
        {
            spdlog::set_level(spdlog::level::level_enum::warn);
        }

        spdlog::set_pattern("%E.%e [%^%l%$] %@ %v");
    });

    spdlog::info("EGT Version {}", EGT_VERSION);

#ifdef HAVE_SIMD
    int info = SimdCpuInfo();
    spdlog::debug("SSE: {}", (info & (1 << SimdCpuInfoSse) ? "Yes" : "No"));
    spdlog::debug("SSE2: {}", (info & (1 << SimdCpuInfoSse2) ? "Yes" : "No"));
    spdlog::debug("SSE3: {}", (info & (1 << SimdCpuInfoSse3) ? "Yes" : "No"));
    spdlog::debug("SSSE3: {}", (info & (1 << SimdCpuInfoSsse3) ? "Yes" : "No"));
    spdlog::debug("SSE4.1: {}", (info & (1 << SimdCpuInfoSse41) ? "Yes" : "No"));
    spdlog::debug("SSE4.2: {}", (info & (1 << SimdCpuInfoSse42) ? "Yes" : "No"));
    spdlog::debug("AVX: {}", (info & (1 << SimdCpuInfoAvx) ? "Yes" : "No"));
    spdlog::debug("AVX2: {}", (info & (1 << SimdCpuInfoAvx2) ? "Yes" : "No"));
    spdlog::debug("AVX-512F: {}", (info & (1 << SimdCpuInfoAvx512f) ? "Yes" : "No"));
    spdlog::debug("AVX-512BW: {}", (info & (1 << SimdCpuInfoAvx512bw) ? "Yes" : "No"));
    spdlog::debug("PowerPC-Altivec: {}", (info & (1 << SimdCpuInfoVmx) ? "Yes" : "No"));
    spdlog::debug("PowerPC-VSX: {}", (info & (1 << SimdCpuInfoVsx) ? "Yes" : "No"));
    spdlog::debug("ARM-NEON: {}", (info & (1 << SimdCpuInfoNeon) ? "Yes" : "No"));
    spdlog::debug("MIPS-MSA: {}", (info & (1 << SimdCpuInfoMsa) ? "Yes" : "No"));
#endif

    if (the_app)
    {
        spdlog::warn("more than one application instance created");
    }
    else
    {
        the_app = this;
    }

    // any added search paths take priority
    if (getenv("EGT_SEARCH_PATH"))
    {
        vector<string> tokens;
        detail::tokenize(getenv("EGT_SEARCH_PATH"), ':', tokens);

        for (auto& token : tokens)
            detail::add_search_path(token);
    }

    // search exe pwd
    detail::add_search_path(detail::exe_pwd());

    // libegt icons
    detail::add_search_path(std::string(DATADIR) + "/libegt/icons");

    if (!name.empty())
    {
        // special handling for installed examples
        detail::add_search_path(std::string(DATADIR) + "/egt/examples/" + name);
    }

    // special handling for running example in the source directory
    detail::add_search_path(detail::exe_pwd() + "/../../../icons");
    detail::add_search_path(detail::exe_pwd() + "/../../icons");

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

    Size size(800, 480);
    const char* sizestr = getenv("EGT_SCREEN_SIZE");
    if (sizestr)
    {
        vector<string> dims;
        detail::tokenize(sizestr, 'x', dims);
        if (dims.size() == 2)
        {
            size.set_width(std::stoi(dims[0]));
            size.set_height(std::stoi(dims[1]));
        }
    }

#ifdef HAVE_X11
    if (backend == "x11")
        new detail::X11Screen(*this, size);
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
                    {
                        device = target;
                    }
                    else
                    {
                        device = detail::extract_dirname(device);
                        device += "/";
                        device += target;
                    }
                }

                if (detail::exists(device))
                    m_input_devices.emplace_back(pair<string, string>(library, device));
            }
        }
    }

    for (auto& device : m_input_devices)
    {
        if (device.first == "tslib")
        {
#ifdef HAVE_TSLIB
            new detail::InputTslib(*this, device.second);
#endif
        }
        else if (device.first == "evdev")
        {
#ifdef HAVE_LINUX_INPUT_H
            new detail::InputEvDev(*this, device.second);
#endif
        }
    }

#ifdef HAVE_LIBINPUT
    new detail::InputLibInput(*this);
#endif

    m_signals.async_wait(std::bind(&Application::signal_handler, this,
                                   std::placeholders::_1, std::placeholders::_2));

    Input::global_input().on_event([this](Event & event)
    {
        if (event.key().keycode == EKEY_SNAPSHOT)
        {
            if (m_argc)
                paint_to_file(string(m_argv[0]) + ".png");
            else
                paint_to_file();
        }

        return 0;
    }, {eventid::keyboard_down});
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
                               main_screen()->size().width(), main_screen()->size().height()),
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

Application::~Application()
{
    if (the_app == this)
        the_app = nullptr;
}

}
}
