/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "widget.h"
#include "geometry.h"
#include "input.h"
#include "screen.h"
#include "utils.h"
#include <vector>
#include <string>
#include <cassert>
#include <cairo.h>
#include <iostream>
#include "window.h"
#include "painter.h"
#include "resource.h"
#include "imagecache.h"
#include <sstream>

using namespace std;

namespace mui
{
    Widget::Widget(int x, int y, int w, int h, uint32_t flags)
        : m_box(x, y, w, h),
          m_visible(true),
          m_focus(false),
          m_active(false),
          m_disabled(false),
          m_parent(0),
          m_flags(flags)
    {}

    int Widget::handle(int event)
    {
        // do nothing
        return 0;
    }

    void Widget::position(int x, int y)
    {
        if (x != box().x || y != box().y)
        {
            damage();
            m_box.x = x;
            m_box.y = y;
        }
    }

    void Widget::size(int w, int h)
    {
        if (w != box().w || h != box().h)
        {
            damage();
            m_box.w = w;
            m_box.h = h;
        }
    }

    void Widget::resize(int w, int h)
    {
        if (w != box().w || h != box().h)
        {
            damage();
            size(w, h);
            damage();
        }
    }

    void Widget::move(int x, int y)
    {
        if (x != box().x || y != box().y)
        {
            damage();
            position(x, y);
            damage();
        }
    }

    void Widget::damage()
    {
        parent()->damage(box());
    }

    void Widget::draw_basic_box(const Rect& rect,
                                const Color& border,
                                const Color& bg)
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

