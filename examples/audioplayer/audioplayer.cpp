/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <cxxopts.hpp>
#include <egt/asio.hpp>
#include <egt/ui>
#include <iomanip>
#include <sstream>

class Controls : public egt::StaticGrid
{
public:

    Controls()
        : egt::StaticGrid(egt::Size(250, 64), std::make_tuple(3, 1)),
          m_previous(egt::Image("file:previous.png")),
          m_play(egt::Image("file:play.png")),
          m_next(egt::Image("file:next.png"))
    {
        align(egt::AlignFlag::center);

        add(expand(m_previous));
        add(expand(m_play));
        add(expand(m_next));

        for (auto& child : m_children)
            child->fill_flags().clear();

        // no forward/rewind support in this example for now
        m_next.hide();
        m_previous.hide();
    }

    egt::ImageButton m_previous;
    egt::ImageButton m_play;
    egt::ImageButton m_next;
};

inline static std::string seconds_to_human(int seconds)
{
    std::ostringstream ss;

    auto minutes = seconds / 60;
    auto hours = minutes / 60;
    seconds = seconds % 60;

    if (hours)
        ss << std::setfill('0') << std::setw(2) << hours << ":";
    ss << std::setfill('0') << std::setw(2) << minutes << ":";
    ss << std::setfill('0') << std::setw(2) << seconds;
    return ss.str();
}

struct AudioRadial : public egt::experimental::Radial
{
    explicit AudioRadial(std::shared_ptr<egt::RangeValue<int>> value)
        : m_value(std::move(value))
    {
    }

    virtual void draw(egt::Painter& painter, const egt::Rect& rect) override
    {
        egt::experimental::Radial::draw(painter, rect);

        const auto offset = 15;

        const auto b = content_area();
        const auto c = b.center();
        const auto radius = std::min(b.width(), b.height()) / 2.f;
        const auto angle2 = egt::detail::to_radians<float>(-90,
                            value_to_degrees(m_value->min(),
                                             m_value->max(),
                                             m_value->value()));

        auto p = c.point_on_circumference(radius + offset, angle2);
        auto text = seconds_to_human(m_value->value());
        auto s = painter.text_size(text);

        if (angle2 > 1.5)
            p -= egt::Point(s.width() + offset, 0);

        if (angle2 < 1.5 || angle2 > 3.14)
            p -= egt::Point(0, s.height());

        painter.set(egt::Font());
        painter.set(egt::Palette::white);
        painter.draw(p);
        painter.draw(text);
    }

    std::shared_ptr<egt::RangeValue<int>> m_value;
};

class AudioPlayerWindow : public egt::TopWindow
{
public:
    AudioPlayerWindow() = delete;

