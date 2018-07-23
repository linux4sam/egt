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
	: Image("smallbubble.png", point.x, point.y),
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

class MyWindow : public PlaneWindow
{
public:
    MyWindow()
	: PlaneWindow(Size(800,500), FLAG_WINDOW_DEFAULT, DRM_FORMAT_XRGB8888)
    {
	Image* img = new Image("water.png");
	add(img);

	m_label = new Label("Objects: 0",
			    Point(10, 10),
			    Size(100, 40),
			    Widget::ALIGN_LEFT | Widget::ALIGN_CENTER);
	m_label->fgcolor(Color::WHITE);
	add(m_label);
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
	std::uniform_int_distribution<int> speed_dist(-20, -1);
	int xspeed = 0;
	int yspeed = speed_dist(e1);

	std::uniform_int_distribution<int> offset_dist(-20, 20);
	int offset = offset_dist(e1);

	std::uniform_real_distribution<float> size_dist(0.1, 1.0);
	float size = size_dist(e1);

	// has to move at some speed
	if (yspeed == 0)
	    yspeed = 1;

	MyImage* image = new MyImage(xspeed, yspeed, p);
	add(image);
	image->scale(size);
	image->move(p.x - image->box().w/2 + offset,
		    p.y - image->box().h/2 + offset);
	m_images.push_back(image);
	objects_changed();
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
		objects_changed();
	    }
	    else
	    {
		x++;
	    }
	}
    }

private:

    void objects_changed()
    {
	ostringstream ss;
	ss << "Objects: " << m_images.size();
	m_label->text(ss.str());
    }

    vector<MyImage*> m_images;
    std::random_device r;
    Label* m_label;
};


int main()
{
#ifdef HAVE_TSLIB
#ifdef HAVE_LIBPLANES
    KMSScreen kms(false);
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
	    Point p(win.box().center());
	    p.y=win.box().h;
	    win.spawn(p);
	});
    spawntimer.start();

#if 0
    WidgetPositionAnimator a1 = WidgetPositionAnimator({&win},
						       WidgetPositionAnimator::CORD_Y,
						       0, -18,
						       1000,
						       easing_easy_slow);

    WidgetPositionAnimator a2 = WidgetPositionAnimator({&win},
						       WidgetPositionAnimator::CORD_Y,
						       -18, 0,
						       1000,
						       easing_extend);

    PeriodicTimer floattimer(1000);
    floattimer.add_handler([&a1,&a2]() {
	    static bool toggle1 = false;
	    if (!toggle1)
	    {
		a2.stop();
		a1.start();
	    }
	    else
	    {
		a1.stop();
		a2.start();
	    }
	    toggle1 = !toggle1;
	});
    floattimer.start();
#endif

    return EventLoop::run();
}
