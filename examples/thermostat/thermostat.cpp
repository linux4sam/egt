/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/detail/alignment.h>
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
               easing_snap)
    {
        set_boxtype(Theme::boxtype::none);

        auto background = make_shared<ImageLabel>(Image("background.png"));
        background->set_align(alignmask::expand);
        background->set_image_align(alignmask::expand);
        add(background);

        auto m_logo = make_shared<ImageLabel>(*this, Image("@128px/microchip_logo_black.png"));
        m_logo->set_align(alignmask::left | alignmask::top);
        m_logo->set_margin(10);

        auto m_title = make_shared<Label>("Living Room",
                                          Size(250, 64),
                                          alignmask::center);
        m_title->set_color(Palette::ColorId::bg, Palette::transparent);
        m_title->set_align(alignmask::center | alignmask::top);
        m_title->set_margin(10);
        add(m_title);

        auto m_radial1 = make_shared<Radial>(*this, Rect(Point(w() / 2 - 350 / 2,
                                             -350),
                                             Size(350, 350)),
                                             0, 100);

        m_radial1->on_event([m_radial1](Event&)
        {
            auto text = std::to_string(static_cast<int>(m_radial1->value())) + "°";
            m_radial1->text(text);

            if (m_radial1->value() > m_radial1->value2())
                m_radial1->set_color(Palette::ColorId::button_bg, Palette::orange);
            else
                m_radial1->reset_palette();
        }, {eventid::property_changed});

        m_radial1->set_value(73);
        m_radial1->set_value2(65);

        auto m_label1 = make_shared<ImageLabel>(*this,
                                                Image("day.png"),
                                                "Day",
                                                Rect(Point(-200, 150),
                                                        Size(180, 64)),
                                                alignmask::center);
        m_label1->set_color(Palette::ColorId::bg, Palette::transparent);
        m_label1->set_color(Palette::ColorId::text, Palette::gray);

        auto m_label2 = make_shared<ImageLabel>(*this,
                                                Image("night.png"),
                                                "Night",
                                                Rect(Point(-200, 214),
                                                        Size(180, 64)),
                                                alignmask::center);
        m_label2->set_color(Palette::ColorId::bg, Palette::transparent);
        m_label2->set_color(Palette::ColorId::text, Palette::gray);

        auto m_label3 = make_shared<ImageLabel>(*this,
                                                Image("vacation.png"),
                                                "Vacation",
                                                Rect(Point(-200, 278),
                                                        Size(180, 64)),
                                                alignmask::center);
        m_label3->set_color(Palette::ColorId::bg, Palette::transparent);
        m_label3->set_color(Palette::ColorId::text, Palette::gray);

        auto m_label4 = make_shared<Label>(*this, "Fan",
                                           Rect(Point(800, 390),
                                                Size(50, 64)),
                                           alignmask::center);
        m_label4->set_color(Palette::ColorId::bg, Palette::transparent);

        auto m_slider1 = make_shared<Slider>(*this, Rect(Point(800, 100),
                                             Size(50, 300)),
                                             0, 100, 0,
                                             orientation::vertical);
        m_slider1->set_value(50);

        m_label1->on_event([m_label1, m_label2, m_label3](Event&)
        {
            m_label1->set_color(Palette::ColorId::label_text, Palette::orange);
            m_label2->set_color(Palette::ColorId::label_text, Palette::gray);
            m_label3->set_color(Palette::ColorId::label_text, Palette::gray);
            m_label1->damage();
            m_label2->damage();
            m_label3->damage();
            return 0;
        }, {eventid::pointer_click});

        m_label2->on_event([m_label1, m_label2, m_label3](Event&)
        {
            m_label2->set_color(Palette::ColorId::label_text, Palette::orange);
            m_label1->set_color(Palette::ColorId::label_text, Palette::gray);
            m_label3->set_color(Palette::ColorId::label_text, Palette::gray);
            m_label1->damage();
            m_label2->damage();
            m_label3->damage();
            return 0;
        }, {eventid::pointer_click});

        m_label3->on_event([m_label1, m_label2, m_label3](Event&)
        {
            m_label3->set_color(Palette::ColorId::label_text, Palette::orange);
            m_label1->set_color(Palette::ColorId::label_text, Palette::gray);
            m_label2->set_color(Palette::ColorId::label_text, Palette::gray);
            m_label1->damage();
            m_label2->damage();
            m_label3->damage();
            return 0;
        }, {eventid::pointer_click});

        m_a1.on_change(std::bind(&ImageLabel::set_x, m_label1, std::placeholders::_1));
        m_a1.on_change(std::bind(&ImageLabel::set_x, m_label2, std::placeholders::_1));
        m_a1.on_change(std::bind(&ImageLabel::set_x, m_label3, std::placeholders::_1));

        m_a2.on_change(std::bind(&Radial::set_y, m_radial1, std::placeholders::_1));

        m_a3.on_change(std::bind(&Slider::set_x, m_slider1, std::placeholders::_1));
        m_a3.on_change(std::bind(&Label::set_x, m_label4, std::placeholders::_1));
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
};

int main(int argc, const char** argv)
{
    Drawer<Radial>::set_draw([](Radial & widget, Painter & painter, const Rect & rect)
    {
        ignoreparam(rect);

        auto v = widget.value_to_degrees(widget.value());
        float linew = 10;
        float handle_radius = 20;

        auto color2 = widget.color(Palette::ColorId::button_bg).color();
        auto color3 = widget.color(Palette::ColorId::button_fg).color();

        float radius = widget.w() / 2 - handle_radius - linew;
        float angle1 = detail::to_radians<float>(-90, 0);
        float angle2 = detail::to_radians<float>(-90, v);

        auto c = widget.center();

        Painter::AutoSaveRestore sr(painter);

        // value arc
        painter.set(color2);
        painter.set_line_width(linew);
        painter.draw(egt::Arc(c, radius, angle1, angle2));
        painter.stroke();

        // handle
        painter.set(color3);
        auto hp = c.point_on_circumference(radius, angle2);
        painter.draw(egt::Arc(hp, handle_radius, 0., static_cast<float>(2 * detail::pi<float>())));
        painter.fill();

        // secondary value
        auto color4 = Palette::red;
        float angle3 = detail::to_radians<float>(-90,
                       widget.value_to_degrees(widget.value2()));
        painter.set(color4);
        painter.set_line_width(linew * 2);
        painter.draw(egt::Arc(c, radius, angle3 - 0.01, angle3 + 0.01));
        painter.stroke();

        //text
        if (!widget.text().empty())
        {
            painter.set(widget.color(Palette::ColorId::text).color());
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

        painter.set(widget.color(Palette::ColorId::text).color());
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
