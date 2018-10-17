/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "mui/frame.h"
#include "mui/painter.h"
#include "mui/screen.h"
#include <sstream>

using namespace std;

namespace mui
{
    static uint32_t frame_id = 0;

    Frame::Frame(const Point& point, const Size& size, widgetmask flags)
        : Widget(point, size, flags | widgetmask::FRAME)
    {
        ostringstream ss;
        ss << "frame" << frame_id++;
        name(ss.str());
    }

    Frame::Frame(Frame& parent, const Point& point, const Size& size, widgetmask flags)
        : Frame(point, size, flags)
    {
        parent.add(this);
    }

    Widget* Frame::add(Widget* widget)
    {
        assert(widget);
        if (!widget)
            return nullptr;

        if (find(m_children.begin(), m_children.end(), widget) == m_children.end())
        {
            // cannot already have a parent
            assert(!widget->m_parent);

            // note order here - set parent and then damage
            widget->m_parent = this;
            widget->damage();
            m_children.push_back(widget);
        }

        return widget;
    }

    Widget* Frame::insert(Widget* widget, uint32_t index)
    {
        assert(widget);
        if (!widget)
            return nullptr;

        if (find(m_children.begin(), m_children.end(), widget) == m_children.end())
        {
            // cannot already have a parent
            assert(!widget->m_parent);

            // note order here - set parent and then damage
            widget->m_parent = this;

            m_children.insert(m_children.begin() + index, widget);

            // damage the whole frame
            damage();
        }

        return widget;
    }

    void Frame::remove(Widget* widget)
    {
        assert(widget);
        if (!widget)
            return;

        auto i = find(m_children.begin(), m_children.end(), widget);
        if (i != m_children.end())
        {
            // note order here - damage and then unset parent
            (*i)->damage();
            (*i)->m_parent = nullptr;
            m_children.erase(i);
        }
    }

    void Frame::remove_all()
    {
        for (auto& i : m_children)
        {
            // note order here - damage and then unset parent
            i->damage();
            i->m_parent = nullptr;
        }

        m_children.clear();
    }

    int Frame::handle(int event)
    {
        DBG(name() << " handle: " << event);

        switch (event)
        {
        case EVT_MOUSE_DOWN:
        case EVT_MOUSE_UP:
        case EVT_MOUSE_MOVE:
            for (auto& child : detail::reverse_iterate(m_children))
            {
                assert(child);

                if (child->disabled() /*&& !child->active()*/)
                    continue;

                /**
                 * @todo Broken. Need to get everything converted to
                 * coordinates of parent frame and away from screen
                 * coordinates.
                 */
                Point pos = screen_to_frame(mouse_position());

                if (Rect::point_inside(pos, child->box()))
                    if (child->handle(event))
                        return 1;
            }
            break;
        case EVT_KEY_DOWN:
        case EVT_KEY_UP:
            for (auto& child : detail::reverse_iterate(m_children))
            {
                if (child->disabled())
                    continue;

                if (child->focus())
                    if (child->handle(event))
                        return 1;
            }
            break;
        }

        return Widget::handle(event);
    }

    void Frame::add_damage(const Rect& rect)
    {
        if (rect.empty())
            return;

        DBG(name() << " damage: " << rect);

        IScreen::damage_algorithm(m_damage, rect);
    }

    void Frame::damage(const Rect& rect)
    {
        if (rect.empty())
            return;

        // damage propogates to top level frame
        if (m_parent)
        {
            m_parent->damage(rect);
            return;
        }

        add_damage(rect);
    }

    /**
     * @todo Must prevent any call to damage() while in a draw() call.
     * This will not work.
     */
    void Frame::draw(Painter& painter, const Rect& rect)
    {
        DBG(name() << " " << __PRETTY_FUNCTION__);
#if 0
        if (!is_flag_set(widgetmask::NO_BACKGROUND))
        {
            Painter::AutoSaveRestore sr(painter);
            painter.set_color(palette().color(Palette::BG));
            cairo_set_operator(painter.context().get(), CAIRO_OPERATOR_SOURCE);
            painter.draw_fill(rect);
        }
#else
        Painter::AutoSaveRestore sr(painter);
        painter.rectangle(rect);
        painter.clip();

        if (!is_flag_set(widgetmask::NO_BACKGROUND))
        {
            Painter::AutoSaveRestore sr(painter);
            auto cr = painter.context();

            cairo_set_operator(painter.context().get(), CAIRO_OPERATOR_SOURCE);
            painter.draw_basic_box(box(), palette().color(Palette::BORDER),
                                   palette().color(Palette::BG));

            //cairo_reset_clip(cr.get());
        }
#endif
        for (auto& child : m_children)
        {
            if (!child->visible())
                continue;

            // don't draw plane frame as children - this is
            // specifically handled by event loop
            if (child->is_flag_set(widgetmask::PLANE_WINDOW))
                continue;

            if (Rect::intersect(rect, child->box()))
            {
                // don't give a child a rectangle that is outside of its own box
                auto r = Rect::intersection(rect, child->box());
                child->draw(painter, r);
            }
        }
    }

    void Frame::do_draw()
    {
        // this is a top level frame, we will use our damage bookkeeping to
        // draw what needs to be drawn
        if (m_damage.empty())
            return;

        DBG(name() << " " << __PRETTY_FUNCTION__);

        Painter painter(screen()->context());

        for (auto& damage : m_damage)
        {
            draw(painter, damage);
            /*
            if (!is_flag_set(widgetmask::NO_BACKGROUND))
            {
                Painter::AutoSaveRestore sr(painter);
                painter.set_color(palette().color(Palette::BG));
                cairo_set_operator(painter.context().get(), CAIRO_OPERATOR_SOURCE);
                painter.draw_fill(damage);
            }

            for (auto& child : m_children)
            {
                if (!child->visible())
                    continue;

                // don't draw plane frame as children - this is
                // specifically handled by event loop
                if (child->is_flag_set(widgetmask::PLANE_WINDOW))
                    continue;

                if (Rect::intersect(damage, child->box()))
                {
                    // don't give a child a rectangle that is outside of its own box
                    auto rect = Rect::intersection(damage, child->box());
                    child->draw(painter, rect);
                }
            }
            */
        }

        screen()->flip(m_damage);
        m_damage.clear();
    }

    void Frame::draw()
    {
        if (m_parent)
        {
            m_parent->draw();
            return;
        }

        do_draw();
    }

    void Frame::save_to_file(const std::string& filename)
    {
        if (m_parent)
        {
            m_parent->draw();
            return;
        }

        auto surface = cairo_get_target(screen()->context().get());
        cairo_surface_write_to_png(surface, filename.c_str());
    }



}
