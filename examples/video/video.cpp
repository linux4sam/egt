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

using namespace std;
using namespace mui;


template<class T>
struct ShowAnimation : public AnimationTimer
{
    ShowAnimation(T* widget)
        : AnimationTimer(480, 400, 1000, easing_rubber),
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

static ShowAnimation<PlaneWindow>* show;
static HideAnimation<PlaneWindow>* hide;

static void set_control_window(PlaneWindow* window)
{
    show = new ShowAnimation<PlaneWindow>(window);
    hide = new HideAnimation<PlaneWindow>(window);
}

template <class T>
class MyVideoWindow : public T
{
public:
    MyVideoWindow(const Size& size, const string& filename)
        : T(size),
          m_moving(false)
    {
        m_fscale = (double)KMSScreen::instance()->size().w / (double)T::w();
        if (m_fscale <= 0)
            m_fscale = 1.0;

        T::move(0, 0);
        T::scale(m_fscale);
        T::set_media(filename);
        T::play();
        T::set_volume(50);
    }

    int handle(int event)
    {
        int ret = T::handle(event);
        if (ret)
            return ret;

        switch (event)
        {
        case EVT_MOUSE_DBLCLICK:
            if (T::scale() <= 1.0)
            {
                T::move(0, 0);
                T::scale(m_fscale);
                show->start();
            }
            else
            {
                T::scale(1.0);
                hide->start();
            }

            return 1;
        case EVT_MOUSE_DOWN:
            if (T::scale() <= 1.0)
            {
                if (!m_moving)
                {
                    m_moving = true;
                    m_starting_point = mouse_position();
                    m_position = Point(T::x(), T::y());
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
                T::move(m_position.x + diff.x, m_position.y + diff.y);
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
    double m_fscale;
};

class FpsWindow : public PlaneWindow
{
public:
    FpsWindow()
        : PlaneWindow(Size(100, 50),
                      FLAG_WINDOW_DEFAULT, DRM_FORMAT_ARGB8888)
    {
        m_label = new Label("FPS: 0",
                            Point(0, 0),
                            Size(100, 50),
                            Widget::ALIGN_CENTER);
        m_label->fgcolor(Color::WHITE);
        add(m_label);
    }

    void text(const std::string& str)
    {
        m_label->text(str);
    }

protected:
    Label* m_label;
};

int main(int argc, const char** argv)
{
#ifdef HAVE_X11
    X11Screen screen(Size(800, 480));
#else
#ifdef HAVE_LIBPLANES
    KMSScreen kms(false);
#else
    FrameBuffer fb("/dev/fb0");
#endif
#ifdef HAVE_TSLIB
    InputTslib input0("/dev/input/touchscreen0");
#endif
#endif

    if (argc != 3)
    {
        cerr << argv[0] << " TYPE FILENAME" << endl;
        return 1;
    }

    VideoWindow* window = 0;
    if (argv[1] == string("v4l2"))
        window = new MyVideoWindow<V4L2SoftwareVideo>(Size(960, 720), argv[2]);
    else if (argv[1] == string("v4l2h"))
        window = new MyVideoWindow<V4L2HardwareVideo>(Size(960, 720), argv[2]);
    else if (argv[1] == string("raw"))
        window = new MyVideoWindow<RawSoftwareVideo>(Size(320, 192), argv[2]);
    else if (argv[1] == string("hardware"))
        window = new MyVideoWindow<HardwareVideo>(Size(960, 540), argv[2]);
    else if (argv[1] == string("software"))
        //	window = new MyVideoWindow<SoftwareVideo>(Size(320,192), argv[2]);
        window = new MyVideoWindow<SoftwareVideo>(Size(960, 540), argv[2]);
    else
    {
        cerr << "unknown type: " << argv[1] << endl;
        return 1;
    }

#if 1
    FpsWindow fpslabel;
    fpslabel.show();

    PlaneWindow ctrlwindow(Size(500, 80));

    {
        Palette p(ctrlwindow.palette());
        p.set(Palette::BG, Palette::GROUP_NORMAL, Color(0x80808055));
        ctrlwindow.set_palette(p);
    }

    ctrlwindow.position((KMSScreen::instance()->size().w / 2) - (ctrlwindow.w() / 2),
                        KMSScreen::instance()->size().h - ctrlwindow.h());
    window->add(&ctrlwindow);

    set_control_window(&ctrlwindow);

    HorizontalPositioner grid(0, 0, 600, 80, 5, Widget::ALIGN_CENTER);

    ImageButton* playbtn = new ImageButton(":play_png", "", Point(), Size(), false);
    playbtn->add_handler([window](EventWidget * widget)
    {
        ImageButton* btn = dynamic_cast<ImageButton*>(widget);
        if (btn->active())
            window->unpause();
    });
    ctrlwindow.add(playbtn);
    grid.add(playbtn);

    ImageButton* pausebtn = new ImageButton(":pause_png", "", Point(), Size(), false);
    pausebtn->add_handler([window](EventWidget * widget)
    {
        ImageButton* btn = dynamic_cast<ImageButton*>(widget);
        if (btn->active())
            window->pause();
    });
    ctrlwindow.add(pausebtn);
    grid.add(pausebtn);

    Slider* position = new Slider(0, 100, Point(), Size(150, 40), Slider::ORIENTATION_HORIZONTAL);
    Palette p(position->palette());
    p.set(Palette::HIGHLIGHT, Palette::GROUP_NORMAL, Color::BLUE);
    position->set_palette(p);
    ctrlwindow.add(position);
    position->disable(true);
    grid.add(position);

    PeriodicTimer postimer(200);
    postimer.add_handler([position, window, &fpslabel]()
    {
        if (window->duration())
        {
            double v = (double)window->position() / (double)window->duration() * 100.;
            position->position(v);
        }
        else
        {
            position->position(0);
        }

        ostringstream ss;
        ss << "fps: " << window->fps();
        fpslabel.text(ss.str());
    });
    postimer.start();

    ImageButton* volumei = new ImageButton(":volumeup_png", "", Point(), Size(), false);
    ctrlwindow.add(volumei);
    grid.add(volumei);

    Slider* volume = new Slider(0, 100, Point(), Size(100, 20), Slider::ORIENTATION_HORIZONTAL);
    volume->add_handler([window](EventWidget * widget)
    {
        Slider* slider = dynamic_cast<Slider*>(widget);
        window->set_volume(slider->position());
    });
    ctrlwindow.add(volume);
    volume->position(50);
    grid.add(volume);

    playbtn->disable(true);
    pausebtn->disable(false);

    window->add_handler([window, playbtn, pausebtn](EventWidget * widget)
    {
        if (window->playing())
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
#endif

    return EventLoop::run();
}
