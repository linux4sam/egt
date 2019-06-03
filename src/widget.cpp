/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/canvas.h"
#include "egt/detail/alignment.h"
#include "egt/frame.h"
#include "egt/geometry.h"
#include "egt/input.h"
#include "egt/painter.h"
#include "egt/screen.h"
#include "egt/utils.h"
#include "egt/widget.h"
#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

namespace egt
{
inline namespace v1
{

static auto global_widget_id = 0;

Widget::Widget(const Rect& rect, const Widget::flags_type& flags) noexcept
    : m_box(rect),
      m_widgetid(global_widget_id++),
      m_widget_flags(flags),
      m_palette(make_unique<Palette>())
{
    set_name("Widget" + std::to_string(m_widgetid));
}

Widget::Widget(Frame& parent, const Rect& rect, const Widget::flags_type& flags) noexcept
    : Widget(rect, flags)
{
    parent.add(*this);
}

void Widget::handle(Event& event)
{
    if (event.quit())
        return;

    DBG(name() << " handle: " << event);

    switch (event.id())
    {
    case eventid::on_gain_focus:
        m_focus = true;
        break;
    case eventid::on_lost_focus:
        m_focus = false;
        break;
    case eventid::raw_pointer_down:
        if (flags().is_set(Widget::flag::grab_mouse))
        {
            set_active(true);
            event.grab(this);
        }
        break;
    case eventid::raw_pointer_up:
        set_active(false);
        break;
    default:
        break;
    }

    invoke_handlers(event);
}

void Widget::move_to_center(const Point& point)
{
    if (center() != point)
    {
        Point pos(point.x - w() / 2,
                  point.y - h() / 2);

        move(pos);
    }
}

void Widget::resize(const Size& s)
{
    if (s != box().size())
    {
        damage();
        m_box.size(s);
        damage();
        parent_layout();
    }
}

void Widget::resize_by_ratio(int hratio, int vratio)
{
    Size size(static_cast<float>(w()) * (static_cast<float>(hratio) / 100.),
              static_cast<float>(h()) * (static_cast<float>(vratio) / 100.));
    resize(size);
}

void Widget::move(const Point& point)
{
    if (point != box().point())
    {
        damage();
        m_box.point(point);
        damage();

        /// @todo this is too expensive for animations
        parent_layout();
    }
}

void Widget::set_box(const Rect& rect)
{
    move(rect.point());
    resize(rect.size());
}

void Widget::hide()
{
    if (flags().is_set(Widget::flag::invisible))
        return;
    // careful attention to ordering
    damage();
    flags().set(Widget::flag::invisible);
    invoke_handlers(eventid::hide);
}

void Widget::show()
{
    if (!flags().is_set(Widget::flag::invisible))
        return;
    // careful attention to ordering
    flags().clear(Widget::flag::invisible);
    damage();
    invoke_handlers(eventid::show);
}

bool Widget::visible() const
{
    return !flags().is_set(Widget::flag::invisible);
}

bool Widget::active() const
{
    return flags().is_set(Widget::flag::active);
}

void Widget::set_active(bool value)
{
    if (flags().is_set(Widget::flag::active) != value)
    {
        if (value)
            flags().set(Widget::flag::active);
        else
            flags().clear(Widget::flag::active);
        damage();
    }
}

bool Widget::readonly() const
{
    return flags().is_set(Widget::flag::readonly);
}

void Widget::set_readonly(bool value)
{
    if (flags().is_set(Widget::flag::readonly) != value)
    {
        if (value)
            flags().set(Widget::flag::readonly);
        else
            flags().clear(Widget::flag::readonly);
        damage();
    }
}

bool Widget::disabled() const
{
    return flags().is_set(Widget::flag::disabled);
}

void Widget::disable()
{
    if (flags().is_set(Widget::flag::disabled))
        return;
    damage();
    flags().set(Widget::flag::disabled);
}

void Widget::enable()
{
    if (!flags().is_set(Widget::flag::disabled))
        return;
    damage();
    flags().clear(Widget::flag::disabled);
}

void Widget::damage()
{
    damage(box());
}

void Widget::damage(const Rect& rect)
{
    if (unlikely(rect.empty()))
        return;

    // don't damage if not even visible
    if (!visible())
        return;

    // damage propagates to top level frame
    if (m_parent)
        m_parent->damage_from_child(to_parent(rect));
}

const Rect& Widget::box() const
{
    return m_box;
}

Size Widget::size() const
{
    return box().size();
}

Point Widget::point() const
{
    return box().point();
}

void Widget::set_palette(const Palette& palette)
{
    m_palette = make_unique<Palette>(palette);
    damage();
}

void Widget::reset_palette()
{
    m_palette.reset();
}

Palette::pattern_type Widget::color(Palette::ColorId id) const
{
    Palette::GroupId group = Palette::GroupId::normal;
    if (disabled())
        group = Palette::GroupId::disabled;
    else if (active())
        group = Palette::GroupId::active;

    return color(id, group);
}

Palette::pattern_type Widget::color(Palette::ColorId id, Palette::GroupId group) const
{
    if (m_palette->exists(id, group))
        return m_palette->color(id, group);

    return default_palette().color(id, group);
}

void Widget::set_color(Palette::ColorId id,
                       const Palette::pattern_type& color,
                       Palette::GroupId group)
{
    m_palette->set(id, group, color);
    damage();
}

const Palette& Widget::default_palette() const
{
    return theme().palette();
}

Frame* Widget::parent()
{
    return m_parent;
}

const Frame* Widget::parent() const
{
    return m_parent;
}

Screen* Widget::screen()
{
    assert(m_parent);
    return parent()->screen();
}

void Widget::set_align(alignmask a)
{
    if (detail::change_if_diff<>(m_align, a))
        parent_layout();
}

Point Widget::to_parent(const Point& r) const
{
    if (parent())
        return r + parent()->point();

    return r;
}

DisplayPoint Widget::display_origin()
{
    DisplayPoint p(x(), y());

    auto par = parent();
    while (par)
    {
        p += DisplayPoint(par->x(), par->y());
        par = par->parent();
    }

    return p;
}

Size Widget::min_size_hint() const
{
    return Size(margin() * 2, margin() * 2) +
           Size(border() * 2, border() * 2) +
           Size(padding() * 2, padding() * 2);
}

void Widget::paint(Painter& painter)
{
    Painter::AutoSaveRestore sr(painter);

    // move origin
    cairo_translate(painter.context().get(),
                    -x(),
                    -y());

    draw(painter, box());
}

void Widget::paint_to_file(const std::string& filename)
{
    string name = filename;
    if (name.empty())
    {
        std::ostringstream ss;
        ss << this->name() << ".png";
        name = ss.str();
    }

    auto surface = shared_cairo_surface_t(
                       cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                               w(), h()),
                       cairo_surface_destroy);

    auto cr = shared_cairo_t(cairo_create(surface.get()), cairo_destroy);

    Painter painter(cr);
    paint(painter);
    cairo_surface_write_to_png(surface.get(), name.c_str());
}

void Widget::dump(std::ostream& out, int level)
{
    out << std::string(level, ' ') << name() <<
        " " << box() << " " << flags();
    out << " box(" << margin() << "," << padding() << "," << border() << ")";
    out << " align(" << align() << ")";
    out << std::endl;
}

void Widget::walk(walk_callback_t callback, int level)
{
    callback(this, level);
}

void Widget::set_theme(const Theme& theme)
{
    m_theme = make_unique<Theme>(theme);
}

void Widget::reset_theme()
{
    m_theme.reset();
}

void Widget::draw_box(Painter& painter, Palette::ColorId bg,
                      Palette::ColorId border) const
{
    theme().draw_box(painter, *this, bg, border);
}

const Theme& Widget::theme() const
{
    if (m_theme)
        return *m_theme;

    if (parent())
        return parent()->theme();

    return global_theme();
}

void Widget::zorder_down()
{
    if (m_parent)
        m_parent->zorder_down(this);
}

void Widget::zorder_up()
{
    if (m_parent)
        m_parent->zorder_up(this);
}

void Widget::detach()
{
    if (m_parent)
    {
        m_parent->remove(this);
        m_parent = nullptr;
    }
}

Rect Widget::content_area() const
{
    auto moat = margin() + padding() + border();
    auto b = box();
    b += Point(moat, moat);
    b -= Size(2. * moat, 2. * moat);
    return b;
}

void Widget::layout()
{}

Widget::~Widget() noexcept
{
    detach();
}

void Widget::set_parent(Frame* parent)
{
    // cannot already have a parent
    assert(!m_parent);

    if (!m_parent)
    {
        m_parent = parent;
        damage();
    }
}

void Widget::parent_layout()
{
    if (!visible())
        return;

    if (flags().is_set(Widget::flag::no_layout))
        return;

    if (parent())
        parent()->layout();
}

DisplayPoint Widget::local_to_display(const Point& p)
{
    DisplayPoint p2(p.x, p.y);

    auto par = parent();
    while (par)
    {
        p2 += DisplayPoint(par->point().x, par->point().y);
        par = par->parent();
    }

    return p2 + DisplayPoint(x(), y());
}

Point Widget::display_to_local(const DisplayPoint& p)
{
    Point p2(p.x, p.y);

    auto par = parent();
    while (par)
    {
        p2 -= par->point();
        par = par->parent();
    }

    return p2 - point();
}

}
}
