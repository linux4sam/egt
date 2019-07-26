/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/app.h"
#include "egt/detail/input/inputtslib.h"
#include <chrono>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <tslib.h>

using namespace std;

namespace egt
{
inline namespace v1
{
namespace detail
{

static const int SLOTS = 1;
static const int SAMPLES = 20;

struct tslibimpl
{
    struct tsdev* ts;
    struct ts_sample_mt** samp_mt;
    std::chrono::time_point<std::chrono::steady_clock> last_down;
};

InputTslib::InputTslib(Application& app, const string& path)
    : m_app(app),
      m_input(app.event().io()),
      m_impl(new detail::tslibimpl)
{
    const int NONBLOCKING = 1;
    m_impl->ts = ts_setup(path.c_str(), NONBLOCKING);

    if (m_impl->ts)
    {
        spdlog::info("added tslib device {}", path);

        m_impl->samp_mt = (struct ts_sample_mt**)malloc(SAMPLES * sizeof(struct ts_sample_mt*));
        assert(m_impl->samp_mt);

        for (int i = 0; i < SAMPLES; i++)
        {
            m_impl->samp_mt[i] = (struct ts_sample_mt*)calloc(SLOTS, sizeof(struct ts_sample_mt));
            if (unlikely(!m_impl->samp_mt[i]))
            {
                free(m_impl->samp_mt);
                ts_close(m_impl->ts);
                assert(0);
            }
        }

        m_input.assign(ts_fd(m_impl->ts));

        asio::async_read(m_input, asio::null_buffers(),
                         std::bind(&InputTslib::handle_read, this, std::placeholders::_1));
    }
    else
    {
        spdlog::error("ts device not found: {}", path);
    }
}

static bool delta(const DisplayPoint& lhs, const DisplayPoint& rhs, int d)
{
    return (std::abs(lhs.x() - rhs.x()) >= d ||
            std::abs(lhs.y() - rhs.y()) >= d);
}

void InputTslib::handle_read(const asio::error_code& error)
{
    if (error)
    {
        spdlog::error("{}", error);
        return;
    }

    struct ts_sample_mt** samp_mt = m_impl->samp_mt;

    int ret = ts_read_mt(m_impl->ts, samp_mt, SLOTS, SAMPLES);
    if (unlikely(ret < 0))
    {
        spdlog::warn("ts_read_mt error");
        return;
    }

    bool move = false;

    for (int j = 0; j < ret; j++)
    {
        for (int i = 0; i < SLOTS; i++)
        {
#ifdef TSLIB_MT_VALID
            if (unlikely(!(samp_mt[j][i].valid & TSLIB_MT_VALID)))
                continue;
#else
            if (unlikely(samp_mt[j][i].valid < 1))
                continue;
#endif

            SPDLOG_TRACE("{}.{}: (slot {}) {} {} {} {} {} {}\n",
                         samp_mt[j][i].tv.tv_sec,
                         samp_mt[j][i].tv.tv_usec,
                         samp_mt[j][i].slot,
                         samp_mt[j][i].tool_type,
                         samp_mt[j][i].x,
                         samp_mt[j][i].y,
                         samp_mt[j][i].pressure,
                         samp_mt[j][i].distance,
                         samp_mt[j][i].pen_down);

            if (unlikely(samp_mt[j][i].x < 0 || samp_mt[j][i].y < 0))
                continue;

            if (m_active)
            {
                if (samp_mt[j][i].pen_down == 0)
                {
                    m_active = false;

                    SPDLOG_TRACE("mouse up {}", m_last_point);

                    m_last_point = DisplayPoint(samp_mt[j][i].x, samp_mt[j][i].y);
                    Event event(eventid::raw_pointer_up, m_last_point);
                    event.pointer().btn = Pointer::button::touch;
                    dispatch(event);
                }
                else
                {
                    DisplayPoint point(samp_mt[j][i].x, samp_mt[j][i].y);
                    if (delta(m_last_point, point, 5))
                    {
                        m_last_point = point;
                        move = true;
                    }
                }
            }
            else
            {
                if (samp_mt[j][i].pen_down == 1)
                {
                    m_last_point = DisplayPoint(samp_mt[j][i].x, samp_mt[j][i].y);

                    std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds> tv
                    {
                        std::chrono::milliseconds{samp_mt[j][i].tv.tv_sec * 1000 + samp_mt[j][i].tv.tv_usec / 1000}
                    };

                    auto DOUBLE_CLICK_DELTA = 300;

                    if (m_impl->last_down.time_since_epoch().count() &&
                        chrono::duration<double, milli>(tv - m_impl->last_down).count() < DOUBLE_CLICK_DELTA)
                    {
                        Event event(eventid::pointer_dblclick, m_last_point);
                        event.pointer().btn = Pointer::button::touch;
                        dispatch(event);
                    }
                    else
                    {
                        m_active = true;

                        SPDLOG_TRACE("mouse down {}", m_last_point);

                        Event event(eventid::raw_pointer_down, m_last_point);
                        event.pointer().btn = Pointer::button::touch;
                        dispatch(event);
                    }

                    m_impl->last_down = tv;
                }
            }
        }
    }

    if (move)
    {
        SPDLOG_TRACE("mouse move {}", m_last_point);

        Event event(eventid::raw_pointer_move, m_last_point);
        event.pointer().btn = Pointer::button::touch;
        dispatch(event);
    }

#ifdef USE_PRIORITY_QUEUE
    asio::async_read(m_input, asio::null_buffers(),
                     m_app.event().queue().wrap(detail::priorities::moderate, std::bind(&InputTslib::handle_read, this, std::placeholders::_1)));
#else
    asio::async_read(m_input, asio::null_buffers(),
                     std::bind(&InputTslib::handle_read, this, std::placeholders::_1));
#endif
}

InputTslib::~InputTslib()
{
    ts_close(m_impl->ts);

    for (int i = 0; i < SAMPLES; i++)
    {
        free(m_impl->samp_mt[i]);
    }

    free(m_impl->samp_mt);
}

}
}
}
