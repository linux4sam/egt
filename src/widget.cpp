/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "mui/frame.h"
#include "mui/geometry.h"
#include "mui/imagecache.h"
#include "mui/input.h"
#include "mui/painter.h"
#include "mui/screen.h"
#include "mui/utils.h"
#include "mui/widget.h"
#include <cassert>
#include <string>
#include <sstream>

using namespace std;

namespace mui
{
    widgetmask& operator&=(widgetmask& X, widgetmask Y)
    {
        X = X & Y;
        return X;
    }

    widgetmask& operator|=(widgetmask& X, widgetmask Y)
    {
        X = X | Y;
        return X;
    }

    widgetmask& operator^=(widgetmask& X, widgetmask Y)
    {
        X = X ^ Y;
        return X;
    }

    alignmask& operator&=(alignmask& X, alignmask Y)
    {
        X = X & Y;
        return X;
    }

    alignmask& operator|=(alignmask& X, alignmask Y)
    {
        X = X | Y;
        return X;
    }

    alignmask& operator^=(alignmask& X, alignmask Y)
    {
        X = X ^ Y;
        return X;
    }

    static auto widget_id = 0;

    Widget::Widget(const Point& point, const Size& size, widgetmask flags) noexcept
        : m_box(point, size),
          m_flags(flags)
    {
        ostringstream ss;
        ss << "widget" << widget_id++;
        name(ss.str());
    }

    Widget::Widget(Frame& parent, const Point& point, const Size& size, widgetmask flags) noexcept
        : Widget(point, size, flags)
    {
        parent.add(this);
    }

    int Widget::handle(int event)
    {
        ignoreparam(event);

        // eat the event if we are in focus
        if (focus())
            return 1;

        // do nothing
        return EVT_NONE;
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

    void Widget::focus(bool value)
    {
        if (m_focus != value)
        {
            m_focus = value;

            if (m_parent)
                m_parent->focus(value ? this : nullptr);
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
        if (m_parent)
            m_parent->damage(rect);
    }

    IScreen* Widget::screen()
    {
        return parent()->screen();
    }

    void Widget::align(alignmask a, int margin)
    {
        if (m_align != a)
        {
            m_align = a;

            if (m_align != alignmask::NONE)
            {
                auto r = align_algorithm(size(), parent()->box(), m_align, margin);
                set_box(r);
            }
        }
    }

    Point Widget::screen_to_frame(const Point& p)
    {
        Point pp;
        Widget* w = this;
        while (w)
        {
            if (is_flag_set(widgetmask::FRAME))
            {
                auto f = reinterpret_cast<Frame*>(w);
                if (f->top_level() || f->is_flag_set(widgetmask::PLANE_WINDOW))
                {
                    pp = f->box().point();
                    break;
                }
            }

            w = w->m_parent;
        }
        return p - pp;
    }

    Widget::~Widget()
    {
        if (m_parent)
            m_parent->remove(this);
    }

    namespace experimental
    {

        Combo::Combo(const string& label, const Point& point, const Size& size)
            : Widget(point, size),
              m_label(label)
        {

        }

        int Combo::handle(int event)
        {
            return Widget::handle(event);
        }

        void Combo::draw(Painter& painter, const Rect& rect)
        {
            ignoreparam(rect);

            Color white(Color::WHITE);

            auto cr = painter.context();

            cairo_save(cr.get());

            // path
            double rx = x(),
                   ry = y(),
                   width = w(),
                   height = h(),
                   aspect = 1.0,
                   corner_radius = 50 / 10.0;

            double radius = corner_radius / aspect;
            double degrees = M_PI / 180.0;

            cairo_new_sub_path(cr.get());
            cairo_arc(cr.get(), rx + width - radius, ry + radius, radius, -90 * degrees, 0 * degrees);
            cairo_arc(cr.get(), rx + width - radius, ry + height - radius, radius, 0 * degrees, 90 * degrees);
            cairo_arc(cr.get(), rx + radius, ry + height - radius, radius, 90 * degrees, 180 * degrees);
            cairo_arc(cr.get(), rx + radius, ry + radius, radius, 180 * degrees, 270 * degrees);
            cairo_close_path(cr.get());

            // fill
            cairo_pattern_t* pat3;
            pat3 = cairo_pattern_create_linear(x() + w() / 2, y(), x() + w() / 2, y() + h());

            Color step = white;
            cairo_pattern_add_color_stop_rgb(pat3, 0, step.redf(), step.greenf(), step.bluef());
            step = white.tint(.9);
            cairo_pattern_add_color_stop_rgb(pat3, 0.43, step.redf(), step.greenf(), step.bluef());
            step = white.tint(.82);
            cairo_pattern_add_color_stop_rgb(pat3, 0.5, step.redf(), step.greenf(), step.bluef());
            step = white.tint(.95);
            cairo_pattern_add_color_stop_rgb(pat3, 1.0, step.redf(), step.greenf(), step.bluef());

            cairo_set_source(cr.get(), pat3);
            cairo_fill_preserve(cr.get());
            cairo_pattern_destroy(pat3);

            // border
            cairo_set_source_rgba(cr.get(),
                                  palette().color(Palette::BORDER).redf(),
                                  palette().color(Palette::BORDER).greenf(),
                                  palette().color(Palette::BORDER).bluef(),
                                  palette().color(Palette::BORDER).alphaf());
            cairo_set_line_width(cr.get(), 1.0);
            cairo_stroke(cr.get());

            // text
            painter.draw_text(m_label, box(), palette().color(Palette::TEXT), alignmask::LEFT | alignmask::CENTER);

#if 0
            // triangle
            cairo_set_source_rgb(cr.get(), border.redf(), border.greenf(), border.bluef());
            cairo_move_to(cr.get(), 240, 40);
            cairo_line_to(cr.get(), 240, 160);
            cairo_line_to(cr.get(), 350, 160);
            cairo_close_path(cr.get());
            cairo_stroke_preserve(cr.get());
            cairo_fill(cr.get());
#endif

            cairo_restore(cr.get());

            // images
            auto up = image_cache.get("icons/bullet_arrow_up.png", 1.0);
            auto down = image_cache.get("icons/bullet_arrow_down.png", 1.0);

#if 0
            auto upwidth = cairo_image_surface_get_width(up.get());
            auto upheight = cairo_image_surface_get_height(up.get());
            screen()->blit(up.get(),
                           x() + w() - upwidth - 5,
                           y() + 5,
                           upwidth,
                           upheight,
                           x() + w() - upwidth - 5,
                           y() + 5,
                           true);

            auto downwidth = cairo_image_surface_get_width(down.get());
            auto downheight = cairo_image_surface_get_height(down.get());
            screen()->blit(down.get(),
                           x() + w() - downwidth - 5,
                           y() + h() - downheight - 5,
                           downwidth,
                           downheight,
                           x() + w() - downwidth - 5,
                           y() + h() - downheight - 5,
                           true);
#endif
        }

        Combo::~Combo()
        {
        }
    }

    namespace experimental
    {
        ScrollWheel::ScrollWheel(const Point& point, const Size& size)
            : Widget(point, size)
        {}

        int ScrollWheel::handle(int event)
        {
            switch (event)
            {
            case EVT_MOUSE_DOWN:
                m_moving_x = screen_to_frame(mouse_position()).y;
                m_start_pos = position();
                active(true);
                return 1;
                break;
            case EVT_MOUSE_UP:
                active(false);
                return 1;
                break;
            case EVT_MOUSE_MOVE:
                if (active())
                {
                    int diff = screen_to_frame(mouse_position()).y - m_moving_x;
                    position(m_start_pos + diff);
                }
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
