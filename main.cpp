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

using namespace std;
using namespace mui;

static float sliding_scale(int win_w, int item_w, int item_pos,
			   float min = 0.3, float max = 2.0)
{
    float range = win_w / 2;
    float delta = fabs(range - (item_pos + (item_w / 2)));
    float scale = 1.0 - (delta / range);
    if (scale < min || scale > max)
	return min;
    return scale;
}

#define NUM_ITEMS 10

class MyWindow;

class MyImage : public Image
{
public:
    MyImage(MyWindow& win, const string& filename, int x = 0, int y = 0)
	: Image(filename, x, y),
	  m_win(win),
	  m_fd(-1),
	  m_animation(0, 600, MyImage::animate, 1000, easing_snap, this)
    {}

    static void animate(float value, void* data)
    {
	MyImage* image = reinterpret_cast<MyImage*>(data);
	assert(image);

	image->scale(value);
    }

    static void timer_callback(int fd, void* data);

    int handle(int event)
    {
	switch (event)
	{
	case EVT_MOUSE_DOWN:
	{
	    if (!m_animation.running())
	    {
		m_animation.set_easing_func(easing_snap);
		m_animation.starting(scale());
		m_animation.ending(scale() + 0.2);
		m_animation.duration(500);
		m_animation.start();
		m_fd = EventLoop::start_periodic_timer(1, MyImage::timer_callback, this);
		return 1;
	    }

	    break;
	}
	case EVT_MOUSE_UP:
	    //EventLoop::quit();
	    break;
	}

	return Image::handle(event);
    }

private:
    MyWindow& m_win;
    int m_fd;
    Animation m_animation;
};

class MyWindow : public SimpleWindow
{
public:
    MyWindow()
	: SimpleWindow(800,480),
	  m_moving(false)
    {}

    int load()
    {
	Image* img = new Image("background.png");
	add(img);

	for (int t = 0; t < NUM_ITEMS; t++)
	{
	    stringstream os;
	    os << "_image" << t << ".png";
	    MyImage* box = new MyImage(*this, os.str());
	    box->position(t * 200, (h() / 2) - (box->h() / 2));
	    add(box);
	    m_boxes[t] = box;

	    scale_box(box, t * 200);
	}

	for (int t = 0; t < NUM_ITEMS; t++)
	    m_positions[t] = m_boxes[t]->x();

	return 0;
    }

    int handle(int event)
    {
	int ret = SimpleWindow::handle(event);
	if (ret)
	    return 1;

	switch (event)
	{
	case EVT_MOUSE_DOWN:
	    if (!m_moving)
	    {
		m_moving = true;
		m_moving_x = mouse_position().x;
		for (int t = 0; t < NUM_ITEMS; t++)
		    m_positions[t] = m_boxes[t]->x();
	    }

	    return 1;
	    break;
	case EVT_MOUSE_UP:
	    m_moving = false;
	    return 1;
	    break;
	case EVT_MOUSE_MOVE:

	    if (m_moving)
	    {
		move();
		return 1;
	    }
	    break;
	}

	return 0;
    }

    void move()
    {
	int diff = mouse_position().x - m_moving_x;

	for (int t = 0; t < NUM_ITEMS; t++)
	{
	    int pos = m_positions[t] + diff;

	    Rect to(m_boxes[t]->box());
	    to.x = pos;
	    bool visible = Rect::is_intersect(Rect::merge(to,m_boxes[t]->box()), this->box());

	    if (visible)
	    {
		scale_box(m_boxes[t], pos);
		m_boxes[t]->move(pos, m_boxes[t]->y());
	    }
	    else
	    {
		m_boxes[t]->position(pos, m_boxes[t]->y());
	    }
	}
    }

    void scale_box(MyImage* image, int pos)
    {
    	float scale = sliding_scale(this->w(), image->w(), pos);
	image->scale(scale);
    }

private:
    bool m_moving;
    int m_moving_x;
    int m_positions[NUM_ITEMS];
    MyImage* m_boxes[NUM_ITEMS];
};

void MyImage::timer_callback(int fd, void* data)
{
    MyImage* image = reinterpret_cast<MyImage*>(data);
    assert(image);

    if (!image->m_animation.next())
    {
	if (image->m_animation.starting() > image->m_animation.ending())
	{
	    image->m_win.scale_box(image, image->x());
	    EventLoop::cancel_periodic_timer(image->m_fd);
	}
	else
	{
	    float starting = image->m_animation.starting();
	    float ending = image->m_animation.ending();
	    image->m_animation.set_easing_func(easing_easy);
	    image->m_animation.starting(ending);
	    image->m_animation.ending(starting);
	    image->m_animation.duration(300);
	    image->m_animation.start();
	}
    }
}

int main()
{
    EventLoop::init();

#ifdef HAVE_TSLIB
#ifdef HAVE_LIBPLANES
    KMSScreen kms;
    InputTslib input0("/dev/input/touchscreen0");
#else
    FrameBuffer fb("/dev/fb0");
    //InputEvDev input1("/dev/input/event4");
#endif
#else
    X11Screen screen(Size(800,480));
#endif

    MyWindow win;
    win.load();

    EventLoop::run();

    return 0;
}
