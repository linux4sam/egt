/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <chrono>
#include <cxxopts.hpp>
#include <egt/detail/string.h>
#include <egt/ui>
#include <sstream>
#include <string>

// warning: not UTF-8 safe
static std::string line_break(const std::string& in, size_t width = 50)
{
    std::string out;
    std::string tmp;
    char last = '\0';
    size_t i = 0;

    for (auto& cur : in)
    {
        if (++i == width)
        {
            tmp = egt::detail::ltrim(tmp);
            out += "\n" + tmp;
            i = tmp.length();
            tmp.clear();
        }
        else if (std::isspace(cur) && !std::isspace(last))
        {
            out += tmp;
            tmp.clear();
        }
        tmp += cur;
        last = cur;
    }
    return out + tmp;
}

int main(int argc, char** argv)
{
    cxxopts::Options options(argv[0], "display camera video stream");
    options.add_options()
    ("h,help", "Show help")
    ("d,device", "V4L2 device", cxxopts::value<std::string>()->default_value("/dev/video0"))
    ("width", "Width of the stream", cxxopts::value<int>()->default_value("320"))
    ("height", "Height of the stream", cxxopts::value<int>()->default_value("240"))
    ("f,format", "Pixel format", cxxopts::value<std::string>()->default_value("yuyv"), "[egt::PixelFormat]");
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

    egt::Size size(args["width"].as<int>(), args["height"].as<int>());
    auto format = egt::detail::enum_from_string<egt::PixelFormat>(args["format"].as<std::string>());
    auto dev(args["device"].as<std::string>());

    egt::TopWindow win;
    win.background(egt::Image("file:background.jpg"));

    egt::CameraWindow player(size, dev, format, egt::WindowHint::overlay);
    player.move_to_center(win.center());
    win.add(player);

    egt::Label errlabel;
    errlabel.align(egt::AlignFlag::expand_horizontal);
    errlabel.text_align(egt::AlignFlag::center_horizontal | egt::AlignFlag::top);
    win.add(errlabel);

    auto message_dialog = std::make_shared<egt::Dialog>(win.size() * 0.75);
    message_dialog->title("egt_camera");
    std::string dialog_text("");
    auto dtext = std::make_shared<egt::TextBox>(dialog_text);
    message_dialog->widget(expand(dtext));
    message_dialog->button(egt::Dialog::ButtonId::button1, "Cancel");
    message_dialog->button(egt::Dialog::ButtonId::button2, "OK");
    win.add(message_dialog);

    message_dialog->on_button2_click([&player]()
    {
        std::vector<std::string> dlist = player.list_devices();
        auto ndev = dlist.at(dlist.size() - 1);
        std::cout << " setting new device " << ndev << std::endl;
        player.device(ndev);
        player.play();
    });

    // wait to start playing the video until the window is shown
    win.on_show([&player]()
    {
        player.play();
    });

    player.on_error([&errlabel](const std::string & err)
    {
        errlabel.text(line_break(err));
    });

    player.on_connect([&player, &errlabel, &dev, &message_dialog, &dtext](const std::string & devnode)
    {
        if (!errlabel.text().empty())
        {
            errlabel.text("");
        }

        auto dmesg =  devnode + " device is connected would like to switch";
        dtext->text(dmesg);
        message_dialog->show_modal(true);
    });

    player.on_disconnect([&player, &errlabel, dev](const std::string & devnode)
    {
        errlabel.text(line_break("Device removed: " + devnode));
        if (player.device() == devnode)
        {
            std::cout << devnode << "is disconnected: stoping it" << std::endl;
            player.stop();
        }
    });

    const auto wscale = static_cast<float>(egt::Application::instance().screen()->size().width()) / size.width();
    const auto hscale = static_cast<float>(egt::Application::instance().screen()->size().height()) / size.height();

    player.user_drag(true);
    player.user_track_drag(true);
    player.on_event([&player, &win, wscale](egt::Event & event)
    {
        static egt::Point drag_start_point;
        switch (event.id())
        {
        case egt::EventId::pointer_drag_start:
        {
            drag_start_point = player.box().point();
            break;
        }
        case egt::EventId::pointer_drag:
        {
            if (!(egt::detail::float_equal(player.hscale(), wscale)))
            {
                auto diff = event.pointer().drag_start - event.pointer().point;
                auto p = drag_start_point - egt::Point(diff.x(), diff.y());
                auto max_x = win.width() - player.width();
                auto max_y = win.height() - player.height();
                if (p.x() >= max_x)
                    p.x(max_x);
                if (p.x() < 0)
                    p.x(0);
                if (p.y() >= max_y)
                    p.y(max_y);
                if (p.y() < 0)
                    p.y(0);
                player.move(p);
            }
        }
        default:
            break;
        }
    });

    egt::Window ctrlwindow(egt::Size(win.width(), 72), egt::PixelFormat::argb8888);
    ctrlwindow.align(egt::AlignFlag::bottom | egt::AlignFlag::center_horizontal);
    ctrlwindow.color(egt::Palette::ColorId::bg, egt::Palette::transparent);
    if (!ctrlwindow.plane_window())
        ctrlwindow.fill_flags(egt::Theme::FillFlag::blend);
    win.add(ctrlwindow);

    egt::HorizontalBoxSizer hpos;
    hpos.align(egt::AlignFlag::center);
    ctrlwindow.add(hpos);

    auto logo = std::make_shared<egt::ImageLabel>(egt::Image("icon:mgs_logo_icon.png;32"));
    logo->margin(10);
    hpos.add(logo);

    egt::ImageButton fullscreen(egt::Image("res:fullscreen_png"));
    fullscreen.fill_flags().clear();
    hpos.add(fullscreen);

    fullscreen.on_event([&fullscreen, &player, wscale, hscale](egt::Event&)
    {
        static bool scaled = true;
        if (scaled)
        {
            player.move(egt::Point(0, 0));
            player.scale(wscale, hscale);
            fullscreen.image(egt::Image("res:fullscreen_exit_png"));
            scaled = false;
        }
        else
        {
            player.move(egt::Point(240, 120));
            player.scale(1.0, 1.0);
            fullscreen.image(egt::Image("res:fullscreen_png"));
            scaled = true;
        }
    }, {egt::EventId::pointer_click});

    egt::Label cpulabel("CPU: 0%", egt::Size(100, 40));
    cpulabel.margin(5);
    hpos.add(cpulabel);

    egt::experimental::CPUMonitorUsage tools;
    egt::PeriodicTimer cputimer(std::chrono::seconds(1));
    cputimer.on_timeout([&cpulabel, &tools]()
    {
        tools.update();
        std::ostringstream ss;
        ss << "CPU: " << static_cast<int>(tools.usage()) << "%";
        cpulabel.text(ss.str());
    });
    cputimer.start();

    win.show();
    win.layout();
    player.show();
    ctrlwindow.show();

    return app.run();
}
