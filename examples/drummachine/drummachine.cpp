/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>

#include <iostream>
#include <memory>
#include <string>

static int fatal_error(const std::string& err)
{
    std::cerr << err << std::endl;

    auto& app = egt::Application::instance();

    auto msg = std::make_shared<egt::Label>(err);
    msg->border(2);
    msg->fill_flags(egt::Theme::FillFlag::solid);
    msg->color(egt::Palette::ColorId::border, egt::Palette::red);
    msg->color(egt::Palette::ColorId::label_bg, egt::Palette::white);
    msg->resize(egt::Application::instance().screen()->size() * 0.75);

    egt::Timer err_timer(std::chrono::milliseconds(3000));
    err_timer.on_timeout([&app]()
    {
        app.quit(-1);
    });
    err_timer.start();

    auto win = app.main_window();
    /*
     * A main window has been already set when this function is called.
     * This is why we don't deal with the case there is no main window.
     * Sanity check just done to prevent a crash.
     */
    if (win)
    {
        win->add(egt::center(msg));
        win->show();
    }

    return app.run();
}

int main(int argc, char** argv)
{
    egt::Application app(argc, argv);
#ifdef EXAMPLEDATA
    egt::add_search_path(EXAMPLEDATA);
#endif

    egt::Palette custom_palette;
    custom_palette.set(egt::Palette::ColorId::bg, egt::Color::css("#101010"));
    custom_palette.set(egt::Palette::ColorId::text, egt::Color::css("#828282"));
    custom_palette.set(egt::Palette::ColorId::border, egt::Color::css("#226e30"));
    custom_palette.set(egt::Palette::ColorId::button_bg, egt::Color::css("#1d1d1d"));
    custom_palette.set(egt::Palette::ColorId::button_bg, egt::Color::css("#1d1d1d"), egt::Palette::GroupId::active);
    custom_palette.set(egt::Palette::ColorId::button_bg, egt::Color::css("#1d1d1d"), egt::Palette::GroupId::disabled);
    custom_palette.set(egt::Palette::ColorId::button_text, egt::Color::css("#828282"));
    custom_palette.set(egt::Palette::ColorId::button_text, egt::Color::css("#828282"), egt::Palette::GroupId::disabled);
    custom_palette.set(egt::Palette::ColorId::label_text, egt::Color::css("#828282"));
    custom_palette.set(egt::Palette::ColorId::label_text, egt::Color::css("#ffffff"), egt::Palette::GroupId::checked);
    custom_palette.set(egt::Palette::ColorId::label_bg, egt::Color::css("#101010"));
    custom_palette.set(egt::Palette::ColorId::label_bg, egt::Color::css("#101010"), egt::Palette::GroupId::checked);

    egt::TopWindow win;
    win.palette(custom_palette);

    egt::VerticalBoxSizer main_sizer;
    win.add(expand(main_sizer));

    egt::StaticGrid grid(egt::StaticGrid::GridSize(3, 3));
    grid.vertical_space(5);
    grid.horizontal_space(5);
    main_sizer.add(expand(grid));

    const std::tuple<const std::string, const std::string, const std::string> drums[] =
    {
        { "     Bass Drum", "file:Bass-Drum-1.wav", "file:bass_drum.png" },
        { "     Beat", "file:Casio-MT-45-16-Beat.wav", "file:beat.png" },
        { "     Crash Cymbal", "file:Ensoniq-SQ-1-Crash-Cymbal.wav", "file:cymbal.png" },
        { "     Hi Hat", "file:Ensoniq-SQ-1-Closed-Hi-Hat.wav", "file:hihat.png" },
        { "     Hi Hat", "file:Ensoniq-SQ-1-Open-Hi-Hat.wav", "file:hihat.png" },
        { "     Hi Tom", "file:Ensoniq-ESQ-1-Hi-Synth-Tom.wav", "file:tom.png" },
        { "     Low Tom", "file:Ensoniq-ESQ-1-Low-Synth-Tom.wav", "file:tom.png" },
        { "     Ride Cymbal", "file:Ensoniq-SQ-1-Ride-Cymbal.wav", "file:cymbal.png" },
        { "     Snare", "file:Ensoniq-ESQ-1-Snare.wav", "file:tom.png" },
    };

    std::vector<std::shared_ptr<egt::SoundEffect>> sounds;

    for (auto& drum : drums)
    {
        std::shared_ptr<egt::SoundEffect> sound;
        bool sound_loaded = true;
        try
        {
            sound = std::make_shared<egt::SoundEffect>(std::get<1>(drum));
            sounds.push_back(sound);
        }
        catch (const std::system_error& e)
        {
            return fatal_error(e.what());
        }
        catch (const std::runtime_error& e)
        {
            std::cerr << "Can't load the sound file (" << std::get<1>(drum) << "): " << e.what() << std::endl;
            sound_loaded = false;
        }
        catch (...)
        {
            std::cerr << "Can't create the SoundEffect for " << std::get<1>(drum) << std::endl;
            sound_loaded = false;
        }

        auto drum_hsizer = std::make_shared<egt::HorizontalBoxSizer>();
        drum_hsizer->color(egt::Palette::ColorId::bg, egt::Color::css("#1d1d1d"));
        drum_hsizer->fill_flags(egt::Theme::FillFlag::solid);
        grid.add(expand(drum_hsizer));

        auto drum_button = std::make_shared<egt::ImageButton>(egt::Image(std::get<2>(drum)), std::get<0>(drum));
        auto drum_repeat = std::make_shared<egt::CheckBox>(" ");
        drum_repeat->switch_image(egt::Image("file:repeat_on.png"), true);
        drum_repeat->switch_image(egt::Image("file:repeat_off.png"), false);

        if (sound_loaded)
        {
            drum_button->on_click([sound](egt::Event&)
            {
                sound->play();
            });

            drum_repeat->on_checked_changed([sound, drum_repeat]()
            {
                sound->repeat(drum_repeat->checked());
            });
        }
        else
        {
            drum_button->disable();
            drum_repeat->disable();
        }

        drum_hsizer->add(expand(drum_button));
        drum_hsizer->add(drum_repeat);
    }

    egt::Frame spacer;
    spacer.height(5);
    main_sizer.add(egt::expand_horizontal(spacer));

    egt::Popup popup(egt::Size(app.screen()->box().width() * 0.4, app.screen()->box().height() * 0.5));
    popup.palette(custom_palette);
    popup.move_to_center(win.center());

    auto devices = std::make_shared<egt::ListBox>();
    popup.add(expand(devices));

    const auto sound_devices = egt::SoundEffect::sound_device_list();
    if (sound_devices.empty())
    {
        return fatal_error("No sound device found");
    }

    egt::ImageButton speaker(egt::Image("file:speaker.png"), sound_devices[0].card_name());
    main_sizer.add(egt::expand_horizontal(speaker));

    for (const auto& device : sound_devices)
        devices->add_item(std::make_shared<egt::StringItem>(device.card_name()));

    egt::Timer timer(std::chrono::milliseconds(1000));
    devices->on_selected([&sound_devices, &sounds, &popup, &speaker, &timer](size_t index)
    {
        const auto card = sound_devices[index].card_index();
        const auto dev = sound_devices[index].device_index();

        for (auto& sound : sounds)
        {
            if (!sound->device({card, dev}))
            {
                std::cerr << "Can't set the device for " << sound->media() << std::endl;
                popup.hide();
                return;
            }
        }

        speaker.text(sound_devices[index].card_name());
        timer.on_timeout([&popup]()
        {
            popup.hide();
        });
        timer.start();
    });

    speaker.on_click([&popup](egt::Event&)
    {
        popup.show_modal();
    });

    win.show();

    return app.run();
}
