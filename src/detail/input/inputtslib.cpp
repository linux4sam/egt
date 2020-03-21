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

namespace egt
{
inline namespace v1
{
namespace detail
{

static constexpr auto CHANNELS = 2;
static constexpr auto SAMPLE_COUNT = 20;

struct tslibimpl
{
    struct tsdev* ts {nullptr};
    struct ts_sample_mt* samp_mt[SAMPLE_COUNT] {};
    std::array<std::chrono::time_point<std::chrono::steady_clock>, 2> last_down;
};

InputTslib::InputTslib(Application& app, const std::string& path)
    : m_input(app.event().io()),
      m_impl(new detail::tslibimpl)
{
    constexpr int NONBLOCKING = 1;
    m_impl->ts = ts_setup(path.c_str(), NONBLOCKING);

    if (m_impl->ts)
    {
        spdlog::info("added tslib device {}", path);

        // NOLINTNEXTLINE(modernize-loop-convert)
        for (auto i = 0; i < SAMPLE_COUNT; i++)
        {
            // NOLINTNEXTLINE (cppcoreguidelines-owning-memory)
            m_impl->samp_mt[i] = new ts_sample_mt[CHANNELS]();
        }

        m_input.assign(ts_fd(m_impl->ts));

        asio::async_read(m_input, asio::null_buffers(),
                         std::bind(&InputTslib::handle_read, this, std::placeholders::_1));
    }
    else
    {
        throw std::runtime_error(fmt::format("could not open tslib device: {}", path));
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

    std::array<bool, 2> move{};

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

            if (m_active[slot])
            {
                if (pen_down == 0)
                {
                    m_active[slot] = false;

                    SPDLOG_TRACE("mouse up {}", m_last_point[slot]);

                    m_last_point[slot] = DisplayPoint(x, y);
                    Event event(EventId::raw_pointer_up, Pointer(m_last_point[slot],
                                Pointer::Button::left));
                    dispatch(event);
                }
                else
                {
                    DisplayPoint point(x, y);
                    if (delta(m_last_point[slot], point, 5))
                    {
                        m_last_point[slot] = point;
                        move[slot] = true;
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

                    constexpr auto DOUBLE_CLICK_DELTA = 300;

                    if (m_impl->last_down[slot].time_since_epoch().count() &&
                        std::chrono::duration<double, std::milli>(tv - m_impl->last_down[slot]).count() < DOUBLE_CLICK_DELTA)
                    {
                        Event event(EventId::pointer_dblclick,
                                    Pointer(m_last_point[slot], Pointer::Button::left));
                        dispatch(event);
                    }
                    else
                    {
                        m_active[slot] = true;

                        SPDLOG_TRACE("mouse down {}", m_last_point[slot]);

                        Event event(EventId::raw_pointer_down,
                                    Pointer(m_last_point[slot], Pointer::Button::left));
                        dispatch(event);
                    }

                    m_impl->last_down[slot] = tv;
                }
            }
        }
    }

    for (size_t slot = 0; slot < move.size(); slot++)
    {
        if (move[slot])
        {
            SPDLOG_TRACE("mouse move {}", m_last_point[slot]);

            Event event(EventId::raw_pointer_move,
                        Pointer(m_last_point[slot], Pointer::Button::left));
            dispatch(event);
        }
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

    // NOLINTNEXTLINE(modernize-loop-convert)
    for (int i = 0; i < SAMPLE_COUNT; i++)
    {
        // NOLINTNEXTLINE (cppcoreguidelines-owning-memory)
        delete [] m_impl->samp_mt[i];
    }
}

}
}
}
