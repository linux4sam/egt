/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <cxxopts.hpp>
#include <egt/ui>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <vector>

class FloatingBox
{
public:
    FloatingBox(std::shared_ptr<egt::Widget> widget,
                egt::DefaultDim mx, egt::DefaultDim my)
        : m_widget(std::move(widget)),
          m_mx(mx),
          m_my(my)
    {
        m_widget->flags().set(egt::Widget::Flag::no_layout);
        m_widget->flags().set(egt::Widget::Flag::grab_mouse);
        m_widget->on_event([this](egt::Event & event)
        {
            handle(event);
        });
    }

    virtual void handle(egt::Event& event)
    {
        switch (event.id())
        {
        case egt::EventId::pointer_dblclick:
            m_mx *= -1;
            m_my *= -1;
            break;
        case egt::EventId::pointer_drag_start:
            m_start_point = m_widget->box().point();
            m_dragging = true;
            break;
        case egt::EventId::pointer_drag_stop:
            m_dragging = false;
            break;
        case egt::EventId::pointer_drag:
        {
            auto diff = event.pointer().drag_start - event.pointer().point;
            auto fromstart = m_start_point - egt::Point(diff.x(), diff.y());
            egt::Rect dest(fromstart, m_widget->box().size());
            if (egt::main_window()->box().contains(dest))
                m_widget->move(fromstart);
            break;
        }
        default:
            break;
        }
    }

    virtual void next_frame()
    {
        if (m_dragging)
            return;

        auto p = egt::Point(m_widget->x() + m_mx,
                            m_widget->y() + m_my);

        if (m_widget->box().right() >= egt::main_window()->size().width())
            m_mx = std::fabs(m_mx) * -1;

        if (p.x() < 0)
            m_mx = std::fabs(m_mx);

        if (m_widget->box().bottom() >= egt::main_window()->size().height())
            m_my = std::fabs(m_my) * -1;

        if (p.y() < 0)
            m_my = std::fabs(m_my);

        m_widget->move(p);
    }

    virtual ~FloatingBox() = default;

protected:
    std::shared_ptr<egt::Widget> m_widget;
    egt::DefaultDim m_mx;
    egt::DefaultDim m_my;
    egt::Point m_start_point;
    bool m_dragging{false};
};

static std::vector<std::shared_ptr<FloatingBox>> boxes;

int main(int argc, char** argv)
{
    cxxopts::Options options(argv[0], "display floating objects");
    options.add_options()
    ("h,help", "Show help")
    ("no-image", "Don't use images but solid colors", cxxopts::value<bool>()->default_value("false"))
    ("soft-objects", "Number of software objects to display", cxxopts::value<int>()->default_value("2"))
    ("soft-hard-objects", "Number of hardware or emulated objects to display", cxxopts::value<int>()->default_value("2"))
    ("move-timer", "Timer to move the objects in milliseconds", cxxopts::value<int>()->default_value("30"));

    auto args = options.parse(argc, argv);

    if (args.count("help"))
    {
        std::cout << options.help() << std::endl;
        return 0;
    }

    egt::Application app(argc, argv, "floating");
    egt::add_search_path(EXAMPLEDATA);

    egt::TopWindow win;
    if (!args.count("no-image"))
        win.background(egt::Image("file:background.png"));
    win.show();

    std::random_device r;
    std::default_random_engine e1 {r()};
    std::uniform_int_distribution<egt::DefaultDim> move {-4, 4};
    std::uniform_real_distribution<float> rgb {0.0, 1.0};

    egt::DefaultDim f = 2;

    auto image_index = 0;

    const auto N_IMAGE = 6;

    // software
    for (auto x = 0; x < args["soft-objects"].as<int>(); x++)
    {
        std::shared_ptr<egt::Widget> object;
        if (args.count("no-image"))
        {
            object = std::make_shared<egt::RectangleWidget>(egt::Size(150, 150));
            object->color(egt::Palette::ColorId::button_bg,
                          egt::Color::rgbaf(rgb(e1), rgb(e1), rgb(e1), 0.75));
        }
        else
        {
            std::stringstream os;
            os << "file:image" << image_index++ % N_IMAGE << ".png";
            object = std::make_shared<egt::ImageLabel>(egt::Image(os.str()));
        }

        // avoid 0 case
        auto mx = move(e1);
        while (!mx)
            mx = move(e1);
        auto my = move(e1);
        while (!my)
            my = move(e1);

        boxes.push_back(std::make_shared<FloatingBox>(object, mx * f, my * f));
        win.add(object);
    }

    // hardware (or emulated)
    for (auto x = 0; x < args["soft-hard-objects"].as<int>(); x++)
    {
        std::shared_ptr<egt::Widget> object;
        if (args.count("no-image"))
        {
            object = std::make_shared<egt::RectangleWidget>(egt::Size(150, 150));
            object->color(egt::Palette::ColorId::button_bg,
                          egt::Color::rgbaf(rgb(e1), rgb(e1), rgb(e1), 0.75));
        }
        else
        {
            std::stringstream os;
            os << "file:image" << image_index++ % N_IMAGE << ".png";
            object = std::make_shared<egt::ImageLabel>(egt::Image(os.str()));
        }
        auto plane = std::make_shared<egt::Window>(egt::Size(object->width(),
                     object->height()));
        plane->color(egt::Palette::ColorId::bg, egt::Palette::transparent);
        plane->fill_flags().clear();
        plane->add(object);
        plane->show();
        plane->move(egt::Point(100, 100));

        // avoid 0 case
        auto mx = move(e1);
        while (!mx)
            mx = move(e1);
        auto my = move(e1);
        while (!my)
            my = move(e1);

        boxes.push_back(std::make_shared<FloatingBox>(plane, mx * f, my * f));
        win.add(plane);
    }

    egt::PeriodicTimer movetimer(std::chrono::milliseconds(args["move-timer"].as<int>()));
    movetimer.on_timeout([&]()
    {
        for (auto& i : boxes)
            i->next_frame();
    });
    movetimer.start();

    egt::Label label1("CPU: ----");
    label1.color(egt::Palette::ColorId::text, egt::Palette::white);
    win.add(bottom(left(label1)));

    egt::experimental::CPUMonitorUsage tools;
    egt::PeriodicTimer cputimer(std::chrono::seconds(1));
    cputimer.on_timeout([&tools, &label1]()
    {
        tools.update();

        std::ostringstream ss;
        ss << "CPU: " << static_cast<int>(tools.usage()) << "%";
        label1.text(ss.str());
    });
    cputimer.start();

    return app.run();
}
