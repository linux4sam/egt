/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "detail/egtlog.h"
#include "egt/app.h"
#include "egt/detail/filesystem.h"
#include "egt/detail/screen/composerscreen.h"
#include "egt/detail/screen/kmsscreen.h"
#include "egt/detail/screen/memoryscreen.h"
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
#include <csignal>
#include <iostream>
#ifdef HAVE_LIBINTL
#include <libintl.h>
#endif
#include <regex>
#include <string>
#include <thread>

#ifdef HAVE_X11
#include "egt/detail/screen/x11screen.h"
#endif

#ifdef HAVE_SDL2
#include "egt/detail/screen/sdlscreen.h"
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

#ifndef SIGUSR1
#define SIGUSR1 SIGINT
#endif

#ifndef SIGUSR2
#define SIGUSR2 SIGTERM
#endif

Application::Application(int argc, char** argv,
                         const std::string& name, bool primary)
    : m_event(*this),
      m_argc(argc),
      m_argv(argv),
      m_signals(event().io(), SIGUSR1, SIGUSR2)
{
    setup_logging();

    setup_info();

    if (the_app)
    {
        detail::warn("more than one application instance created");
    }
    else
    {
        the_app = this;
    }

    setup_search_paths();

    setup_locale(name);

    setup_backend(primary, name);

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
    detail::info("EGT Version {}", egt_version());
    detail::info("EGT Git Version {}", egt_git_version());

#ifdef HAVE_SIMD
    auto info = SimdCpuInfo();
    detail::debug("SSE: {}", ((info & (1 << SimdCpuInfoSse)) ? "Yes" : "No"));
    detail::debug("SSE2: {}", ((info & (1 << SimdCpuInfoSse2)) ? "Yes" : "No"));
    detail::debug("SSE3: {}", ((info & (1 << SimdCpuInfoSse3)) ? "Yes" : "No"));
    detail::debug("SSSE3: {}", ((info & (1 << SimdCpuInfoSsse3)) ? "Yes" : "No"));
    detail::debug("SSE4.1: {}", ((info & (1 << SimdCpuInfoSse41)) ? "Yes" : "No"));
    detail::debug("SSE4.2: {}", ((info & (1 << SimdCpuInfoSse42)) ? "Yes" : "No"));
    detail::debug("AVX: {}", ((info & (1 << SimdCpuInfoAvx)) ? "Yes" : "No"));
    detail::debug("AVX2: {}", ((info & (1 << SimdCpuInfoAvx2)) ? "Yes" : "No"));
    detail::debug("AVX-512F: {}", ((info & (1 << SimdCpuInfoAvx512f)) ? "Yes" : "No"));
    detail::debug("AVX-512BW: {}", ((info & (1 << SimdCpuInfoAvx512bw)) ? "Yes" : "No"));
    detail::debug("PowerPC-Altivec: {}", ((info & (1 << SimdCpuInfoVmx)) ? "Yes" : "No"));
    detail::debug("PowerPC-VSX: {}", ((info & (1 << SimdCpuInfoVsx)) ? "Yes" : "No"));
    detail::debug("ARM-NEON: {}", ((info & (1 << SimdCpuInfoNeon)) ? "Yes" : "No"));
    detail::debug("MIPS-MSA: {}", ((info & (1 << SimdCpuInfoMsa)) ? "Yes" : "No"));
#endif
}

void Application::setup_locale(const std::string& name)
{
    if (!name.empty())
    {
        setlocale(LC_ALL, "");
#ifdef HAVE_LIBINTL
        bindtextdomain(name.c_str(), (std::string(DATAPATH) + "/locale/").c_str());
        textdomain(name.c_str());
#endif
    }
}

void Application::setup_logging()
{
    auto level = getenv("EGT_DEBUG");
    if (level && strlen(level))
    {
        auto loglevel = std::stoi(level);
        detail::loglevel(loglevel);
    }
}

