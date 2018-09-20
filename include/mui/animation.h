/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef ANIMATION_H
#define ANIMATION_H

#include "mui/timer.h"
#include <cstdint>
#include <chrono>
#include <vector>

namespace mui
{

    float easing_linear(float percent);
    float easing_easy(float percent);
    float easing_easy_slow(float percent);
    float easing_extend(float percent);
    float easing_drop(float percent);
    float easing_drop_slow(float percent);
    float easing_snap(float percent);
    float easing_bounce(float percent);
    float easing_bouncy(float percent);
    float easing_rubber(float percent);
    float easing_spring(float percent);
    float easing_boing(float percent);

    typedef void (*animation_callback)(float value, void* data);
    typedef float (*easing_func)(float percent);

    /**
     * Animation class with configurable easing function.
     *
     * See <https://easings.net> for more information about other easing
     * functions.
     */
    class Animation
    {
    public:

	/**
	 *
	 */
	Animation(float start, float end, animation_callback callback,
		  uint64_t duration, easing_func func = easing_linear,
		  void* data = 0);

	float starting() const { return m_start; }
	void starting(float start) { m_start = start; }
	float ending() const { return m_end; }
	void ending(float end) { m_end = end; }
	void duration(float dur) { m_duration = dur; }
	virtual void start();
	virtual bool next();
	virtual void stop();
	bool running() const { return m_running; }
	void set_easing_func(easing_func func);
	void reverse(bool rev) { m_reverse = rev; }

    protected:

	float m_start;
	float m_end;
	animation_callback m_callback;
	easing_func m_easing;
	float m_current;
	uint64_t m_duration;
	std::chrono::time_point<std::chrono::steady_clock> m_start_time;
	std::chrono::time_point<std::chrono::steady_clock> m_stop_time;
	bool m_running;
	bool m_reverse;
	void* m_data;
    };

    class Widget;

    class WidgetPositionAnimator : public Animation
    {
    public:
	enum
	{
	    CORD_X,
	    CORD_Y
	};

	WidgetPositionAnimator(Widget* widget,
			       int coordinate,
			       int start, int end,
			       uint64_t duration,
			       easing_func func = easing_linear);

	WidgetPositionAnimator(std::vector<Widget*> widgets,
			       int coordinate,
			       int start, int end,
			       uint64_t duration,
			       easing_func func = easing_linear);

	void start();

	void reset();

	static void timer_callback(int fd, void* data);

	static void callback(float value, void* data);

    protected:
	std::vector<Widget*> m_widgets;
	int m_fd;
	int m_coord;
    };

    class AnimationTimer : public PeriodicTimer
    {
    public:
	AnimationTimer(int start, int end, uint64_t duration, easing_func func);

	void start();

	virtual void step(int value) = 0;

    protected:

	void timeout();

	static void animation_callback(float value, void* data);

	Animation m_animation;
    };
}

#endif
