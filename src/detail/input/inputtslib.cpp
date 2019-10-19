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

static const int CHANNELS = 2;
static const int SAMPLE_COUNT = 20;

struct tslibimpl
{
    struct tsdev* ts;
    struct ts_sample_mt** samp_mt;
    std::array<std::chrono::time_point<std::chrono::steady_clock>, 2> last_down;
};

InputTslib::InputTslib(Application& app, const string& path)
    : m_input(app.event().io()),
      m_impl(new detail::tslibimpl)
{
    const int NONBLOCKING = 1;
    m_impl->ts = ts_setup(path.c_str(), NONBLOCKING);

    if (m_impl->ts)
    {
        spdlog::info("added tslib device {}", path);

        m_impl->samp_mt = (struct ts_sample_mt**)malloc(SAMPLE_COUNT * sizeof(struct ts_sample_mt*));
        assert(m_impl->samp_mt);

        for (int i = 0; i < SAMPLE_COUNT; i++)
        {
            m_impl->samp_mt[i] = (struct ts_sample_mt*)calloc(CHANNELS, sizeof(struct ts_sample_mt));
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

    int ret = ts_read_mt(m_impl->ts, samp_mt, CHANNELS, SAMPLE_COUNT);
    if (unlikely(ret < 0))
    {
        spdlog::warn("ts_read_mt error");
        return;
    }

    bool move = false;

    for (int j = 0; j < ret; j++)
    {
        for (int i = 0; i < CHANNELS; i++)
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

            const auto x = samp_mt[j][i].x;
            const auto y = samp_mt[j][i].y;
            const auto slot = samp_mt[j][i].slot;
            const auto pen_down = samp_mt[j][i].pen_down;

            if (unlikely(x < 0 || y < 0))
                continue;

            if (m_active)
            {
                if (pen_down == 0)
                {
                    m_active = false;

                    SPDLOG_TRACE("mouse up {}", m_last_point);

                    m_last_point[slot] = DisplayPoint(x, y);
                    Event event(eventid::raw_pointer_up, Pointer(m_last_point[slot],
                                Pointer::button::left));
                    dispatch(event);
                }
                else
                {
                    DisplayPoint point(x, y);
                    if (delta(m_last_point[slot], point, 5))
                    {
                        m_last_point[slot] = point;
                        move = true;
                    }
                }
            }
            else
            {
                if (pen_down == 1)
                {
                    m_last_point[slot] = DisplayPoint(x, y);

                    std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds> tv
                    {
                        std::chrono::milliseconds{samp_mt[j][i].tv.tv_sec * 1000 + samp_mt[j][i].tv.tv_usec / 1000}
                    };

                    auto DOUBLE_CLICK_DELTA = 300;

                    if (m_impl->last_down[slot].time_since_epoch().count() &&
                        chrono::duration<double, milli>(tv - m_impl->last_down).count() < DOUBLE_CLICK_DELTA)
                    {
                        Event event(eventid::pointer_dblclick, Pointer(m_last_point, Pointer::button::left));
                        dispatch(event);
                    }
                    else
                    {
                        m_active = true;

                        SPDLOG_TRACE("mouse down {}", m_last_point[slot]);

                        Event event(eventid::raw_pointer_down, Pointer(m_last_point[slot],
                                    Pointer::button::left));
                        dispatch(event);
                    }

                    m_impl->last_down[slot] = tv;
                }
            }
        }
    }

    if (move)
    {
        SPDLOG_TRACE("mouse move {}", m_last_point[slot]);

        Event event(eventid::raw_pointer_move, Pointer(m_last_point,
                    Pointer::button::left));
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

    for (int i = 0; i < SAMPLE_COUNT; i++)
    {
        free(m_impl->samp_mt[i]);
    }

    free(m_impl->samp_mt);
}

}
}
}
