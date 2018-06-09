/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */

#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include <stdint.h>

typedef void (*event_callback)(int fd, uint32_t mask, void *data);

struct event_source
{
	int fd;
	event_callback func;
	void *data;
};

enum
{
	EVENT_WRITEABLE = 1<<0,
	EVENT_READABLE = 1<<1
};

class EventLoop
{
public:

	static void init();

	static void add_fd(int fd, uint32_t mask, event_callback func);

	static int wait();

	static void run();

	static void close();
};

#endif
