/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include "asio.hpp"
#include <iostream>

using namespace std;
using namespace egt;

class ThermostatWindow : public Window
{
public:
    explicit ThermostatWindow(const Size& size = Size())
        : Window(size),
          m_a1(-200, 20,
               std::chrono::milliseconds(1500),
               easing_snap),
          m_a2(-350, 480 / 2 - 350 / 2,
               std::chrono::seconds(1),
               easing_snap),
          m_a3(800, 700,
               std::chrono::milliseconds(1500),
               easing_snap),
          m_background(*this, "background.png"),
          m_logo(*this, "@microchip_logo_black.png"),
          m_title(*this, "Living Room",
                  Rect(Point(), Size(250, 64)),
                  alignmask::CENTER,
                  egt::Font(32, Font::weightid::BOLD)),
          m_radial1(Rect(Point(800 / 2 - 350 / 2,
                               480 / 2 - 350 / 2),
                         Size(350, 350)),
                    0, 100),
          m_label1("day.png",
                   "Day",
                   Rect(Point(40, 150),
                        Size(180, 64)),
                   egt::Font(30)),
          m_label2("night.png",
                   "Night",
                   Rect(Point(40, 214),
                        Size(180, 64)),
                   egt::Font(30)),
          m_label3("vacation.png",
                   "Vacation",
                   Rect(Point(40, 278),
                        Size(180, 64)),
                   egt::Font(30)),
          m_label4("Fan",
                   Rect(Point(700, 390),
                        Size(50, 64)),
                   alignmask::CENTER,
                   egt::Font(16)),
          m_slider1(0, 100,
                    Rect(Point(700, 100),
                         Size(50, 300)),
                    orientation::VERTICAL)
    {
        m_logo.set_align(alignmask::LEFT | alignmask::TOP, 10);

        m_title.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);
        m_title.set_align(alignmask::CENTER | alignmask::TOP, 10);

        m_radial1.on_event([this](eventid event)
        {
            if (event == eventid::PROPERTY_CHANGED)
            {
                auto text = std::to_string((int)m_radial1.value()) + "°";
                m_radial1.text(text);

                if (m_radial1.value() > m_radial1.value2())
                    m_radial1.palette().set(Palette::HIGHLIGHT, Palette::GROUP_NORMAL, Color::ORANGE);
                else
                    m_radial1.reset_palette();
            }
        });

        add(&m_radial1);
        m_radial1.value(73);
        m_radial1.value2(65);

        m_label1.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);
        add(&m_label1);

        m_label2.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);
        add(&m_label2);

        m_label3.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);
        add(&m_label3);

        m_label1.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::GRAY);
        m_label2.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::GRAY);
        m_label3.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::GRAY);

        m_label1.on_event([this](eventid event)
        {
            if (event == eventid::MOUSE_DOWN)
            {
                m_label1.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, global_palette().color(Palette::HIGHLIGHT));
                m_label2.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::GRAY);
                m_label3.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::GRAY);
                m_label1.damage();
                m_label2.damage();
                m_label3.damage();
            }
        });

        m_label2.on_event([this](eventid event)
        {
            if (event == eventid::MOUSE_DOWN)
            {
                m_label2.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, global_palette().color(Palette::HIGHLIGHT));
                m_label1.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::GRAY);
                m_label3.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::GRAY);
                m_label1.damage();
                m_label2.damage();
                m_label3.damage();
            }
        });

        m_label3.on_event([this](eventid event)
        {
            if (event == eventid::MOUSE_DOWN)
            {
                m_label3.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, global_palette().color(Palette::HIGHLIGHT));
                m_label1.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::GRAY);
                m_label2.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::GRAY);
                m_label1.damage();
                m_label2.damage();
                m_label3.damage();
            }
        });

        add(&m_label4);
        m_label4.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);

        add(&m_slider1);
        m_slider1.position(50);

        m_a1.on_change(std::bind(&ImageLabel::set_x, std::ref(m_label1), std::placeholders::_1));
        m_a1.on_change(std::bind(&ImageLabel::set_x, std::ref(m_label2), std::placeholders::_1));
        m_a1.on_change(std::bind(&ImageLabel::set_x, std::ref(m_label3), std::placeholders::_1));

        m_a2.on_change(std::bind(&Radial<int>::set_y, std::ref(m_radial1), std::placeholders::_1));

        m_a3.on_change(std::bind(&Slider::set_x, std::ref(m_slider1), std::placeholders::_1));
        m_a3.on_change(std::bind(&Label::set_x, std::ref(m_label4), std::placeholders::_1));
    }

    virtual void show() override
    {
        Window::show();

        m_a1.start();
        m_a2.start();
        m_a3.start();
    }

protected:

    PropertyAnimator m_a1;
    PropertyAnimator m_a2;
    PropertyAnimator m_a3;
    Image m_background;
    Image m_logo;
    Label m_title;
    Radial<int> m_radial1;
    ImageLabel m_label1;
    ImageLabel m_label2;
    ImageLabel m_label3;
    Label m_label4;
    Slider m_slider1;
};

int main(int argc, const char** argv)
{
    Application app(argc, argv);

    set_image_path("../share/egt/examples/thermostat/");

    ThermostatWindow win;
    win.show();

    return app.run();
}
