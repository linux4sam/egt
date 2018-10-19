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

    Button::Button(const string& text, const Point& point, const Size& size,
                   widgetmask flags) noexcept
        : Label(text, point, size, alignmask::CENTER, Font(), flags)
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
            focus(true);
            active(true);
            invoke_handlers(event);
            return 1;
        case EVT_MOUSE_UP:
            focus(false);
            active(false);
            invoke_handlers(event);
            return 1;
        }

        return Widget::handle(event);
    }

    void Button::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        // box
        painter.draw_gradient_box(box(), palette().color(Palette::BORDER),
                                  active(),
                                  active() ? palette().color(Palette::HIGHLIGHT, Palette::GROUP_ACTIVE) :
                                  palette().color(Palette::LIGHT, Palette::GROUP_NORMAL));

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
                             widgetmask flags) noexcept
        : Button(text, point, size, flags),
          m_image_align(alignmask::CENTER)
    {
        set_label_align(alignmask::CENTER | alignmask::BOTTOM);

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
        ignoreparam(rect);

        if (!is_flag_set(widgetmask::NO_BORDER))
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
