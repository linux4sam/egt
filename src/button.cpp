/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "button.h"
#include "imagecache.h"

using namespace std;

namespace mui
{

    Button::Button(const string& label, const Point& point, const Size& size)
        : Widget(point.x, point.y, size.w, size.h),
          m_label(label)
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
        // box
        draw_gradient_box(box(), palette().color(Palette::BORDER),
                          palette().color(Palette::HIGHLIGHT), active());

        // text
        draw_text(m_label, box());
    }

    Button::~Button()
    {
    }

    ImageButton::ImageButton(const string& image,
                             const string& label,
                             const Point& point,
                             const Size& size,
                             bool border)
        : Button(label, point, size),
          m_border(border),
          m_image_align(ALIGN_CENTER),
          m_label_align(ALIGN_CENTER | ALIGN_BOTTOM)
    {
        if (!image.empty())
            set_image(image);
    }

    void ImageButton::set_image(const std::string& image)
    {
        m_image = image_cache.get(image, 1.0);
        assert(m_image.get());
        assert(cairo_surface_status(m_image.get()) == CAIRO_STATUS_SUCCESS);

        auto width = cairo_image_surface_get_width(m_image.get());
        auto height = cairo_image_surface_get_height(m_image.get());
        m_box.w = width;
        m_box.h = height;
    }

    void ImageButton::draw(const Rect& rect)
    {
        if (m_border)
            draw_gradient_box(box(), palette().color(Palette::BORDER), palette().color(Palette::HIGHLIGHT));

        draw_image(m_image, m_image_align, 10, disabled());

        if (!m_label.empty())
            draw_text(m_label, box(), palette().color(Palette::TEXT),
                      m_label_align, 10,
                      m_font);
    }

    ImageButton::~ImageButton()
    {
    }
}
