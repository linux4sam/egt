/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
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
                painter.set_font(m_font);
                Size s = painter.text_size(m_text);
                resize(s);
            }
        }
    }

    void Label::text(const std::string& str)
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

        if (!is_flag_set(widgetmask::NO_BACKGROUND))
        {
            if (!is_flag_set(widgetmask::NO_BORDER))
                painter.draw_box(box(),
                                 palette().color(Palette::BORDER),
                                 palette().color(Palette::BG),
                                 Painter::boxtype::border);
            else
                painter.draw_box(box(),
                                 palette().color(Palette::BORDER),
                                 palette().color(Palette::BG),
                                 Painter::boxtype::flat);
        }

        painter.set_color(palette().color(Palette::TEXT));
        painter.set_font(m_font);
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

    CheckBox::CheckBox(const std::string& text,
                       const Rect& rect)
        : Label(text, rect),
          m_checked(false)
    {}

    int CheckBox::handle(eventid event)
    {
        switch (event)
        {
        case eventid::MOUSE_DOWN:
            check(!checked());
            return 1;
        default:
            break;
        }

        return Widget::handle(event);
    }

    void CheckBox::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        static const int STANDOFF = 5;

        // image
        Rect r(x() + STANDOFF,
               y() + STANDOFF,
               h() - STANDOFF * 2,
               h() - STANDOFF * 2);

        if (checked())
        {
            painter.draw_box(r,
                             palette().color(Palette::BORDER),
                             palette().color(Palette::HIGHLIGHT),
                             Painter::boxtype::rounded_gradient);

            static const int OFFSET = 5;
            auto cr = painter.context();

            painter.set_color(palette().color(Palette::DARK));

            cairo_move_to(cr.get(), r.x + OFFSET, r.y + OFFSET);
            cairo_line_to(cr.get(), r.x + r.w - OFFSET, r.y + r.h - OFFSET);
            cairo_move_to(cr.get(), r.x + r.w - OFFSET, r.y + OFFSET);
            cairo_line_to(cr.get(), r.x + OFFSET, r.y + r.h - OFFSET);
            cairo_set_line_width(cr.get(), 2.0);
            cairo_stroke(cr.get());
        }
        else
        {
            painter.draw_box(r,
                             palette().color(Palette::BORDER),
                             palette().color(Palette::BG),
                             Painter::boxtype::rounded_border);
        }

        // text
        painter.draw_text(m_text,
                          box(),
                          palette().color(Palette::TEXT),
                          alignmask::LEFT | alignmask::CENTER,
                          h());
    }

    CheckBox::~CheckBox()
    {}

    SlidingCheckBox::SlidingCheckBox(const Rect& rect)
        : CheckBox("", rect)
    {}

    void SlidingCheckBox::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        painter.draw_box(box(),
                         palette().color(Palette::BORDER),
                         palette().color(Palette::BG),
                         Painter::boxtype::rounded_border);

        if (checked())
        {
            Rect rect = box();
            rect.w /= 2;
            rect.x += rect.w;
            painter.draw_box(rect,
                             palette().color(Palette::BORDER),
                             palette().color(Palette::HIGHLIGHT),
                             Painter::boxtype::rounded_gradient);
        }
        else
        {
            Rect rect = box();
            rect.w /= 2;
            painter.draw_box(rect,
                             palette().color(Palette::BORDER),
                             palette().color(Palette::MID),
                             Painter::boxtype::rounded_gradient);
        }
    }

    SlidingCheckBox::~SlidingCheckBox()
    {}

    ImageLabel::ImageLabel(const std::string& image,
                           const std::string& text,
                           const Rect& rect,
                           const Font& font)
        : Label(text, rect, alignmask::LEFT | alignmask::CENTER, font),
          m_image(detail::image_cache.get(image, 1.0))
    {
        assert(cairo_surface_status(m_image.get()) == CAIRO_STATUS_SUCCESS);
    }

    void ImageLabel::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        // image
        painter.draw_image(m_image, box(), alignmask::LEFT | alignmask::CENTER);

        auto width = cairo_image_surface_get_width(m_image.get());
        painter.draw_text(m_text, box(), palette().color(Palette::TEXT),
                          alignmask::LEFT | alignmask::CENTER, width + 5, m_font);
    }

    ImageLabel::~ImageLabel()
    {
    }

}
