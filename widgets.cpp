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

class MyWindow : public SimpleWindow
{
public:
    MyWindow()
	: SimpleWindow(800,480)
    {}

    int handle(int event)
    {
	return SimpleWindow::handle(event);
    }
};

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

    //InputEvDev input1("/dev/input/event4");

    MyWindow win;

    Label label1("left align", Point(100,50), Size(200,40), Widget::ALIGN_LEFT | Widget::ALIGN_CENTER);
    win.add(&label1);

    Label label2("right align", Point(100,100), Size(200,40), Widget::ALIGN_RIGHT | Widget::ALIGN_CENTER);
    win.add(&label2);

    Label label3("top align", Point(100,150), Size(200,40), Widget::ALIGN_TOP | Widget::ALIGN_CENTER);
    win.add(&label3);

    Label label4("bottom align", Point(100,200), Size(200,40), Widget::ALIGN_BOTTOM | Widget::ALIGN_CENTER);
    win.add(&label4);

    Button btn1("button 1", Point(100,250), Size(100,40));
    win.add(&btn1);
    btn1.focus(true);

    Slider slider1(0,100, Point(100,300), Size(200,40));
    win.add(&slider1);

    Combo combo1("combo 1", Point(100,350), Size(200,40));
    win.add(&combo1);

    SimpleText text1("text 1", Point(100,400), Size(200,40));
    win.add(&text1);

    //CheckBox checkbox1("checkbox 1", Point(350,200), Size(200,50));
    //win.add(&checkbox1);

    ImageLabel imagelabel1("icons/bug.png",
			   "Bug",
			   Point(350,250),
			   Size(200,40));
    win.add(&imagelabel1);

    ImageLabel imagelabel2("icons/phone.png",
			   "Phone",
			   Point(350,300),
			   Size(200,40));
    win.add(&imagelabel2);

    PieChart pie1(Point(600,50), Size(200,200));
    std::map<std::string, float> data;
    data.insert(make_pair("truck", .25));
    data.insert(make_pair("car", .55));
    data.insert(make_pair("bike", .10));
    data.insert(make_pair("motorcycle", .10));
    pie1.data(data);
    win.add(&pie1);

    EventLoop::run();

    return 0;
}
