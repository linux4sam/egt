/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/frame.h"
#include "egt/geometry.h"
#include "egt/input.h"
#include "egt/painter.h"
#include "egt/screen.h"
#include "egt/utils.h"
#include "egt/widget.h"
#include "egt/canvas.h"
#include <cassert>
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

namespace egt
{
inline namespace v1
{

Rect Widget::align_algorithm(const Size& size, const Rect& bounding,
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

void Widget::double_align(const Rect& main,
                          const Size& fsize, alignmask first_align, Rect& first,
                          const Size& ssize, alignmask second_align, Rect& second,
                          int margin)
{
    auto ftarget = Widget::align_algorithm(fsize, main, first_align, margin);
    auto starget = Widget::align_algorithm(ssize, main, second_align, margin);

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

Widget::Widget(const Rect& rect, widgetmask flags) noexcept
    : m_box(rect),
      m_flags(flags)
{
    static auto widget_id = 0;

    ostringstream ss;
    ss << "Widget" << widget_id++;
    set_name(ss.str());
}

Widget::Widget(Frame& parent, const Rect& rect, widgetmask flags) noexcept
    : Widget(rect, flags)
{
    parent.add(this);
}

int Widget::handle(eventid event)
{
    DBG(name() << " handle: " << event);

    if (is_flag_set(widgetmask::GRAB_MOUSE))
    {
        switch (event)
        {
        case eventid::RAW_POINTER_DOWN:
        {
            mouse_grab(this);
            break;
        }
        case eventid::RAW_POINTER_UP:
        {
            mouse_grab(nullptr);
            break;
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
    if (!m_visible)
        return;
    // careful attention to ordering
    damage();
    m_visible = false;
    invoke_handlers(eventid::HIDE);
}

void Widget::show()
{
    if (m_visible)
        return;
    // careful attention to ordering
    m_visible = true;
    damage();
    invoke_handlers(eventid::SHOW);
}

bool Widget::visible() const
{
    return m_visible;
}

bool Widget::active() const
{
    return m_active;
}

void Widget::set_active(bool value)
{
    if (m_active != value)
    {
        m_active = value;
        damage();
    }
}

bool Widget::disabled() const
{
    return m_disabled;
}

void Widget::disable()
{
    if (!m_disabled)
        damage();
    m_disabled = true;
}

void Widget::enable()
{
    if (m_disabled)
        damage();
    m_disabled = false;
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

        auto r = align_algorithm(size(), bounding, m_align, m_margin);
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
    out << std::string(level, ' ') << "Widget: " << name() <<
        " " << box() << std::endl;
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

void Widget::set_boxtype(const Theme::boxtype type)
{
    m_boxtype = type;
}

void Widget::draw_box(Painter& painter, const Rect& rect)
{
    if (is_flag_set(widgetmask::NO_BACKGROUND))
        return;

    theme().draw_box(painter, *this, m_boxtype, rect);
}

Theme& Widget::theme()
{
    if (m_theme.get())
        return *m_theme;

    return global_theme();
}

Widget::~Widget()
{
    if (m_parent)
        m_parent->remove(this);
}

TextWidget::TextWidget(const std::string& text, const Rect& rect,
                       alignmask align, const Font& font, widgetmask flags) noexcept
    : Widget(rect, flags),
      m_text_align(align),
      m_text(text)
{
    set_font(font);
}

void TextWidget::clear()
{
    if (!m_text.empty())
    {
        m_text.clear();
        damage();
    }
}

void TextWidget::set_text(const std::string& str)
{
    if (m_text != str)
    {
        m_text = str;
        damage();
    }
}

Font TextWidget::scale_font(const Size& target, const std::string& text, const Font& font)
{
    auto surface = shared_cairo_surface_t(cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 10, 10),
                                          cairo_surface_destroy);
    auto cr = shared_cairo_t(cairo_create(surface.get()), cairo_destroy);
    Painter painter(cr);

    auto nfont = font;
    while (true)
    {
        painter.set_font(nfont);

        cairo_text_extents_t textext;
        cairo_text_extents(cr.get(), text.c_str(), &textext);

        if (textext.width - textext.x_bearing < target.w &&
            textext.height - textext.y_bearing < target.h)
            return nfont;

        nfont.size(nfont.size() - 1);
        if (nfont.size() < 1)
            return font;
    }
    return nfont;
}

Size TextWidget::text_size()
{
    if (m_parent)
    {
        Canvas canvas(Size(100, 100));
        Painter painter(canvas.context());
        painter.set_font(font());
        return painter.text_size(m_text);
    }

    return Size();
}

namespace experimental
{
ScrollWheel::ScrollWheel(const Rect& rect)
    : Widget(rect)
{}

int ScrollWheel::handle(eventid event)
{
    switch (event)
    {
    case eventid::RAW_POINTER_DOWN:
        m_moving_x = from_screen(event_mouse()).y;
        m_start_pos = position();
        set_active(true);
        return 1;
        break;
    case eventid::RAW_POINTER_UP:
        set_active(false);
        return 1;
        break;
    case eventid::RAW_POINTER_MOVE:
        if (active())
        {
            int diff = from_screen(event_mouse()).y - m_moving_x;
            position(m_start_pos + diff);
        }
        break;
    default:
        break;
    }

    return Widget::handle(event);
}

void ScrollWheel::draw(Painter& painter, const Rect& rect)
{
    ignoreparam(rect);

    Color border(Color::BLACK);
    Color glass(0x00115555);
    Color color(0x4169E1ff);

    auto cr = painter.context();

    cairo_save(cr.get());

    cairo_text_extents_t textext;
    cairo_select_font_face(cr.get(), "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr.get(), 16);
    cairo_text_extents(cr.get(), "a", &textext);

    painter.set_color(border);
    painter.set_line_width(3.0);
    cairo_move_to(cr.get(), x(), y());
    cairo_line_to(cr.get(), x(), y() + h());
    cairo_move_to(cr.get(), x() + w(), y());
    cairo_line_to(cr.get(), x() + w(), y() + h());
    cairo_stroke(cr.get());

    painter.set_color(glass);
    cairo_rectangle(cr.get(), x(),  y() - textext.height + (1 * h() / 3), w(), (1 * h() / 3));
    cairo_fill(cr.get());

    cairo_pattern_t* pat;
    pat = cairo_pattern_create_linear(x(), y(), x(), y() + h() / 2);

    Color step = Color(Color::GRAY);
    cairo_pattern_add_color_stop_rgb(pat, 0, step.redf(), step.greenf(), step.bluef());
    step = Color(Color::WHITE);
    cairo_pattern_add_color_stop_rgb(pat, 0.5, step.redf(), step.greenf(), step.bluef());
    step = Color(Color::GRAY);
    cairo_pattern_add_color_stop_rgb(pat, 1.0, step.redf(), step.greenf(), step.bluef());

    cairo_set_source(cr.get(), pat);

    int offset = y() + textext.height;
    for (int index = position();
         index < (int)m_values.size() && index < position() + 3; index++)
    {
        cairo_move_to(cr.get(), x(), offset);
        cairo_show_text(cr.get(), m_values[index].c_str());

        offset += h() / 3; //textext.height + 10;
    }

    //cairo_stroke(cr);
    cairo_pattern_destroy(pat);

    cairo_restore(cr.get());
}
}

}
}
