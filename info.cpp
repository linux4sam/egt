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

class MyButton : public ImageButton
{
public:
    MyButton(const string& filename,
	     const string& label,
	     int x = 0,
	     int y = 0,
	     bool border = true)
	: ImageButton(filename, label, Point(x, y), Size(), border)
    {}

    int handle(int event)
    {
	switch (event)
	{
	case EVT_MOUSE_DOWN:
	{
	    cout << "button pressed" << endl;
	    break;
	}
	case EVT_MOUSE_UP:
	    break;
	}

	return ImageButton::handle(event);
    }
};

class MyWindow : public SimpleWindow
{
public:
    MyWindow(int w, int h)
	: SimpleWindow(w, h)
    {}

};

class Box : public Widget
{
public:
    Box(int x, int y, int w, int h, const Color& color)
	: Widget(x,y,w,h),
	  m_color(color)

    {}

    virtual void draw(const Rect& rect)
    {
	// TODO: this logic needs to be pushed up into draw() caller
	Rect i = rect;
	if (Rect::is_intersect(box(), rect))
	    i = Rect::intersect(rect, box());

	screen()->rect(i, m_color);
    }

protected:
    Color m_color;
};

int main()
{
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

    MyWindow win(800, 480);
    win.bgcolor(Color::LIGHTBLUE);

    StaticGrid grid(0, 60, 800, 330, 4, 2, 10);

    MyButton b1("infores/sound.png", "Sound");
    win.add(&b1);
    grid.add(&b1, 0, 0);

    MyButton b2("infores/clock.png", "Clock");
    win.add(&b2);
    grid.add(&b2, 1, 0);

    MyButton b3("infores/bluetooth.png", "Bluetooth");
    win.add(&b3);
    grid.add(&b3, 2, 0);

    MyButton b4("infores/phone.png", "Phone");
    win.add(&b4);
    grid.add(&b4, 3, 0);

    MyButton b5("infores/apps.png", "Mobile Apps");
    win.add(&b5);
    grid.add(&b5, 0, 1);

    MyButton b6("infores/navigation.png", "Navigation");
    win.add(&b6);
    grid.add(&b6, 1, 1);

    MyButton b7("infores/general.png", "General");
    win.add(&b7);
    grid.add(&b7, 2, 1);

#if 0
    PlaneWindow plane(800, 60);
    plane.position(0,0);
    plane.bgcolor(Color::BLACK);
    plane.active(true);

    Image i1("infores/home.png",
	     5,5);
    plane.add(&i1);

    Label l1("12:05", Point(320, 0), Size(100, 60), Label::ALIGN_CENTER, Font("Arial", 32));
    l1.fgcolor(Color::WHITE);
    plane.add(&l1);

    Label l2("48°", Point(420, 0), Size(100, 60), Label::ALIGN_CENTER, Font("Arial", 24));
    l2.fgcolor(Color::WHITE);
    plane.add(&l2);

    Image i2("infores/wifi.png",
	     800-80,5);
    plane.add(&i2);
#endif

    Box box1(0,0,800, 60, Color::BLACK);
    win.add(&box1);

    Image i1("infores/home.png",
	     5,5);
    win.add(&i1);

    Label l1("12:05", Point(320, 0), Size(100, 60), Label::ALIGN_CENTER, Font("Arial", 32));
    l1.fgcolor(Color::WHITE);
    win.add(&l1);

    Label l2("48°", Point(420, 0), Size(100, 60), Label::ALIGN_CENTER, Font("Arial", 24));
    l2.fgcolor(Color::WHITE);
    win.add(&l2);

    Image i2("infores/wifi.png",
	     800-50, 10);
    win.add(&i2);

    Box box2(0,390,800, 90, Color::LIGHTGRAY);
    win.add(&box2);

    StaticGrid grid2(0, 390, 800, 90, 5, 1, 0);

    MyButton bb1("infores/audio_s.png", "Audio", 0, 0, false);
    win.add(&bb1);
    grid2.add(&bb1, 0, 0);

    MyButton bb2("infores/climate_s.png", "Climate", 0, 0, false);
    win.add(&bb2);
    grid2.add(&bb2, 1, 0);

    MyButton bb3("infores/navigation_s.png", "Nav", 0, 0, false);
    win.add(&bb3);
    grid2.add(&bb3, 2, 0);

    MyButton bb4("infores/phone_s.png", "Phone", 0, 0, false);
    win.add(&bb4);
    grid2.add(&bb4, 3, 0);

    MyButton bb5("infores/apps_s.png", "Apps", 0, 0, false);
    win.add(&bb5);
    grid2.add(&bb5, 4, 0);

    EventLoop::run();

    return 0;
}
