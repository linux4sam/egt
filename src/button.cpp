/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/button.h"
#include "egt/imagecache.h"
#include "egt/painter.h"
#include "egt/frame.h"

using namespace std;

namespace egt
{
    static const auto DEFAULT_BUTTON_SIZE = Size(100, 50);

    Button::Button(const string& text, const Rect& rect,
                   const Font& font, widgetmask flags) noexcept
        : Label(text, rect, alignmask::CENTER, font, flags)
    {
        flag_set(widgetmask::CLICK_FOCUS);

        if (rect.size().empty())
        {
            /** @todo Smarter if we look at size of text and grow from default size. */
            resize(DEFAULT_BUTTON_SIZE);
        }
    }

    Button::Button(Frame& parent, const string& text, const Rect& rect,
                   const Font& font, widgetmask flags) noexcept
        : Button(text, rect, font, flags)
    {
        parent.add(this);
    }

    int Button::handle(eventid event)
    {
        DBG(name() << " handle: " << event);

        switch (event)
        {
        case eventid::MOUSE_DOWN:
        {
            set_active(true);
            break;
        }
        case eventid::MOUSE_UP:
        {
            set_active(false);
            break;
        }
        default:
            break;
        }

        return Widget::handle(event);
    }

    void Button::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        // box
        if (is_flag_set(widgetmask::NO_BORDER))
        {
            painter.draw_box(*this, Painter::boxtype::fill);
        }
        else if (is_flag_set(widgetmask::NO_BACKGROUND))
        {
            painter.draw_box(*this, Painter::boxtype::border);
        }
        else
        {
            painter.draw_box(*this, Painter::boxtype::rounded_gradient);
        }

        // text
        painter.set_color(palette().color(Palette::ColorId::TEXT));
        painter.set_font(font());
        painter.draw_text(box(), m_text, m_text_align, 5);
    }

    Button::~Button()
    {
    }

    ImageButton::ImageButton(const string& image,
                             const string& text,
                             const Rect& rect,
                             widgetmask flags) noexcept
        : Button(text, rect, Font(), flags),
          m_image_align(alignmask::CENTER)
    {
        text_align(alignmask::CENTER | alignmask::BOTTOM);

        if (!image.empty())
            do_set_image(image);
    }

    ImageButton::ImageButton(Frame& parent,
                             const string& image,
                             const string& text,
                             const Rect& rect,
                             widgetmask flags) noexcept
        : ImageButton(image, text, rect, flags)
    {
        parent.add(this);
    }

    void ImageButton::do_set_image(const std::string& image)
    {
        m_image = detail::image_cache.get(image, 1.0);

#if 0
        auto width = cairo_image_surface_get_width(m_image.get());
        auto height = cairo_image_surface_get_height(m_image.get());
        m_box.w = width;
        m_box.h = height;
#endif
    }

    void ImageButton::set_image(const std::string& image)
    {
        do_set_image(image);
        damage();
    }

    void ImageButton::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        if (!is_flag_set(widgetmask::NO_BORDER))
        {
            painter.draw_box(*this, Painter::boxtype::border);
        }

        if (!is_flag_set(widgetmask::NO_BACKGROUND))
        {
            painter.draw_box(*this, Painter::boxtype::fill);
        }

        painter.draw_image(m_image, box(), m_image_align, 0, disabled());

        if (!m_text.empty())
        {
            painter.set_color(palette().color(Palette::ColorId::TEXT));
            painter.set_font(font());
            painter.draw_text(box(), m_text, m_text_align, 5);
        }
    }

    ImageButton::~ImageButton()
    {}
}
