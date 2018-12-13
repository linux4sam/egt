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
#include <cassert>
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

namespace egt
{
    static auto widget_id = 0;

    Rect Widget::align_algorithm(const Size& size, const Rect& bounding,
                                 alignmask align, int margin)
    {
        assert(align != alignmask::NONE);

        if ((align & alignmask::EXPAND) == alignmask::EXPAND)
            return bounding;

        Point p;
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
            nsize.w = bounding.w;
            p.x = bounding.x;
        }

        if ((align & alignmask::EXPAND_VERTICAL) == alignmask::EXPAND_VERTICAL)
        {
            nsize.h = bounding.h;
            p.y = bounding.y;
        }

        return Rect(p, nsize);
    }

    Widget::Widget(const Rect& rect, widgetmask flags) noexcept
        : m_box(rect),
          m_flags(flags)
    {
        ostringstream ss;
        ss << "widget" << widget_id++;
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
            case eventid::MOUSE_DOWN:
            {
                mouse_grab(this);
                break;
            }
            case eventid::MOUSE_UP:
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

    void Widget::damage()
    {
        damage(box());
    }

    void Widget::damage(const Rect& rect)
    {
        if (rect.empty())
            return;

        // don't damage if not even visible
        if (!visible())
            return;

        if (m_parent)
            m_parent->damage(to_parent(rect));
    }

    IScreen* Widget::screen()
    {
        return parent()->screen();
    }

    void Widget::set_align(alignmask a, int margin)
    {
        if (m_align != a || m_margin != margin)
        {
            m_align = a;
            m_margin = margin;

            if (m_align != alignmask::NONE)
            {
                auto r = align_algorithm(size(), box_to_child(parent()->box()), m_align, m_margin);
                set_box(r);
            }
        }
    }

    Rect Widget::box_to_child(const Rect& r)
    {
        if (parent())
            return Rect(Point(0, 0), r.size());

        return r;
    }

    Rect Widget::to_child(const Rect& r)
    {
        if (parent())
            return Rect(parent()->box().point() - r.point(), r.size());

        return r;
    }

    Rect Widget::to_parent(const Rect& r)
    {
        if (parent())
            return Rect(parent()->box().point() + r.point(), r.size());

        return r;
    }

    Point Widget::from_screen(const Point& p)
    {
        if (top_level())
            return p;

        return parent()->from_screen(p - box().point());
    }

    Rect Widget::from_screen(const Rect& r)
    {
        Point origin = from_screen(r.point());
        return Rect(origin, r.size());
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

    void Widget::draw_box(Painter& painter, const Rect& rect)
    {
        if (is_flag_set(widgetmask::NO_BACKGROUND))
            return;

        theme().draw_box(painter, *this, m_boxtype, rect);
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

    namespace experimental
    {
        ScrollWheel::ScrollWheel(const Rect& rect)
            : Widget(rect)
        {}

        int ScrollWheel::handle(eventid event)
        {
            switch (event)
            {
            case eventid::MOUSE_DOWN:
                m_moving_x = from_screen(event_mouse()).y;
                m_start_pos = position();
                set_active(true);
                return 1;
                break;
            case eventid::MOUSE_UP:
                set_active(false);
                return 1;
                break;
            case eventid::MOUSE_MOVE:
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

            cairo_set_source_rgb(cr.get(), border.redf(), border.greenf(), border.bluef());
            cairo_set_line_width(cr.get(), 3.0);
            cairo_move_to(cr.get(), x(), y());
            cairo_line_to(cr.get(), x(), y() + h());
            cairo_move_to(cr.get(), x() + w(), y());
            cairo_line_to(cr.get(), x() + w(), y() + h());
            cairo_stroke(cr.get());

            cairo_set_source_rgba(cr.get(), glass.redf(), glass.greenf(), glass.bluef(), glass.alphaf());
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
