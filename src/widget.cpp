/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/canvas.h"
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

namespace detail
{
Rect align_algorithm(const Size& size, const Rect& bounding,
                     alignmask align, int margin)
{
    if (unlikely(align == alignmask::NONE))
        return Rect(bounding.point(), size);

    auto p = bounding.point();
    auto nsize = size;

    if ((align & alignmask::CENTER) == alignmask::CENTER)
    {
        p.x = bounding.x + (bounding.w / 2) - (size.w / 2);
        p.y = bounding.y + (bounding.h / 2) - (size.h / 2);
    }

    if ((align & alignmask::LEFT) == alignmask::LEFT)
        p.x = bounding.x + margin;
    else if ((align & alignmask::RIGHT) == alignmask::RIGHT)
        p.x = bounding.x + bounding.w - size.w - margin;

    if ((align & alignmask::TOP) == alignmask::TOP)
        p.y = bounding.y + margin;
    else if ((align & alignmask::BOTTOM) == alignmask::BOTTOM)
        p.y = bounding.y + bounding.h - size.h - margin;

    if ((align & alignmask::EXPAND_HORIZONTAL) == alignmask::EXPAND_HORIZONTAL)
    {
        nsize.w = bounding.w - margin * 2;
        p.x = bounding.x + margin;
    }

    if ((align & alignmask::EXPAND_VERTICAL) == alignmask::EXPAND_VERTICAL)
    {
        nsize.h = bounding.h - margin * 2;
        p.y = bounding.y + margin;
    }

    return Rect(p, nsize);
}

void double_align(const Rect& main,
                  const Size& fsize, alignmask first_align, Rect& first,
                  const Size& ssize, alignmask second_align, Rect& second,
                  int margin)
{
    auto ftarget = detail::align_algorithm(fsize, main, first_align, margin);
    auto starget = detail::align_algorithm(ssize, main, second_align, margin);

    if ((second_align & alignmask::CENTER) == alignmask::CENTER)
    {
        starget.move_to_center(ftarget.center());
    }

    /*
     * The basic algoithm is to position the first target, then position the
     * second target relative to it.  If that results in the second target
     * going out of bounds of the main rectangle, move both inside the
     * rectangle.
     */
    if ((second_align & alignmask::LEFT) == alignmask::LEFT)
    {
        starget.x = ftarget.x - starget.w - margin;
        if (starget.x - margin < main.x)
        {
            auto diff = main.x - starget.x + margin;
            starget.x += diff;
            ftarget.x += diff;
        }
    }
    else if ((second_align & alignmask::RIGHT) == alignmask::RIGHT)
    {
        starget.x = ftarget.right() + margin;
        if (starget.right() + margin > main.right())
        {
            auto diff = starget.right() + margin - main.right();
            starget.x -= diff;
            ftarget.x -= diff;
        }
    }

    if ((second_align & alignmask::TOP) == alignmask::TOP)
    {
        starget.y = ftarget.y - starget.h - margin;
        if (starget.y - margin < main.y)
        {
            auto diff = main.y - starget.y + margin;
            starget.y += diff;
            ftarget.y += diff;
        }
    }
    else if ((second_align & alignmask::BOTTOM) == alignmask::BOTTOM)
    {
        starget.y = ftarget.bottom() + margin;
        if (starget.bottom() + margin > main.bottom())
        {
            auto diff = starget.bottom() + margin - main.bottom();
            starget.y -= diff;
            ftarget.y -= diff;
        }
    }

    first = ftarget;
    second = starget;
}
}

static auto global_widget_id = 0;

Widget::Widget(const Rect& rect, const widgetflags& flags) noexcept
    : m_box(rect),
      m_widgetid(global_widget_id++),
      m_flags(flags)
{
    set_name("Widget" + std::to_string(m_widgetid));
}

Widget::Widget(Frame& parent, const Rect& rect, const widgetflags& flags) noexcept
    : Widget(rect, flags)
{
    parent.add(this);
}