    explicit AudioPlayerWindow(const std::string& uri)
        : m_colormap({egt::Color(76, 181, 253), egt::Color(34, 186, 133)})
    {
        background(egt::Image("file:background.png"));

        auto range0 = std::make_shared<egt::RangeValue<int>>(0, 100, 100);
        auto range1 = std::make_shared<egt::RangeValue<int>>(0, 100, 100);
        auto range2 = std::make_shared<egt::RangeValue<int>>(0, 100, 0);

        m_dial = std::make_shared<AudioRadial>(range2);
        m_dial->add(range0, egt::Color(egt::Palette::white, 55), 21);
        m_dial->add(range1, egt::Color(17, 17, 17, 180), 13);
        egt::experimental::Radial::RadialFlags flags
        {
            egt::experimental::Radial::RadialFlag::input_value,
            egt::experimental::Radial::RadialFlag::rounded_cap
        };
        auto range2handle = m_dial->add(range2, {}, 15, flags);
        m_dial->margin(50);
        m_dial->align(egt::AlignFlag::expand);
        add(m_dial);
        add(m_controls);

        m_controls.m_play.on_click([this](egt::Event&)
        {
            if (m_player.playing())
            {
                m_controls.m_play.image(egt::Image("file:play.png"));
                m_player.pause();
                m_animation.stop();
            }
            else
            {
                m_controls.m_play.image(egt::Image("file:pause.png"));
                m_player.play();
                m_animation.start();
            }
        });

        auto logo = std::make_shared<egt::ImageLabel>(egt::Image("icon:egt_logo_white.png;128"));
        logo->align(egt::AlignFlag::left | egt::AlignFlag::top);
        logo->margin(10);
        add(logo);

        auto message_dialog = std::make_shared<egt::Dialog>(this->size() * 0.75);
        message_dialog->title("Audio Player Example");
        std::string dialog_text("This is an Ensemble Graphics "
                                "Toolkit audio player example that "
                                "uses egt::AudioPlayer to play mp3, "
                                "wav, ogg, and more audio formats "
                                "seamlessly.");
        auto text = std::make_shared<egt::TextBox>(dialog_text);
        text->readonly(true);
        message_dialog->widget(expand(text));
        message_dialog->button(egt::Dialog::ButtonId::button1, "");
        message_dialog->button(egt::Dialog::ButtonId::button2, "OK");
        add(message_dialog);

        auto note = std::make_shared<egt::ImageButton>(egt::Image("file:note.png"));
        note->fill_flags().clear();
        note->align(egt::AlignFlag::right | egt::AlignFlag::bottom);
        note->margin(10);
        add(note);
        note->on_click([message_dialog, text, dialog_text](egt::Event&)
        {
            text->text(dialog_text);
            message_dialog->button(egt::Dialog::ButtonId::button2, "OK");
            message_dialog->show_modal(true);
        });

        m_player.on_error([message_dialog, text](std::string err)
        {
            if (!err.empty())
            {
                text->text("Error : " + err);
                message_dialog->button(egt::Dialog::ButtonId::button2, "");
                message_dialog->show_modal(true);
            }
        });

        // handle input event to seek player
        m_dial->on_user_input_changed([this, range2]()
        {
            m_player.seek(range2->value());
        });

        m_player.on_position_changed([this, range2](uint64_t position)
        {
            if (m_player.playing())
            {
                auto duration = static_cast<int>(m_player.duration());
                if (duration > 0)
                    range2->max(duration);

                range2->value(static_cast<int>(position));
            }
        });

        m_player.on_state_changed([this, range2]()
        {
            if (!m_player.playing())
            {
                m_controls.m_play.image(egt::Image("file:play.png"));
                m_animation.stop();
            }
        });

        m_player.media(uri);

        auto glow = [this, range2handle](egt::PropertyAnimatorF::Value value)
        {
            m_dial->color(range2handle, m_colormap.interp_cached(value));
        };

        auto glow_out = std::make_shared<egt::PropertyAnimatorF>();
        glow_out->starting(0);
        glow_out->ending(1);
        glow_out->duration(std::chrono::milliseconds(1500));
        glow_out->easing_func(egt::easing_linear);
        glow_out->on_change(glow);

        auto glow_in = std::make_shared<egt::PropertyAnimatorF>();
        glow_in->starting(1);
        glow_in->ending(0);
        glow_in->duration(std::chrono::milliseconds(2000));
        glow_in->easing_func(egt::easing_linear);
        glow_in->on_change(glow);

        m_animation.add(glow_out);
        m_animation.add(glow_in);
    }

protected:

    std::shared_ptr<AudioRadial> m_dial;
    Controls m_controls;
    egt::AudioPlayer m_player;
    egt::AnimationSequence m_animation{true};
    egt::experimental::ColorMap m_colormap;
};

int main(int argc, char** argv)
{
    cxxopts::Options options(argv[0], "play audio file");
    options.add_options()
    ("h,help", "Show help")
    ("i,input", "URI to audio file", cxxopts::value<std::string>()->default_value("file:concerto.mp3"));

    auto args = options.parse(argc, argv);

    if (args.count("help"))
    {
        std::cout << options.help() << std::endl;
        return 0;
    }

    egt::Application app(argc, argv);
#ifdef EXAMPLEDATA
    egt::add_search_path(EXAMPLEDATA);
#endif

    egt::Application::instance().screen()->high_fidelity();

    AudioPlayerWindow win(args["input"].as<std::string>());
    win.show();

    return app.run();
}
