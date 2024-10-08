/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <cxxopts.hpp>
#include <egt/asio.hpp>
#include <egt/ui>
#include <exception>
#include <iomanip>
#include <memory>
#include <sstream>

inline uint64_t nsec_to_sec(uint64_t s)
{
    return (s / 1000000000ULL);
}

inline uint64_t sec_to_nsec(int s)
{
    return (s * 1000000000ULL);
}

class Controls : public egt::StaticGrid
{
public:

    Controls()
        : egt::StaticGrid(egt::Size(250, 64), egt::StaticGrid::GridSize(3, 1)),
          m_previous(egt::Image("file:previous.png")),
          m_play(egt::Image("file:play.png")),
          m_next(egt::Image("file:next.png"))
    {
        align(egt::AlignFlag::center);

        add(expand(m_previous));
        add(expand(m_play));
        add(expand(m_next));

        for (auto& child : children())
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

    void draw(egt::Painter& painter, const egt::Rect& rect) override
    {
        egt::experimental::Radial::draw(painter, rect);

        const auto offset = 15;

        const auto b = content_area();
        const auto c = b.center();
        const auto radius = std::min(b.width(), b.height()) / 2.f;
        const auto angle2 = egt::detail::to_radians<float>(-90,
                            value_to_degrees(m_value->start(),
                                             m_value->end(),
                                             m_value->value()));

        auto p = c.point_on_circumference(radius + offset, angle2);
        auto text = seconds_to_human(m_value->value());
        auto s = painter.text_size(text);

        if (angle2 > 1.5f)
            p -= egt::Point(s.width() + offset, 0);

        if (angle2 < 1.5f || angle2 > 3.14f)
            p -= egt::Point(0, s.height());

        painter.set(this->font());
        painter.set(egt::Palette::white);
        painter.draw(p);
        painter.draw(text);
    }

    std::shared_ptr<egt::RangeValue<int>> m_value;
};

class AudioPlayer : public egt::Widget
{
public:
    AudioPlayer() = delete;

    explicit AudioPlayer(const std::string& uri)
        : m_colormap({egt::Color(76, 181, 253), egt::Color(34, 186, 133)})

    {
        fill_flags({egt::Theme::FillFlag::solid});
        background(egt::Image("file:background.png"));

        auto range0 = std::make_shared<egt::RangeValue<int>>(0, 100, 100);
        auto range1 = std::make_shared<egt::RangeValue<int>>(0, 100, 100);
        auto range2 = std::make_shared<egt::RangeValue<int>>(0, 100, 0);

        m_dial = std::make_unique<AudioRadial>(range2);
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
        add_component(*m_dial.get());
        add_component(m_controls);

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

        m_logo.align(egt::AlignFlag::left | egt::AlignFlag::top);
        m_logo.margin(10);
        // The size needs to be set again since the margin has been modified.
        const auto m = m_logo.moat();
        m_logo.resize(m_logo.image().size_orig() + egt::Size(2 * m, 2 * m));
        add_component(m_logo);

        m_message_dialog.ratio(75);
        m_message_dialog.title("Audio Player Example");
        std::string dialog_text("This is an Ensemble Graphics "
                                "Toolkit audio player example that "
                                "uses egt::AudioPlayer to play mp3, "
                                "wav, ogg, and more audio formats "
                                "seamlessly.");
        auto text = std::make_shared<egt::TextBox>(dialog_text,
                    egt::TextBox::TextFlags({egt::TextBox::TextFlag::multiline, egt::TextBox::TextFlag::word_wrap}));
        text->readonly(true);
        m_message_dialog.widget(expand(text));
        m_message_dialog.button(egt::Dialog::ButtonId::button1, "");
        m_message_dialog.button(egt::Dialog::ButtonId::button2, "OK");
        add_component(m_message_dialog);

        note.fill_flags().clear();
        note.align(egt::AlignFlag::right | egt::AlignFlag::bottom);
        note.margin(10);
        add_component(note);
        note.on_click([this, text, dialog_text](egt::Event&)
        {
            text->text(dialog_text);
            m_message_dialog.button(egt::Dialog::ButtonId::button2, "OK");
            m_message_dialog.show_modal(true);
        });

        m_player.on_error([this, text](const std::string & err)
        {
            if (!err.empty())
            {
                text->text("Error : " + err);
                m_message_dialog.button(egt::Dialog::ButtonId::button2, "");
                m_message_dialog.show_modal(true);
            }
        });

        // handle input event to seek player
        m_dial->on_user_input_changed([this, range2]()
        {
            m_player.seek(sec_to_nsec(range2->value()));
        });

        m_player.on_position_changed([this, range2](uint64_t position)
        {
            if (m_player.playing())
            {
                auto duration = static_cast<int>(nsec_to_sec(m_player.duration()));
                if (duration > 0)
                    range2->end(duration);

                range2->value(static_cast<int>(nsec_to_sec(position)));
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

    std::unique_ptr<AudioRadial> m_dial;
    Controls m_controls;
    egt::AudioPlayer m_player;
    egt::AnimationSequence m_animation{true};
    egt::experimental::ColorMap m_colormap;
    egt::ImageLabel m_logo{(egt::Image("icon:mgs_logo_white.png;128"))};
    egt::Dialog m_message_dialog;
    egt::ImageButton note{egt::Image("file:note.png")};
};

int main(int argc, char** argv)
{
#ifdef EXAMPLEDATA
    egt::add_search_path(EXAMPLEDATA);
#endif

    auto default_file = std::string{};
    egt::detail::resolve_path("file:concerto.mp3", default_file);

    cxxopts::Options options(argv[0], "play audio file");
    options.add_options()
    ("h,help", "Show help")
    ("i,input", "URI to audio file", cxxopts::value<std::string>()->default_value("file://" + default_file));

    auto args = options.parse(argc, argv);

    if (args.count("help"))
    {
        std::cout << options.help() << std::endl;
        return 0;
    }

    egt::Application app(argc, argv);
#ifdef EXAMPLEDATA
    // Need to redo it as the Application ctor clears the search paths.
    egt::add_search_path(EXAMPLEDATA);
#endif

    egt::Application::instance().screen()->high_fidelity();

    egt::TopWindow top_window;

    /*
     * The AudioPlayerWindow has an AudioPlayer widget that throws an
     * exception if there is no sound card.
     */
    try
    {
        auto win = std::make_shared<AudioPlayer>(args["input"].as<std::string>());
        top_window.add(egt::expand(win));
    }
    catch (std::exception& e)
    {
        top_window.color(egt::Palette::ColorId::bg, egt::Palette::black);

        auto msg = std::make_shared<egt::Label>(e.what());
        msg->border(2);
        msg->fill_flags(egt::Theme::FillFlag::solid);
        msg->color(egt::Palette::ColorId::border, egt::Palette::red);
        msg->color(egt::Palette::ColorId::label_bg, egt::Palette::white);
        msg->resize(egt::Application::instance().screen()->size() * 0.75);
        top_window.add(egt::center(msg));
    }

    top_window.show();

    return app.run();
}
