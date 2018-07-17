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
#include <chrono>
#include <iomanip>

using namespace std;
using namespace mui;


template<class T>
struct ShowAnimation : public AnimationTimer
{
    ShowAnimation(T* widget)
	: AnimationTimer(480,400,1000, easing_rubber),
	  m_widget(widget)
    {}

    virtual void step(int value)
    {
	m_widget->move(m_widget->x(), value);
    }

    T* m_widget;
};

template<class T>
struct HideAnimation : public AnimationTimer
{
    HideAnimation(T* widget)
	: AnimationTimer(400, 480, 1000, easing_rubber),
	  m_widget(widget)
    {}

    virtual void step(int value)
    {
	m_widget->move(m_widget->x(), value);
    }

    T* m_widget;
};

class VideoWindow : public SoftwareVideo
{
public:
    VideoWindow(const Size& size, const string& filename)
	: SoftwareVideo(size),
	  m_moving(false)
    {
	move(0,0);
	scale(2.5);
	set_media(filename);
	play();
	set_volume(50);
    }

    void set_control_window(PlaneWindow* window)
    {
	show = new ShowAnimation<PlaneWindow>(window);
	hide = new HideAnimation<PlaneWindow>(window);
    }

    int handle(int event)
    {
	int ret = SimpleWindow::handle(event);
	if (ret)
	    return ret;

	switch (event)
	{
	case EVT_MOUSE_DBLCLICK:
	    if (scale() <= 1.0)
	    {
		move(0,0);
		scale(2.5);
		show->start();
	    }
	    else
	    {
		scale(1.0);
		hide->start();
	    }

	    return 1;
	case EVT_MOUSE_DOWN:
	    if (scale() <= 1.0)
	    {
		if (!m_moving)
		{
		    m_moving = true;
		    m_starting_point = mouse_position();
		    m_position = Point(x(),y());
		}
	    }

	    return 1;
	case EVT_MOUSE_UP:
	    m_moving = false;
	    return 1;
	case EVT_MOUSE_MOVE:
	    if (m_moving)
	    {
		Point diff = mouse_position() - m_starting_point;
		move(m_position.x + diff.x, m_position.y + diff.y);
		return 1;
	    }
	    break;
	}
	return 0;
    }

private:
    bool m_moving;
    Point m_starting_point;
    Point m_position;
    ShowAnimation<PlaneWindow>* show;
    HideAnimation<PlaneWindow>* hide;
};

int main(int argc, const char** argv)
{
#ifdef HAVE_TSLIB
#ifdef HAVE_LIBPLANES
    KMSScreen kms(false);
    InputTslib input0("/dev/input/touchscreen0");
#else
    FrameBuffer fb("/dev/fb0");
#endif
#else
    X11Screen screen(Size(800,480));
#endif

    if (argc != 2)
    {
	cerr << argv[0] << " FILENAME" << endl;
	return 1;
    }

    VideoWindow window(Size(320,192), argv[1]);

    PlaneWindow ctrlwindow(Size(600, 80));
    /*
      {
      Palette p(ctrlwindow.palette());
      p.set(Palette::BG, Palette::GROUP_NORMAL, Color(0x80808055));
      ctrlwindow.set_palette(p);
      }
    */

    ctrlwindow.position(100,400);
    window.add(&ctrlwindow);

    window.set_control_window(&ctrlwindow);

    HorizontalPositioner grid(0, 0, 600, 80, 5, Widget::ALIGN_CENTER);

    ImageButton* playbtn = new ImageButton("play.png","",Point(),Size(),false);
    playbtn->add_handler([&window](EventWidget* widget){
	    ImageButton* btn = dynamic_cast<ImageButton*>(widget);
	    if (btn->active())
		window.unpause();
	});
    ctrlwindow.add(playbtn);
    grid.add(playbtn);

    ImageButton* pausebtn = new ImageButton("pause.png", "", Point(), Size(), false);
    pausebtn->add_handler([&window](EventWidget* widget){
	    ImageButton* btn = dynamic_cast<ImageButton*>(widget);
	    if (btn->active())
		window.pause();
	});
    ctrlwindow.add(pausebtn);
    grid.add(pausebtn);

    Slider* position = new Slider(0, 100, Point(), Size(250,40), Slider::ORIENTATION_HORIZONTAL);
    Palette p(position->palette());
    p.set(Palette::HIGHLIGHT, Palette::GROUP_NORMAL, Color::BLUE);
    position->set_palette(p);
    ctrlwindow.add(position);
    position->disable(true);
    grid.add(position);

    PeriodicTimer postimer(200);
    postimer.add_handler([position,&window]() {
	    if (window.duration())
	    {
		double v = (double)window.position() / (double)window.duration() * 100.;
		position->position(v);
	    }
	    else
	    {
		position->position(0);
	    }
	});
    postimer.start();

    ImageButton* volumei = new ImageButton("volumeup.png","",Point(),Size(),false);
    ctrlwindow.add(volumei);
    grid.add(volumei);

    Slider* volume = new Slider(0, 100, Point(), Size(100,20), Slider::ORIENTATION_HORIZONTAL);
    volume->add_handler([&window](EventWidget* widget){
	    Slider* slider = dynamic_cast<Slider*>(widget);
	    window.set_volume(slider->position());
	});
    ctrlwindow.add(volume);
    volume->position(50);
    grid.add(volume);

    playbtn->disable(true);
    pausebtn->disable(false);

    window.add_handler([&window,playbtn,pausebtn](EventWidget* widget){
    	    if (window.playing())
	    {
		playbtn->disable(true);
		pausebtn->disable(false);
	    }
	    else
	    {
		playbtn->disable(false);
		pausebtn->disable(true);
	    }
	});

    grid.reposition();

    ctrlwindow.show();

    return EventLoop::run();
}