    void Widget::draw_gradient_box(const Rect& rect,
                                   const Color& border,
                                   const Color& bg,
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
            Color step = bg;
            cairo_pattern_add_color_stop_rgb(pat, 0, step.redf(), step.greenf(), step.bluef());
            step = bg.tint(.9);
            cairo_pattern_add_color_stop_rgb(pat, 0.43, step.redf(), step.greenf(), step.bluef());
            step = bg.tint(.82);
            cairo_pattern_add_color_stop_rgb(pat, 0.5, step.redf(), step.greenf(), step.bluef());
            step = bg.tint(.95);
            cairo_pattern_add_color_stop_rgb(pat, 1.0, step.redf(), step.greenf(), step.bluef());

            cairo_set_line_width(cr.get(), 1.0);
        }
        else
        {
            Color step = bg;
            cairo_pattern_add_color_stop_rgb(pat, 1, step.redf(), step.greenf(), step.bluef());
            step = bg.tint(.9);
            cairo_pattern_add_color_stop_rgb(pat, 0.5, step.redf(), step.greenf(), step.bluef());
            step = bg.tint(.82);
            cairo_pattern_add_color_stop_rgb(pat, 0.43, step.redf(), step.greenf(), step.bluef());
            step = bg.tint(.95);
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

    Widget::~Widget()
    {}

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

        assert(CAIRO_HAS_PNG_FUNCTIONS == 1);

        // images
        auto up = shared_cairo_surface_t(cairo_image_surface_create_from_png("icons/bullet_arrow_up.png"), cairo_surface_destroy);
        assert(cairo_surface_status(up.get()) == CAIRO_STATUS_SUCCESS);

        auto down = shared_cairo_surface_t(cairo_image_surface_create_from_png("icons/bullet_arrow_down.png"), cairo_surface_destroy);
        assert(cairo_surface_status(down.get()) == CAIRO_STATUS_SUCCESS);

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

    Slider::Slider(int min, int max, const Point& point,
                   const Size& size, int orientation)
        : Widget(point.x, point.y, size.w, size.h),
          m_min(min),
          m_max(max),
          m_pos(min),
          m_moving_x(0),
          m_orientation(orientation)
    {
    }

    int Slider::handle(int event)
    {
        switch (event)
        {
        case EVT_MOUSE_DOWN:
        {
            Rect bounding;

            if (m_orientation == ORIENTATION_HORIZONTAL)
            {
                bounding = Rect(x() + normalize(m_pos) + 1,
                                y() + 1,
                                h() - 2,
                                h() - 2);
            }
            else
            {
                bounding = Rect(x() + 1,
                                y() + normalize(m_pos) + 1,
                                w() - 2,
                                w() - 2);
            }

            if (Rect::point_inside(screen_to_window(mouse_position()), bounding))
            {
                if (m_orientation == ORIENTATION_HORIZONTAL)
                    m_moving_x = screen_to_window(mouse_position()).x;
                else
                    m_moving_x = screen_to_window(mouse_position()).y;
                m_start_pos = position();
                active(true);
                return 1;
            }

            break;
        }
        case EVT_MOUSE_UP:
            active(false);
            return 1;
        case EVT_MOUSE_MOVE:
            if (active())
            {
                if (m_orientation == ORIENTATION_HORIZONTAL)
                {
                    int diff = screen_to_window(mouse_position()).x - m_moving_x;
                    position(m_start_pos + denormalize(diff));
                }
                else
                {
                    int diff = screen_to_window(mouse_position()).y - m_moving_x;
                    position(m_start_pos + denormalize(diff));
                }
                return 1;
            }
            break;
        }

        return Widget::handle(event);
    }

    void Slider::draw(const Rect& rect)
    {
        auto cr = screen()->context();

        cairo_save(cr.get());

        cairo_set_source_rgba(cr.get(),
                              palette().color(Palette::HIGHLIGHT).redf(),
                              palette().color(Palette::HIGHLIGHT).greenf(),
                              palette().color(Palette::HIGHLIGHT).bluef(),
                              palette().color(Palette::HIGHLIGHT).alphaf());

        if (m_orientation == ORIENTATION_HORIZONTAL)
        {
            cairo_set_line_width(cr.get(), h() / 5.0);

            // line
            cairo_move_to(cr.get(), x(), y() + h() / 2);
            cairo_line_to(cr.get(), x() + normalize(m_pos), y() + h() / 2);
            cairo_stroke(cr.get());

            cairo_set_source_rgba(cr.get(),
                                  palette().color(Palette::BORDER).redf(),
                                  palette().color(Palette::BORDER).greenf(),
                                  palette().color(Palette::BORDER).bluef(),
                                  palette().color(Palette::BORDER).alphaf());

            cairo_move_to(cr.get(), x() + normalize(m_pos) + 1, y() + h() / 2);
            cairo_line_to(cr.get(), x() + w(), y() + h() / 2);
            cairo_stroke(cr.get());

            // handle
            draw_gradient_box(Rect(x() + normalize(m_pos) + 1,
                                   y() + 1,
                                   h() - 2,
                                   h() - 2),
                              palette().color(Palette::BORDER),
                              palette().color(Palette::HIGHLIGHT));
        }
        else
        {
            cairo_set_line_width(cr.get(), w() / 5.0);

            // line
            cairo_move_to(cr.get(), x() + w() / 2, y() + h());
            cairo_line_to(cr.get(), x() + w() / 2, y() + normalize(m_pos));
            cairo_stroke(cr.get());

            cairo_set_source_rgba(cr.get(),
                                  palette().color(Palette::BORDER).redf(),
                                  palette().color(Palette::BORDER).greenf(),
                                  palette().color(Palette::BORDER).bluef(),
                                  palette().color(Palette::BORDER).alphaf());

            cairo_move_to(cr.get(), x() + w() / 2, y() + normalize(m_pos) + 1);
            cairo_line_to(cr.get(), x() + w() / 2, y());
            cairo_stroke(cr.get());

            // handle
            draw_gradient_box(Rect(x() + 1,
                                   y() + normalize(m_pos) + 1,
                                   w() - 2,
                                   w() - 2),
                              palette().color(Palette::BORDER),
                              palette().color(Palette::HIGHLIGHT));
        }

        cairo_restore(cr.get());
    }

    Slider::~Slider()
    {
    }

    SimpleText::SimpleText(const std::string& text, const Point& point, const Size& size)
        : Widget(point.x, point.y, size.w, size.h),
          m_text(text)
    {}

    int SimpleText::handle(int event)
    {
        switch (event)
        {
        case EVT_MOUSE_DOWN:
            damage();
            focus(true);
            return 1;
        case EVT_KEY_DOWN:
            if (focus())
            {
                m_text.append(1, (char)key_position());
                damage();
                return 1;
            }
            break;
        }

        return Widget::handle(event);
    }

    void SimpleText::draw(const Rect& rect)
    {
        // box
        draw_basic_box(box(), palette().color(Palette::BORDER),
                       palette().color(Palette::HIGHLIGHT));

        // text
        draw_text(m_text, box(), palette().color(Palette::TEXT), ALIGN_CENTER | ALIGN_LEFT);
    }

    SimpleText::~SimpleText()
    {
    }



    ListBox::ListBox(const std::vector<string>& items,
                     const Point& point,
                     const Size& size)
        : Widget(point.x, point.y, size.w, size.h),
          m_items(items),
          m_selected(0)
    {

    }

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
                if (Rect::point_inside(screen_to_window(mouse_position()), item_rect(i)))
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
        auto cr = screen()->context();

        Font font;
        cairo_text_extents_t textext;
        cairo_select_font_face(cr.get(),
                               font.face().c_str(),
                               CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr.get(), font.size());
        cairo_text_extents(cr.get(), "a", &textext);

        draw_basic_box(Rect(x(), y(), w(), 40 * m_items.size()),
                       palette().color(Palette::BORDER),
                       palette().color(Palette::HIGHLIGHT));

        if (!m_items.empty())
        {
            draw_basic_box(item_rect(m_selected),
                           palette().color(Palette::BORDER), Color::ORANGE);

            for (uint32_t i = 0; i < m_items.size(); i++)
            {
                draw_text(m_items[i], item_rect(i), Color::BLACK, ALIGN_CENTER);
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
            m_moving_x = screen_to_window(mouse_position()).y;
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
                int diff = screen_to_window(mouse_position()).y - m_moving_x;
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
