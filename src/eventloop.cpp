/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "egt/app.h"
#include "egt/eventloop.h"
#include "egt/painter.h"
#include "egt/timer.h"
#include "egt/widget.h"
#include "egt/window.h"
#include <asio.hpp>
#include <chrono>
#include <deque>
#include <numeric>

using namespace std;

namespace egt
{
    namespace detail
    {
        struct eventloopimpl
        {
            eventloopimpl()
                : m_work(m_io)
            {}

            asio::io_context m_io;
            asio::io_context::work m_work;
        };
    }

    EventLoop::EventLoop() noexcept
        : m_impl(new detail::eventloopimpl)
    {}

    asio::io_context& EventLoop::io()
    {
        return m_impl->m_io;
    }

    int EventLoop::wait()
    {
        int ret;

        experimental::code_timer(false, "wait: ", [&]()
        {
            //ret = m_impl->m_io.run_one();
            ret = m_impl->m_io.run_one_for(std::chrono::milliseconds(30));
            if (ret)
                ret += m_impl->m_io.run_for(std::chrono::milliseconds(10));
            //ret = m_impl->m_io.run_for(std::chrono::milliseconds(10));
            //ret = m_impl->m_io.poll();
        });

        if (!ret)
            invoke_idle_callbacks();

        return ret;
    }

    static bool do_quit = false;

    void EventLoop::quit()
    {
        do_quit = true;
        m_impl->m_io.stop();
    }

    void EventLoop::draw()
    {
        experimental::code_timer(false, "draw: ", [&]()
        {
            for (auto& w : windows())
            {
                if (!w->visible())
                    continue;

                // draw top level frames and plane frames
                if (w->top_level() || w->is_flag_set(widgetmask::PLANE_WINDOW))
                    w->top_draw();
            }
        });

    }

    int EventLoop::run(bool enable_fps)
    {
        experimental::Fps fps;

        draw();

        do_quit = false;
        while (!do_quit)
        {
            if (wait())
            {
                draw();

                if (enable_fps)
                    fps.end_frame();

                if (enable_fps && fps.ready())
                {
                    cout << "fps: " << std::round(fps.fps()) << endl;
                }
            }
        }

        return 0;
    }

    void EventLoop::add_idle_callback(event_callback func)
    {
        m_idle.push_back(func);
    }

    void EventLoop::invoke_idle_callbacks()
    {
        for (auto& i : m_idle)
        {
            i();
        }
    }

    EventLoop::~EventLoop()
    {}

}
