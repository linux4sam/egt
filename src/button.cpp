/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/button.h"
#include "egt/imagecache.h"
#include "egt/painter.h"
#include "egt/frame.h"
#include "egt/widget.h"

using namespace std;

namespace egt
{
    inline namespace v1
    {
        static const auto DEFAULT_BUTTON_SIZE = Size(100, 50);

        Button::Button(const std::string& text, const Rect& rect,
                       const Font& font, widgetmask flags) noexcept
            : TextWidget(text, rect, alignmask::CENTER, font, flags)
        {
            set_boxtype(Theme::boxtype::rounded_gradient);
            flag_set(widgetmask::GRAB_MOUSE);

            if (rect.size().empty())
            {
                /** @todo Smarter if we look at size of text and grow from default size. */
                resize(DEFAULT_BUTTON_SIZE);
            }
        }

        Button::Button(Frame& parent, const std::string& text, const Rect& rect,
                       const Font& font, widgetmask flags) noexcept
            : Button(text, rect, font, flags)
        {
            parent.add(this);
        }

        int Button::handle(eventid event)
        {
            auto ret = TextWidget::handle(event);

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

            return ret;
        }

        void Button::draw(Painter& painter, const Rect& rect)
        {
            ignoreparam(rect);

            draw_box(painter);

            // text
            painter.set_color(palette().color(Palette::ColorId::TEXT));
            painter.set_font(font());
            painter.draw_text(box(), m_text, m_text_align, 5);
        }

        Button::~Button()
        {
        }

        ImageButton::ImageButton(const Image& image,
                                 const std::string& text,
                                 const Rect& rect,
                                 widgetmask flags) noexcept
            : Button(text, rect, Font(), flags)
        {
            if (text.empty())
                set_image_align(alignmask::CENTER);
            do_set_image(image);
        }

        ImageButton::ImageButton(Frame& parent,
                                 const Image& image,
                                 const std::string& text,
                                 const Rect& rect,
                                 widgetmask flags) noexcept
            : ImageButton(image, text, rect, flags)
        {
            parent.add(this);
        }

        void ImageButton::do_set_image(const Image& image)
        {
            if (!image.empty())
            {
                m_image = image;
#if 0
                auto width = cairo_image_surface_get_width(m_image.get());
                auto height = cairo_image_surface_get_height(m_image.get());
                m_box.w = width;
                m_box.h = height;
#endif
                damage();
            }
        }

        void ImageButton::set_image(const Image& image)
        {
            do_set_image(image);
        }

        void ImageButton::draw(Painter& painter, const Rect& rect)
        {
            ignoreparam(rect);

            draw_box(painter);

            if (!m_text.empty())
            {
                painter.set_font(font());
                auto text_size = painter.text_size(m_text);

                Rect tbox;
                Rect ibox;

                if (m_position_image_first)
                    Widget::double_align(box(),
                                         m_image.size(), m_image_align, ibox,
                                         text_size, m_text_align, tbox, 5);
                else
                    Widget::double_align(box(),
                                         text_size, m_text_align, tbox,
                                         m_image.size(), m_image_align, ibox, 5);

                painter.draw_image(ibox.point(), m_image, disabled());

                painter.draw_text(m_text, tbox, palette().color(Palette::TEXT),
                                  alignmask::CENTER, 0, font());
            }
            else
            {
                Rect target = Widget::align_algorithm(m_image.size(), box(), m_image_align, 0);
                painter.draw_image(target.point(), m_image, disabled());
            }
        }

        ImageButton::~ImageButton()
        {}

    }
}