void Application::setup_search_paths(const std::vector<std::string>& extra_paths)
{
    // clear the current search paths
    clear_search_paths();

    // any added search paths take priority
    auto path = getenv("EGT_SEARCH_PATH");
    if (path && strlen(path))
    {
        std::vector<std::string> tokens;
        detail::tokenize(path, detail::path_separator(), tokens);

        for (auto& token : tokens)
            add_search_path(token);
    }

    // add extra search paths
    for (auto& path : extra_paths)
        add_search_path(path);

    // search cwd
    add_search_path(detail::cwd());

    // search exe directory
    add_search_path(detail::exe_pwd());
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Application::setup_backend(bool primary, const std::string& name)
{
    detail::ignoreparam(primary);
    std::string backend;

    auto value = getenv("EGT_BACKEND");
    if (value && strlen(value))
        backend = value;

    Size size(800, 480);
    auto sizestr = getenv("EGT_SCREEN_SIZE");
    if (sizestr && strlen(sizestr))
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
            detail::warn("invalid EGT_SCREEN_SIZE: {}", sizestr);
        }
    }

    // backends listed in order of automatic priority
    const std::pair<const char*, std::function<std::unique_ptr<egt::Screen>()>> backends[] =
    {
#ifdef HAVE_LIBPLANES
        {"kms", [&primary]() { return std::make_unique<detail::KMSScreen>(primary); }},
#endif
#ifdef HAVE_X11
        {"x11", [this, &size, &name]() { return std::make_unique<detail::X11Screen>(*this, size, name); }},
#endif
#ifdef HAVE_SDL2
        {"sdl2", [this, &size, &name]() { return std::make_unique<detail::SDLScreen>(*this, size, name); }},
#endif
        {"memory", [&size]() { return std::make_unique<detail::MemoryScreen>(size); }},
        {"composer", [&size]() { return std::make_unique<detail::ComposerScreen>(size); }},
    };

    if (backend != "none")
    {
        for (auto& b : backends)
        {
            if (backend.empty() || b.first == backend)
            {
                m_screen = b.second();
                return;
            }
        }
    }

    detail::info("no screen backend");
}

void Application::setup_inputs()
{
    m_input_devices.clear();
    m_inputs.clear();

    // EGT_INPUT_DEVICES=library:event_device1,event_device2;library:event_device3
    auto tmp = getenv("EGT_INPUT_DEVICES");

    if (tmp && strlen(tmp))
    {
        std::string env_val = tmp;
        std::vector<std::string> inputs;

        detail::tokenize(env_val, ';', inputs);

        for (auto& input : inputs)
        {
            std::vector<std::string> tokens;
            detail::tokenize(input, detail::path_separator(), tokens);

            if (tokens.size() != 2)
            {
                detail::warn("invalid EGT_INPUT_DEVICES string: {}", input);
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
            detail::warn("tslib requested but no support compiled in");
#endif
        }
        else if (device.first == "evdev")
        {
#ifdef HAVE_LINUX_INPUT_H
            m_inputs.push_back(std::make_unique<detail::InputEvDev>(*this, device.second));
#else
            detail::warn("evdev requested but no support compiled in");
#endif
        }
        else if (device.first == "libinput")
        {
#ifndef HAVE_LIBINPUT
            detail::warn("libinput requested but no support compiled in");
#endif
        }
        else
        {
            detail::warn("unknown input type requested: {}", device.first);
        }
    }

#ifdef HAVE_LIBINPUT
    if (m_inputs.empty())
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

bool Application::is_composer() const
{
    return m_screen->is_composer();
}

int Application::run()
{
    return m_event.run();
}

void Application::quit(int exit_value)
{
    m_event.quit(exit_value);
}

void Application::paint_to_file(const std::string& filename)
{
#if CAIRO_HAS_PNG_FUNCTIONS == 1
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
#else
    detail::ignoreparam(filename);
    detail::error("png support not available");
#endif
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Application::dump(std::ostream& out) const
{
    OstreamWidgetSerializer s;
    XmlWidgetSerializer s_xml;

    for (const auto& w : windows())
    {
        // draw top level frames and plane frames
        if (w->top_level())
        {
            s.add(w);
            s_xml.add(w);
        }
    }

    s.write(out);
    s_xml.write("ui.xml");

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
