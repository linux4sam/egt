/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "button.h"
#include "imagecache.h"
#include "painter.h"

using namespace std;

namespace mui
{

    Button::Button(const string& text, const Point& point, const Size& size)
        : Label(text, point, size, ALIGN_CENTER)
    {}

    int Button::handle(int event)
    {
        switch (event)
        {
        case EVT_MOUSE_DOWN:
            if (!active())
            {
                damage();
                active(true);
                invoke_handlers();
                return 1;
            }
            break;
        case EVT_MOUSE_UP:
            if (active())
            {
                damage();
                active(false);
                invoke_handlers();
                return 1;
            }
            break;
        case EVT_MOUSE_MOVE:
            break;
        case EVT_KEY_DOWN:
            break;
        }

        return Widget::handle(event);
    }

    void Button::draw(const Rect& rect)
    {
        Painter painter(screen()->context());

        // box
        draw_gradient_box(box(), palette().color(Palette::BORDER), active());

        // text
        painter.set_color(palette().color(Palette::TEXT));
        painter.set_font(m_font);
        painter.draw_text(box(), m_text, m_align, 5);
    }

    Button::~Button()
    {
    }

    ImageButton::ImageButton(const string& image,
                             const string& text,
                             const Point& point,
                             const Size& size,
                             bool border)
        : Button(text, point, size),
          m_border(border),
          m_image_align(ALIGN_CENTER)
    {
        set_label_align(ALIGN_CENTER | ALIGN_BOTTOM);

        if (!image.empty())
            set_image(image);
    }

    void ImageButton::set_image(const std::string& image)
    {
        m_image = image_cache.get(image, 1.0);

        //if (box().size().empty())
        //{
        auto width = cairo_image_surface_get_width(m_image.get());
        auto height = cairo_image_surface_get_height(m_image.get());
        m_box.w = width;
        m_box.h = height;
        //}
    }

    void ImageButton::draw(const Rect& rect)
    {
        if (m_border)
            draw_gradient_box(box(), palette().color(Palette::BORDER));

        draw_image(m_image, m_image_align, 10, disabled());

        Painter painter(screen()->context());
        painter.set_color(palette().color(Palette::TEXT));
        painter.set_font(m_font);
        painter.draw_text(box(), m_text, m_align, 5);
    }

    ImageButton::~ImageButton()
    {
    }
}
