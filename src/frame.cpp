/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/egtlog.h"
#include "detail/dump.h"
#include "egt/detail/layout.h"
#include "egt/detail/math.h"
#include "egt/frame.h"
#include "egt/input.h"
#include "egt/painter.h"
#include "egt/screen.h"
#include <cstdlib>
#include <string>

namespace egt
{
inline namespace v1
{

Frame::Frame(const Rect& rect, const Widget::Flags& flags) noexcept
    : Widget(rect, flags | Widget::Flag::frame)
{
    name("Frame" + std::to_string(m_widgetid));
    m_damage.reserve(10);
}

Frame::Frame(Frame& parent, const Rect& rect, const Widget::Flags& flags) noexcept
    : Frame(rect, flags)
{
    parent.add(*this);
}

void Frame::add(const std::shared_ptr<Widget>& widget)
{
    if (!widget)
        return;

    widget->set_parent(this);
    m_children.emplace_back(widget);
    layout();
}

bool Frame::is_child(Widget* widget) const
{
    if (!widget)
        return false;

    auto i = std::find_if(m_children.begin(), m_children.end(),
                          [widget](const auto & ptr)
    {
        return ptr.get() == widget;
    });

    return (i != m_children.end());
}

void Frame::remove(Widget* widget)
{
    if (!widget)
        return;

    auto i = std::find_if(m_children.begin(), m_children.end(),
                          [widget](const auto & ptr)
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

void Frame::remove_all_basic()
{
    for (auto& i : m_children)
    {
        // note order here - damage and then unset parent
        i->damage();
        i->m_parent = nullptr;
    }

    m_children.clear();
}

void Frame::remove_all()
{
    remove_all_basic();
    layout();
}

void Frame::handle(Event& event)
{
    if (event.quit())
        return;

    Widget::handle(event);

    switch (event.id())
    {
    case EventId::raw_pointer_down:
    case EventId::raw_pointer_up:
    case EventId::raw_pointer_move:
    case EventId::pointer_click:
    case EventId::pointer_dblclick:
    case EventId::pointer_hold:
    case EventId::pointer_drag_start:
    case EventId::pointer_drag:
    case EventId::pointer_drag_stop:
    {
        auto pos = display_to_local(event.pointer().point);

        for (auto& child : detail::reverse_iterate(m_children))
        {
            if (!child->can_handle_event())
                continue;

            if (child->box().intersect(pos))
            {
                child->handle(event);
                break;
            }
        }

        break;
    }

    case EventId::keyboard_down:
    case EventId::keyboard_up:
    case EventId::keyboard_repeat:
    {
        for (auto& child : detail::reverse_iterate(m_children))
        {
            if (!child->can_handle_event())
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
            if (child->frame())
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

    if (egt_unlikely(rect.empty()))
        return;

    // not allowed to damage() in draw()
    assert(!m_in_draw);
    if (m_in_draw)
        return;

    EGTLOG_TRACE("{} damage:{}", name(), rect);

    // No damage outside of our box().  There are cases where this is expected,
    // for example, when a widget is halfway off the screen. So, we truncate the
    // to just the part we care about.
    auto r = Rect::intersection(rect, to_child(box()));

    Screen::damage_algorithm(m_damage, r);
}

void Frame::damage(const Rect& rect)
{
    if (egt_unlikely(rect.empty()))
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

void Frame::walk(const WalkCallback& callback, int level)
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

void Frame::zorder_down(const Widget* widget)
{
    auto i = std::find_if(m_children.begin(), m_children.end(),
                          [widget](const auto & ptr)
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
                          [widget](const auto & ptr)
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
            layout();
        }
    }
}

void Frame::zorder_bottom(const Widget* widget)
{
    if (m_children.size() <= 1)
        return;

    auto i = std::find_if(m_children.begin(), m_children.end(),
                          [widget](const auto & ptr)
    {
        return ptr.get() == widget;
    });
    if (i != m_children.end() && i != m_children.begin())
    {
        std::rotate(m_children.begin(), i, i + 1);
        layout();
    }
}

void Frame::zorder_top(const Widget* widget)
{
    if (m_children.size() <= 1)
        return;

    auto i = std::find_if(m_children.begin(), m_children.end(),
                          [widget](const auto & ptr)
    {
        return ptr.get() == widget;
    });
    if (i != m_children.end())
    {
        std::rotate(i, i + 1, m_children.end());
        layout();
    }
}

size_t Frame::zorder(const Widget* widget) const
{
    auto i = std::find_if(m_children.begin(), m_children.end(),
                          [widget](const auto & ptr)
    {
        return ptr.get() == widget;
    });
    if (i != m_children.end())
    {
        return std::distance(m_children.begin(), i);
    }

    return 0;
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
    EGTLOG_TRACE("{} draw {}", name(), rect);

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

    if (clip())
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
    if (!fill_flags().empty() || border())
    {
        Palette::GroupId group = Palette::GroupId::normal;
        if (disabled())
            group = Palette::GroupId::disabled;
        else if (active())
            group = Palette::GroupId::active;

        theme().draw_box(painter,
                         fill_flags(),
                         to_child(box()),
                         color(Palette::ColorId::border, group),
                         color(Palette::ColorId::bg, group),
                         border(),
                         margin(),
                         border_radius(),
                         border_flags());
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
        if (child->plane_window())
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

        if (detail::float_equal(child->alpha(), 1.f))
        {
            Painter::AutoSaveRestore sr2(painter);

            // no matter what the child draws, clip the output to only the
            // rectangle we care about updating
            if (clip())
            {
                painter.draw(r);
                painter.clip();
            }

            detail::code_timer(time_child_draw_enabled(), child->name() + " draw: ", [child, &painter, &r]()
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
                if (clip())
                {
                    painter.draw(r);
                    painter.clip();
                }

                detail::code_timer(time_child_draw_enabled(), child->name() + " draw: ", [child, &painter, &r]()
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
#if CAIRO_HAS_PNG_FUNCTIONS == 1
    /// @todo should this be redirected to parent()?
    std::string name = filename;
    if (name.empty())
        name = fmt::format("{}.png", this->name());

    auto surface = cairo_get_target(screen()->context().get());
    cairo_surface_write_to_png(surface, name.c_str());
#else
    detail::ignoreparam(filename);
    detail::error("png support not available");
#endif
}

void Frame::paint_children_to_file()
{
    for (auto& child : m_children)
    {
        if (child->frame())
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
    auto reset = detail::on_scope_exit([this]() { m_in_layout = false; });

    auto area = content_area();

    for (auto& child : m_children)
    {
        auto bounding = to_child(area);
        if (bounding.empty())
            continue;

        child->layout();

        auto r = detail::align_algorithm(child->box(),
                                         bounding,
                                         child->align(),
                                         0,
                                         child->horizontal_ratio(),
                                         child->vertical_ratio(),
                                         child->xratio(),
                                         child->yratio());
        child->box(r);
    }
}

void Frame::serialize(Serializer& serializer) const
{
    Widget::serialize(serializer);

    int level = serializer.level() + 1;

    for (auto& child : m_children)
        serializer.add(child.get(), level);
}

void Frame::deserialize(const std::string& name, const std::string& value,
                        const Serializer::Attributes& attrs)
{
    Widget::deserialize(name, value, attrs);
}

Frame::~Frame() noexcept
{
    remove_all_basic();
}

}
}
