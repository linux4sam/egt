/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/detail/alignment.h>
#include <asio.hpp>
#include <egt/ui>
#include <iostream>

using namespace std;
using namespace egt;



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
                  alignmask::center,
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
                   alignmask::center,
                   egt::Font(16)),
          m_slider1(Rect(Point(800, 100),
                         Size(50, 300)),
                    0, 100, 0,
                    orientation::vertical)
    {
        set_boxtype(Theme::boxtype::none);

        m_logo.set_align(alignmask::left | alignmask::top, 10);

        m_title.palette().set(Palette::ColorId::bg, Palette::GroupId::normal, Palette::transparent);
        m_title.set_align(alignmask::center | alignmask::top, 10);

        m_radial1.on_event([this](eventid event)
        {
            if (event == eventid::property_changed)
            {
                auto text = std::to_string((int)m_radial1.value()) + "°";
                m_radial1.text(text);

                if (m_radial1.value() > m_radial1.value2())
                    m_radial1.palette().set(Palette::ColorId::highlight, Palette::GroupId::normal, Palette::orange);
                else
                    m_radial1.reset_palette();

                return 1;
            }
            return 0;
        });

        add(m_radial1);
        m_radial1.set_value(73);
        m_radial1.set_value2(65);

        m_label1.palette().set(Palette::ColorId::bg, Palette::GroupId::normal, Palette::transparent);
        add(m_label1);

        m_label2.palette().set(Palette::ColorId::bg, Palette::GroupId::normal, Palette::transparent);
        add(m_label2);

        m_label3.palette().set(Palette::ColorId::bg, Palette::GroupId::normal, Palette::transparent);
        add(m_label3);

        m_label1.palette().set(Palette::ColorId::text, Palette::GroupId::normal, Palette::gray);
        m_label2.palette().set(Palette::ColorId::text, Palette::GroupId::normal, Palette::gray);
        m_label3.palette().set(Palette::ColorId::text, Palette::GroupId::normal, Palette::gray);

        m_label1.on_event([this](eventid event)
        {
            if (event == eventid::pointer_click)
            {
                m_label1.palette().set(Palette::ColorId::text, Palette::GroupId::normal, global_palette().color(Palette::ColorId::highlight));
                m_label2.palette().set(Palette::ColorId::text, Palette::GroupId::normal, Palette::gray);
                m_label3.palette().set(Palette::ColorId::text, Palette::GroupId::normal, Palette::gray);
                m_label1.damage();
                m_label2.damage();
                m_label3.damage();
            }

            return 0;
        });

        m_label2.on_event([this](eventid event)
        {
            if (event == eventid::pointer_click)
            {
                m_label2.palette().set(Palette::ColorId::text, Palette::GroupId::normal, global_palette().color(Palette::ColorId::highlight));
                m_label1.palette().set(Palette::ColorId::text, Palette::GroupId::normal, Palette::gray);
                m_label3.palette().set(Palette::ColorId::text, Palette::GroupId::normal, Palette::gray);
                m_label1.damage();
                m_label2.damage();
                m_label3.damage();
            }

            return 0;
        });

        m_label3.on_event([this](eventid event)
        {
            if (event == eventid::pointer_click)
            {
                m_label3.palette().set(Palette::ColorId::text, Palette::GroupId::normal, global_palette().color(Palette::ColorId::highlight));
                m_label1.palette().set(Palette::ColorId::text, Palette::GroupId::normal, Palette::gray);
                m_label2.palette().set(Palette::ColorId::text, Palette::GroupId::normal, Palette::gray);
                m_label1.damage();
                m_label2.damage();
                m_label3.damage();
            }

            return 0;
        });

        add(m_label4);
        m_label4.palette().set(Palette::ColorId::bg, Palette::GroupId::normal, Palette::transparent);

        add(m_slider1);
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
    Radial m_radial1;
    ImageLabel m_label1;
    ImageLabel m_label2;
    ImageLabel m_label3;
    Label m_label4;
    Slider m_slider1;
};

int main(int argc, const char** argv)
{
    Drawer<Radial>::set_draw([](Radial & widget, Painter & painter, const Rect & rect)
    {
        ignoreparam(rect);

        auto v = widget.value_to_degrees(widget.value());
        float linew = 10;
        float handle_radius = 20;

        auto color1 = widget.palette().color(Palette::ColorId::bg);
        color1.alpha(0x55);
        auto color2 = widget.palette().color(Palette::ColorId::highlight);
        auto color3 = widget.palette().color(Palette::ColorId::mid);

        float radius = widget.w() / 2 - handle_radius - linew;
        float angle1 = detail::to_radians<float>(-90, 0);
        float angle2 = detail::to_radians<float>(-90, v);

        auto c = widget.center();

        Painter::AutoSaveRestore sr(painter);

        // value arc
        painter.set(color2);
        painter.set_line_width(linew);
        painter.draw(Arc(c, radius, angle1, angle2));
        painter.stroke();

        // handle
        painter.set(color3);
        auto hp = c.point_on_circumference(radius, angle2);
        painter.draw(Arc(hp, handle_radius, 0., (float)(2 * M_PI)));
        painter.fill();

        // secondary value
        auto color4 = Palette::red;
        float angle3 = detail::to_radians<float>(-90,
                       widget.value_to_degrees(widget.value2()));
        painter.set(color4);
        painter.set_line_width(linew * 2);
        painter.draw(Arc(c, radius, angle3 - 0.01, angle3 + 0.01));
        painter.stroke();

        //text
        if (!widget.text().empty())
        {
            painter.set(widget.palette().color(Palette::ColorId::text));
            painter.set(Font(72));

            auto text_size = painter.text_size(widget.text());
            Rect target = detail::align_algorithm(text_size,
                                                  widget.box(),
                                                  alignmask::center,
                                                  0);
            painter.draw(target.point());
            painter.draw(widget.text());
        }

        string current = "Current " + std::to_string(widget.value2()) + "°";

        painter.set(widget.palette().color(Palette::ColorId::text));
        painter.set(Font(24));

        auto txt_size = painter.text_size(current);
        Rect tbox = detail::align_algorithm(txt_size,
                                            widget.box(),
                                            alignmask::center | alignmask::bottom,
                                            80);
        painter.draw(tbox.point());
        painter.draw(current);

    });

    Application app(argc, argv, "thermostat");

    ThermostatWindow win;
    win.show();

    return app.run();
}
