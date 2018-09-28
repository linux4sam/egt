/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "frame.h"
#include "geometry.h"
#include "imagecache.h"
#include "input.h"
#include "painter.h"
#include "screen.h"
#include "utils.h"
#include "widget.h"
#include <cairo.h>
#include <cassert>
#include <string>

using namespace std;

namespace mui
{
    Widget::Widget(const Point& point, const Size& size, uint32_t flags) noexcept
        : m_box(point, size),
          m_parent(nullptr),
          m_visible(true),
          m_focus(false),
          m_active(false),
          m_disabled(false),
          m_flags(flags)
    {}

    int Widget::handle(int event)
    {
        // do nothing
        return EVT_NONE;
    }

    void Widget::position(const Point& point)
    {
        if (point != box().point())
        {
            damage();
            m_box.point(point);
        }
    }

    void Widget::size(const Size& size)
    {
        if (size != box().size())
        {
            damage();
            m_box.size(size);
        }
    }

    void Widget::resize(const Size& size)
    {
        if (size != box().size())
        {
            this->size(size);
            damage();
        }
    }

    void Widget::move(const Point& point)
    {
        if (point != box().point())
        {
            position(point);
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

    void Widget::draw_basic_box(const Rect& rect, const Color& border, const Color& bg)
    {
        auto cr = screen()->context();

        cairo_save(cr.get());

        double rx = rect.x,
               ry = rect.y,
               width = rect.w,
               height = rect.h,
               aspect = 1.0,
               corner_radius = 50/*height*/ / 10.0;

        double radius = corner_radius / aspect;
        double degrees = M_PI / 180.0;

        cairo_new_sub_path(cr.get());
        cairo_arc(cr.get(), rx + width - radius, ry + radius, radius, -90 * degrees, 0 * degrees);
        cairo_arc(cr.get(), rx + width - radius, ry + height - radius, radius, 0 * degrees, 90 * degrees);
        cairo_arc(cr.get(), rx + radius, ry + height - radius, radius, 90 * degrees, 180 * degrees);
        cairo_arc(cr.get(), rx + radius, ry + radius, radius, 180 * degrees, 270 * degrees);
        cairo_close_path(cr.get());

        cairo_set_source_rgba(cr.get(),
                              bg.redf(),
                              bg.greenf(),
                              bg.bluef(),
                              bg.alphaf());
        cairo_fill_preserve(cr.get());

        cairo_set_source_rgba(cr.get(),
                              border.redf(),
                              border.greenf(),
                              border.bluef(),
                              border.alphaf());
        cairo_set_line_width(cr.get(), 1.0);
        cairo_stroke(cr.get());

        cairo_restore(cr.get());
    }

    void Widget::draw_gradient_box(const Rect& rect, const Color& border,
                                   bool active)
    {
        auto cr = screen()->context();

        cairo_save(cr.get());

        double rx = rect.x,
               ry = rect.y,
               width = rect.w,
               height = rect.h,
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

        cairo_pattern_t* pat;
        pat = cairo_pattern_create_linear(rx + width / 2, ry, rx + width / 2, ry + height);

        if (!active)
        {
            Color step = Color::WHITE;
            cairo_pattern_add_color_stop_rgb(pat, 0, step.redf(), step.greenf(), step.bluef());
            step = Color::WHITE.tint(.9);
            cairo_pattern_add_color_stop_rgb(pat, 0.43, step.redf(), step.greenf(), step.bluef());
            step = Color::WHITE.tint(.82);
            cairo_pattern_add_color_stop_rgb(pat, 0.5, step.redf(), step.greenf(), step.bluef());
            step = Color::WHITE.tint(.95);
            cairo_pattern_add_color_stop_rgb(pat, 1.0, step.redf(), step.greenf(), step.bluef());

            cairo_set_line_width(cr.get(), 1.0);
        }
        else
        {
            Color step = Color::WHITE;
            cairo_pattern_add_color_stop_rgb(pat, 1, step.redf(), step.greenf(), step.bluef());
            step = Color::WHITE.tint(.9);
            cairo_pattern_add_color_stop_rgb(pat, 0.5, step.redf(), step.greenf(), step.bluef());
            step = Color::WHITE.tint(.82);
            cairo_pattern_add_color_stop_rgb(pat, 0.43, step.redf(), step.greenf(), step.bluef());
            step = Color::WHITE.tint(.95);
            cairo_pattern_add_color_stop_rgb(pat, 0, step.redf(), step.greenf(), step.bluef());

            cairo_set_line_width(cr.get(), 2.0);
        }

        cairo_set_source(cr.get(), pat);
        cairo_fill_preserve(cr.get());

        cairo_set_source_rgba(cr.get(),
                              border.redf(),
                              border.greenf(),
                              border.bluef(),
                              border.alphaf());
        cairo_set_line_width(cr.get(), 1.0);
        cairo_stroke(cr.get());

        cairo_restore(cr.get());
    }


    void Widget::draw_text(const std::string& text, const Rect& rect,
                           const Color& color, int align, int standoff,
                           const Font& font)
    {
        auto cr = screen()->context();

        cairo_save(cr.get());

        cairo_text_extents_t textext;
        cairo_select_font_face(cr.get(),
                               font.face().c_str(),
                               CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr.get(), font.size());
        cairo_text_extents(cr.get(), text.c_str(), &textext);

        Rect target = align_algorithm(Size(textext.width, textext.height),
                                      rect,
                                      align,
                                      standoff);

        cairo_set_source_rgba(cr.get(),
                              color.redf(),
                              color.greenf(),
                              color.bluef(),
                              color.alphaf());
        cairo_move_to(cr.get(), target.x + textext.x_bearing, target.y - textext.y_bearing);
        cairo_show_text(cr.get(), text.c_str());
        cairo_stroke(cr.get());

        cairo_restore(cr.get());
    }

    void Widget::draw_image(shared_cairo_surface_t image, int align, int standoff, bool bw)
    {
        auto width = cairo_image_surface_get_width(image.get());
        auto height = cairo_image_surface_get_height(image.get());

        Rect target = align_algorithm(Size(width, height), box(), align, standoff);

        Painter painter(screen()->context());
#if 1
        painter.draw_image(target.point(), image, bw);
#else
        painter.paint_surface_with_drop_shadow(
            image.get(),
            5,
            0.2,
            0.4,
            target.x,
            target.y,
            width,
            height,
            target.x,
            target.y);
#endif
    }

    Point Widget::screen_to_frame(const Point& p)
    {
        Point pp;
        Widget* w = this;
        while (w)
        {
            if (is_flag_set(FLAG_FRAME))
            {
                auto f = reinterpret_cast<Frame*>(w);
                if (f->top_level() || f->is_flag_set(FLAG_PLANE_WINDOW))
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

#ifdef DEVELOPMENT
    Combo::Combo(const string& label, const Point& point, const Size& size)
        : Widget(point.x, point.y, size.w, size.h),
          m_label(label)
    {

    }

    int Combo::handle(int event)
    {
        return Widget::handle(event);
    }

    void Combo::draw(const Rect& rect)
    {
        Color white(Color::WHITE);

        auto cr = screen()->context();

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
        draw_text(m_label, box(), palette().color(Palette::TEXT), ALIGN_LEFT | ALIGN_CENTER);

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
    }

    Combo::~Combo()
    {
    }
#endif

    ListBox::ListBox(const item_array& items,
                     const Point& point,
                     const Size& size)
        : Widget(point, size),
          m_items(items)
    {}

    Rect ListBox::item_rect(uint32_t index) const
    {
        int height = 40;
        Rect r(box().x, box().y, box().w, height);
        r.y += (height * index);
        return r;
    }

    int ListBox::handle(int event)
    {
        switch (event)
        {
        case EVT_MOUSE_DOWN:
        {
            for (uint32_t i = 0; i < m_items.size(); i++)
            {
                if (Rect::point_inside(screen_to_frame(mouse_position()), item_rect(i)))
                {
                    if (m_selected != i)
                    {
                        m_selected = i;
                        damage();
                        on_selected(i);
                    }

                    break;
                }
            }

            return 1;
        }
        }

        return Widget::handle(event);
    }

    void ListBox::draw(const Rect& rect)
    {
#if 0
        for (uint32_t i = 0; i < m_items.size(); i++)
        {
            if (i == m_selected)
            {
                draw_basic_box(item_rect(i), palette().color(Palette::BORDER), Color::ORANGE);
            }
            else
            {
                draw_gradient_box(item_rect(i));
            }

            draw_text(m_items[i], item_rect(i), Color::BLACK, ALIGN_CENTER);
        }
#else
        Painter painter(screen()->context());
        auto cr = screen()->context();

        draw_basic_box(Rect(x(), y(), w(), 40 * m_items.size()),
                       palette().color(Palette::BORDER),
                       palette().color(Palette::BG));

        if (!m_items.empty())
        {
            draw_basic_box(item_rect(m_selected),
                           palette().color(Palette::BORDER), Color::ORANGE);

            for (uint32_t i = 0; i < m_items.size(); i++)
            {
                painter.set_color(palette().color(Palette::TEXT));
                painter.set_font(m_font);
                painter.draw_text(item_rect(i), m_items[i], ALIGN_CENTER);
            }
        }
#endif
    }

    void ListBox::selected(uint32_t index)
    {
        if (m_selected != index)
        {
            m_selected = index;
            damage();
            on_selected(index);
        }
    }

    ListBox::~ListBox()
    {
    }

#ifdef DEVELOPMENT
    ScrollWheel::ScrollWheel(const Point& point, const Size& size)
        : Widget(point.x, point.y, size.w, size.h),
          m_pos(0),
          m_moving_x(0)
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

    void ScrollWheel::draw(const Rect& rect)
    {
        Color border(Color::BLACK);
        Color glass(0x00115555);
        Color color(0x4169E1ff);

        auto cr = screen()->context();

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
#endif
}