Widget::Widget(const Widget& rhs) noexcept
    : m_box(rhs.m_box),
      m_widgetid(global_widget_id++),
      m_flags(rhs.m_flags),
      m_align(rhs.m_align),
      m_margin(rhs.m_margin),
      m_boxtype(rhs.m_boxtype)
{
    /// @todo Handle m_focus

    set_name("Widget" + std::to_string(m_widgetid));

    if (rhs.m_palette)
        m_palette.reset(new Palette(*rhs.m_palette.get()));
    if (rhs.m_theme)
        m_theme.reset(new Theme(*rhs.m_theme.get()));

    if (rhs.m_parent)
        rhs.m_parent->add(this);
}

Widget::Widget(Widget&& rhs) noexcept
    : m_box(std::move(rhs.m_box)),
      m_widgetid(std::move(rhs.m_widgetid)),
      m_flags(std::move(rhs.m_flags)),
      m_palette(std::move(rhs.m_palette)),
      m_name(std::move(rhs.m_name)),
      m_align(std::move(rhs.m_align)),
      m_margin(std::move(rhs.m_margin)),
      m_boxtype(std::move(rhs.m_boxtype)),
      m_theme(std::move(rhs.m_theme))
{
    /// @todo Handle m_focus

    if (rhs.m_palette)
        m_palette.reset(rhs.m_palette.release());
    if (rhs.m_theme)
        m_theme.reset(rhs.m_theme.release());

    if (rhs.m_parent)
        rhs.m_parent->add(this);
}

Widget& Widget::operator=(const Widget& rhs) noexcept
{
    detatch();

    m_box = rhs.m_box;
    m_flags = rhs.m_flags;
    m_align = rhs.m_align;
    m_margin = rhs.m_margin;
    m_boxtype = rhs.m_boxtype;

    /// @todo Handle m_focus

    if (rhs.m_palette)
        m_palette.reset(new Palette(*rhs.m_palette.get()));
    if (rhs.m_theme)
        m_theme.reset(new Theme(*rhs.m_theme.get()));

    if (rhs.m_parent)
        rhs.m_parent->add(this);

    return *this;
}

Widget& Widget::operator=(Widget&& rhs) noexcept
{
    detatch();

    m_box = std::move(rhs.m_box);
    m_widgetid = std::move(rhs.m_widgetid);
    m_flags = std::move(rhs.m_flags);
    m_palette = std::move(rhs.m_palette);
    m_name = std::move(rhs.m_name);
    m_align = std::move(rhs.m_align);
    m_margin = std::move(rhs.m_margin);
    m_boxtype = std::move(rhs.m_boxtype);
    m_theme = std::move(rhs.m_theme);

    /// @todo Handle m_focus

    if (rhs.m_parent)
        rhs.m_parent->add(this);

    return *this;
}

