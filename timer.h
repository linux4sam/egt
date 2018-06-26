/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef TIMER_H
#define TIMER_H

#include <cstdint>
#include <vector>
#include <functional>

namespace mui
{
    /**
     * Timer callback function definition.
     */
    typedef std::function<void()> timer_callback_t;

    /**
     * Basic one shot timer.
     */
    class Timer
    {
    public:
	Timer(uint64_t duration = 0);

	/**
	 * Start the timer.
	 */
	virtual void start();

	/**
	 * Start the timer with the specified duration.  This overwrites any
	 * duration specified previously.
	 */
	virtual void start(uint64_t duration);

	/**
	 * Cancel the timer.
	 */
	virtual void cancel();

	/**
	 * Called when the timer times out.
	 *
	 * This will invoke any callback registered with add_handler(). If you
	 * override this you must make sure to call Timer::timeout() or
	 * equivelant if you want callbacks to still be called.
	 */
	virtual void timeout();
	inline uint64_t duration() const { return m_duration; }

	void add_handler(timer_callback_t callback)
	{
	    m_callbacks.push_back(callback);
	}

	virtual ~Timer();

    protected:

	static void timer_callback(int fd, void* data);

	int m_fd;
	uint64_t m_duration;
	std::vector<timer_callback_t> m_callbacks;
    };

    /**
     * Periodic timer.
     */
    class PeriodicTimer : public Timer
    {
    public:
	PeriodicTimer(uint64_t period = 0);
	virtual void start();
	virtual void cancel();
	virtual ~PeriodicTimer();
    };

}

#endif
