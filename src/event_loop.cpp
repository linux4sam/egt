/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */

#include "asio.hpp"
#include "mui/app.h"
#include "mui/event_loop.h"
#include "mui/timer.h"
#include "mui/widget.h"
#include "mui/window.h"
#include <chrono>
#include <deque>
#include <numeric>

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
#ifdef STATS2
        auto start = chrono::steady_clock::now();
#endif
        //int ret = m_impl->m_io.run_one();
        int ret = m_impl->m_io.run_for(std::chrono::milliseconds(1));
        //int ret = m_impl->m_io.poll();
        if (!ret)
            invoke_idle_callbacks();

#ifdef STATS2
        auto end = chrono::steady_clock::now();
        auto diff = end - start;

        cout << "wait: " << chrono::duration<double, milli>(diff).count() << endl;
#endif
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
#ifdef STATS2
        auto start = chrono::steady_clock::now();
#endif
        for (auto& w : windows())
        {
            if (!w->visible())
                continue;

            // draw top level frames and plane frames
            if (w->top_level() || w->is_flag_set(widgetmask::PLANE_WINDOW))
                w->draw();
        }
#ifdef STATS2
        auto end = chrono::steady_clock::now();
        auto diff = end - start;

        cout << "draw: " << chrono::duration<double, milli>(diff).count() << endl;
#endif
    }

    int EventLoop::run()
    {
        PeriodicTimer drawtimer(std::chrono::milliseconds(30));
        drawtimer.on_timeout([this]()
        {
            draw();
        });
        drawtimer.start();

#ifdef STATS
        deque<uint64_t> times;
#endif
        do_quit = false;
        while (!do_quit)
        {
#ifdef STATS
            auto start = chrono::steady_clock::now();
#endif

            //draw();

            if (wait())
            {
                //drawtimer.start();
                //draw();
            }

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
