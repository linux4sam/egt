/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/canvas.h"
#include "egt/detail/alignment.h"
#include "egt/detail/math.h"
#include "egt/detail/serialize.h"
#include "egt/frame.h"
#include "egt/geometry.h"
#include "egt/input.h"
#include "egt/painter.h"
#include "egt/screen.h"
#include "egt/widget.h"
#include <cassert>
#include <iostream>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string>

using namespace std;

namespace egt
{
inline namespace v1
{

static auto global_widget_id = 0;

Widget::Widget(const Rect& rect, const Widget::Flags& flags) noexcept
    : m_box(rect),
      m_widgetid(global_widget_id++),
      m_widget_flags(flags),
      m_palette(detail::make_unique<Palette>())
{
    name("Widget" + std::to_string(m_widgetid));

    m_align.on_change([this]()
    {
        parent_layout();
    });

    on_gain_focus([this]()
    {
        m_focus = true;
    });

    on_lost_focus([this]()
    {
        m_focus = false;
    });
}

Widget::Widget(Frame& parent, const Rect& rect, const Widget::Flags& flags) noexcept
    : Widget(rect, flags)
{
    parent.add(*this);
}

void Widget::handle(Event& event)
{
    if (event.quit())
        return;

    SPDLOG_TRACE("{} handle {}", name(), event);

    switch (event.id())
    {
    case EventId::raw_pointer_down:
        if (flags().is_set(Widget::Flag::grab_mouse))
        {
            active(true);
            event.grab(this);
        }
        break;
    case EventId::raw_pointer_up:
        active(false);
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
        Point pos(point.x() - width() / 2,
                  point.y() - height() / 2);

        move(pos);
    }
}

void Widget::move_to_center()
{
    if (!m_parent)
        return;

    move_to_center(m_parent->center());
}

void Widget::resize(const Size& size)
{
    if (size != this->size())
    {
        damage();
        m_box.size(size);
        damage();

        parent_layout();
    }
}

void Widget::resize_by_ratio(int hratio, int vratio)
{
    Size size(static_cast<float>(width()) * (static_cast<float>(hratio) / 100.),
              static_cast<float>(height()) * (static_cast<float>(vratio) / 100.));
    resize(size);
}

void Widget::move(const Point& point)
{
    if (point != box().point())
    {
        damage();
        m_box.point(point);
        damage();

        parent_layout();
    }
}

void Widget::box(const Rect& rect)
{
    move(rect.point());
    resize(rect.size());
}

void Widget::hide()
{
    if (flags().is_set(Widget::Flag::invisible))
        return;
    // careful attention to ordering
    damage();
    flags().set(Widget::Flag::invisible);
    on_hide.invoke();
}

void Widget::show()
{
    if (!flags().is_set(Widget::Flag::invisible))
        return;
    // careful attention to ordering
    flags().clear(Widget::Flag::invisible);
    damage();
    on_show.invoke();
}

void Widget::visible(bool value)
{
    if (visible() != value)
    {
        if (value)
            show();
        else
            hide();
    }
}

bool Widget::active() const
{
    return flags().is_set(Widget::Flag::active);
}

void Widget::active(bool value)
{
    if (flags().is_set(Widget::Flag::active) != value)
    {
        if (value)
            flags().set(Widget::Flag::active);
        else
            flags().clear(Widget::Flag::active);
        damage();
    }
}

void Widget::readonly(bool value)
{
    if (flags().is_set(Widget::Flag::readonly) != value)
    {
        if (value)
        {
            flags().set(Widget::Flag::readonly);

            if (detail::keyboard_focus() == this)
                detail::keyboard_focus(nullptr);
        }
        else
            flags().clear(Widget::Flag::readonly);
        damage();
    }
}

void Widget::disable()
{
    if (flags().is_set(Widget::Flag::disabled))
        return;
    damage();
    flags().set(Widget::Flag::disabled);

    if (detail::keyboard_focus() == this)
        detail::keyboard_focus(nullptr);
}

void Widget::enable()
{
    if (!flags().is_set(Widget::Flag::disabled))
        return;
    damage();
    flags().clear(Widget::Flag::disabled);
}

void Widget::alpha(float alpha)
{
    alpha = detail::clamp<>(alpha, 0.f, 1.f);

    if (detail::change_if_diff<float>(m_alpha, alpha))
        damage();
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

void Widget::palette(const Palette& palette)
{
    m_palette = detail::make_unique<Palette>(palette);
    damage();
}

void Widget::reset_palette()
{
    assert(m_palette);
    if (m_palette)
        m_palette->clear_all();
    damage();
}

Palette::pattern_type Widget::color(Palette::ColorId id) const
{
    Palette::GroupId group = Palette::GroupId::normal;
    if (disabled())
        group = Palette::GroupId::disabled;
    else if (active())
        group = Palette::GroupId::active;
    else if (checked())
        group = Palette::GroupId::checked;

    return color(id, group);
}

Palette::pattern_type Widget::color(Palette::ColorId id, Palette::GroupId group) const
{
    assert(m_palette);

    if (m_palette && m_palette->exists(id, group))
        return m_palette->color(id, group);

    return default_palette().color(id, group);
}

void Widget::color(Palette::ColorId id,
                   const Palette::pattern_type& color,
                   Palette::GroupId group)
{
    assert(m_palette);
    if (m_palette)
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

Screen* Widget::screen() const
{
    assert(m_parent);
    return parent()->screen();
}

void Widget::align(const AlignFlags& a)
{
    m_align = a;
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
    if (!m_min_size.empty())
        return m_min_size;

    return Size(moat() * 2., moat() * 2.);
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
                               width(), height()),
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
    out << " zorder(" << zorder() << ")";
    out << std::endl;
}

void Widget::walk(WalkCallback callback, int level)
{
    callback(this, level);
}

void Widget::theme(const Theme& theme)
{
    m_theme = detail::make_unique<Theme>(theme);
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

void Widget::draw_circle(Painter& painter, Palette::ColorId bg,
                         Palette::ColorId border) const
{
    theme().draw_circle(painter, *this, bg, border);
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

void Widget::zorder_bottom()
{
    if (m_parent)
        m_parent->zorder_bottom(this);
}

void Widget::zorder_top()
{
    if (m_parent)
        m_parent->zorder_top(this);
}

size_t Widget::zorder() const
{
    if (m_parent)
        return m_parent->zorder(this);

    return 0;
}

void Widget::detach()
{
    if (m_parent)
    {
        m_parent->remove(this);
        m_parent = nullptr;
    }
}

size_t Widget::moat() const
{
    return margin() + padding() + border();
}

Rect Widget::content_area() const
{
    auto m = moat();
    auto b = box();
    b += Point(m, m);
    b -= Size(2. * m, 2. * m);
    // don't return a negative size
    if (b.empty())
        return Rect(point(), Size());
    return b;
}

void Widget::layout()
{
    if (!flags().is_set(Widget::Flag::no_autoresize))
    {
        auto s = size();
        if (s.width() < min_size_hint().width())
            s.width(min_size_hint().width());
        if (s.height() < min_size_hint().height())
            s.height(min_size_hint().height());
        resize(s);
    }
}

void Widget::checked(bool value)
{
    if (flags().is_set(Widget::Flag::checked) != value)
    {
        if (value)
            flags().set(Widget::Flag::checked);
        else
            flags().clear(Widget::Flag::checked);
        damage();
    }
}

void Widget::serialize(detail::Serializer& serializer) const
{
    serializer.add_property("x", x());
    serializer.add_property("y", y());
    serializer.add_property("width", width());
    serializer.add_property("height", height());
    if (!align().empty())
        serializer.add_property("align", align());
    serializer.add_property("flags", flags().to_string());
    if (padding())
        serializer.add_property("padding", padding());
    if (margin())
        serializer.add_property("margin", margin());
    if (border())
        serializer.add_property("border", border());
    if (xratio())
        serializer.add_property("ratio:x", xratio());
    if (yratio())
        serializer.add_property("ratio:y", yratio());
    if (horizontal_ratio())
        serializer.add_property("ratio:horizontal", horizontal_ratio());
    if (vertical_ratio())
        serializer.add_property("ratio:vertical", vertical_ratio());
    if (!boxtype().empty())
        serializer.add_property("boxtype", boxtype().to_string());

    if (m_palette)
        m_palette->serialize("color", serializer);

    if (m_font)
        m_font->serialize("font", serializer);
}

void Widget::deserialize(const std::string& name, const std::string& value,
                         const std::map<std::string, std::string>& attrs)
{
    if (name == "width")
        width(std::stoi(value));
    else if (name == "height")
        height(std::stoi(value));
    else if (name == "x")
        x(std::stoi(value));
    else if (name == "y")
        y(std::stoi(value));
    else if (name == "align")
        align(AlignFlags(value));
    else if (name == "flags")
        m_widget_flags.from_string(value);
    else if (name == "alpha")
        alpha(std::stoi(value));
    else if (name == "padding")
        padding(std::stoi(value));
    else if (name == "margin")
        margin(std::stoi(value));
    else if (name == "border")
        border(std::stoi(value));
    else if (name == "boxtype")
        m_boxtype.from_string(value);
    else if (name == "ratio:x")
    {}
    else if (name == "ratio:y")
    {}
    else if (name == "ratio:horizontal")
    {}
    else if (name == "ratio:vertical")
    {}
    else if (name == "font")
    {
        Font font;
        font.deserialize(name, value, attrs);
        this->font(font);
    }
    else if (name == "color")
        m_palette->deserialize(name, value, attrs);
}

Widget::~Widget() noexcept
{
    detach();

    if (detail::mouse_grab() == this)
        detail::mouse_grab(nullptr);

    if (detail::keyboard_focus() == this)
        detail::keyboard_focus(nullptr);
}

void Widget::parent(Frame* parent)
{
    // cannot already have a parent
    assert(!m_parent);
    if (m_parent)
        throw std::runtime_error("widget already has a parent");

    if (parent == this)
        throw std::runtime_error("cannot add a widget to itself");

    m_parent = parent;
    damage();
}

void Widget::parent_layout()
{
    if (!visible())
        return;

    if (flags().is_set(Widget::Flag::no_layout))
        return;

    if (parent())
        parent()->layout();
}

DisplayPoint Widget::local_to_display(const Point& p)
{
    DisplayPoint p2(p.x(), p.y());

    auto par = parent();
    while (par)
    {
        p2 += DisplayPoint(par->point().x(), par->point().y());
        par = par->parent();
    }

    return p2 + DisplayPoint(x(), y());
}

Point Widget::display_to_local(const DisplayPoint& p)
{
    Point p2(p.x(), p.y());

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
