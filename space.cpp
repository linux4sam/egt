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
#include <random>
#include <cmath>
#include "tools.h"
#include <chrono>

using namespace std;
using namespace mui;

class MyImage : public Image
{
public:
    MyImage(int xspeed, int yspeed, const Point& point)
	: Image("metalball.png", point.x, point.y),
	  m_xspeed(xspeed),
	  m_yspeed(yspeed)
    {}

    bool animate()
    {
	bool visible = Rect::is_intersect(Rect(Point(0,0),main_screen()->size()), box());

	if (visible)
	{
	    Point to(box().point());
	    to += Point(m_xspeed, m_yspeed);
	    move(to);
	}

	return visible;
    }

private:
    int m_xspeed;
    int m_yspeed;
    float m_angle;
};

class MyWindow : public SimpleWindow
{
public:
    MyWindow()
	: SimpleWindow()
    {
	Palette p(palette());
	p.set(Palette::BG, Palette::GROUP_NORMAL, Color::BLACK);
	set_palette(p);
    }

    int handle(int event)
    {
	switch (event)
	{
	case EVT_MOUSE_MOVE:
	    spawn(mouse_position());
	    break;
	}

	return SimpleWindow::handle(event);
    }

    void spawn(const Point& p)
    {
	std::default_random_engine e1(r());
	std::uniform_int_distribution<int> speed_dist(-20, 20);
	int xspeed = speed_dist(e1);
	int yspeed = speed_dist(e1);

	std::uniform_real_distribution<float> size_dist(0.1, 1.0);
	float size = size_dist(e1);

	// has to move at some speed
	if (xspeed == 0 && yspeed == 0)
	    xspeed = yspeed = 1;

	MyImage* image = new MyImage(xspeed, yspeed, p);
	add(image);
	image->scale(size);
	image->move(p.x - image->box().w/2,
		    p.y - image->box().h/2);
	m_images.push_back(image);
    }

    void animate()
    {
	for (auto x = m_images.begin(); x != m_images.end();)
	{
	    if (!(*x)->animate())
	    {
		remove(*x);
		delete *x;
		x = m_images.erase(x);
	    }
	    else
	    {
		x++;
	    }
	}
    }

private:
    vector<MyImage*> m_images;
    std::random_device r;
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

    MyWindow win;
    win.show();

    PeriodicTimer animatetimer(33);
    animatetimer.add_handler([&win]() {
	    win.animate();
	});
    animatetimer.start();

    PeriodicTimer spawntimer(1000);
    spawntimer.add_handler([&win]() {
	    win.spawn(win.box().center());
	});
    spawntimer.start();

    return EventLoop::run();
}
