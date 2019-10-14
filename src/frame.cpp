/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/layout.h"
#include "egt/detail/math.h"
#include "egt/frame.h"
#include "egt/input.h"
#include "egt/painter.h"
#include "egt/screen.h"
#include <cstdlib>
#include <iostream>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string>

namespace egt
{
inline namespace v1
{

Frame::Frame(const Rect& rect, const flags_type& flags) noexcept
    : Widget(rect, flags | Widget::flag::frame)
{
    set_name("Frame" + std::to_string(m_widgetid));
    set_boxtype(Theme::boxtype::none);
}

void Frame::add(std::shared_ptr<Widget> widget)
{
    if (!widget)
        return;

    widget->set_parent(this);
    m_children.emplace_back(std::move(widget));
    layout();
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
        layout();
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
    layout();
}

void Frame::handle(Event& event)
{
    if (event.quit())
        return;

    Widget::handle(event);

    switch (event.id())
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
        auto pos = display_to_local(event.pointer().point);

        for (auto& child : detail::reverse_iterate(m_children))
        {
            if (child->readonly() ||
                child->disabled() ||
                !child->visible())
                continue;

            if (child->box().intersect(pos))
            {
                child->handle(event);
                break;
            }
        }

        break;
    }

    case eventid::keyboard_down:
    case eventid::keyboard_up:
    case eventid::keyboard_repeat:
    {
        for (auto& child : detail::reverse_iterate(m_children))
        {
            if (child->readonly() ||
                child->disabled() ||
                !child->visible())
                continue;

            child->handle(event);
            if (event.quit())
                return;
        }

        break;
    }

    default:
        break;
    }
}

Widget* Frame::hit_test(const DisplayPoint& point)
{
    Point pos = display_to_local(point);

    for (auto& child : detail::reverse_iterate(m_children))
    {
        if (child->box().intersect(pos))
        {
            if (child->flags().is_set(Widget::flag::frame))
            {
                auto frame = dynamic_cast<Frame*>(child.get());
                if (frame)
                    return frame->hit_test(point);
            }
            else
            {
                return child.get();
            }
        }
    }

    if (local_box().intersect(pos))
        return this;

    return nullptr;
}

void Frame::add_damage(const Rect& rect)
{
    // if we get here, we must have a screen
    assert(has_screen());
    if (!has_screen())
        return;

    if (unlikely(rect.empty()))
        return;

    // not allowed to damage() in draw()
    assert(!m_in_draw);
    if (m_in_draw)
        return;

    SPDLOG_TRACE("{} damage:{}", name(), rect);

    // No damage outside of our box().  There are cases where this is expected,
    // for example, when a widget is halfway off the screen. So, we truncate the
    // to just the part we care about.
    auto r = Rect::intersection(rect, to_child(box()));

    Screen::damage_algorithm(m_damage, r);
}

void Frame::damage(const Rect& rect)
{
    if (unlikely(rect.empty()))
        return;

    // don't damage if not even visible
    if (!visible())
        return;

    // damage propagates up to frame with screen
    if (!has_screen())
    {
        if (parent())
            parent()->damage_from_child(to_parent(rect));

        // have no parent or screen - nowhere to put damage
        return;
    }

    add_damage(rect);
}

void Frame::dump(std::ostream& out, int level)
{
    Widget::dump(out, level);

    for (auto& child : m_children)
        child->dump(out, level + 1);
}

void Frame::walk(walk_callback_t callback, int level)
{
    if (!callback(this, level))
        return;

    for (auto& child : m_children)
        child->walk(callback, level + 1);
}

Point Frame::to_panel(const Point& p)
{
    if (has_screen())
        return p - point();

    if (parent())
        return parent()->to_panel(p - point());

    return p;
}

void Frame::zorder_down()
{
    Widget::zorder_down();
}

void Frame::zorder_up()
{
    Widget::zorder_up();
}

void Frame::zorder_down(const Widget* widget)
{
    auto i = std::find_if(m_children.begin(), m_children.end(),
                          [widget](const std::shared_ptr<Widget>& ptr)
    {
        return ptr.get() == widget;
    });
    if (i != m_children.end() && i != m_children.begin())
    {
        auto to = std::prev(i);
        (*i)->damage();
        (*to)->damage();
        std::iter_swap(i, to);
    }
}

void Frame::zorder_up(const Widget* widget)
{
    auto i = std::find_if(m_children.begin(), m_children.end(),
                          [widget](const std::shared_ptr<Widget>& ptr)
    {
        return ptr.get() == widget;
    });
    if (i != m_children.end())
    {
        auto to = std::next(i);
        if (to != m_children.end())
        {
            (*i)->damage();
            (*to)->damage();
            std::iter_swap(i, to);
        }
    }
}

void Frame::zorder_bottom()
{
    Widget::zorder_bottom();
}

void Frame::zorder_top()
{
    Widget::zorder_top();
}

void Frame::zorder_bottom(const Widget* widget)
{
    auto i = std::find_if(m_children.begin(), m_children.end(),
                          [widget](const std::shared_ptr<Widget>& ptr)
    {
        return ptr.get() == widget;
    });
    if (i != m_children.end())
    {
        std::rotate(m_children.begin(), i, i + 1);
    }
}

