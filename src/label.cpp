/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "label.h"
#include "imagecache.h"
#include "painter.h"

using namespace std;

namespace mui
{

    Label::Label(const std::string& text, const Point& point, const Size& size,
                 int align, const Font& font)
        : Widget(point, size),
          m_align(align),
          m_text(text),
          m_font(font)
    {}

    void Label::text(const std::string& str)
    {
        if (m_text != str)
        {
            m_text = str;
            damage();
        }
    }

    void Label::draw(const Rect& rect)
    {
        Painter painter(screen()->context());

        if (!is_flag_set(FLAG_NO_BACKGROUND))
        {
            if (is_flag_set(FLAG_BORDER))
                draw_basic_box(box(), palette().color(Palette::BORDER), palette().color(Palette::BG));
            else
                draw_basic_box(box(), palette().color(Palette::BG), palette().color(Palette::BG));
        }

        painter.set_color(palette().color(Palette::TEXT));
        painter.set_font(m_font);
        painter.draw_text(box(), m_text, m_align, 5);
    }

    Label::~Label()
    {
    }

    CheckBox::CheckBox(const std::string& text,
                       const Point& point,
                       const Size& size)
        : Label(text, point, size),
          m_checked(false)
    {}

    int CheckBox::handle(int event)
    {
        switch (event)
        {
        case EVT_MOUSE_DOWN:
            damage();
            checked(!checked());
            return 1;
        }

        return Widget::handle(event);
    }

    void CheckBox::draw(const Rect& rect)
    {
        static const int STANDOFF = 5;

        // image
        Rect r(x() + STANDOFF,
               y() + STANDOFF,
               h() - STANDOFF * 2,
               h() - STANDOFF * 2);

        if (checked())
        {
            draw_basic_box(r, palette().color(Palette::BORDER),
                           palette().color(Palette::HIGHLIGHT));

            static const int OFFSET = 5;
            auto cr = screen()->context();

            cairo_set_source_rgba(cr.get(),
                                  palette().color(Palette::DARK).redf(),
                                  palette().color(Palette::DARK).greenf(),
                                  palette().color(Palette::DARK).bluef(),
                                  palette().color(Palette::DARK).alphaf());

            cairo_move_to(cr.get(), r.x + OFFSET, r.y + OFFSET);
            cairo_line_to(cr.get(), r.x + r.w - OFFSET, r.y + r.h - OFFSET);
            cairo_move_to(cr.get(), r.x + r.w - OFFSET, r.y + OFFSET);
            cairo_line_to(cr.get(), r.x + OFFSET, r.y + r.h - OFFSET);
            cairo_set_line_width(cr.get(), 2.0);
            cairo_stroke(cr.get());
        }
        else
        {
            draw_gradient_box(r, palette().color(Palette::BORDER));
        }

        // text
        draw_text(m_text,
                  box(),
                  palette().color(Palette::TEXT),
                  ALIGN_LEFT | ALIGN_CENTER,
                  h());
    }

    CheckBox::~CheckBox()
    {}

    ImageLabel::ImageLabel(const std::string& image,
                           const std::string& text,
                           const Point& point,
                           const Size& size,
                           const Font& font)
        : Label(text, point, size, ALIGN_LEFT | ALIGN_CENTER, font)
    {
        m_image = image_cache.get(image, 1.0);
        assert(m_image.get());
        assert(cairo_surface_status(m_image.get()) == CAIRO_STATUS_SUCCESS);
    }

    void ImageLabel::draw(const Rect& rect)
    {
        // image
        draw_image(m_image, ALIGN_LEFT | ALIGN_CENTER);

        // text
        //Label::draw(rect);

        auto width = cairo_image_surface_get_width(m_image.get());
        draw_text(m_text, box(), palette().color(Palette::TEXT),
                  ALIGN_LEFT | ALIGN_CENTER, width + 5, m_font);

    }

    ImageLabel::~ImageLabel()
    {
    }

}
