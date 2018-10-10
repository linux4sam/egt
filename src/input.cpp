/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mui/app.h"
#include "mui/event_loop.h"
#include "mui/geometry.h"
#include "mui/input.h"
#include "mui/widget.h"
#include "mui/window.h"
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <string>
#include <sys/time.h>
#include <unistd.h>
#ifdef HAVE_TSLIB
#include "tslib.h"
#endif

using namespace std;

namespace mui
{

    static Point pointer_abs_pos;
    Point& mouse_position()
    {
        return pointer_abs_pos;
    }

    static int event_key;
    int& key_value()
    {
        return event_key;
    }

    void IInput::dispatch(int event)
    {
        for (auto& w : windows())
        {
            if (!w->visible())
                continue;

            if (!w->top_level())
                continue;

            w->handle(event);
        }
    }

    InputEvDev::InputEvDev(const string& path)
        : m_input(main_app().event().io()),
          m_input_buf(sizeof(struct input_event) * 10)
    {
        int fd = open(path.c_str(), O_RDONLY);
        if (fd >= 0)
        {
            m_input.assign(fd);

            asio::async_read(m_input, asio::buffer(m_input_buf.data(), m_input_buf.size()),
                             std::bind(&InputEvDev::handle_read, this,
                                       std::placeholders::_1,
                                       std::placeholders::_2));
        }
    }

    void InputEvDev::handle_read(const asio::error_code& error, std::size_t length)
    {
        if (error)
        {
            cout << error << endl;
            return;
        }

        struct input_event* ev = reinterpret_cast<struct input_event*>(m_input_buf.data());
        struct input_event* e, *end;

        int dx = 0;
        int dy = 0;
        int x = 0;
        int y = 0;
        bool absolute_event = false;

        if (length == 0 || length % sizeof(e[0]) != 0)
        {
            assert(0);
        }

        e = ev;
        end = ev + (length / sizeof(e[0]));
        for (e = ev; e < end; e++)
        {
            int value = e->value;

            DBG(value);
            switch (e->type)
            {
            case EV_REL:
                switch (e->code)
                {
                case REL_X:
                    dx += value;
                    break;
                case REL_Y:
                    dy += value;
                    break;
                default:
                    assert(0);
                }

            case EV_ABS:
                absolute_event = true;
                switch (e->code)
                {
                case ABS_X:
                    x = (double)value;
                    break;
                case ABS_Y:
                    y = (double)value;
                    break;
                }

            case EV_KEY:
                switch (e->code)
                {
                case BTN_TOUCH:
                case BTN_TOOL_PEN:
                case BTN_TOOL_RUBBER:
                case BTN_TOOL_BRUSH:
                case BTN_TOOL_PENCIL:
                case BTN_TOOL_AIRBRUSH:
                case BTN_TOOL_FINGER:
                case BTN_TOOL_MOUSE:
                case BTN_TOOL_LENS:
                    break;
                case BTN_LEFT:
                    dispatch(value ? EVT_MOUSE_DOWN : EVT_MOUSE_UP);
                    break;
                case BTN_RIGHT:
                    dispatch(value ? EVT_MOUSE_DOWN : EVT_MOUSE_UP);
                    break;
                case BTN_MIDDLE:
                    dispatch(value ? EVT_MOUSE_DOWN : EVT_MOUSE_UP);
                    break;
                default:
                    int v = -1;
                    if (value == 1)
                        v = EVT_KEY_DOWN;
                    else if (value == 0)
                        v = EVT_KEY_UP;
                    else if (value == 2)
                        v = EVT_KEY_REPEAT;
                    if (v != -1)
                    {
                        event_key = e->code;
                        dispatch(v);
                    }
                }
            }
        }

        if (absolute_event)
        {
            pointer_abs_pos = Point(x, y);
            dispatch(EVT_MOUSE_MOVE);
        }
        else
        {
            if (dx != 0 || dy != 0)
            {
                pointer_abs_pos = Point(pointer_abs_pos.x + dx, pointer_abs_pos.y + dy);
                dispatch(EVT_MOUSE_MOVE);
            }
        }

        asio::async_read(m_input, asio::buffer(m_input_buf.data(), m_input_buf.size()),
                         std::bind(&InputEvDev::handle_read, this,
                                   std::placeholders::_1,
                                   std::placeholders::_2));
    }

