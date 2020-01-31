/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <memory>
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

int main(int argc, const char** argv)
{
    egt::Application app(argc, argv, "floating");

    egt::TopWindow win;
    win.background(egt::Image("file:background.png"));
    win.show();

    egt::DefaultDim f = 2;

    std::vector<std::pair<egt::DefaultDim, egt::DefaultDim>> moveparms =
    {
        std::make_pair(1 * f, 2 * f),
        std::make_pair(3 * f, -2 * f),
        std::make_pair(-3 * f, 2 * f),
        std::make_pair(-3 * f, 3 * f),
        std::make_pair(2 * f, 3 * f),
        std::make_pair(2 * f, 2 * f),
        std::make_pair(4 * f, 2 * f),
        std::make_pair(-4 * f, 2 * f),
    };

    auto image_index = 0;

    const auto SOFT_COUNT = 2;

    // software
    for (auto x = 0; x < SOFT_COUNT; x++)
    {
        std::stringstream os;
        os << "file:image" << image_index++ << ".png";
        auto image = std::make_shared<egt::ImageLabel>(egt::Image(os.str()));
        boxes.push_back(std::make_shared<FloatingBox>(image,
                        moveparms[x].first,
                        moveparms[x].second));
        win.add(image);
    }

    const auto SOFTHARD_COUNT = 2;

    // hardware (or emulated)
    for (auto x = 0; x < SOFTHARD_COUNT; x++)
    {
        std::stringstream os;
        os << "file:image" << image_index++ << ".png";
        auto image = std::make_shared<egt::ImageLabel>(egt::Image(os.str()));
        auto plane = std::make_shared<egt::Window>(egt::Size(image->width(),
                     image->height()));
        plane->color(egt::Palette::ColorId::bg, egt::Palette::transparent);
        plane->fill_flags().clear();
        plane->add(image);
        plane->show();
        plane->move(egt::Point(100, 100));
        boxes.push_back(std::make_shared<FloatingBox>(plane, moveparms[x].first,
                        moveparms[x].second));
        win.add(plane);
    }

    egt::PeriodicTimer movetimer(std::chrono::milliseconds(30));
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
        ss << "CPU: " << static_cast<int>(tools.usage(0)) << "%";
        label1.text(ss.str());
    });
    cputimer.start();

    return app.run();
}
