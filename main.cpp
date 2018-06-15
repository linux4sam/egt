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

class MyWindow;

class MyImage : public Image
{
public:
    MyImage(MyWindow& win, const string& filename, int x = 0, int y = 0)
	: Image(filename, x, y),
	  m_win(win),
	  m_fd(-1),
	  m_animation(0, 600, MyImage::animate, 1000, easing_snap, this)
    {}

    static void animate(float value, void* data)
    {
	MyImage* image = reinterpret_cast<MyImage*>(data);
	assert(image);

	image->scale(value);
    }

    static void timer_callback(int fd, void* data);

    int handle(int event)
    {
	switch (event)
	{
	case EVT_MOUSE_DOWN:
	{
	    if (!m_animation.running())
	    {
		m_animation.set_easing_func(easing_snap);
		m_animation.starting(scale());
		m_animation.ending(scale() + 0.2);
		m_animation.duration(500);
		m_animation.start();
		m_fd = EventLoop::start_periodic_timer(1, MyImage::timer_callback, this);
		return 1;
	    }

	    break;
	}
	case EVT_MOUSE_UP:
	    //EventLoop::quit();
	    break;
	}

	return Image::handle(event);
    }

private:
    MyWindow& m_win;
    int m_fd;
    Animation m_animation;
};

class MySimpleWindow : public SimpleWindow
{
    WidgetPositionAnimator* m_a1;
    WidgetPositionAnimator* m_a2;
    WidgetPositionAnimator* m_a3;

    ImageLabel* il1;
    ImageLabel* il2;
    ImageLabel* il3;
    Label* l1;

public:
    MySimpleWindow(int w, int h)
	: SimpleWindow(w, h)
    {
    }

    virtual void exit()
    {
	SimpleWindow::exit();

	m_a1->reset();
	m_a2->reset();
	m_a3->reset();
    }

    virtual void enter()
    {
	SimpleWindow::enter();

	m_a1->start();
	m_a2->start();
	m_a3->start();
    }

    virtual int load()
    {
	Image* img = new Image("background2.png");
	add(img);

	il1 = new ImageLabel("day.png",
			     "Day",
			     Point(40,150),
			     Size(200,64),
			     Font(32));
	il1->fgcolor(Color(0x80808055));
	add(il1);
	il2 = new ImageLabel("night.png",
			     "Night",
			     Point(40,214),
			     Size(200,64),
			     Font(32));
	il2->fgcolor(Color(0x80808055));
	add(il2);
	il3 = new ImageLabel("vacation.png",
			     "Vacation",
			     Point(40,278),
			     Size(200,64),
			     Font(32));
	add(il3);

	m_a1 = new WidgetPositionAnimator({il1, il2, il3},
					  WidgetPositionAnimator::CORD_X,
					  -200, 40,
					  1500,
					  easing_snap);

	Radial* radial1 = new Radial(Point(800/2-350/2,480/2 - 350/2), Size(350,350));
	radial1->label(" F");
	add(radial1);

	m_a2 = new WidgetPositionAnimator(radial1,
					  WidgetPositionAnimator::CORD_Y,
					  -350, 480/2 - 350/2,
					  1000,
					  easing_snap);

	Slider* slider1 = new Slider(0, 100,
				     Point(700,100),
				     Size(50,300),
				     Slider::ORIENTATION_VERTICAL);
	add(slider1);
	slider1->position(50);

	l1 = new Label("Fan",
		       Point(700,390),
		       Size(50,64),
		       ALIGN_CENTER,
		       Font(16));
	add(l1);
	l1->fgcolor(Color::GRAY);

	m_a3 = new WidgetPositionAnimator({slider1,l1},
					  WidgetPositionAnimator::CORD_X,
					  800, 700,
					  1500,
					  easing_snap);

	m_a1->reset();
	m_a2->reset();
	m_a3->reset();

	return 0;
    }
};

class MyWindow : public MySimpleWindow
{
public:
    MyWindow()
	: MySimpleWindow(800,480),
	  m_moving(false)
    {}

        virtual void exit()
    {
	SimpleWindow::exit();
    }

    virtual void enter()
    {
	SimpleWindow::enter();
    }

    virtual int load()
    {
	Image* img = new Image("background.png");
	add(img);

	for (int t = 0; t < NUM_ITEMS; t++)
	{
	    stringstream os;
	    os << "_image" << t << ".png";
	    MyImage* box = new MyImage(*this, os.str());
	    add(box);

	    box->position(t * 200, (h() / 2) - (box->h() / 2));
	    m_boxes[t] = box;

	    scale_box(box, t * 200);
	}

	for (int t = 0; t < NUM_ITEMS; t++)
	    m_positions[t] = m_boxes[t]->x();

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
		scale_box(m_boxes[t], pos);
		m_boxes[t]->move(pos, m_boxes[t]->y());
	    }
	    else
	    {
		m_boxes[t]->position(pos, m_boxes[t]->y());
	    }
	}
    }

    void scale_box(MyImage* image, int pos)
    {
    	float scale = sliding_scale(this->w(), image->w(), pos);
	image->scale(scale);
    }

private:
    bool m_moving;
    int m_moving_x;
    int m_positions[NUM_ITEMS];
    MyImage* m_boxes[NUM_ITEMS];
};

static MyWindow* win1;
static MySimpleWindow* win2;
static MySimpleWindow* win3;

void MyImage::timer_callback(int fd, void* data)
{
    MyImage* image = reinterpret_cast<MyImage*>(data);
    assert(image);

    if (!image->m_animation.next())
    {
	//if (image->m_animation.starting() > image->m_animation.ending())
//	{
	image->m_win.scale_box(image, image->x());
	EventLoop::cancel_periodic_timer(image->m_fd);

	main_window()->exit();
	main_window() = win2;
	main_window()->enter();


/*	}
	else
	{
	float starting = image->m_animation.starting();
	float ending = image->m_animation.ending();
	image->m_animation.set_easing_func(easing_easy);
	image->m_animation.starting(ending);
	image->m_animation.ending(starting);
	image->m_animation.duration(300);
	image->m_animation.start();
	}
*/
    }
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
    //InputEvDev input1("/dev/input/event4");
#endif
#else
    X11Screen screen(Size(800,480));
#endif

    win1 = new MyWindow;
    win1->load();

    win2 = new MySimpleWindow(800,480);
    win2->load();

    struct MyButton : public ImageButton
    {
	MyButton(const std::string& image, const Point& point = Point(),
		 const Size& size = Size())
	    : ImageButton(image, point, size)
	{}

	int handle(int event)
	{
	    switch (event)
	    {
	    case EVT_MOUSE_DOWN:
	    {
		main_window()->exit();
		main_window() = win1;
		main_window()->enter();
		return 1;
	    }
	    }
	    return Button::handle(event);
	}
    };

    MyButton btn1("home.png", Point(10,10), Size(64,64));
    win2->add(&btn1);

    win3 = new MySimpleWindow(800,480);
    win3->load();

    PieChart pie2(Point(600,50), Size(200,200));
    win3->add(&pie2);

    std::map<std::string, float> data;
    data.insert(make_pair("truck", .25));
    data.insert(make_pair("car", .55));
    data.insert(make_pair("bike", .10));
    data.insert(make_pair("motorcycle", .10));
    pie2.data(data);

    EventLoop::run();

    return 0;
}
