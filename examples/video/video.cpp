/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include <mui/ui>
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

#if 0
template<class T>
struct ShowAnimation : public experimental::AnimationTimer
{
    ShowAnimation(T* widget)
        : AnimationTimer(480, 400, std::chrono::seconds(1), easing_rubber),
          m_widget(widget)
    {}

    virtual void step(int value)
    {
        m_widget->move(Point(m_widget->x(), value));
    }

    T* m_widget;
};

template<class T>
struct HideAnimation : public experimental::AnimationTimer
{
    HideAnimation(T* widget)
        : AnimationTimer(400, 480, std::chrono::seconds(1), easing_rubber),
          m_widget(widget)
    {}

    virtual void step(int value)
    {
        m_widget->move(Point(m_widget->x(), value));
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
#endif

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

        T::move(Point(0, 0));
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
                T::move(Point(0, 0));
                T::scale(m_fscale);
#if 0
                show->start();
#endif
            }
            else
            {
                T::scale(1.0);
#if 0
                hide->start();
#endif
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
                T::move(Point(m_position.x + diff.x, m_position.y + diff.y));
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
                      widgetmask::WINDOW_DEFAULT, DRM_FORMAT_ARGB8888)
    {
        m_label = new Label("FPS: 0",
                            Point(0, 0),
                            Size(100, 50),
                            alignmask::CENTER);
        m_label->palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::WHITE);
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
    if (argc != 3)
    {
        cerr << argv[0] << " TYPE FILENAME" << endl;
        return 1;
    }

    Application app(false);

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
        window = new MyVideoWindow<SoftwareVideo>(Size(320, 192), argv[2]);
    //window = new MyVideoWindow<SoftwareVideo>(Size(960, 540), argv[2]);
    else
    {
        cerr << "unknown type: " << argv[1] << endl;
        return 1;
    }

    window->name("video");

#if 1
#ifdef FPS
    FpsWindow fpslabel;
    fpslabel.show();
#endif

    PlaneWindow ctrlwindow(Size(600, 80));
    ctrlwindow.name("ctrl");
    window->add(&ctrlwindow);
    ctrlwindow.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color(0x80808055));

    ctrlwindow.move(Point((KMSScreen::instance()->size().w / 2) - (ctrlwindow.w() / 2),
                          KMSScreen::instance()->size().h - ctrlwindow.h()));

#if 0
    set_control_window(&ctrlwindow);
#endif

    HorizontalPositioner grid(Point(0, 0), Size(600, 80), 5, alignmask::CENTER);
    grid.name("grid");
    ctrlwindow.add(&grid);

    ImageButton* playbtn = new ImageButton(":play_png", "", Point(), Size(), widgetmask::NO_BORDER);
    grid.add(playbtn);

    playbtn->add_handler([window](EventWidget * widget, int event)
    {
        ignoreparam(event);
        ImageButton* btn = dynamic_cast<ImageButton*>(widget);
        if (btn->active())
            window->unpause();
    });

    ImageButton* pausebtn = new ImageButton(":pause_png", "", Point(), Size(), widgetmask::NO_BORDER);
    grid.add(pausebtn);
    pausebtn->add_handler([window](EventWidget * widget, int event)
    {
        ignoreparam(event);
        cout << "pause button" << endl;
        ImageButton* btn = dynamic_cast<ImageButton*>(widget);
        if (btn->active())
            window->pause();
    });

    Slider* position = new Slider(0, 100, Point(), Size(150, 40), orientation::HORIZONTAL);
    grid.add(position);
    position->palette().set(Palette::HIGHLIGHT, Palette::GROUP_NORMAL, Color::BLUE);
    position->disable(true);

    PeriodicTimer postimer(std::chrono::milliseconds(200));
    postimer.add_handler([position, window
#ifdef FPS
                                    , &fpslabel
#endif
                                   ]()
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

#ifdef FPS
        ostringstream ss;
        ss << "fps: " << window->fps();
        fpslabel.text(ss.str());
#endif
    });
    postimer.start();

    ImageButton* volumei = new ImageButton(":volumeup_png", "", Point(), Size(), widgetmask::NO_BORDER);
    grid.add(volumei);

    Slider* volume = new Slider(0, 100, Point(), Size(100, 20), orientation::HORIZONTAL);
    grid.add(volume);
    volume->add_handler([window](EventWidget * widget, int event)
    {
        ignoreparam(event);
        Slider* slider = dynamic_cast<Slider*>(widget);
        window->set_volume(slider->position());
    });
    volume->position(50);

    playbtn->disable(true);
    pausebtn->disable(false);

    window->add_handler([window, playbtn, pausebtn](EventWidget * widget, int event)
    {
        ignoreparam(widget);
        ignoreparam(event);
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

    window->show();

    return app.run();
}
