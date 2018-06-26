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

class Keyboard : public PlaneWindow
{
public:
    Keyboard()
	: PlaneWindow(Size(800, 200)),
	  m_grid(0, 0, 800, 200, 10, 4, 5)
    {
	Palette p(palette());
	p.set(Palette::BG, Palette::GROUP_NORMAL, Color::BLACK);
	set_palette(p);

	vector<vector<string>> buttons = {
	    {"q","w","e","r","t","y","u","i","o","p" },
	    {"", "a","s","d","f","g","h","j","k","l" },
	    {"icons/arrow_up.png","z","x","c","v","b","n","m", "", "icons/shape_move_back.png"},
	    {"123", ",","","", "space", "","","","","."}
	};

	for (size_t r = 0; r < buttons.size(); r++)
	{
	    for (size_t c = 0; c < buttons[r].size(); c++)
	    {
		string label = buttons[r][c];
		if (label.empty())
		    continue;

		Button* b;

		if (label.find(".png") != string::npos)
		    b = new ImageButton(label);
		else
		    b = new Button(label);

		add(b);
		m_grid.add(b, c, r);
	    }
	}

	m_grid.reposition();
    }

protected:
    StaticGrid m_grid;
};

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

    SimpleWindow win1(Size(800,480));

    Label label1("left align", Point(100,50), Size(200,40), Widget::ALIGN_LEFT | Widget::ALIGN_CENTER);
    win1.add(&label1);

    Label label2("right align", Point(100,100), Size(200,40), Widget::ALIGN_RIGHT | Widget::ALIGN_CENTER);
    win1.add(&label2);

    Label label3("top align", Point(100,150), Size(200,40), Widget::ALIGN_TOP | Widget::ALIGN_CENTER);
    win1.add(&label3);

    Label label4("bottom align", Point(100,200), Size(200,40), Widget::ALIGN_BOTTOM | Widget::ALIGN_CENTER);
    win1.add(&label4);

    Button btn1("button 1", Point(100,250), Size(100,40));
    win1.add(&btn1);
    btn1.focus(true);

    Slider slider1(0,100, Point(100,300), Size(200,40));
    win1.add(&slider1);

    Slider slider2(0,100, Point(10,200), Size(40,200), Slider::ORIENTATION_VERTICAL);
    win1.add(&slider2);

    Combo combo1("combo 1", Point(100,350), Size(200,40));
    win1.add(&combo1);

    SimpleText text1("text 1", Point(100,400), Size(200,40));
    win1.add(&text1);

    vector<string> items = { "item 1", "item 2", "item3" };
    ListBox list1(items, Point(350,50), Size(200,200));
    win1.add(&list1);
    list1.selected(1);

    ImageLabel imagelabel1("icons/bug.png",
			   "Bug",
			   Point(350,250),
			   Size(200,40));
    win1.add(&imagelabel1);

    ImageLabel imagelabel2("icons/phone.png",
			   "Phone",
			   Point(350,300),
			   Size(200,40));
    win1.add(&imagelabel2);

    CheckBox checkbox1("checkbox 1", Point(350,350), Size(200,40));
    win1.add(&checkbox1);

    CheckBox checkbox2("checkbox 2", Point(350,400), Size(200,40));
    win1.add(&checkbox2);

    PieChart pie1(Point(600,50), Size(200,200));
    win1.add(&pie1);

    std::map<std::string, float> data;
    data.insert(make_pair("truck", .25));
    data.insert(make_pair("car", .55));
    data.insert(make_pair("bike", .10));
    data.insert(make_pair("motorcycle", .10));
    pie1.data(data);

    LevelMeter lp1(Point(600, 250), Size(50,100));
    win1.add(&lp1);

    AnalogMeter am1(Point(600, 280), Size(180,180));
    win1.add(&am1);

    Tools tools;
    PeriodicTimer cputimer(1000);
    cputimer.add_handler([&tools,&lp1,&am1]() {
	    tools.updateCpuUsage();
	    lp1.percent(tools.cpu_usage[0]);
	    am1.percent(tools.cpu_usage[0]);
	});
    cputimer.start();

    //Keyboard keyboard;
    //keyboard.show();

    return EventLoop::run();
}
