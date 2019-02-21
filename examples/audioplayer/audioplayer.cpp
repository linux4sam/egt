/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <asio.hpp>
#include <egt/ui>
#include <iomanip>
#include <iostream>

using namespace std;
using namespace egt;

class Controls : public StaticGrid
{
public:

    Controls()
        : StaticGrid(Rect(Point(), Size(250, 64)), 3, 1),
          m_previous(Image("previous.png")),
          m_play(Image("play.png")),
          m_next(Image("next.png"))
    {
        set_align(alignmask::CENTER);

        add(&m_previous);
        add(&m_play);
        add(&m_next);

        for (auto& child : m_children)
            child->set_boxtype(Theme::boxtype::none);
    }

    ImageButton m_previous;
    ImageButton m_play;
    ImageButton m_next;
};

class AudioPlayerWindow : public TopWindow
{
public:
    explicit AudioPlayerWindow(const Size& size = Size())
        : TopWindow(size),
          m_background(*this, Image("background.png")),
          m_logo(*this, Image("@microchip_logo_black.png")),
          m_dial(*this, Rect(), 0, 100, 0)
    {
        if (m_background.h() != h())
        {
            double scale = (double)h() / (double)m_background.h();
            m_background.scale_image(scale);
        }

        m_dial.set_align(alignmask::EXPAND);
        add(&m_controls);

        m_controls.m_play.on_event([this](eventid)
        {
            if (m_player.playing())
            {
                m_controls.m_play.set_image(Image("play.png"));
                m_player.pause();
            }
            else
            {
                m_controls.m_play.set_image(Image("pause.png"));
                m_player.play();
            }

            return 0;
        }, {eventid::POINTER_CLICK});

        m_logo.set_align(alignmask::LEFT | alignmask::TOP, 10);

        m_dial.on_event([this](eventid)
        {
            m_player.seek(m_dial.value());
            return 0;
        }, {eventid::INPUT_PROPERTY_CHANGED});

        add(&m_dial);

        m_dial.set_radial_flags({Radial::radial_flags::primary_value});

        m_player.on_event([this](eventid)
        {
            if (m_player.playing())
            {
                if (m_player.duration() > 0)
                    m_dial.set_max(m_player.duration());
                m_dial.set_value(m_player.position());
            }
            else
            {
                m_controls.m_play.set_image(Image("play.png"));
            }

            return 0;
        }, {eventid::PROPERTY_CHANGED});

        m_player.set_media("/root/concerto.mp3");
    }

protected:

    ImageLabel m_background;
    ImageLabel m_logo;
    Radial m_dial;
    Controls m_controls;
    AudioPlayer m_player;
};

inline string s_to_human(int seconds)
{
    ostringstream ss;

    auto minutes = seconds / 60;
    auto hours = minutes / 60;
    seconds = seconds % 60;

    if (hours)
        ss << hours << ":";
    if (minutes)
    {
        ss << minutes << ":";
        ss << std::setfill('0') << std::setw(2);
    }
    ss << seconds;
    return ss.str();
}

int main(int argc, const char** argv)
{
    Drawer<Radial>::set_draw([](Radial & widget, Painter & painter, const Rect & rect)
    {
        ignoreparam(rect);

        auto v = widget.value_to_degrees(widget.value());

        auto color1 = widget.palette().color(Palette::MID);
        auto color2 = widget.palette().color(Palette::HIGHLIGHT);

        float smalldim = std::min(widget.w(), widget.h());
        float linew = smalldim / 10;
        float radius = (smalldim / 2) - (linew * 2);
        float angle1 = detail::to_radians<float>(-90, 0);
        float angle2 = detail::to_radians<float>(-90, v);

        auto c = widget.center();

        // bottom full circle
        painter.set_color(color1);
        painter.set_line_width(linew);
        painter.arc(Arc(c, radius, 0, 2 * M_PI));
        painter.stroke();

        // value arc
        painter.set_color(color2);
        painter.set_line_width(linew - (linew / 3));
        painter.arc(Arc(c, radius, angle1, angle2));
        painter.stroke();

        auto p = c.point_on_circumference(radius + linew, angle2);
        auto text = s_to_human(widget.value());
        auto s = painter.text_size(text);

        if (angle2 > 1.5)
            p -= Point(s.w, 0);

        painter.set_font(Font());
        painter.set_color(Color::WHITE);
        painter.draw_text(text, Rect(p, s),
                          alignmask::CENTER, 0, Font());

    });

    Application app(argc, argv, "audioplayer");

    AudioPlayerWindow win;
    win.show();

    return app.run();
}
