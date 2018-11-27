/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/frame.h"
#include "egt/painter.h"
#include "egt/screen.h"
#include <sstream>

using namespace std;

namespace egt
{
    static auto frame_id = 0;

    Frame::Frame(const Rect& rect, widgetmask flags)
        : Widget(rect, flags | widgetmask::FRAME)
    {
        ostringstream ss;
        ss << "frame" << frame_id++;
        set_name(ss.str());
    }

    Frame::Frame(Frame& parent, const Rect& rect, widgetmask flags)
        : Frame(rect, flags)
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

    void Frame::move(const Point& point)
    {
        if (point != box().point())
        {
            //if (top_level())
            {
                auto diff = point - m_box.point();

                for (auto& i : m_children)
                {
                    i->move(i->box().point() + diff);
                }
            }

            Widget::move(point);
        }
    }

    Widget& Frame::add(Widget& widget)
    {
        add(&widget);
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

    void Frame::set_focus(Widget* widget)
    {
        if (m_parent)
            m_parent->set_focus(widget);
        else if (m_focus_widget != widget)
        {
            if (widget)
                DBG(name() << " has new focus widget: " << widget->name());
            else
                DBG(name() << " has no focus widget");
            if (m_focus_widget)
                m_focus_widget->lost_focus();

            m_focus_widget = widget;
        }
    }

    int Frame::handle(eventid event)
    {
        DBG(name() << " handle: " << event);

        switch (event)
        {
        case eventid::MOUSE_UP:
        //case eventid::MOUSE_MOVE:
        //case eventid::KEYBOARD_UP:
        //case eventid::KEYBOARD_REPEAT:
        //case eventid::BUTTON_UP:
        // pair the eventid::MOUSE_UP event with the focus widget
        //if (m_focus_widget)
        //  break;
        case eventid::MOUSE_DOWN:
        case eventid::MOUSE_MOVE:
        case eventid::BUTTON_DOWN:
        case eventid::MOUSE_DBLCLICK:
        case eventid::KEYBOARD_DOWN:
            for (auto& child : detail::reverse_iterate(m_children))
            {
                if (child->disabled())
                    continue;

                if (!child->visible())
                    continue;

                Point pos = screen_to_frame(event_mouse());

                if (Rect::point_inside(pos, child->box()))
                {
                    auto ret = child->handle(event);

                    if (event == eventid::MOUSE_DOWN &&
                        child->is_flag_set(widgetmask::CLICK_FOCUS))
                    {
                        set_focus(child);
                    }

                    if (ret)
                    {
                        return ret;
                    }

                    //break;
                }
            }

            break;
        default:
            break;
        }

#if 0
        if (m_focus_widget)
        {
            switch (event)
            {
            case eventid::MOUSE_UP:
            //case eventid::KEYBOARD_UP:
            //case eventid::KEYBOARD_REPEAT:
            //case eventid::BUTTON_UP:
            case eventid::MOUSE_MOVE:
                //case eventid::MOUSE_DOWN:
                //case eventid::MOUSE_UP:
                //case eventid::BUTTON_UP:
                //case eventid::BUTTON_DOWN:
                //case eventid::MOUSE_DBLCLICK:
            {
                Point pos = screen_to_frame(event_mouse());

                if (Rect::point_inside(pos, m_focus_widget->box()))
                {
                    auto ret = m_focus_widget->handle(event);
                    if (ret)
                        return ret;
                }
                break;
            }
            default:
            {
                auto ret = m_focus_widget->handle(event);
                if (ret)
                    return ret;
                break;
            }
            }
        }
#endif

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

        // damage propagates to top level frame
        if (m_parent)
        {
            m_parent->damage(rect);
            return;
        }

        add_damage(rect);
    }

    /**
     * @todo Prevent any call to damage() while in a draw() call.
     */
    void Frame::draw(Painter& painter, const Rect& rect)
    {
        DBG(name() << " " << __PRETTY_FUNCTION__);

        if (!is_flag_set(widgetmask::NO_BACKGROUND))
        {
            Painter::AutoSaveRestore sr(painter);
            painter.rectangle(rect);
            painter.clip();

            if (!is_flag_set(widgetmask::TRANSPARENT_BACKGROUND))
                cairo_set_operator(painter.context().get(), CAIRO_OPERATOR_SOURCE);

            painter.draw_box(*this,
                             Painter::boxtype::fill,
                             screen_to_frame(box()));
        }

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
                Painter::AutoSaveRestore sr(painter);
                painter.rectangle(child->box());
                painter.clip();

                // don't give a child a rectangle that is outside of its own box
                auto r = Rect::intersection(rect, child->box());

                //#define TIME_DRAW
#ifdef TIME_DRAW
                auto start = chrono::steady_clock::now();
#endif

                child->draw(painter, r);

#ifdef TIME_DRAW
                auto end = chrono::steady_clock::now();
                auto diff = end - start;

                cout << child->name() << " draw: " << chrono::duration<double, milli>(diff).count() << endl;
#endif
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

    void Frame::paint_to_file(const std::string& filename)
    {
        // TODO: hmm, should this be redirected to parent()?
        string name = filename;
        if (name.empty())
        {
            std::ostringstream ss;
            ss << this->name() << ".png";
            name = ss.str();
        }

        auto surface = cairo_get_target(screen()->context().get());
        cairo_surface_write_to_png(surface, name.c_str());
    }

    void Frame::paint_children_to_file()
    {
        for (auto& child : m_children)
        {
            if (child->is_flag_set(widgetmask::FRAME))
            {
                auto frame = dynamic_cast<Frame*>(child);
                frame->paint_children_to_file();
            }
            else
            {
                child->paint_to_file();
            }
        }
    }

}
