/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */

#include "event_loop.h"
#include "geometry.h"
#include "input.h"
#ifdef HAVE_TSLIB
#include "tslib.h"
#endif
#include "widget.h"
#include <cassert>
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/time.h>
#include <unistd.h>
#include "window.h"

using namespace std;

namespace mui
{

    static Point pointer_abs_pos;
    Point& mouse_position()
    {
	return pointer_abs_pos;
    }

    static int event_key;
    int& key_position()
    {
	return event_key;
    }

    InputEvDev::InputEvDev(const string& path)
	: m_fd(-1)
    {
	m_fd = open(path.c_str(), O_RDONLY);
	assert(m_fd >= 0);

	EventLoop::add_fd(m_fd, EVENT_READABLE, InputEvDev::process);
    }

    void InputEvDev::process(int fd, uint32_t mask, void *data)
    {
	struct input_event ev[8], *e, *end;

	int dx = 0;
	int dy = 0;
	int x = 0;
	int y = 0;
	bool absolute_event = false;

	int len = read(fd, &ev, sizeof(ev));
	if (len < 0 || len % sizeof(e[0]) != 0)
	{
	    assert(0);
	}

	e = ev;
	end = ev + (len/sizeof(e[0]));
	for (e = ev; e < end; e++)
	{
	    int value = e->value;

	    dbg << value << endl;
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
		    x = (double)value;// * 800./1024.;
		    break;
		case ABS_Y:
		    y = (double)value;// * 480./1024.;
		    break;
		}

	    case EV_KEY:
		if (value == 2)
		    break;

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
		    //device->tool = value ? e->code : 0;
		    break;

		case BTN_LEFT:
		    main_window()->handle(value ? EVT_MOUSE_DOWN : EVT_MOUSE_UP);
		    break;

		case BTN_RIGHT:
		    main_window()->handle(value ? EVT_MOUSE_DOWN : EVT_MOUSE_UP);
		    break;

		case BTN_MIDDLE:
		    main_window()->handle(value ? EVT_MOUSE_DOWN : EVT_MOUSE_UP);
		    break;
		}
	    }
	}

	if (absolute_event)
	{
	    pointer_abs_pos = Point(x,y);
	    main_window()->handle(EVT_MOUSE_MOVE);
	}
	else
	{
	    if (dx != 0 || dy != 0)
	    {
		pointer_abs_pos = Point(pointer_abs_pos.x+dx,pointer_abs_pos.y+dy);
		main_window()->handle(EVT_MOUSE_MOVE);
	    }
	}
    }

    InputEvDev::~InputEvDev()
    {
	if (m_fd >= 0)
	    close(m_fd);
    }

#ifdef HAVE_TSLIB

    static const int SLOTS = 1;
    static const int SAMPLES = 20;
    static struct tsdev *ts;
    static struct ts_sample_mt **samp_mt;

    //bool active = false;
    //static struct timeval m_last_time = {0,0};

    //static inline int diff_ms(const timeval& t1, const timeval& t2)
    //{
//	return (((t1.tv_sec - t2.tv_sec) * 1000000) +
//		(t1.tv_usec - t2.tv_usec))/1000;
    //  }

    InputTslib::InputTslib(const string& path)
	: m_fd(-1),
	  m_timerfd(-1),
	  m_active(false)
    {
	const int NONBLOCKING = 1;
	int i;

	ts = ts_setup(path.c_str(), NONBLOCKING);
	assert(ts);

	samp_mt = (struct ts_sample_mt **)malloc(SAMPLES * sizeof(struct ts_sample_mt *));
	assert(samp_mt);

	for (i = 0; i < SAMPLES; i++) {
	    samp_mt[i] = (struct ts_sample_mt *)calloc(SLOTS, sizeof(struct ts_sample_mt));
	    if (!samp_mt[i]) {
		free(samp_mt);
		ts_close(ts);
		assert(0);
	    }
	}

	EventLoop::add_fd(ts_fd(ts), EVENT_READABLE, InputTslib::process, this);
    }

    void InputTslib::timer_callback(int fd, void* data)
    {
	InputTslib* obj = reinterpret_cast<InputTslib*>(data);
	assert(obj);
	obj->m_active = false;
	main_window()->handle(EVT_MOUSE_UP);
	dbg << "mouse up " << pointer_abs_pos << endl;
	obj->m_timerfd = -1;
    }

    void InputTslib::process(int fd, uint32_t mask, void *data)
    {
	InputTslib* obj = reinterpret_cast<InputTslib*>(data);
	int ret;
	int i;
	int j;

	ret = ts_read_mt(ts, samp_mt, SLOTS, SAMPLES);
	if (ret < 0)
	{
	    perror("ts_read_mt");
	    return;
	}

	bool move = false;
	bool start = false;

	if (obj->m_timerfd > 0)
	{
	    EventLoop::rem_fd(obj->m_timerfd);
	    close(obj->m_timerfd);
	    obj->m_timerfd = -1;
	}

	for (j = 0; j < ret; j++)
	{
	    for (i = 0; i < SLOTS; i++)
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

		if (obj->m_active)
		{
		    pointer_abs_pos = Point(samp_mt[j][i].x, samp_mt[j][i].y);
		    move = true;
		    start = true;
		}
		else
		{
		    if (samp_mt[j][i].pressure > 0 && samp_mt[j][i].pen_down == 1)
		    {
			pointer_abs_pos = Point(samp_mt[j][i].x, samp_mt[j][i].y);
			main_window()->handle(EVT_MOUSE_DOWN);
			dbg << "mouse down " << pointer_abs_pos << endl;
			obj->m_active = true;
			start = true;
		    }
		}
	    }
	}

	if (move)
	{
	    dbg << "mouse move " << pointer_abs_pos << endl;
	    main_window()->handle(EVT_MOUSE_MOVE);
	}

	if (start)
	{
	    obj->m_timerfd = EventLoop::start_timer(200, InputTslib::timer_callback, obj);
	}
    }

    InputTslib::~InputTslib()
    {
	ts_close(ts);
    }
#endif

}
