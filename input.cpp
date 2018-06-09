/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */

#include "event_loop.h"
#include "geometry.h"
#include "input.h"
#include "tslib.h"
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

using namespace std;

static Point event_position;
const Point& mouse_position()
{
	return event_position;
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

		cout << value << endl;
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
				main_window()->handle(value ? MOUSE_DOWN : MOUSE_UP);
				break;

			case BTN_RIGHT:
				main_window()->handle(value ? MOUSE_DOWN : MOUSE_UP);
				break;

			case BTN_MIDDLE:
				main_window()->handle(value ? MOUSE_DOWN : MOUSE_UP);
				break;
			}
		}
	}

	if (absolute_event)
	{
		event_position = Point(x,y);
		main_window()->handle(MOUSE_MOVE);
	}
	else
	{
		if (dx != 0 || dy != 0)
		{
			event_position = Point(event_position.x+dx,event_position.y+dy);
			main_window()->handle(MOUSE_MOVE);
		}
	}
}

InputEvDev::~InputEvDev()
{
	if (m_fd >= 0)
		close(m_fd);
}

static const int SLOTS = 2;
static const int SAMPLES = 50;

static struct tsdev *ts;
static struct ts_sample_mt **samp_mt = NULL;
//static struct input_absinfo slot;

#define NONBLOCKING 1

bool active = false;

//static Point last_event_position;

InputTslib::InputTslib(const string& path)
	: m_fd(-1)
{
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

	EventLoop::add_fd(ts_fd(ts), EVENT_READABLE, InputTslib::process);
}

static struct timeval m_last_time = {0,0};

static inline int diff_ms(const timeval& t1, const timeval& t2)
{
	return (((t1.tv_sec - t2.tv_sec) * 1000000) +
		(t1.tv_usec - t2.tv_usec))/1000;
}


void InputTslib::process(int fd, uint32_t mask, void *data)
{
	int ret;
	int i;
	int j;
	ret = ts_read_mt(ts, samp_mt, SLOTS, SAMPLES);
	if (ret < 0) {
		perror("ts_read_mt");
	}
	else if (ret != 1)
	{
	}
	else {
		bool move = false;
		Point move_event_position;

		for (j = 0; j < ret; j++) {
			for (i = 0; i < SLOTS; i++) {
#ifdef TSLIB_MT_VALID
				if (!(samp_mt[j][i].valid & TSLIB_MT_VALID))
					continue;
#else
#warning no multitouch
				if (samp_mt[j][i].valid < 1)
					continue;
#endif

#if 0
				printf("%ld.%06ld: (slot %d) %d %6d %6d %6d\n",
				       samp_mt[j][i].tv.tv_sec,
				       samp_mt[j][i].tv.tv_usec,
				       samp_mt[j][i].slot,
       				       samp_mt[j][i].tool_type,
				       samp_mt[j][i].x,
				       samp_mt[j][i].y,
				       samp_mt[j][i].pressure);
#endif


				if (active)
				{
					int elapsed = diff_ms(samp_mt[j][i].tv, m_last_time);

					if (elapsed > 200 || samp_mt[j][i].pressure == 0)
					{
						event_position = Point(samp_mt[j][i].x,samp_mt[j][i].y);
						main_window()->handle(MOUSE_UP);
						active = false;
					}
					else
					{
						move_event_position = Point(samp_mt[j][i].x,samp_mt[j][i].y);
						move = true;
						event_position = move_event_position;
						main_window()->handle(MOUSE_MOVE);
					}
				}
				else
				{
					if (samp_mt[j][i].pressure > 0)
					{
						event_position = Point(samp_mt[j][i].x,samp_mt[j][i].y);
						main_window()->handle(MOUSE_DOWN);
						active = true;
					}
				}

				m_last_time = samp_mt[j][i].tv;

			}
		}

		if (move)
		{
			//if (last_event_position.x == 0 &&
			//    last_event_position.y == 0 ||
			//    (abs(last_event_position.x-event_position.x) > 5 ||
			//     abs(last_event_position.y-event_position.y) > 5))
			//{

			//event_position = move_event_position;
			//	main_window()->handle(MOUSE_MOVE);


				//last_event_position = event_position;
				//}

		}


	}
}

InputTslib::~InputTslib()
{
	ts_close(ts);
}
