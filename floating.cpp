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

class Box : public Widget
{
public:
    Box(int x, int y, int w, int h, int mx, int my, const Color& color)
	: Widget(x, y, w, h),
	  m_color(color),
	  m_mx(mx),
	  m_my(my)
    {}

    virtual void draw(const Rect& rect)
    {
	Rect i = rect;
	if (Rect::is_intersect(box(), rect))
	    i = Rect::intersect(rect, box());

	screen()->rect(i, m_color);
    }

    virtual void next_frame()
    {
	int x = this->x() + m_mx;
	int y = this->y() + m_my;

	if (x + w() >= screen()->size().w)
	    m_mx *= -1;

	if (x < 0)
	    m_mx *= -1;

	if (y + h() >= screen()->size().h)
	    m_my *= -1;

	if (y < 0)
	    m_my *= -1;

	move(x, y);
    }

protected:
    Color m_color;
    int m_mx;
    int m_my;
};

static vector<Box*> boxes;

static void timer_callback(int fd, void* data)
{
    for (auto i : boxes)
	i->next_frame();
}

int main()
{
    EventLoop::init();
#ifdef HAVE_TSLIB
#ifdef HAVE_LIBPLANES
    KMSScreen kms;
    InputTslib input0("/dev/input/touchscreen0");

    KMSOverlayScreen overlay(kms.allocate_overlay(Rect(100,100,200,200)));

    PlaneWindow win2(&overlay);
    win2.position(200,200);
    win2.active(true);

    Color colorz(255,0,0,0x55);
    Box* boxz1 = new Box(0,0,100,100,1,2,colorz);
    win2.add(boxz1);

#else
    FrameBuffer fb("/dev/fb0");
#endif
#else
    X11Screen screen(Size(800,480));
#endif

    MyWindow win;
    win.active(true);

    Color color1(255,0,0,0x55);
    Box* box1 = new Box(0,0,100,100,1,2,color1);
    boxes.push_back(box1);
    win.add(box1);

    Color color2(0,0,255,0x55);
    Box* box2 = new Box(100,100,100,100,3,-2,color2);
        boxes.push_back(box2);
    win.add(box2);

    Color color3(0,255,0,0x55);
    Box* box3 = new Box(200,200,100,100,-3,2,color3);
        boxes.push_back(box3);
    win.add(box3);

    Color color4(0xc0,0xc0,0xc0,0x55);
    Box* box4 = new Box(300,300,100,100,-3,3,color4);
        boxes.push_back(box4);
    win.add(box4);

    Color color5(0xff,0xff,0x00,0x55);
    Box* box5 = new Box(400,300,100,100,3,3,color5);
    boxes.push_back(box5);
    win.add(box5);

    EventLoop::start_periodic_timer(40, timer_callback, NULL);

    EventLoop::run();

    return 0;
}