int Widget::handle(eventid event)
{
    DBG(name() << " handle: " << event);

    if (is_flag_set(widgetflag::GRAB_MOUSE))
    {
        switch (event)
        {
        case eventid::RAW_POINTER_DOWN:
        {
            mouse_grab(this);
            return 1;
        }
        case eventid::RAW_POINTER_UP:
        {
            mouse_grab(nullptr);
            return 1;
        }
        default:
            break;
        }
    }

    return invoke_handlers(event);
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

void Widget::resize(const Size& size)
{
    if (size != box().size())
    {
        damage();
        m_box.size(size);
        damage();
    }
}

void Widget::move(const Point& point)
{
    if (point != box().point())
    {
        damage();
        m_box.point(point);
        damage();
    }
}

void Widget::set_box(const Rect& rect)
{
    move(rect.point());
    resize(rect.size());
}

void Widget::hide()
{
    if (is_flag_set(widgetflag::INVISIBLE))
        return;
    // careful attention to ordering
    damage();
    set_flag(widgetflag::INVISIBLE);
    invoke_handlers(eventid::HIDE);
}

void Widget::show()
{
    if (!is_flag_set(widgetflag::INVISIBLE))
        return;
    // careful attention to ordering
    clear_flag(widgetflag::INVISIBLE);
    damage();
    invoke_handlers(eventid::SHOW);
}

bool Widget::visible() const
{
    return !is_flag_set(widgetflag::INVISIBLE);
}

bool Widget::active() const
{
    return is_flag_set(widgetflag::ACTIVE);
}

void Widget::set_active(bool value)
{
    if (is_flag_set(widgetflag::ACTIVE) != value)
    {
        value ? set_flag(widgetflag::ACTIVE) : clear_flag(widgetflag::ACTIVE);
        damage();
    }
}

bool Widget::readonly() const
{
    return is_flag_set(widgetflag::READONLY);
}

void Widget::set_readonly(bool value)
{
    if (is_flag_set(widgetflag::READONLY) != value)
    {
        value ? set_flag(widgetflag::READONLY) : clear_flag(widgetflag::READONLY);
        damage();
    }
}

bool Widget::disabled() const
{
    return is_flag_set(widgetflag::DISABLED);
}

void Widget::disable()
{
    if (is_flag_set(widgetflag::DISABLED))
        return;
    damage();
    set_flag(widgetflag::DISABLED);
}

void Widget::enable()
{
    if (!is_flag_set(widgetflag::DISABLED))
        return;
    damage();
    clear_flag(widgetflag::DISABLED);
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
        m_parent->damage(to_parent(rect));
}

const Rect& Widget::box() const
{
    return m_box;
}

Size Widget::size() const
{
    return m_box.size();
}

Point Widget::point() const
{
    return m_box.point();
}

Palette& Widget::palette()
{
    if (!m_palette.get())
        m_palette.reset(new Palette(global_palette()));

    return *m_palette.get();
}

const Palette& Widget::palette() const
{
    if (m_palette.get())
        return *m_palette;

    return global_palette();
}

void Widget::set_palette(const Palette& palette)
{
    m_palette.reset(new Palette(palette));
}

void Widget::reset_palette()
{
    m_palette.reset();
}

Frame* Widget::parent()
{
    return m_parent;
}

const Frame* Widget::parent() const
{
    return m_parent;
}

IScreen* Widget::screen()
{
    assert(m_parent);
    return parent()->screen();
}

void Widget::set_align(alignmask a, int margin)
{
    m_align = a;
    m_margin = margin;

    // can't go any further, but keep the alignment setting
    if (!parent())
        return;

    if (m_align != alignmask::NONE)
    {
        assert(m_parent);

        auto bounding = parent()->to_child(parent()->child_area());
        if (bounding.empty())
            return;

        auto r = detail::align_algorithm(size(), bounding, m_align, m_margin);
        set_box(r);
    }
}

Rect Widget::to_parent(const Rect& r)
{
    if (parent())
        return r + parent()->box().point();

    return r;
}

Point Widget::to_screen_back(const Point& p)
{
    if (parent())
        return parent()->to_screen_back(box().point() + p);

    return box().point() + p;
}

Point Widget::to_screen(const Point& p)
{
    if (parent())
        return parent()->to_screen_back(p);

    return p;
}

Point Widget::from_screen_back(const Point& p)
{
    if (parent())
        return parent()->from_screen_back(p - box().point());

    return p - box().point();
}

Point Widget::from_screen(const Point& p)
{
    if (parent())
        return parent()->from_screen_back(p);

    return p;
}

const std::string& Widget::name() const
{
    return m_name;
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

#if 0
shared_cairo_surface_t Widget::surface()
{
    auto surface = shared_cairo_surface_t(
                       cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                               w(), h()),
                       cairo_surface_destroy);

    auto cr = shared_cairo_t(cairo_create(surface.get()), cairo_destroy);

    Painter painter(cr);
    paint(painter);
    return surface;
}
#endif

void Widget::dump(std::ostream& out, int level)
{
    out << std::string(level, ' ') << name() <<
        " " << box() << " " << m_flags << std::endl;
}

void Widget::on_gain_focus()
{
    m_focus = true;
}

void Widget::on_lost_focus()
{
    m_focus = false;
}

void Widget::set_theme(const Theme& theme)
{
    m_theme.reset(new Theme(theme));
}

void Widget::reset_theme()
{
    m_theme.reset();
}

void Widget::draw_box(Painter& painter, const Rect& rect)
{
    if (m_boxtype == Theme::boxtype::none)
        return;

    theme().draw_box(painter, *this, m_boxtype, rect);
}

Theme& Widget::theme()
{
    if (m_theme.get())
        return *m_theme;

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

void Widget::detatch()
{
    if (m_parent)
    {
        m_parent->remove(this);
        m_parent = nullptr;
    }
}

Widget::~Widget() noexcept
{
    detatch();
}

}
}
