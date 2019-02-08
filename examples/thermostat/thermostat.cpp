/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <asio.hpp>
#include <egt/ui>
#include <iostream>

using namespace std;
using namespace egt;

class TempRadial : public Radial
{
public:
    TempRadial(Frame& parent, const Rect& rect, int min, int max, int value = 0)
        : Radial(parent, rect, min, max, value)
    {}

    virtual void draw(Painter& painter, const Rect& rect) override
    {
        ignoreparam(rect);

        auto v = this->value_to_degrees(this->value());
        float linew = 10;
        float handle_radius = 20;

        auto color1 = this->palette().color(Palette::BG);
        color1.alpha(0x55);
        auto color2 = this->palette().color(Palette::HIGHLIGHT);
        auto color3 = this->palette().color(Palette::MID);

        float radius = this->w() / 2 - handle_radius - linew;
        float angle1 = to_radians<float>(-90, 0);
        float angle2 = to_radians<float>(-90, v);

        auto c = this->center();

        Painter::AutoSaveRestore sr(painter);

        // value arc
        painter.set_color(color2);
        painter.set_line_width(linew);
        painter.arc(Arc(c, radius, angle1, angle2));
        painter.stroke();

        // handle
        painter.set_color(color3);
        auto hp = c.point_on_circumference(radius, angle2);
        painter.arc(Arc(hp, handle_radius, 0., (float)(2 * M_PI)));
        painter.fill();

        // secondary value
        auto color4 = Color::RED;
        float angle3 = to_radians<float>(-90,
                                         this->value_to_degrees(this->value2()));
        painter.set_color(color4);
        painter.set_line_width(linew * 2);
        painter.arc(Arc(c, radius, angle3 - 0.01, angle3 + 0.01));
        painter.stroke();

        if (!m_text.empty())
            painter.draw_text(m_text, this->box(), this->palette().color(Palette::TEXT),
                              alignmask::CENTER, 0, Font(72));

        string current = "Current " + std::to_string(value2()) + "°";
        painter.draw_text(current, this->box(), this->palette().color(Palette::TEXT),
                          alignmask::CENTER | alignmask::BOTTOM, 80, Font(24));
    }

    virtual ~TempRadial()
    {
    }
};


class ThermostatWindow : public TopWindow
{
public:
    explicit ThermostatWindow(const Size& size = Size())
        : TopWindow(size),
          m_a1(-200, 20,
               std::chrono::milliseconds(1500),
               easing_snap),
          m_a2(-350, 480 / 2 - 350 / 2,
               std::chrono::seconds(1),
               easing_snap),
          m_a3(800, 700,
               std::chrono::milliseconds(1500),
               easing_snap),
          m_background(*this, Image("background.png")),
          m_logo(*this, Image("@microchip_logo_black.png")),
          m_title(*this, "Living Room",
                  Rect(Point(), Size(250, 64)),
                  alignmask::CENTER,
                  egt::Font(32, Font::weightid::BOLD)),
          m_radial1(*this, Rect(Point(w() / 2 - 350 / 2,
                                      -350),
                                Size(350, 350)),
                    0, 100),
          m_label1(Image("day.png"),
                   "Day",
                   Rect(Point(-200, 150),
                        Size(180, 64)),
                   egt::Font(30)),
          m_label2(Image("night.png"),
                   "Night",
                   Rect(Point(-200, 214),
                        Size(180, 64)),
                   egt::Font(30)),
          m_label3(Image("vacation.png"),
                   "Vacation",
                   Rect(Point(-200, 278),
                        Size(180, 64)),
                   egt::Font(30)),
          m_label4("Fan",
                   Rect(Point(800, 390),
                        Size(50, 64)),
                   alignmask::CENTER,
                   egt::Font(16)),
          m_slider1(Rect(Point(800, 100),
                         Size(50, 300)),
                    0, 100, 0,
                    orientation::VERTICAL)
    {
        flag_set(widgetmask::NO_BACKGROUND);

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

                return 1;
            }
            return 0;
        });

        add(&m_radial1);
        m_radial1.set_value(73);
        m_radial1.set_value2(65);

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
            if (event == eventid::POINTER_CLICK)
            {
                m_label1.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, global_palette().color(Palette::HIGHLIGHT));
                m_label2.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::GRAY);
                m_label3.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::GRAY);
                m_label1.damage();
                m_label2.damage();
                m_label3.damage();
            }

            return 0;
        });

        m_label2.on_event([this](eventid event)
        {
            if (event == eventid::POINTER_CLICK)
            {
                m_label2.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, global_palette().color(Palette::HIGHLIGHT));
                m_label1.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::GRAY);
                m_label3.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::GRAY);
                m_label1.damage();
                m_label2.damage();
                m_label3.damage();
            }

            return 0;
        });

        m_label3.on_event([this](eventid event)
        {
            if (event == eventid::POINTER_CLICK)
            {
                m_label3.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, global_palette().color(Palette::HIGHLIGHT));
                m_label1.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::GRAY);
                m_label2.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::GRAY);
                m_label1.damage();
                m_label2.damage();
                m_label3.damage();
            }

            return 0;
        });

        add(&m_label4);
        m_label4.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);

        add(&m_slider1);
        m_slider1.set_value(50);

        m_a1.on_change(std::bind(&ImageLabel::set_x, std::ref(m_label1), std::placeholders::_1));
        m_a1.on_change(std::bind(&ImageLabel::set_x, std::ref(m_label2), std::placeholders::_1));
        m_a1.on_change(std::bind(&ImageLabel::set_x, std::ref(m_label3), std::placeholders::_1));

        m_a2.on_change(std::bind(&Radial::set_y, std::ref(m_radial1), std::placeholders::_1));

        m_a3.on_change(std::bind(&Slider::set_x, std::ref(m_slider1), std::placeholders::_1));
        m_a3.on_change(std::bind(&Label::set_x, std::ref(m_label4), std::placeholders::_1));
    }

    virtual void show() override
    {
        TopWindow::show();

        m_a1.start();
        m_a2.start();
        m_a3.start();
    }

protected:

    PropertyAnimator m_a1;
    PropertyAnimator m_a2;
    PropertyAnimator m_a3;
    ImageLabel m_background;
    ImageLabel m_logo;
    Label m_title;
    TempRadial m_radial1;
    ImageLabel m_label1;
    ImageLabel m_label2;
    ImageLabel m_label3;
    Label m_label4;
    Slider m_slider1;
};

int main(int argc, const char** argv)
{
    Application app(argc, argv, "thermostat");

    ThermostatWindow win;
    win.show();

    return app.run();
}
