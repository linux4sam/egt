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

class MyImage : public Image
{
public:
    MyImage(const string& filename, int x = 0, int y = 0)
	: Image(filename, x, y)
    {}

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
	: SimpleWindow(800,480)
    {
	// TODO: don't like this API one bit - changing pointer ownership
	Image* img = new Image("background.png");
	add(img);
    }

    int handle(int event)
    {
	return SimpleWindow::handle(event);
    }
};

class Box
{
public:
    Box(Widget* widget, int mx, int my)
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

static vector<Box*> boxes;

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

    vector<Color> colors = {
	Color(255,0,0,0x55),
	Color(0,0,255,0x55),
	Color(0,255,0,0x55),
	Color(0xc0,0xc0,0xc0,0x55),
	Color(0xff,0xff,0x00,0x55),
	Color(255,0,0,0x55),
    };

    int f = 2;

    vector<std::pair<int,int>> moveparms = {
	std::make_pair(1 * f,2 * f),
	std::make_pair(3* f,-2 * f),
	std::make_pair(-3 * f,2 * f),
	std::make_pair(-3 * f,3 * f),
	std::make_pair(2 * f,3 * f),
	std::make_pair(2,2),
    };

    // software
    for (uint32_t x = 0; x < 3;x++)
    {
	stringstream os;
	os << "_image" << x << ".png";
	Image* image = new Image(os.str(), 100, 100);
	boxes.push_back(new Box(image, moveparms[x].first, moveparms[x].second));
	win.add(image);
    }

#ifdef HAVE_LIBPLANES
    // hardware
    for (uint32_t x = 3;
	 x < 3 + KMSScreen::instance()->count_planes(DRM_PLANE_TYPE_OVERLAY); x++)
    {
	stringstream os;
	os << "_image" << x << ".png";
	Image* image = new Image(os.str(), 0, 0);
	PlaneWindow* plane = new PlaneWindow(image->w(), image->h());
	plane->add(image);
	plane->active(true);
	plane->position(100,100);
	boxes.push_back(new Box(plane, moveparms[x].first, moveparms[x].second));
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

    Label label1("CPU: 0%",
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
