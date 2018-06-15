/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "timer.h"
#include "event_loop.h"
#include <cassert>
#include <unistd.h>

namespace mui
{

    Timer::Timer(uint64_t duration)
	: m_fd(-1),
	  m_duration(duration)
    {
    }

    void Timer::start()
    {
	m_fd = EventLoop::start_timer(m_duration, Timer::timer_callback, this);
    }

    void Timer::stop()
    {
	if (m_fd >= 0)
	{
	    close(m_fd);
	    m_fd = -1;
	}
    }

    void Timer::timer_callback(int fd, void* data)
    {
	Timer* timer = reinterpret_cast<Timer*>(data);
	assert(timer);

	timer->timeout();
    }

    Timer::~Timer()
    {
	stop();
    }

    PeriodicTimer::PeriodicTimer(uint64_t duration)
	: Timer(duration)
    {
    }

    void PeriodicTimer::start()
    {
	m_fd = EventLoop::start_periodic_timer(m_duration, PeriodicTimer::timer_callback, this);
    }

    void PeriodicTimer::stop()
    {
	if (m_fd >= 0)
	{
	    EventLoop::cancel_periodic_timer(m_fd);
	    close(m_fd);
	    m_fd = -1;
	}
    }

    PeriodicTimer::~PeriodicTimer()
    {
	stop();
    }

}
