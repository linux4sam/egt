/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <math.h>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace egt;

template <class T>
class MyVideoWindow : public T
{
public:
    MyVideoWindow(const Size& size, const string& filename)
        : T(size),
          m_moving(false)
    {
        m_fscale = (double)main_screen()->size().w / (double)T::w();
        if (m_fscale <= 0)
            m_fscale = 1.0;

        T::move(Point(0, 0));
        T::set_scale(m_fscale);
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
        case eventid::POINTER_DBLCLICK:
            if (T::scale() <= 1.0)
            {
                T::move(Point(0, 0));
                T::set_scale(m_fscale);
            }
            else
            {
                T::scale(1.0);
            }

            return 1;
        case eventid::RAW_POINTER_DOWN:
            if (T::scale() <= 1.0)
            {
                if (!m_moving)
                {
                    m_moving = true;
                    m_starting_point = event::pointer().point;
                    m_position = Point(T::x(), T::y());
                }
            }

            return 1;
        case eventid::RAW_POINTER_UP:
            m_moving = false;
            return 1;
        case eventid::RAW_POINTER_MOVE:
            if (m_moving)
            {
                auto diff = event::pointer().point - m_starting_point;
                T::move(Point(m_position.x + diff.x, m_position.y + diff.y));
                return 1;
            }
            break;
        }
        return 0;
    }

private:
    bool m_moving;
    DisplayPoint m_starting_point;
    Point m_position;
    double m_fscale;
};

class FpsWindow : public Window
{
public:
    FpsWindow()
        : Window(Size(100, 50),
                 widgetflags(), pixel_format::argb8888)
    {
        m_label = new Label("FPS: 0",
                            Rect(Size(100, 50)),
                            alignmask::CENTER);
        m_label->palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::WHITE);
        add(m_label);
    }

    void set_text(const std::string& str)
    {
        m_label->set_text(str);
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

    Application app(argc, argv, "video");
    TopWindow win;

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

    window->set_name("video");
    win.add(window);

#if 1
    //#define FPS
#ifdef FPS
    FpsWindow fpslabel;
    win.add(&fpslabel);
    fpslabel.show();
#endif

    Window ctrlwindow(Size(600, 80));
    ctrlwindow.set_name("ctrl");
    window->add(&ctrlwindow);
    ctrlwindow.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color(0x80808055));

    ctrlwindow.move(Point((main_screen()->size().w / 2) - (ctrlwindow.w() / 2),
                          main_screen()->size().h - ctrlwindow.h()));

#if 0
    set_control_window(&ctrlwindow);
#endif

    HorizontalPositioner grid(Rect(Size(600, 80)), 5, alignmask::CENTER);
    grid.set_name("grid");
    ctrlwindow.add(&grid);

    ImageButton* playbtn = new ImageButton(Image(":play_png"));
    playbtn->set_boxtype(Theme::boxtype::none);
    grid.add(playbtn);

    playbtn->on_event([playbtn, window](eventid event)
    {
        ignoreparam(event);
        if (playbtn->active())
            window->unpause();
        return 0;
    });

    ImageButton* pausebtn = new ImageButton(Image(":pause_png"));
    pausebtn->set_boxtype(Theme::boxtype::none);
    grid.add(pausebtn);
    pausebtn->on_event([pausebtn, window](eventid event)
    {
        ignoreparam(event);
        if (pausebtn->active())
            window->pause();
        return 0;
    });

    Slider* position = new Slider(Rect(Size(150, 40)), 0, 100, 0, orientation::HORIZONTAL);
    grid.add(position);
    position->palette().set(Palette::HIGHLIGHT, Palette::GROUP_NORMAL, Color::BLUE);
    position->readonly();

    PeriodicTimer postimer(std::chrono::milliseconds(200));
    postimer.on_timeout([position, window
#ifdef FPS
                                   , &fpslabel
#endif
                                  ]()
    {
        if (window->duration())
        {
            double v = (double)window->position() / (double)window->duration() * 100.;
            position->set_value(v);
        }
        else
        {
            position->set_value(0);
        }

#ifdef FPS
        ostringstream ss;
        ss << "fps: " << window->fps();
        fpslabel.set_text(ss.str());
#endif
    });
    postimer.start();

    ImageButton* volumei = new ImageButton(Image(":volumeup_png"));
    volumei->set_boxtype(Theme::boxtype::none);
    grid.add(volumei);

    Slider* volume = new Slider(Rect(Size(100, 20)), 0, 100, 0, orientation::HORIZONTAL);
    grid.add(volume);
    volume->on_event([volume, window](eventid event)
    {
        ignoreparam(event);
        window->set_volume(volume->value());
        return 0;
    });
    volume->set_value(50);

    playbtn->disable();
    pausebtn->enable();

    window->on_event([window, playbtn, pausebtn](eventid event)
    {
        ignoreparam(event);
        if (window->playing())
        {
            playbtn->disable();
            pausebtn->enable();
        }
        else
        {
            playbtn->enable();
            pausebtn->disable();
        }
        return 0;
    });

    grid.reposition();

    ctrlwindow.show();
#endif

    window->show();

    return app.run();
}
