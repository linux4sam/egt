/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */

#include "mui/app.h"
#include "mui/event_loop.h"
#include "mui/widget.h"
#include "mui/window.h"
#include <chrono>
#include <deque>
#include <numeric>
#include "asio.hpp"

using namespace std;

namespace mui
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
        return m_impl->m_io.run_one();
    }

    static bool do_quit = false;

    void EventLoop::quit()
    {
        do_quit = true;
        m_impl->m_io.stop();
    }

    int EventLoop::run()
    {
#ifdef STATS
        deque<uint64_t> times;
#endif
        do_quit = false;
        while (!do_quit)
        {
#ifdef STATS
            auto start = chrono::steady_clock::now();
#endif

            for (auto& w : windows())
            {
                if (!w->visible())
                    continue;

                // draw top level frames and plane frames
                if (w->top_level() || w->is_flag_set(FLAG_PLANE_WINDOW))
                    w->draw();
            }

            wait();

#ifdef STATS
            auto end = chrono::steady_clock::now();
            auto diff = end - start;
            times.push_back(chrono::duration<double, milli>(diff).count());

            static unsigned int COUNT = 50;
            if (times.size() == COUNT)
            {
                auto avg = std::accumulate(times.begin(), times.end(), 0) / COUNT;
                if (avg > 0)
                    cout << 1000. / avg << " fps" << endl;
                times.clear();
            }
#endif
        }

        return 0;
    }

    void EventLoop::close()
    {

    }

    static std::vector<event_callback> idle;

    void EventLoop::add_idle_callback(event_callback func)
    {
        idle.push_back(func);
    }

    EventLoop::~EventLoop()
    {}

}
