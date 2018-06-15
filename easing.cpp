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

using namespace std;
using namespace mui;

class MyImage : public Image
{
public:
    MyImage(const string& filename, int x = 0, int y = 0)
	: Image(filename, x, y)
    {
    }

private:
};

static Animation* animation = 0;

static easing_func curves[] = {
    easing_linear,
    easing_easy,
    easing_easy_slow,
    easing_extend,
    easing_drop,
    easing_drop_slow,
    easing_snap,
    easing_bounce,
    easing_bouncy,
    easing_rubber,
    easing_spring,
    easing_boing
};

static vector<string> curves_names = {
    "linear",
    "easy",
    "easy slow",
    "extend",
    "drop",
    "drop slow",
    "snap",
    "bounce",
    "bouncy",
    "rubber",
    "spring",
    "boing"
};

class MyListBox : public ListBox
{
public:
    MyListBox(const std::vector<std::string>& items,
	      const Point& point = Point(),
	      const Size& size = Size())
	: ListBox(items, point, size)
    {}

    void on_selected(int index)
    {
	animation->stop();
	animation->set_easing_func(curves[index]);
	animation->start();
    }
};

#define NUM_ITEMS 1

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

	MyListBox* list1 = new MyListBox(curves_names, Point(700,0), Size(100,480));
	add(list1);
	list1->selected(7);

	for (int t = 0; t < NUM_ITEMS; t++)
	{
	    MyImage* box = new MyImage("ball.png");
	    add(box);

	    box->position(600/2, -101);

	    m_boxes[t] = box;
	}

	return 0;
    }

    void move_item(int x)
    {
	for (int t = 0; t < NUM_ITEMS; t++)
	{
	    int pos = x;

	    Rect to(m_boxes[t]->box());
	    to.y = pos;
	    bool visible = Rect::is_intersect(Rect::merge(to,m_boxes[t]->box()), this->box());

	    if (visible)
	    {
		m_boxes[t]->move(m_boxes[t]->x(), pos);
	    }
	    else
	    {
		m_boxes[t]->position(m_boxes[t]->x(), pos);
	    }
	}
    }

private:
    bool m_moving;
    int m_moving_x;
    int m_positions[NUM_ITEMS];
    MyImage* m_boxes[NUM_ITEMS];
};

static MyWindow* window = 0;

static void timer_callback2(int fd, void* data)
{
    animation->start();
}

static void timer_callback(int fd, void* data)
{
    if (animation->running())
	animation->next();
    else
    {
	EventLoop::start_timer(1000, timer_callback2, NULL);
    }
}

static void animate(float value, void* data)
{
    window->move_item(value);
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
#endif
#else
    X11Screen screen(Size(800,480));
#endif

    window = new MyWindow;

    animation = new Animation(-101, 380, animate, 2000);

    window->load();

    EventLoop::start_periodic_timer(1, timer_callback, NULL);

    EventLoop::run();

    return 0;
}
