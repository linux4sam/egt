/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <memory>
#include <string>
#include <vector>

class MainWindow : public egt::TopWindow
{
public:

    class LauncherItem : public egt::Frame
    {
    public:
        explicit LauncherItem(const egt::Image& image)
            : m_background(image)
        {
            fill_flags().clear();
            flags().set(egt::Widget::Flag::no_layout);

            m_background.align(egt::AlignFlag::expand);
            m_background.image_align(egt::AlignFlag::expand);
            add(m_background);
        }

    private:
        egt::ImageLabel m_background;
    };

    MainWindow()
    {
        background(egt::Image("file:images/background.png"));

        auto egt_logo = std::make_shared<egt::ImageLabel>(egt::Image("icon:egt_logo_black.png;128"));
        egt_logo->align(egt::AlignFlag::center | egt::AlignFlag::top);
        egt_logo->margin(5);
        add(egt_logo);

        m_animation.starting(0);
        m_animation.duration(std::chrono::seconds(2));
        m_animation.easing_func(egt::easing_cubic_easeout);
        m_animation.on_change(std::bind(&MainWindow::move_boxes,
                                        this, std::placeholders::_1));
        m_seq.add(m_delay);
        m_seq.add(m_animation);

        load();
    }

    void handle(egt::Event& event) override
    {
        egt::TopWindow::handle(event);

        switch (event.id())
        {
        case egt::EventId::pointer_drag_start:
            m_seq.reset();
            if (!m_boxes.empty())
                m_start = m_boxes.front()->x();
            break;
        case egt::EventId::pointer_drag:
            move_boxes(event.pointer().point.x() -
                       event.pointer().drag_start.x());
            event.stop();
            break;
        case egt::EventId::pointer_drag_stop:
            if (!m_boxes.empty())
            {
                m_start = m_boxes.front()->x();
                m_animation.ending(width() - m_boxes.front()->width() - m_boxes.front()->x());
                m_seq.start();
            }
            break;
        default:
            break;
        }
    }

    void move_boxes(int diff = 0)
    {
        if (m_boxes.empty())
            return;

        const auto& last = m_boxes.back();
        if (diff < 0 && last->x() <= center().x())
            return;

        auto x = m_start + diff;
        for (auto& box : m_boxes)
        {
            if (x + box->width() > width())
                x = width() - box->width();
            box->x(x);
            x += box->width() + 10;
        }
    }

private:

    void load()
    {
        for (auto x = 0; x < 9; x++)
        {
            auto image = egt::Image("file:images/image" + std::to_string(x) + ".png");
            auto box = std::make_shared<LauncherItem>(image);
            box->resize(egt::Size(width() / 4, height() - 80));
            m_boxes.push_back(box);
        }

        for (auto& box : egt::detail::reverse_iterate(m_boxes))
        {
            box->y(center().y() - box->height() / 2);
            box->x(width() - box->width());
            add(box);
        }

        if (!m_boxes.empty())
        {
            m_start = m_boxes.front()->x();
            move_boxes();
        }
    }

    std::vector<std::shared_ptr<LauncherItem>> m_boxes;
    int m_start{0};
    egt::AnimationSequence m_seq;
    egt::AnimationDelay m_delay{std::chrono::seconds(2)};
    egt::PropertyAnimator m_animation;
};

int main(int argc, const char** argv)
{
    egt::Application app(argc, argv, "imagestack");
    egt::add_search_path(EXAMPLEDATA);

    MainWindow window;
    window.show();

    return app.run();
}