    InputEvDev::~InputEvDev()
    {
    }

#ifdef HAVE_TSLIB

    static const int SLOTS = 1;
    static const int SAMPLES = 10;

    namespace detail
    {
        struct tslibimpl
        {
            struct tsdev* ts;
            struct ts_sample_mt** samp_mt;
            struct timeval last_down = {0, 0};
        };
    }

    InputTslib::InputTslib(const string& path)
        : m_input(main_app().event().io()),
          m_active(false),
          m_impl(new detail::tslibimpl)
    {
        const int NONBLOCKING = 1;
        m_impl->ts = ts_setup(path.c_str(), NONBLOCKING);

        if (m_impl->ts)
        {
            m_impl->samp_mt = (struct ts_sample_mt**)malloc(SAMPLES * sizeof(struct ts_sample_mt*));
            assert(m_impl->samp_mt);

            for (int i = 0; i < SAMPLES; i++)
            {
                m_impl->samp_mt[i] = (struct ts_sample_mt*)calloc(SLOTS, sizeof(struct ts_sample_mt));
                if (!m_impl->samp_mt[i])
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
            ERR("ts device not found: " << path);
        }
    }

    static inline int diff_ms(timeval t1, timeval t2)
    {
        return (((t1.tv_sec - t2.tv_sec) * 1000000) +
                (t1.tv_usec - t2.tv_usec)) / 1000;
    }

    void InputTslib::handle_read(const asio::error_code& error)
    {
        if (error)
        {
            cout << error << endl;
            return;
        }

        struct ts_sample_mt** samp_mt = m_impl->samp_mt;

        int ret = ts_read_mt(m_impl->ts, samp_mt, SLOTS, SAMPLES);
        if (ret < 0)
        {
            ERR("ts_read_mt");
            return;
        }

        bool move = false;

        for (int j = 0; j < ret; j++)
        {
            for (int i = 0; i < SLOTS; i++)
            {
#ifdef TSLIB_MT_VALID
                if (!(samp_mt[j][i].valid & TSLIB_MT_VALID))
                    continue;
#else
                if (samp_mt[j][i].valid < 1)
                    continue;
#endif

#if 0
                printf("%ld.%06ld: (slot %d) %d %6d %6d %6d %d %d\n",
                       samp_mt[j][i].tv.tv_sec,
                       samp_mt[j][i].tv.tv_usec,
                       samp_mt[j][i].slot,
                       samp_mt[j][i].tool_type,
                       samp_mt[j][i].x,
                       samp_mt[j][i].y,
                       samp_mt[j][i].pressure,
                       samp_mt[j][i].distance,
                       samp_mt[j][i].pen_down);
#endif
                if (samp_mt[j][i].x < 0 || samp_mt[j][i].y < 0)
                    continue;

                if (m_active)
                {
                    if (samp_mt[j][i].pen_down == 0)
                    {
                        pointer_abs_pos = Point(samp_mt[j][i].x, samp_mt[j][i].y);
                        m_active = false;
                        dispatch(EVT_MOUSE_UP);
                        DBG("mouse up " << pointer_abs_pos);
                    }
                    else
                    {
                        pointer_abs_pos = Point(samp_mt[j][i].x, samp_mt[j][i].y);
                        move = true;
                    }
                }
                else
                {
                    if (samp_mt[j][i].pen_down == 1)
                    {
                        pointer_abs_pos = Point(samp_mt[j][i].x, samp_mt[j][i].y);

                        if ((m_impl->last_down.tv_sec || m_impl->last_down.tv_usec) &&
                            diff_ms(samp_mt[j][i].tv, m_impl->last_down) < 200)
                        {
                            dispatch(EVT_MOUSE_DBLCLICK);
                        }
                        else
                        {
                            dispatch(EVT_MOUSE_DOWN);
                            DBG("mouse down " << pointer_abs_pos);
                            m_active = true;
                        }

                        m_impl->last_down = samp_mt[j][i].tv;
                    }
                }
            }
        }

        if (move)
        {
            DBG("mouse move " << pointer_abs_pos);
            dispatch(EVT_MOUSE_MOVE);
        }

        asio::async_read(m_input, asio::null_buffers(),
                         std::bind(&InputTslib::handle_read, this, std::placeholders::_1));
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
#endif

}
