/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/frame.h"
#include "egt/input.h"
#include "egt/painter.h"
#include "egt/screen.h"
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

namespace egt
{
inline namespace v1
{

Frame::Frame(const Rect& rect, const Widget::flags_type& flags) noexcept
    : Widget(rect, flags)
{
    set_name("Frame" + std::to_string(m_widgetid));

    ncflags().set(Widget::flag::frame);
    set_boxtype(Theme::boxtype::none);
}

Frame::Frame(const Frame& rhs) noexcept
    : Widget(rhs),
      m_damage(rhs.m_damage)
{
    // perform deep copy of all children
    for (auto& child : rhs.m_children)
    {
        m_children.push_back(child->clone());
        m_children.back()->m_parent = this;
    }
}

Frame::Frame(Frame&& rhs) noexcept
    : Widget(std::move(rhs)),
      m_children(std::move(rhs.m_children)),
      m_damage(std::move(rhs.m_damage))
{
    for (auto& child : m_children)
        child->m_parent = this;
}

Frame& Frame::operator=(const Frame& rhs) noexcept
{
    if (&rhs != this)
    {
        Widget::operator=(rhs);

        m_damage = rhs.m_damage;

        // perform deep copy of all children
        for (auto& child : rhs.m_children)
        {
            m_children.push_back(child->clone());
            m_children.back()->m_parent = this;
        }
    }
    return *this;
}

Frame& Frame::operator=(Frame&& rhs) noexcept
{
    Widget::operator=(std::move(rhs));

    m_children = std::move(rhs.m_children);
    m_damage = std::move(rhs.m_damage);

    for (auto& child : m_children)
        child->m_parent = this;

    return *this;
}

void Frame::remove(Widget* widget)
{
    if (!widget)
        return;

    auto i = std::find_if(m_children.begin(), m_children.end(),
                          [widget](const std::shared_ptr<Widget>& ptr)
    {
        return ptr.get() == widget;
    });
    if (i != m_children.end())
    {
        // note order here - damage and then unset parent
        (*i)->damage();
        (*i)->m_parent = nullptr;
        m_children.erase(i);
    }
    else if (widget->m_parent == this)
    {
        widget->m_parent = nullptr;
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

int Frame::handle(eventid event)
{
    auto ret = Widget::handle(event);

    switch (event)
    {
    case eventid::raw_pointer_down:
    case eventid::raw_pointer_up:
    case eventid::raw_pointer_move:
    case eventid::pointer_click:
    case eventid::pointer_dblclick:
    case eventid::pointer_hold:
    case eventid::pointer_drag_start:
    case eventid::pointer_drag:
    case eventid::pointer_drag_stop:
    {
        for (auto& child : detail::reverse_iterate(m_children))
        {
            if (child->readonly())
                continue;

            if (child->disabled())
                continue;

            if (!child->visible())
                continue;

            Point pos = to_child(from_display(event::pointer().point));

            if (Rect::point_inside(pos, child->box()))
            {
                auto cret = child->handle(event);
                if (cret)
                    return cret;
            }
        }

        break;
    }
    default:
        break;
    }

    return ret;
}

void Frame::add_damage(const Rect& rect)
{
    // not allowed to damage in draw()
    assert(!m_in_draw);
    if (m_in_draw)
        return;

    if (unlikely(rect.empty()))
        return;

    DBG(name() << " damage: " << rect);

    Screen::damage_algorithm(m_damage, rect);
}

void Frame::damage(const Rect& rect)
{
    if (unlikely(rect.empty()))
        return;

    // don't damage if not even visible
    if (!visible())
        return;

    // damage propagates to top level frame
    if (m_parent)
    {
        m_parent->damage(to_parent(rect));
        return;
    }

    add_damage(rect);
}

void Frame::dump(std::ostream& out, int level)
{
    out << std::string(level, ' ') << name() <<
        " " << box() << " " << flags() << endl;

    for (auto& child : m_children)
    {
        child->dump(out, level + 1);
    }
}

Point Frame::to_panel(const Point& p)
{
    Point pp;
    Widget* w = this;
    while (w)
    {
        if (w->flags().is_set(Widget::flag::frame))
        {
            auto f = reinterpret_cast<Frame*>(w);

            if (f->flags().is_set(Widget::flag::plane_window))
            {
                pp = Point();
                break;
            }

            if (f->top_level())
            {
                pp = f->box().point();
                break;
            }

        }

        w = w->m_parent;
    }

    return p - pp;
}

/**
 * @todo Prevent any call to damage() while in a draw() call.
 */
void Frame::draw(Painter& painter, const Rect& rect)
{
    DBG(name() << " " << __PRETTY_FUNCTION__ << " " << rect);

    Painter::AutoSaveRestore sr(painter);

    draw_box(painter, rect);

    //
    // Origin about to change
    //

    auto cr = painter.context();
    Rect crect;

    if (!flags().is_set(Widget::flag::plane_window))
    {
        Point origin = to_panel(box().point());
        if (origin.x || origin.y)
            cairo_translate(cr.get(),
                            origin.x,
                            origin.y);
        crect = rect - origin;
    }
    else
    {
        crect = rect;
    }

    // OK, nasty - doing two loops: one for non windows, then for windows

    for (auto& child : m_children)
    {
        if (child->flags().is_set(Widget::flag::window))
            continue;

        if (!child->visible())
            continue;

        // don't draw plane frame as child - this is
        // specifically handled by event loop
        if (child->flags().is_set(Widget::flag::plane_window))
            continue;

        if (Rect::intersect(crect, child->box()))
        {
            // don't give a child a rectangle that is outside of its own box
            auto r = Rect::intersection(crect, child->box());

            // no matter what the child draws, clip the output to only the
            // rectangle we care about updating
            Painter::AutoSaveRestore sr2(painter);
            if (!child->flags().is_set(Widget::flag::no_clip))
            {
                painter.draw(r);
                painter.clip();
            }

            experimental::code_timer(false, child->name() + " draw: ", [&]()
            {
                child->draw(painter, r);
            });
        }
    }

    for (auto& child : m_children)
    {
        if (!child->flags().is_set(Widget::flag::window))
            continue;

        if (!child->visible())
            continue;

        // don't draw plane frame as child - this is
        // specifically handled by event loop
        if (child->flags().is_set(Widget::flag::plane_window))
            continue;

        if (Rect::intersect(crect, child->box()))
        {
            // don't give a child a rectangle that is outside of its own box
            auto r = Rect::intersection(crect, child->box());

            // no matter what the child draws, clip the output to only the
            // rectangle we care about updating
            Painter::AutoSaveRestore sr2(painter);
            if (!child->flags().is_set(Widget::flag::no_clip))
            {
                painter.draw(r);
                painter.clip();
            }

            experimental::code_timer(false, child->name() + " draw: ", [&]()
            {
                child->draw(painter, r);
            });
        }
    }
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
        if (child->flags().is_set(Widget::flag::frame))
        {
            auto frame = std::dynamic_pointer_cast<Frame>(child);
            frame->paint_children_to_file();
        }
        else
        {
            child->paint_to_file();
        }
    }
}

Frame::~Frame() noexcept
{
    remove_all();
}

}
}
