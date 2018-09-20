/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include <mui/ui.h>
#include <math.h>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <libintl.h>
#include <locale.h>

using namespace std;
using namespace mui;

class MainWindow;
class ChildWindow;

MainWindow* win1;
ChildWindow* win2;

#define _(String) gettext(String)

class MyButton : public ImageButton
{
public:
    MyButton(const string& filename,
	     const string& label,
	     int x = 0,
	     int y = 0,
	     bool border = true)
	: ImageButton(filename, label, Point(x, y), Size(), border)
    {
	set_image_align(ALIGN_CENTER | ALIGN_TOP);
	set_label_align(ALIGN_CENTER | ALIGN_BOTTOM);
    }

    int handle(int event)
    {
	switch (event)
	{
	case EVT_MOUSE_DOWN:
	{
	    if (main_window() == reinterpret_cast<SimpleWindow*>(win1))
	    {
		main_window()->exit();
		main_window() = reinterpret_cast<SimpleWindow*>(win2);
		main_window()->enter();
	    }
	    else
	    {
		main_window()->exit();
		main_window() = reinterpret_cast<SimpleWindow*>(win1);
		main_window()->enter();
	    }
	    break;
	}
	case EVT_MOUSE_UP:
	    break;
	}

	return ImageButton::handle(event);
    }
};

class HomeImage : public Image
{
public:
    HomeImage(const string& filename,
	     int x = 0,
	     int y = 0)
	: Image(filename, x, y)
    {}

    int handle(int event)
    {
	switch (event)
	{
	case EVT_MOUSE_DOWN:
	{
	    if (main_window() == reinterpret_cast<SimpleWindow*>(win1))
	    {
		main_window()->exit();
		main_window() = reinterpret_cast<SimpleWindow*>(win2);
		main_window()->enter();
	    }
	    else
	    {
		main_window()->exit();
		main_window() = reinterpret_cast<SimpleWindow*>(win1);
		main_window()->enter();
	    }
	    break;
	}
	case EVT_MOUSE_UP:
	    break;
	}

	return Image::handle(event);
    }
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

static void top_menu(SimpleWindow* win)
{
#if 0
    PlaneWindow plane(800, 60);
    plane.position(0,0);
    plane.bgcolor(Color::BLACK);
    plane.active(true);

    Image i1("res/home.png", 5,5);
    plane.add(&i1);

    Label l1("12:05", Point(320, 0), Size(100, 60), Label::ALIGN_CENTER, Font("Arial", 32));
    l1.fgcolor(Color::WHITE);
    plane.add(&l1);

    Label l2("48°", Point(420, 0), Size(100, 60), Label::ALIGN_CENTER, Font("Arial", 24));
    l2.fgcolor(Color::WHITE);
    plane.add(&l2);

    Image i2("res/wifi.png",
	     800-80,5);
    plane.add(&i2);
#endif

    Box* box1 = new Box(0,0,800, 60, Color::BLACK);
    win->add(box1);

    HomeImage* i1 = new HomeImage("res/home.png", 5,5);
    win->add(i1);

    Label* l1 = new Label("", Point(320, 0), Size(100, 60), Label::ALIGN_CENTER, mui::Font("Arial", 32));
    l1->fgcolor(Color::WHITE);
    win->add(l1);

    struct TimeTimer : public PeriodicTimer
    {
	TimeTimer(Label& label)
	    : PeriodicTimer(100),
	      m_label(label)
	{}

	void timeout()
	{
	    auto now = std::chrono::system_clock::now();
	    time_t tt = std::chrono::system_clock::to_time_t(now);
	    tm local_tm = *localtime(&tt);

	    ostringstream ss;
	    ss << local_tm.tm_hour << ":" << std::setfill('0') << std::setw(2) << local_tm.tm_min;
	    m_label.text(ss.str());
	}

	Label& m_label;
    };

    TimeTimer* timer = new TimeTimer(*l1);

    timer->start();

    Label* l2 = new Label("48°", Point(420, 0), Size(100, 60), Label::ALIGN_CENTER, mui::Font("Arial", 24));
    l2->fgcolor(Color::WHITE);
    win->add(l2);

    Image* i2 = new Image("res/wifi.png", 800-50, 10);
    win->add(i2);
}

static void bottom_menu(SimpleWindow* win)
{
    Box* box2 = new Box(0,390,800, 90, Color::LIGHTGRAY);
    win->add(box2);

    StaticGrid grid2(0, 390, 800, 90, 5, 1, 0);

    MyButton* bb1 = new MyButton("res/audio_s.png", _("Audio"), 0, 0, false);
    win->add(bb1);
    grid2.add(bb1, 0, 0);

    MyButton* bb2 = new MyButton("res/climate_s.png", _("Climate"), 0, 0, false);
    win->add(bb2);
    grid2.add(bb2, 1, 0);

    MyButton* bb3 = new MyButton("res/navigation_s.png", _("Nav"), 0, 0, false);
    win->add(bb3);
    grid2.add(bb3, 2, 0);

    MyButton* bb4 = new MyButton("res/phone_s.png", _("Phone"), 0, 0, false);
    win->add(bb4);
    grid2.add(bb4, 3, 0);

    MyButton* bb5 = new MyButton("res/apps_s.png", _("Apps"), 0, 0, false);
    win->add(bb5);
    grid2.add(bb5, 4, 0);

    grid2.reposition();
}

class MainWindow : public SimpleWindow
{
public:
    MainWindow(const Size& size)
	: SimpleWindow(size)
    {
	palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::LIGHTBLUE);

	StaticGrid grid(0, 60, 800, 330, 4, 2, 10);

	MyButton* b1 = new MyButton("res/sound.png", _("Sound"));
	add(b1);
	grid.add(b1, 0, 0);

	MyButton* b2 = new MyButton("res/clock.png", _("Clock"));
	add(b2);
	grid.add(b2, 1, 0);

	MyButton* b3 = new MyButton("res/bluetooth.png", _("Bluetooth"));
	add(b3);
	grid.add(b3, 2, 0);

	MyButton* b4 = new MyButton("res/phone.png", _("Phone"));
	add(b4);
	grid.add(b4, 3, 0);

	MyButton* b5 = new MyButton("res/apps.png", _("Mobile Apps"));
	add(b5);
	grid.add(b5, 0, 1);

	MyButton* b6 = new MyButton("res/navigation.png", _("Navigation"));
	add(b6);
	grid.add(b6, 1, 1);

	MyButton* b7 = new MyButton("res/general.png", _("General"));
	add(b7);
	grid.add(b7, 2, 1);

	grid.reposition();

	top_menu(this);
	bottom_menu(this);
    }
};

class ChildWindow : public SimpleWindow
{
public:
    ChildWindow(const Size& size)
	: SimpleWindow(size)
    {
	palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::LIGHTBLUE);

	Label* label = new Label(_("Blank Screen"), Point(0, 60), Size(800, 330));
	add(label);

	top_menu(this);
	bottom_menu(this);
    }
};

int main()
{
    setlocale(LC_ALL, "");
    //bindtextdomain("info", ".");
    textdomain("info");

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

    win1 = new MainWindow(Size(800, 480));
    win2 = new ChildWindow(Size(800, 480));

    return EventLoop::run();
}
