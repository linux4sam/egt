/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/dump.h"
#include "detail/priorityqueue.h"
#include "egt/app.h"
#include "egt/eventloop.h"
#include "egt/tools.h"
#include "egt/widget.h"
#include "egt/window.h"
#include <cstdlib>
#include <egt/asio.hpp>
#include <numeric>
#include <spdlog/spdlog.h>

namespace egt
{
inline namespace v1
{

struct EventLoop::EventLoopImpl
{
    asio::io_context m_io;
    asio::executor_work_guard<asio::io_context::executor_type> m_work{egt::asio::make_work_guard(m_io)};
    detail::PriorityQueue m_queue;
};

EventLoop::EventLoop() noexcept
    : m_impl(new EventLoopImpl)
{}

asio::io_context& EventLoop::io()
{
    return m_impl->m_io;
}

static inline bool time_event_loop_enabled()
{
    static int value = 0;
    if (value == 0)
    {
        if (std::getenv("EGT_TIME_EVENTLOOP"))
            value += 1;
        else
            value -= 1;
    }
    return value == 1;
}

// maximum number of handlers when in a tight poll loop
static const auto MAX_POLL_COUNT = 10;

int EventLoop::wait()
{
    int ret = 0;

    detail::code_timer(time_event_loop_enabled(), "wait: ", [this, &ret]()
    {
        ret = m_impl->m_io.run_one_for(std::chrono::milliseconds(100));
        if (ret)
        {
            // hmm, libinput async_read will always return something on poll_one()
            // until we have satisfied the handler, so we have to give up at
            // some point
            int count = MAX_POLL_COUNT;
            while (m_impl->m_io.poll_one() && count--)
            {}

#ifdef USE_PRIORITY_QUEUE
            m_impl->m_queue.execute_all();
#endif
        }
    });

    if (!ret)
    {
        if (!m_idle.empty())
        {
            invoke_idle_callbacks();
            // fake out that we did something in the idle callback
            ret = 1;
        }
    }

    return ret;
}

void EventLoop::quit()
{
    m_do_quit = true;
    m_impl->m_io.stop();
}

void EventLoop::draw()
{
    detail::code_timer(time_event_loop_enabled(), "draw: ", []()
    {
        for (auto& w : windows())
        {
            if (!w->visible())
                continue;

            // draw top level frames and plane frames
            if (w->top_level() || w->flags().is_set(Widget::Flag::plane_window))
                w->begin_draw();
        }
    });
}

int EventLoop::poll()
{
    int ret = 0;
    int count = MAX_POLL_COUNT;
    while (m_impl->m_io.poll_one() && count--)
    {
        ret++;
    }
    return ret;
}

int EventLoop::step()
{
    auto ret = poll();
    if (ret)
        draw();

    return ret;
}

static inline bool show_fps_enabled()
{
    static int value = 0;
    if (value == 0)
    {
        if (std::getenv("EGT_SHOW_FPS"))
            value += 1;
        else
            value -= 1;
    }
    return value == 1;
}

int EventLoop::run()
{
    experimental::FramesPerSecond fps;

    // initial draw
    draw();

    m_do_quit = false;
    m_impl->m_io.restart();
    while (!m_do_quit)
    {
        // process events
        if (wait())
        {
            // draw anything that's changed
            draw();

            if (show_fps_enabled())
            {
                fps.end_frame();

                if (fps.ready())
                    fmt::print("fps: {}\n", std::round(fps.fps()));
            }
        }
    }

    SPDLOG_TRACE("EventLoop::run() exiting");

    return 0;
}

void EventLoop::add_idle_callback(IdleCallback func)
{
    m_idle.emplace_back(std::move(func));
}

void EventLoop::invoke_idle_callbacks()
{
    for (auto& i : m_idle)
        i();
}

detail::PriorityQueue& EventLoop::queue()
{
    return m_impl->m_queue;
}

EventLoop::~EventLoop() = default;

}
}
