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

Frame::Frame(const Rect& rect, const widgetflags& flags)
    : Widget(rect, flags)
{
    set_name("Frame" + std::to_string(m_widgetid));

    set_flag(widgetflag::FRAME);
    set_boxtype(Theme::boxtype::none);
}

Frame::Frame(Frame& parent, const Rect& rect, const widgetflags& flags)
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
        m_children.push_back(widget);
        widget->set_parent(this);
        // ensure alignment is set now
        widget->set_align(widget->align(), widget->margin());
    }

    return widget;
}

Widget& Frame::add(Widget& widget)
{
    add(&widget);
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

int Frame::handle(eventid event)
{
    auto ret = Widget::handle(event);

    switch (event)
    {
    case eventid::RAW_POINTER_DOWN:
    case eventid::RAW_POINTER_UP:
    case eventid::RAW_POINTER_MOVE:
    case eventid::POINTER_CLICK:
    case eventid::POINTER_DBLCLICK:
    case eventid::POINTER_HOLD:
    case eventid::POINTER_DRAG_START:
    case eventid::POINTER_DRAG:
    case eventid::POINTER_DRAG_STOP:
    case eventid::POINTER_BUTTON_DOWN:
    case eventid::POINTER_BUTTON_UP:
    {
        for (auto& child : detail::reverse_iterate(m_children))
        {
            if (child->readonly())
                continue;

            if (child->disabled())
                continue;

            if (!child->visible())
                continue;

            Point pos = to_child(from_screen(event_mouse()));

            if (Rect::point_inside(pos, child->box()))
            {
                auto ret = child->handle(event);

                if (ret)
                    return ret;

                break;
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
    if (unlikely(rect.empty()))
        return;

    DBG(name() << " damage: " << rect);

    IScreen::damage_algorithm(m_damage, rect);
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
        " " << box() << " " << m_flags << endl;

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
        if (w->is_flag_set(widgetflag::FRAME))
        {
            auto f = reinterpret_cast<Frame*>(w);

            if (f->is_flag_set(widgetflag::PLANE_WINDOW))
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

    if (!is_flag_set(widgetflag::PLANE_WINDOW))
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
        if (child->is_flag_set(widgetflag::WINDOW))
            continue;

        if (!child->visible())
            continue;

        // don't draw plane frame as child - this is
        // specifically handled by event loop
        if (child->is_flag_set(widgetflag::PLANE_WINDOW))
            continue;

        if (Rect::intersect(crect, child->box()))
        {
            // don't give a child a rectangle that is outside of its own box
            auto r = Rect::intersection(crect, child->box());

            // no matter what the child draws, clip the output to only the
            // rectangle we care about updating
            Painter::AutoSaveRestore sr(painter);
            painter.rectangle(r);
            painter.clip();

            experimental::code_timer(false, child->name() + " draw: ", [&]()
            {
                child->draw(painter, r);
            });
        }
    }

    for (auto& child : m_children)
    {
        if (!child->is_flag_set(widgetflag::WINDOW))
            continue;

        if (!child->visible())
            continue;

        // don't draw plane frame as child - this is
        // specifically handled by event loop
        if (child->is_flag_set(widgetflag::PLANE_WINDOW))
            continue;

        if (Rect::intersect(crect, child->box()))
        {
            // don't give a child a rectangle that is outside of its own box
            auto r = Rect::intersection(crect, child->box());

            // no matter what the child draws, clip the output to only the
            // rectangle we care about updating
            Painter::AutoSaveRestore sr(painter);
            painter.rectangle(r);
            painter.clip();

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
        if (child->is_flag_set(widgetflag::FRAME))
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

Frame::~Frame()
{
    remove_all();
}

}
}
