/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/label.h"
#include "egt/imagecache.h"
#include "egt/painter.h"
#include "egt/frame.h"

using namespace std;

namespace egt
{

    Label::Label(const std::string& text, const Rect& rect,
                 alignmask align, const Font& font, widgetmask flags) noexcept
        : TextWidget(text, rect, align, font, flags)
    {
        set_boxtype(Theme::boxtype::fill);
    }

    Label::Label(Frame& parent, const std::string& text, const Rect& rect,
                 alignmask align, const Font& font, widgetmask flags) noexcept
        : Label(text, rect, align, font, flags)
    {
        parent.add(this);
        first_resize();
    }

    void Label::first_resize()
    {
        if (box().size().empty())
        {
            if (!m_text.empty())
            {
                Painter painter(screen()->context());
                painter.set_font(font());
                Size s = painter.text_size(m_text);
                resize(s);
            }
        }
    }

    void Label::set_text(const std::string& str)
    {
        if (m_text != str)
        {
            bool doresize = m_text.empty();
            m_text = str;
            if (doresize)
            {
                first_resize();
            }
            damage();
        }
    }

    void Label::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        draw_box(painter);

        painter.set_color(palette().color(Palette::TEXT));
        painter.set_font(font());
        painter.draw_text(box(), m_text, m_text_align, 5);
#if 0
        auto cr = painter.context();
        cairo_text_extents_t textext;
        cairo_text_extents(cr.get(), "A", &textext);

        std::stringstream ss;//(m_text_wrapped);

        size_t n = 0;
        for (size_t i = 0; i < m_text.size(); i++)
        {
            if (i % int(box().w / textext.width) == 0)
            {
                n++;
                ss << '\n';
            }
            ss << m_text[i];
        }

        //std::stringstream ss(m_text_wrapped);
        std::string line;

        Rect linerect = box();
        linerect.y += textext.height / 3;
        linerect.h = textext.height + textext.height / 3;

        //size_t n = std::count(ss.str().begin(), ss.str().end(), '\n');
        size_t lines = box().h / linerect.h;

        while (std::getline(ss, line, '\n'))
        {
            if (n-- >= lines)
                continue;

            painter.draw_text(line, linerect,
                              palette().color(Palette::TEXT),
                              m_text_align,
                              5,
                              m_font);
            linerect.y += linerect.h;
        }
#endif
    }

    Label::~Label()
    {}

    ImageLabel::ImageLabel(const Image& image,
                           const std::string& text,
                           const Rect& rect,
                           const Font& font)
        : Label(text, rect, alignmask::RIGHT | alignmask::CENTER, font),
          m_image(image)
    {
        set_boxtype(Theme::boxtype::none);

        if (text.empty())
            set_image_align(alignmask::CENTER);

        if (rect.empty())
            m_box = Rect(rect.point(), m_image.size());
    }

    ImageLabel::ImageLabel(const Image& image,
                           const Point& point)
        : ImageLabel(image, "", Rect(point, image.size()), Font())
    {
    }

    ImageLabel::ImageLabel(const Image& image,
                           const std::string& text,
                           const Point& point,
                           const Font& font)
        : ImageLabel(image, text, Rect(point, image.size()), font)
    {
    }

    ImageLabel::ImageLabel(Frame& parent,
                           const Image& image,
                           const std::string& text,
                           const Rect& rect,
                           const Font& font)
        : ImageLabel(image, text, rect, font)
    {
        parent.add(this);
    }

    void ImageLabel::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        draw_box(painter);

        if (m_label && !m_text.empty())
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

            painter.draw_image(ibox.point(), m_image);

            painter.draw_text(m_text, tbox, palette().color(Palette::TEXT),
                              alignmask::CENTER, 0, font());
        }
        else
        {
            Rect target = Widget::align_algorithm(m_image.size(), box(), m_image_align, 0);
            painter.draw_image(target.point(), m_image);
        }
    }

    void ImageLabel::set_image(const Image& image)
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

    void ImageLabel::label_enabled(bool value)
    {
        if (m_label != value)
        {
            m_label = value;
            damage();
        }
    }

    ImageLabel::~ImageLabel()
    {
    }

}
