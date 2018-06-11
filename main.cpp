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

class MyImage : public Image
{
public:
    MyImage(const string& filename, int x = 0, int y = 0)
	: Image(filename, x, y)
    {
    }

    int handle(int event)
    {
	switch (event)
	{
	case EVT_MOUSE_UP:
	    cout << "image clicked" << endl;
	    //EventLoop::quit();
	    break;
	}

	return Image::handle(event);
    }
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
	    MyImage* box = new MyImage(os.str());
	    box->position(t * 200, (h() / 2) - (box->h() / 2));
	    add(box);
	    m_boxes[t] = box;

	    process(t, t * 200);
	}

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
		process(t, pos);
		m_boxes[t]->move(pos, m_boxes[t]->y());
	    }
	    else
	    {
		m_boxes[t]->position(pos, m_boxes[t]->y());
	    }
	}
    }

    void process(int t, int pos)
    {
	float scale = sliding_scale(this->w(), m_boxes[t]->w(), pos);
	m_boxes[t]->scale(scale);
    }

private:
    bool m_moving;
    int m_moving_x;
    int m_positions[NUM_ITEMS];
    MyImage* m_boxes[NUM_ITEMS];
};

static void timer_callback(int fd, void* data)
{
    cout << "timer_callback" << endl;
    EventLoop::start_timer(100, timer_callback, NULL);
}

int main()
{
    EventLoop::init();
#ifdef HAVE_TSLIB
    //FrameBuffer fb("/dev/fb0");
    KMSScreen kms;
    InputTslib input0("/dev/input/touchscreen0");
#else
    X11Screen screen(Size(800,480));
#endif

    //InputEvDev input1("/dev/input/event4");

    MyWindow win;
    win.load();

#if 0
    Button btn1("button 1", Point(100,200), Size(100,50));
    win.add(&btn1);
    btn1.focus(true);

    Slider slider1(0,100, Point(100,250), Size(200,50));
    win.add(&slider1);

    Combo combo1("combo 1", Point(100,300), Size(200,50));
    win.add(&combo1);

    Label label1("label 1", Point(100,350), Size(200,50));
    win.add(&label1);

    SimpleText text1("text 1", Point(100,400), Size(200,50));
    win.add(&text1);

    //EventLoop::start_timer(100, timer_callback, NULL);
#endif

    EventLoop::run();

    return 0;
}
