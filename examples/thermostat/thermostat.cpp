/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include <mui/ui>
#include "asio.hpp"
#include <iostream>

using namespace std;
using namespace mui;

class ThermostatWindow : public Window
{
public:
    ThermostatWindow(const Size& size = Size())
        : Window(size),
          m_a1(-200, 40,
               std::chrono::milliseconds(1500),
               easing_snap),
          m_a2(-350, 480 / 2 - 350 / 2,
               std::chrono::seconds(1),
               easing_snap),
          m_a3(800, 700,
               std::chrono::milliseconds(1500),
               easing_snap),
          m_radial1(Rect(Point(800 / 2 - 350 / 2,
                               480 / 2 - 350 / 2),
                         Size(350, 350))),
          m_label1("day.png",
                   "Day",
                   Rect(Point(40, 150),
                        Size(200, 64)),
                   mui::Font(32)),
          m_label2("night.png",
                   "Night",
                   Rect(Point(40, 214),
                        Size(200, 64)),
                   mui::Font(32)),
          m_label3("vacation.png",
                   "Vacation",
                   Rect(Point(40, 278),
                        Size(200, 64)),
                   mui::Font(32)),
          m_label4("Fan",
                   Rect(Point(700, 390),
                        Size(50, 64)),
                   alignmask::CENTER,
                   mui::Font(16)),
          m_slider1(0, 100,
                    Rect(Point(700, 100),
                         Size(50, 300)),
                    orientation::VERTICAL)

    {
        Image* img = new Image("background.png");
        add(img);

        add(&m_radial1);
        m_radial1.label(" F");
        m_radial1.value(300);

        m_label1.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color(0x80808055))
        .set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);
        add(&m_label1);

        m_label2.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color(0x80808055))
        .set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);
        add(&m_label2);

        m_label3.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);
        add(&m_label3);

        add(&m_label4);
        m_label4.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::GRAY)
        .set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);

        add(&m_slider1);
        m_slider1.position(50);

        m_a1.on_change(std::bind(&ImageLabel::movex, std::ref(m_label1), std::placeholders::_1));
        m_a1.on_change(std::bind(&ImageLabel::movex, std::ref(m_label2), std::placeholders::_1));
        m_a1.on_change(std::bind(&ImageLabel::movex, std::ref(m_label3), std::placeholders::_1));

        m_a2.on_change(std::bind(&Radial::movey, std::ref(m_radial1), std::placeholders::_1));

        m_a3.on_change(std::bind(&Slider::movex, std::ref(m_slider1), std::placeholders::_1));
        m_a3.on_change(std::bind(&Label::movex, std::ref(m_label4), std::placeholders::_1));
    }

    virtual void show() override
    {
        Window::show();

        m_a1.start();
        m_a2.start();
        m_a3.start();
    }

protected:

    experimental::PropertyAnimator m_a1;
    experimental::PropertyAnimator m_a2;
    experimental::PropertyAnimator m_a3;
    Radial m_radial1;
    ImageLabel m_label1;
    ImageLabel m_label2;
    ImageLabel m_label3;
    Label m_label4;
    Slider m_slider1;
};

int main()
{
    Application app;

    set_image_path("../share/mui/examples/thermostat/");

    ThermostatWindow win;
    win.show();

    asio::signal_set signals(app.event().io(), SIGQUIT);
    signals.async_wait([&win](const asio::error_code & error, int signal_number)
    {
        ignoreparam(signal_number);
        if (error)
            return;
        win.save_children_to_file();
    });

    return app.run();
}
