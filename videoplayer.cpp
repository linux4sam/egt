/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "ui.h"
#include <math.h>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <thread>

using namespace std;
using namespace mui;

struct ProgressAnimation : public AnimationTimer
{
    ProgressAnimation(ProgressBar* progress)
	: AnimationTimer(0,100,5000, easing_easy_slow),
	  m_progress(progress)
    {}

    virtual void step(int value)
    {
	m_progress->value(value);
    }

    ProgressBar* m_progress;
};

class MainWindow : public SimpleWindow
{
public:
    MainWindow(const Size& size)
	: SimpleWindow(size),
	  m_moving(false),
	  m_video(Size(480,240))
    {
	Palette p(palette());
	p.set(Palette::BG, Palette::GROUP_NORMAL, Color::LIGHTBLUE);
	set_palette(p);

	Label* label = new Label("Blank Screen", Point(0, 60), Size(800, 480));
	add(label);

	ProgressBar* progress = new ProgressBar(Point(10, 10), Size(200, 30));
	add(progress);

	//SpinProgress* progress = new SpinProgress(Point(10, 10), Size(50, 50));
	//add(progress);

	ProgressAnimation* ap = new ProgressAnimation(progress);
	ap->start();

	m_video.move(200,200);

	m_video.set_media("/root/tst/video0.mp4");
	m_video.play();

	m_video.show();
    }

    int handle(int event)
    {
	switch (event)
	{
	case EVT_MOUSE_DOWN:
	    if (!m_moving)
	    {
		m_moving = true;
		m_starting_point = mouse_position();
		m_position = Point(m_video.x(),m_video.y());
	    }
	    return 1;
	case EVT_MOUSE_UP:
	    m_moving = false;
	    return 1;
	case EVT_MOUSE_MOVE:
	    if (m_moving)
	    {
		Point diff = mouse_position() - m_starting_point;
		m_video.move(m_position.x + diff.x, m_position.y + diff.y);
		return 1;
	    }
	    break;
	}

	return 0;
    }

private:
    bool m_moving;
    Point m_starting_point;
    Point m_position;
    VideoWindow m_video;
};

int main()
{
#ifdef HAVE_TSLIB
#ifdef HAVE_LIBPLANES
    KMSScreen kms;
    InputTslib input0("/dev/input/touchscreen0");
#else
    FrameBuffer fb("/dev/fb0");
#endif
#else
    X11Screen screen(Size(800,480));
#endif

    MainWindow window(Size(800, 480));

    std::thread t([](){
	    GMainLoop *mainloop = g_main_loop_new(NULL, FALSE);
	    g_main_loop_run(mainloop);
	});

    return EventLoop::run();
}