void Frame::zorder_top(const Widget* widget)
{
    auto i = std::find_if(m_children.begin(), m_children.end(),
                          [widget](const std::shared_ptr<Widget>& ptr)
    {
        return ptr.get() == widget;
    });
    if (i != m_children.end())
    {
        std::rotate(i, i + 1, m_children.end());
    }
}

size_t Frame::zorder(const Widget* widget) const
{
    auto i = std::find_if(m_children.begin(), m_children.end(),
                          [widget](const std::shared_ptr<Widget>& ptr)
    {
        return ptr.get() == widget;
    });
    if (i != m_children.end())
    {
        return std::distance(m_children.begin(), i);
    }

    return 0;
}

size_t Frame::zorder() const
{
    return Widget::zorder();
}

static inline bool time_child_draw_enabled()
{
    static int value = 0;
    if (value == 0)
    {
        if (std::getenv("EGT_TIME_DRAW"))
            value += 1;
        else
            value -= 1;
    }
    return value == 1;
}

void Frame::draw(Painter& painter, const Rect& rect)
{
    SPDLOG_TRACE("{} draw {}", name(), rect);

    Painter::AutoSaveRestore sr(painter);

    // child rect
    auto crect = rect;

    // if this frame does not have a screen, it means the damage rect is in
    // coordinates of some parent frame, so we have to adjust the physical origin
    // and take it into account when looking at children, who's coordinates are
    // respective of this frame
    if (!has_screen())
    {
        const auto& origin = point();
        if (origin.x() || origin.y())
        {
            //
            // Origin about to change
            //
            auto cr = painter.context();
            cairo_translate(cr.get(),
                            origin.x(),
                            origin.y());
        }

        // adjust our child rect for comparison's below
        crect -= origin;
    }

    if (!flags().is_set(Widget::flag::no_clip))
    {
        // clip the damage rectangle, otherwise we will draw this whole frame
        // and then only draw the children inside the actual damage rect, which
        // will cover them
        painter.draw(crect);
        painter.clip();
    }

    // draw our frame box, but now that the physical origin has possibly changed
    // and our box() is relative to our parent, we have to adjust to our local
    // origin
    if (boxtype() != Theme::boxtype::none)
    {
        Palette::GroupId group = Palette::GroupId::normal;
        if (disabled())
            group = Palette::GroupId::disabled;
        else if (active())
            group = Palette::GroupId::active;

        theme().draw_box(painter,
                         boxtype(),
                         to_child(box()),
                         color(Palette::ColorId::border, group),
                         color(Palette::ColorId::bg, group),
                         border(),
                         margin());
    }

    if (m_children.empty())
        return;

    // keep the crect inside our content area
    crect = Rect::intersection(crect, to_child(content_area()));

    for (auto& child : m_children)
    {
        if (!child->visible())
            continue;

        // don't draw plane frame as child - this is
        // specifically handled by event loop
        if (child->flags().is_set(Widget::flag::plane_window))
            continue;

        draw_child(painter, crect, child.get());
    }
}

void Frame::draw_child(Painter& painter, const Rect& crect, Widget* child)
{
    if (child->box().intersect(crect))
    {
        // don't give a child a rectangle that is outside of its own box
        auto r = Rect::intersection(crect, child->box());
        if (r.empty())
            return;

        if (detail::float_compare(child->alpha(), 1.f))
        {
            Painter::AutoSaveRestore sr2(painter);

            // no matter what the child draws, clip the output to only the
            // rectangle we care about updating
            if (!child->flags().is_set(Widget::flag::no_clip))
            {
                painter.draw(r);
                painter.clip();
            }

            detail::code_timer(time_child_draw_enabled(), child->name() + " draw: ", [&]()
            {
                child->draw(painter, r);
            });
        }
        else
        {
            {
                Painter::AutoGroup group(painter);

                // no matter what the child draws, clip the output to only the
                // rectangle we care about updating
                if (!child->flags().is_set(Widget::flag::no_clip))
                {
                    painter.draw(r);
                    painter.clip();
                }

                detail::code_timer(time_child_draw_enabled(), child->name() + " draw: ", [&]()
                {
                    child->draw(painter, r);
                });
            }

            // we pushed a group for the child to draw into it, now paint that
            // child with its alpha component
            painter.paint(child->alpha());
        }

        special_child_draw(painter, child);
    }
}

void Frame::paint_to_file(const std::string& filename)
{
    /// @todo should this be redirected to parent()?
    std::string name = filename;
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

void Frame::layout()
{
    if (!visible())
        return;

    // we cannot layout with no space
    if (size().empty())
        return;

    if (m_in_layout)
        return;

    if (m_children.empty())
        return;

    m_in_layout = true;
    detail::scope_exit reset([this]() { m_in_layout = false; });

    auto area = content_area();

    for (auto& child : m_children)
    {
        //
        // first directly align the child, then if it is a frame, tell the
        // frame to layout().
        //
        if (child->align() != alignmask::none)
        {
            auto bounding = to_child(area);
            if (bounding.empty())
                return;

            child->layout();

            auto r = detail::align_algorithm(child->box(),
                                             bounding,
                                             child->align(),
                                             0,
                                             child->horizontal_ratio(),
                                             child->vertical_ratio(),
                                             child->xratio(),
                                             child->yratio());
            child->set_box(r);
        }
    }
}

Frame::~Frame() noexcept
{
    remove_all();
}

}
}
