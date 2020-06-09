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
#include "egt/detail/screen/kmsscreen.h"
#include "egt/detail/string.h"
#include "egt/eventloop.h"
#include "egt/input.h"
#include "egt/painter.h"
#include "egt/respath.h"
#include "egt/serialize.h"
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

namespace egt
{
inline namespace v1
{

static Application* the_app{nullptr};

Application& Application::instance()
{
    if (!the_app)
        throw std::runtime_error("no application instance");
    return *the_app;
}

bool Application::check_instance()
{
    return !!the_app;
}

Application::Application(int argc, char** argv,
                         const std::string& name, bool primary)
    : m_argc(argc),
      m_argv(argv),
      m_signals(event().io(), SIGUSR1, SIGUSR2)
{
    setup_logging();

    setup_info();

    if (the_app)
    {
        spdlog::warn("more than one application instance created");
    }
    else
    {
        the_app = this;
    }

    setup_search_paths();

    setup_locale(name);

    setup_backend(primary);

    setup_inputs();

    setup_events();
}

void Application::setup_events()
{
    m_signals.async_wait(std::bind(&Application::signal_handler, this,
                                   std::placeholders::_1, std::placeholders::_2));

    m_handle = Input::global_input().on_event([this](Event & event)
    {
        if (event.key().keycode == EKEY_SNAPSHOT)
        {
            if (m_argc)
                paint_to_file(fmt::format("{}.png", m_argv[0]));
            else
                paint_to_file();
        }
    }, {EventId::keyboard_down});
}

void Application::setup_info()
{
    spdlog::info("EGT Version {}", egt_version());

#ifdef HAVE_SIMD
    auto info = SimdCpuInfo();
    spdlog::debug("SSE: {}", ((info & (1 << SimdCpuInfoSse)) ? "Yes" : "No"));
    spdlog::debug("SSE2: {}", ((info & (1 << SimdCpuInfoSse2)) ? "Yes" : "No"));
    spdlog::debug("SSE3: {}", ((info & (1 << SimdCpuInfoSse3)) ? "Yes" : "No"));
    spdlog::debug("SSSE3: {}", ((info & (1 << SimdCpuInfoSsse3)) ? "Yes" : "No"));
    spdlog::debug("SSE4.1: {}", ((info & (1 << SimdCpuInfoSse41)) ? "Yes" : "No"));
    spdlog::debug("SSE4.2: {}", ((info & (1 << SimdCpuInfoSse42)) ? "Yes" : "No"));
    spdlog::debug("AVX: {}", ((info & (1 << SimdCpuInfoAvx)) ? "Yes" : "No"));
    spdlog::debug("AVX2: {}", ((info & (1 << SimdCpuInfoAvx2)) ? "Yes" : "No"));
    spdlog::debug("AVX-512F: {}", ((info & (1 << SimdCpuInfoAvx512f)) ? "Yes" : "No"));
    spdlog::debug("AVX-512BW: {}", ((info & (1 << SimdCpuInfoAvx512bw)) ? "Yes" : "No"));
    spdlog::debug("PowerPC-Altivec: {}", ((info & (1 << SimdCpuInfoVmx)) ? "Yes" : "No"));
    spdlog::debug("PowerPC-VSX: {}", ((info & (1 << SimdCpuInfoVsx)) ? "Yes" : "No"));
    spdlog::debug("ARM-NEON: {}", ((info & (1 << SimdCpuInfoNeon)) ? "Yes" : "No"));
    spdlog::debug("MIPS-MSA: {}", ((info & (1 << SimdCpuInfoMsa)) ? "Yes" : "No"));
#endif
}

void Application::setup_locale(const std::string& name)
{
    if (!name.empty())
    {
        setlocale(LC_ALL, "");
        bindtextdomain(name.c_str(), (std::string(DATADIR) + "/locale/").c_str());
        textdomain(name.c_str());
    }
}

void Application::setup_logging()
{
    auto level = getenv("EGT_DEBUG");
    if (level)
    {
        auto loglevel = static_cast<spdlog::level::level_enum>(
                            std::stoi(level));
        spdlog::set_level(loglevel);
    }
    else
    {
        spdlog::set_level(spdlog::level::level_enum::warn);
    }

    spdlog::set_pattern("%E.%e [%^%l%$] %@ %v");
}

void Application::setup_search_paths()
{
    // any added search paths take priority
    if (getenv("EGT_SEARCH_PATH"))
    {
        std::vector<std::string> tokens;
        detail::tokenize(getenv("EGT_SEARCH_PATH"), ':', tokens);

        for (auto& token : tokens)
            add_search_path(token);
    }

    // search cwd
    add_search_path(detail::cwd());

    // search exe directory
    add_search_path(detail::exe_pwd());
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Application::setup_backend(bool primary)
{
    detail::ignoreparam(primary);

    std::string backend;

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
    {
        Size size(800, 480);
        const char* sizestr = getenv("EGT_SCREEN_SIZE");
        if (sizestr)
        {
            std::vector<std::string> dims;
            detail::tokenize(sizestr, 'x', dims);
            if (dims.size() == 2)
            {
                size.width(std::stoi(dims[0]));
                size.height(std::stoi(dims[1]));
            }
            else
            {
                spdlog::warn("invalid EGT_SCREEN_SIZE: {}", sizestr);
            }
        }

        m_screen = std::make_unique<detail::X11Screen>(*this, size);
    }
    else
#endif
#ifdef HAVE_LIBPLANES
        if (backend == "kms")
            m_screen = std::make_unique<detail::KMSScreen>(primary);
        else
#endif
#ifdef HAVE_FBDEV
            if (backend == "fbdev")
                m_screen = std::make_unique<detail::FrameBuffer>("/dev/fb0");
            else
#endif
                spdlog::info("no screen backend");

}

void Application::setup_inputs()
{
    m_input_devices.clear();
    m_inputs.clear();

    // EGT_INPUT_DEVICES=library:event_device1,event_device2;library:event_device3
    const char* tmp = getenv("EGT_INPUT_DEVICES");

    if (tmp)
    {
        std::string env_val = tmp;
        std::vector<std::string> inputs;

        detail::tokenize(env_val, ';', inputs);

        for (auto& input : inputs)
        {
            std::vector<std::string> tokens;
            detail::tokenize(input, ':', tokens);

            if (tokens.size() != 2)
            {
                spdlog::warn("invalid EGT_INPUT_DEVICES string: {}", input);
                continue;
            }

            auto library = tokens.front();
            auto devices = tokens.back();
            std::vector<std::string> device_tokens;
            detail::tokenize(devices, ',', device_tokens);

            for (auto& device : device_tokens)
            {
                // deal with symlink
                auto target = detail::readlink(device);
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
                    m_input_devices.emplace_back(std::make_pair(library, device));
            }
        }
    }

    for (auto& device : m_input_devices)
    {
        if (device.first == "tslib")
        {
#ifdef HAVE_TSLIB
            m_inputs.push_back(std::make_unique<detail::InputTslib>(*this, device.second));
#else
            spdlog::warn("tslib requested but no support compiled in");
#endif
        }
        else if (device.first == "evdev")
        {
#ifdef HAVE_LINUX_INPUT_H
            m_inputs.push_back(std::make_unique<detail::InputEvDev>(*this, device.second));
#else
            spdlog::warn("evdev requested but no support compiled in");
#endif
        }
        else if (device.first == "libinput")
        {
#ifndef HAVE_LIBINPUT
            spdlog::warn("libinput requested but no support compiled in");
#endif
        }
        else
        {
            spdlog::warn("unknown input type requested: {}", device.first);
        }
    }

#ifdef HAVE_LIBINPUT
    m_inputs.push_back(std::make_unique<detail::InputLibInput>(*this));
#endif
}

void Application::signal_handler(const asio::error_code& error, int signum)
{
    if (error)
        return;

    if (signum == SIGUSR1)
        dump(std::cout);
    else if (signum == SIGUSR2)
    {
        if (m_argc)
            paint_to_file(std::string(m_argv[0]) + ".png");
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

void Application::quit()
{
    m_event.quit();
}

void Application::paint_to_file(const std::string& filename)
{
    auto name = filename;
    if (name.empty())
    {
        name = "screen.png";
    }

    auto surface = shared_cairo_surface_t(
                       cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                               screen()->size().width(), screen()->size().height()),
                       cairo_surface_destroy);

    auto cr = shared_cairo_t(cairo_create(surface.get()), cairo_destroy);

    Painter painter(cr);

    for (auto& w : windows())
    {
        if (!w->visible())
            continue;

        // draw top level frames and plane frames
        if (w->top_level() || w->plane_window())
            w->paint(painter);
    }

    cairo_surface_write_to_png(surface.get(), name.c_str());
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Application::dump(std::ostream& out) const
{
    OstreamWidgetSerializer s;

    for (const auto& w : windows())
    {
        // draw top level frames and plane frames
        if (w->top_level())
            s.add(w);
    }

    s.write(out);

    dump_timers(out);
}

void Application::dump_timers(std::ostream& out) const
{
    for (const auto& timer : m_timers)
    {
        out << fmt::format("Timer name({}) duration({} ms) status({})\n",
                           timer->name(),
                           timer->duration().count(),
                           (timer->running() ? "running" : "idle"));
    }
}

const std::vector<std::pair<std::string, std::string>>& Application::get_input_devices()
{
    return m_input_devices;
}

Application::~Application() noexcept
{
    Input::global_input().remove_handler(m_handle);

    if (the_app == this)
        the_app = nullptr;
}

}
}
