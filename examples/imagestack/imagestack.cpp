/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <memory>
#include <string>
#include <vector>

using namespace egt;

class MainWindow : public TopWindow
{
public:

    class LauncherItem : public Frame
    {
    public:
        explicit LauncherItem(const Image& image)
            : m_background(image)
        {
            boxtype(Theme::boxtype::none);
            flags().set(Widget::flag::no_layout);

            m_background.align(alignmask::expand);
            m_background.image_align(alignmask::expand);
            add(m_background);
        }

    private:
        ImageLabel m_background;
    };

    MainWindow()
    {
        background(Image("background.png"));

        auto egt_logo = std::make_shared<ImageLabel>(Image("@128px/egt_logo_black.png"));
        egt_logo->align(alignmask::center | alignmask::top);
        egt_logo->margin(5);
        add(egt_logo);

        m_animation.starting(0);
        m_animation.duration(std::chrono::seconds(2));
        m_animation.easing_func(easing_cubic_easeout);
        m_animation.on_change(std::bind(&MainWindow::move_boxes,
                                        this, std::placeholders::_1));
        m_seq.add(m_delay);
        m_seq.add(m_animation);

        load();
    }

    void handle(Event& event) override
    {
        TopWindow::handle(event);

        switch (event.id())
        {
        case eventid::pointer_drag_start:
            m_seq.reset();
            if (!m_boxes.empty())
                m_start = m_boxes.front()->x();
            break;
        case eventid::pointer_drag:
            move_boxes(event.pointer().point.x() -
                       event.pointer().drag_start.x());
            event.stop();
            break;
        case eventid::pointer_drag_stop:
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
            auto image = Image("image" + std::to_string(x) + ".png");
            auto box = std::make_shared<LauncherItem>(image);
            box->resize(Size(width() / 4, height() - 100));
            m_boxes.push_back(box);
        }

        for (auto& box : detail::reverse_iterate(m_boxes))
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
    AnimationSequence m_seq;
    AnimationDelay m_delay{std::chrono::seconds(2)};
    PropertyAnimator m_animation;
};

int main(int argc, const char** argv)
{
    Application app(argc, argv, "imagestack");

    add_search_path("images/");

    MainWindow window;
    window.show();

    return app.run();
}
