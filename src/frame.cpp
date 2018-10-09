/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "frame.h"
#include "mui/painter.h"
#include "mui/screen.h"

using namespace std;

namespace mui
{

    void Frame::add(Widget* widget)
    {
        assert(widget);
        if (!widget)
            return;

        if (find(m_children.begin(), m_children.end(), widget) == m_children.end())
        {
            // cannot already have a parent
            assert(!widget->m_parent);

            // note order here - set parent and then damage
            widget->m_parent = this;
            widget->damage();
            m_children.push_back(widget);
        }
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
        for (auto i : m_children)
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
            for (auto& child : reverse_iterate(m_children))
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
            for (auto& child : reverse_iterate(m_children))
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

        Rect damage = rect;

        if (!is_flag_set(FLAG_NO_BACKGROUND))
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
            if (child->is_flag_set(FLAG_PLANE_WINDOW))
                continue;

            if (Rect::is_intersect(damage, child->box()))
            {
                // don't give a child a rectangle that is outside of its own box
                auto rect = Rect::intersect(damage, child->box());
                child->draw(painter, rect);
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
            if (!is_flag_set(FLAG_NO_BACKGROUND))
            {
                Painter::AutoSaveRestore sr(painter);
                painter.set_color(palette().color(Palette::BG));
                cairo_set_operator(painter.context().get(), CAIRO_OPERATOR_SOURCE);
                painter.draw_fill(damage);
            }

            for (auto& child : m_children)
            {
                assert(child);

                if (!child->visible())
                    continue;

                // don't draw plane frame as children - this is
                // specifically handled by event loop
                if (child->is_flag_set(FLAG_PLANE_WINDOW))
                    continue;

                if (Rect::is_intersect(damage, child->box()))
                {
                    // don't give a child a rectangle that is outside of its own box
                    auto rect = Rect::intersect(damage, child->box());

                    // should not be necessary
                    //Painter::AutoSaveRestore sr(painter);

                    child->draw(painter, rect);
                }
            }
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

}
