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
            no_layout(true);

            m_background.align(egt::AlignFlag::expand);
            m_background.image_align(egt::AlignFlag::center | egt::AlignFlag::expand);
            add(m_background);
        }

    private:
        egt::ImageLabel m_background;
    };

    MainWindow()
    {
        m_landscape = width() >= height();

        background(egt::Image("file:images/background.png"));

        auto egt_logo = std::make_shared<egt::ImageLabel>(egt::Image("icon:mgs_logo_black.png;128"));
        egt_logo->align(egt::AlignFlag::center_horizontal | egt::AlignFlag::top);
        egt_logo->margin(5);
        // The size needs to be set again since the margin has been modified.
        const auto m = egt_logo->moat();
        egt_logo->resize(egt_logo->image().size_orig() + egt::Size(2 * m, 2 * m));
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
            m_seq.stop();
            if (!m_boxes.empty())
                m_start = to_dim(m_boxes.front()->point());
            break;
        case egt::EventId::pointer_drag:
            move_boxes(to_dim(event.pointer().point) -
                       to_dim(event.pointer().drag_start));
            event.stop();
            break;
        case egt::EventId::pointer_drag_stop:
            if (!m_boxes.empty())
            {
                m_start = to_dim(m_boxes.front()->point());
                m_animation.ending(to_dim(size()) - to_dim(m_boxes.front()->size()) - to_dim(m_boxes.front()->point()));
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
        if (diff < 0 && to_dim(last->point()) <= to_dim(center()))
            return;

        auto pos = m_start + diff;
        for (auto& box : m_boxes)
        {
            if (pos + to_dim(box->size()) > to_dim(size()))
                pos = to_dim(size()) - to_dim(box->size());
            box->move(dim_to_point(box->point(), pos));
            pos += to_dim(box->size()) + 10;
        }
    }

protected:

    bool internal_drag() const override { return true; }

private:

    void load()
    {
        for (auto x = 0; x < 9; x++)
        {
            auto image = egt::Image("file:images/image" + std::to_string(x) + ".png");
            auto box = std::make_shared<LauncherItem>(image);
            if (m_landscape)
                box->resize(egt::Size(width() / 4, height()));
            else
                box->resize(egt::Size(width(), height() / 3));
            m_boxes.push_back(box);
        }

        for (auto& box : egt::detail::reverse_iterate(m_boxes))
        {
            auto p = other_dim_to_point(box->point(), to_other_dim(center()) - to_other_dim(box->size()) / 2);
            p = dim_to_point(p, to_dim(size()) - to_dim(box->size()));
            box->move(p);
            add(box);
        }

        if (!m_boxes.empty())
        {
            m_start = to_dim(m_boxes.front()->point());
            move_boxes();
        }
    }

    egt::DefaultDim to_dim(const egt::Point& p) const
    {
        return m_landscape ? p.x() : p.y();
    }

    egt::DefaultDim to_dim(const egt::DisplayPoint& p) const
    {
        return m_landscape ? p.x() : p.y();
    }

    egt::DefaultDim to_dim(const egt::Size& s) const
    {
        return m_landscape ? s.width() : s.height();
    }

    egt::DefaultDim to_other_dim(const egt::Point& p) const
    {
        return m_landscape ? p.y() : p.x();
    }

    egt::DefaultDim to_other_dim(const egt::Size& s) const
    {
        return m_landscape ? s.height() : s.width();
    }

    egt::Point dim_to_point(const egt::Point& point, egt::DefaultDim value) const
    {
        auto p = point;
        if (m_landscape)
            p.x(value);
        else
            p.y(value);
        return p;
    }

    egt::Point other_dim_to_point(const egt::Point& point, egt::DefaultDim value) const
    {
        auto p = point;
        if (m_landscape)
            p.y(value);
        else
            p.x(value);
        return p;
    }

    bool m_landscape{true};
    std::vector<std::shared_ptr<LauncherItem>> m_boxes;
    int m_start{0};
    egt::AnimationSequence m_seq;
    egt::AnimationDelay m_delay{std::chrono::seconds(2)};
    egt::PropertyAnimator m_animation;
};

int main(int argc, char** argv)
{
    egt::Application app(argc, argv);
#ifdef EXAMPLEDATA
    egt::add_search_path(EXAMPLEDATA);
#endif

    MainWindow window;
    window.show();

    return app.run();
}
