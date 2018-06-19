/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef TIMER_H
#define TIMER_H

#include <cstdint>

namespace mui
{

    /**
     * Basic one shot timer.
     */
    class Timer
    {
    public:
	Timer(uint64_t duration = 0);
	virtual void start();
	virtual void stop();
	virtual void timeout() = 0;
	virtual ~Timer();

    protected:

	static void timer_callback(int fd, void* data);

	int m_fd;
	uint64_t m_duration;
    };

    /**
     * Periodic timer.
     */
    class PeriodicTimer : public Timer
    {
    public:
	PeriodicTimer(uint64_t period = 0);
	virtual void start();
	virtual void stop();
	virtual ~PeriodicTimer();
    };

}

#endif
