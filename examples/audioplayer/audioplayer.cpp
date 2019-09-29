/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/asio.hpp>
#include <egt/detail/imagecache.h>
#include <egt/detail/filesystem.h>
#include <egt/ui>
#include <iomanip>
#include <iostream>

using namespace std;
using namespace egt;
using namespace egt::experimental;

class Controls : public StaticGrid
{
public:

    Controls()
        : StaticGrid(Size(250, 64), std::make_tuple(3, 1)),
          m_previous(Image("previous.png")),
          m_play(Image("play.png")),
          m_next(Image("next.png"))
    {
        set_align(alignmask::center);

        add(expand(m_previous));
        add(expand(m_play));
        add(expand(m_next));

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
        : TopWindow(size)
    {
        auto range = std::make_shared<RangeValue<int>>(0, 100, 0);
        m_dial.add(range);

        set_background(Image("background.png"));

        m_dial.set_align(alignmask::expand);
        add(m_dial);
        add(m_controls);

        m_controls.m_play.on_event([this](Event&)
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
        }, {eventid::pointer_click});

        auto logo = std::make_shared<ImageLabel>(Image("@128px/microchip_logo_black.png"));
        logo->set_align(alignmask::left | alignmask::top);
        logo->set_margin(10);
        add(logo);

        m_dial.on_event([this, range](Event&)
        {
            m_player.seek(range->value());
        }, {eventid::input_property_changed});

        m_player.on_event([this, range](Event&)
        {
            if (m_player.playing())
            {
                if (m_player.duration() > 0)
                    range->set_max(m_player.duration());
                range->set_value(m_player.position());
            }
            else
            {
                m_controls.m_play.set_image(Image("play.png"));
            }
        }, {eventid::property_changed});

        m_player.set_media(detail::resolve_file_path("concerto.mp3"));
    }

protected:

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
#if 0
    Drawer<Radial>::set_draw([](Radial & widget, Painter & painter, const Rect & rect)
    {
        ignoreparam(rect);

        auto v = widget.value_to_degrees(widget.value());

        auto color1 = widget.color(Palette::ColorId::button_fg).color();
        auto color2 = widget.color(Palette::ColorId::button_fg, Palette::GroupId::disabled).color();

        float smalldim = std::min(widget.width(), widget.height());
        float linew = smalldim / 10;
        float radius = (smalldim / 2) - (linew * 2);
        float angle1 = detail::to_radians<float>(-90, 0);
        float angle2 = detail::to_radians<float>(-90, v);

        auto c = widget.center();

        // bottom full circle
        painter.set(color1);
        painter.set_line_width(linew);
        painter.draw(Arc(c, radius, 0, 2 * M_PI));
        painter.stroke();

        // value arc
        painter.set(color2);
        painter.set_line_width(linew - (linew / 3));
        painter.draw(Arc(c, radius, angle1, angle2));
        painter.stroke();

        auto p = c.point_on_circumference(radius + linew, angle2);
        auto text = s_to_human(widget.value());
        auto s = painter.text_size(text);

        if (angle2 > 1.5)
            p -= Point(s.width(), 0);

        painter.set(Font());
        painter.set(Palette::white);
        painter.draw(p);
        painter.draw(text);
    });
#endif

    Application app(argc, argv, "audioplayer");

    AudioPlayerWindow win;
    win.show();

    return app.run();
}
