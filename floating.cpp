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
#include "tools.h"

using namespace std;
using namespace mui;

class MyWindow : public SimpleWindow
{
public:
    MyWindow()
	: SimpleWindow(Size(800,480), FLAG_NO_BACKGROUND),
	  m_img("background.png")
    {
	add(&m_img);
    }

    Image m_img;
};

class FloatingBox
{
public:
    FloatingBox(Widget* widget, int mx, int my)
	: m_widget(widget),
	  m_mx(mx),
	  m_my(my)
    {}

    /*
      virtual void draw(const Rect& rect)
      {
      // TODO: this logic needs to be pushed up into draw() caller
      Rect i = rect;
      if (Rect::is_intersect(box(), rect))
      i = Rect::intersect(rect, box());

      screen()->rect(i, m_color);
      }
    */

    virtual void next_frame()
    {
	int x = m_widget->x() + m_mx;
	int y = m_widget->y() + m_my;

	if (x + m_widget->w() >= main_window()->size().w)
	    m_mx *= -1;

	if (x < 0)
	    m_mx *= -1;

	if (y + m_widget->h() >= main_window()->size().h)
	    m_my *= -1;

	if (y < 0)
	    m_my *= -1;

	m_widget->move(x, y);
    }

protected:
    Widget* m_widget;
    int m_mx;
    int m_my;
};

static vector<FloatingBox*> boxes;

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
    win.active(true);

    int f = 2;

    vector<std::pair<int,int>> moveparms = {
	std::make_pair(1 * f, 2 * f),
	std::make_pair(3* f, -2 * f),
	std::make_pair(-3 * f, 2 * f),
	std::make_pair(-3 * f, 3 * f),
	std::make_pair(2 * f, 3 * f),
	std::make_pair(2 * f, 2 * f),
	std::make_pair(4 * f, 2 * f),
	std::make_pair(-4 * f, 2 * f),
    };

    uint32_t SOFT_COUNT = 5;

    // software
    for (uint32_t x = 0; x < SOFT_COUNT;x++)
    {
	stringstream os;
	os << "_image" << x << ".png";
	Image* image = new Image(os.str(), 100, 100);
	boxes.push_back(new FloatingBox(image, moveparms[x].first, moveparms[x].second));
	win.add(image);
    }

#ifdef HAVE_LIBPLANES
    int total = KMSScreen::instance()->count_planes(DRM_PLANE_TYPE_OVERLAY);
#else
    int total = 3;
#endif

#ifdef HAVE_LIBPLANES
    // hardware (or emulated)
    for (uint32_t x = SOFT_COUNT; x < SOFT_COUNT + total; x++)
    {
	stringstream os;
	os << "_image" << x << ".png";
	Image* image = new Image(os.str(), 0, 0);
	PlaneWindow* plane = new PlaneWindow(Size(image->w(), image->h()));
	plane->add(image);
	plane->active(true);
	plane->position(100,100);
	boxes.push_back(new FloatingBox(plane, moveparms[x].first, moveparms[x].second));
    }
#endif

    struct MoveTimer : public PeriodicTimer
    {
	MoveTimer()
	    : PeriodicTimer(33)
	{}

	void timeout()
	{
	    for (auto i : boxes)
		i->next_frame();
	}
    } timer;

    timer.start();

    Label label1("CPU: -",
		 Point(40, win.size().h-40),
		 Size(100, 40),
		 Widget::ALIGN_LEFT | Widget::ALIGN_CENTER);
    label1.fgcolor(Color::WHITE);
    win.add(&label1);

    struct CPUTimer: public PeriodicTimer
    {
	CPUTimer(Label& label)
	    : PeriodicTimer(1000),
	      m_label(label)
	{}

	void timeout()
	{
	    m_tools.updateCpuUsage();

	    ostringstream ss;
	    ss << "CPU: " << (int)m_tools.cpu_usage[0] << "%";
	    m_label.text(ss.str());
	}

	Label& m_label;
	Tools m_tools;
    };

    CPUTimer cputimer(label1);
    cputimer.start();

    EventLoop::run();

    return 0;
}
