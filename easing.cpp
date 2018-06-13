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

#define NUM_ITEMS 1

class MyImage : public Image
{
public:
    MyImage(const string& filename, int x = 0, int y = 0)
	: Image(filename, x, y),
	  m_fd(-1)
    {
    }

    static void scaler_callback(int fd, void* data)
    {
	MyImage* image = reinterpret_cast<MyImage*>(data);
	assert(image);

	float scale = image->scale() + image->m_factor;
	if (image->m_factor > 0.0 && scale > image->m_starting_scale + 0.3)
	{
	    image->m_factor *= -1.0;
	}
	else if (image->m_factor < 0.0 && scale < image->m_starting_scale + 0.01)
	{
	    EventLoop::cancel_periodic_timer(image->m_fd);
	    image->m_fd = -1;
	    scale = image->m_starting_scale;
	}
	image->scale(scale);
    }

    int handle(int event)
    {
	switch (event)
	{
	case EVT_MOUSE_DOWN:
	{
	    if (m_fd == -1)
	    {
		m_factor = 0.07;
		m_starting_scale = scale();
		m_fd = EventLoop::start_periodic_timer(50, MyImage::scaler_callback, this);
		return 1;
	    }

	    break;
	}
	case EVT_MOUSE_UP:
	    cout << "image clicked" << endl;
	    //EventLoop::quit();
	    break;
	}

	return Image::handle(event);
    }

private:
    double m_factor;
    double m_starting_scale;
    int m_fd;
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
	    box->position((t * 200) - 200, (h() / 2) - (box->h() / 2));
	    add(box);
	    m_boxes[t] = box;

	    //process(t, t * 200);
	}

	return 0;
    }

    int handle(int event)
    {
	return SimpleWindow::handle(event);
    }

    void move_item(int x)
    {
	for (int t = 0; t < NUM_ITEMS; t++)
	{
	    int pos = x +  m_positions[t];

	    Rect to(m_boxes[t]->box());
	    to.x = pos;
	    bool visible = Rect::is_intersect(Rect::merge(to,m_boxes[t]->box()), this->box());

	    if (visible)
	    {
		//process(t, pos);
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

static Animation* animation = 0;
static MyWindow* window = 0;

static int curve_index = 0;
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
	animation->set_easing_func(curves[curve_index++]);

	EventLoop::start_timer(1000, timer_callback2, NULL);

	if (curve_index >= sizeof(curves)/sizeof(curves[0]))
	    curve_index = 0;
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
    window->load();

    //Button btn1("button 1", Point(100,200), Size(100,50));
    //window->add(&btn1);

    animation = new Animation(0, 600, animate, 2000);

    EventLoop::start_periodic_timer(1, timer_callback, NULL);

    EventLoop::run();

    return 0;
}
