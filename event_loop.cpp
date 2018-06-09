/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */

#include "event_loop.h"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include "widget.h"

static int epoll_fd = -1;

void EventLoop::init()
{
	epoll_fd = epoll_create(16);
	assert(epoll_fd >= 0);
}

void EventLoop::add_fd(int fd, uint32_t mask, event_callback func)
{
	event_source *source;
	epoll_event ep;

	source = new event_source();
	if (source == NULL)
		return;

	source->fd = fd;
	source->func = func;

	ep.events = 0;
	if (mask & EVENT_READABLE)
		ep.events |= EPOLLIN;
	if (mask & EVENT_WRITEABLE)
		ep.events |= EPOLLOUT;
	ep.data.ptr = source;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ep) < 0)
	{
		assert(0);
		free(source);
	}
}

#define ARRAY_LENGTH(a) (sizeof (a) / sizeof (a)[0])

int EventLoop::wait()
{
	struct epoll_event ep[32];
	event_source *source;
	int i, count, timeout;
	uint32_t mask;

	timeout = -1;

	count = epoll_wait(epoll_fd, ep, ARRAY_LENGTH(ep), timeout);
	if (count < 0)
		return -1;

	for (i = 0; i < count; i++) {
		source = reinterpret_cast<event_source*>(ep[i].data.ptr);
		mask = 0;
		if (ep[i].events & EPOLLIN)
			mask |= EVENT_READABLE;
		if (ep[i].events & EPOLLOUT)
			mask |= EVENT_WRITEABLE;

		source->func(source->fd, mask, source->data);
	}

	return 0;
}

void EventLoop::run()
{
	while (true)
	{
		main_window()->draw();
		wait();
	}
}

void EventLoop::close()
{
	::close(epoll_fd);
}
