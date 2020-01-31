/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/asio.hpp>
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
          m_previous(Image("file:previous.png")),
          m_play(Image("file:play.png")),
          m_next(Image("file:next.png"))
    {
        align(AlignFlag::center);

        add(expand(m_previous));
        add(expand(m_play));
        add(expand(m_next));

        for (auto& child : m_children)
            child->fill_flags().clear();

        // no forward/rewind support in this example for now
        m_next.hide();
        m_previous.hide();
    }

    ImageButton m_previous;
    ImageButton m_play;
    ImageButton m_next;
};

inline static string seconds_to_human(int seconds)
{
    ostringstream ss;

    auto minutes = seconds / 60;
    auto hours = minutes / 60;
    seconds = seconds % 60;

    if (hours)
        ss << std::setfill('0') << std::setw(2) << hours << ":";
    ss << std::setfill('0') << std::setw(2) << minutes << ":";
    ss << std::setfill('0') << std::setw(2) << seconds;
    return ss.str();
}

struct AudioRadial : public Radial
{
    explicit AudioRadial(std::shared_ptr<RangeValue<int>> value)
        : m_value(std::move(value))
    {
    }

    virtual void draw(Painter& painter, const Rect& rect) override
    {
        Radial::draw(painter, rect);

        const auto offset = 15;

        const auto b = content_area();
        const auto c = b.center();
        const auto radius = std::min(b.width(), b.height()) / 2.f;
        const auto angle2 = detail::to_radians<float>(-90,
                            value_to_degrees(m_value->min(),
                                             m_value->max(),
                                             m_value->value()));

        auto p = c.point_on_circumference(radius + offset, angle2);
        auto text = seconds_to_human(m_value->value());
        auto s = painter.text_size(text);

        if (angle2 > 1.5)
            p -= Point(s.width() + offset, 0);

        if (angle2 < 1.5 || angle2 > 3.14)
            p -= Point(0, s.height());

        painter.set(Font());
        painter.set(Palette::white);
        painter.draw(p);
        painter.draw(text);
    }

    std::shared_ptr<RangeValue<int>> m_value;
};

class AudioPlayerWindow : public TopWindow
{
public:
    AudioPlayerWindow() = delete;

    explicit AudioPlayerWindow(const std::string& uri)
        : m_colormap({Color(76, 181, 253), Color(34, 186, 133)})
    {
        background(Image("file:background.png"));

        auto range0 = std::make_shared<RangeValue<int>>(0, 100, 100);
        auto range1 = std::make_shared<RangeValue<int>>(0, 100, 100);
        auto range2 = std::make_shared<RangeValue<int>>(0, 100, 0);

        m_dial = std::make_shared<AudioRadial>(range2);
        m_dial->add(range0, Color(Palette::white, 55), 21);
        m_dial->add(range1, Color(17, 17, 17, 180), 13);
        Radial::RadialFlags flags = Radial::RadialFlags({Radial::RadialFlag::input_value, Radial::RadialFlag::rounded_cap});
        auto range2handle = m_dial->add(range2, {}, 15, flags);
        m_dial->margin(50);
        m_dial->align(AlignFlag::expand);
        add(m_dial);
        add(m_controls);

        m_controls.m_play.on_event([this](Event&)
        {
            if (m_player.playing())
            {
                m_controls.m_play.image(Image("file:play.png"));
                m_player.pause();
                m_animation.stop();
            }
            else
            {
                m_controls.m_play.image(Image("file:pause.png"));
                m_player.play();
                m_animation.start();
            }
        }, {EventId::pointer_click});

        auto logo = std::make_shared<ImageLabel>(Image("icon:egt_logo_white.png;128"));
        logo->align(AlignFlag::left | AlignFlag::top);
        logo->margin(10);
        add(logo);

        auto message_dialog = std::make_shared<Dialog>(this->size() * 0.75);
        message_dialog->title("Audio Player Example");
        auto text = std::make_shared<TextBox>("This is an Ensemble Graphics "
                                              "Toolkit audio player example that "
                                              "uses egt::AudioPlayer to play mp3, "
                                              "wav, ogg, and more audio formats "
                                              "seamlessly.");
        text->readonly(true);
        message_dialog->widget(expand(text));
        message_dialog->button(Dialog::ButtonId::button1, "");
        message_dialog->button(Dialog::ButtonId::button2, "OK");
        add(message_dialog);

        auto note = std::make_shared<ImageButton>(Image("file:note.png"));
        note->fill_flags().clear();
        note->align(AlignFlag::right | AlignFlag::bottom);
        note->margin(10);
        add(note);
        note->on_click([message_dialog](Event&)
        {
            message_dialog->show_modal(true);
        });

        // handle input event to seek player
        m_dial->on_user_input_changed([this, range2]()
        {
            m_player.seek(range2->value());
        });

        m_player.on_position_changed([this, range2]()
        {
            if (m_player.playing())
            {
                if (m_player.duration() > 0)
                    range2->max(m_player.duration());
                range2->value(m_player.position());
            }
        });

        m_player.on_state_changed([this, range2]()
        {
            if (!m_player.playing())
            {
                m_controls.m_play.image(Image("file:play.png"));
                m_animation.stop();
            }
        });

        m_player.media(uri);

        auto glow = [this, range2handle](float value)
        {
            m_dial->color(range2handle, m_colormap.interp_cached(value));
        };

        auto glow_out = std::make_shared<PropertyAnimatorF>();
        glow_out->starting(0);
        glow_out->ending(1);
        glow_out->duration(std::chrono::milliseconds(1500));
        glow_out->easing_func(easing_linear);
        glow_out->on_change(glow);

        auto glow_in = std::make_shared<PropertyAnimatorF>();
        glow_in->starting(1);
        glow_in->ending(0);
        glow_in->duration(std::chrono::milliseconds(2000));
        glow_in->easing_func(easing_linear);
        glow_in->on_change(glow);

        m_animation.add(glow_out);
        m_animation.add(glow_in);
    }

protected:

    std::shared_ptr<AudioRadial> m_dial;
    Controls m_controls;
    AudioPlayer m_player;
    egt::AnimationSequence m_animation{true};
    egt::experimental::ColorMap m_colormap;
};

int main(int argc, const char** argv)
{
    Application app(argc, argv);

    Application::instance().screen()->high_fidelity();

    std::string uri = "file:concerto.mp3";
    if (argc >= 2)
        uri = argv[1];

    AudioPlayerWindow win(uri);
    win.show();

    return app.run();
}
