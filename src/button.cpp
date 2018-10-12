/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "mui/button.h"
#include "mui/imagecache.h"
#include "mui/painter.h"

using namespace std;

namespace mui
{
    static const auto DEFAULT_BUTTON_SIZE = Size(100, 50);

    Button::Button(const string& text, const Point& point, const Size& size)
        : Label(text, point, size, ALIGN_CENTER)
    {
        if (size.empty())
        {
            /** @todo Smarter if we look at size of text and grow from default size. */
            resize(DEFAULT_BUTTON_SIZE);
        }
    }

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

    void Button::draw(Painter& painter, const Rect& rect)
    {
        // box
        painter.draw_gradient_box(box(), palette().color(Palette::BORDER),
                                  active(),
                                  palette().color(Palette::LIGHT));

        // text
        painter.set_color(palette().color(Palette::TEXT));
        painter.set_font(m_font);
        painter.draw_text(box(), m_text, m_text_align, 5);
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

    void ImageButton::draw(Painter& painter, const Rect& rect)
    {
        if (m_border)
            painter.draw_gradient_box(box(), palette().color(Palette::BORDER));

        painter.draw_image(m_image, box(), m_image_align, 10, disabled());

        painter.set_color(palette().color(Palette::TEXT));
        painter.set_font(m_font);
        painter.draw_text(box(), m_text, m_text_align, 5);
    }

    ImageButton::~ImageButton()
    {
    }
}
