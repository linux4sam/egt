/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "mui/label.h"
#include "mui/imagecache.h"
#include "mui/painter.h"
#include "mui/frame.h"

using namespace std;

namespace mui
{

    Label::Label(const std::string& text, const Rect& rect,
                 alignmask align, const Font& font, widgetmask flags) noexcept
        : Widget(rect, flags),
          m_text_align(align),
          m_text(text),
          m_font(font)
    {}

    Label::Label(Frame& parent, const std::string& text, const Rect& rect,
                 alignmask align, const Font& font, widgetmask flags) noexcept
        : Label(text, rect, align, font, flags)
    {
        parent.add(this);
    }

    void Label::text(const std::string& str)
    {
        if (m_text != str)
        {
            m_text = str;
            damage();
        }
    }

    void Label::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        if (!is_flag_set(widgetmask::NO_BACKGROUND))
        {
            if (!is_flag_set(widgetmask::NO_BORDER))
                painter.draw_basic_box(box(), palette().color(Palette::BORDER),
                                       palette().color(Palette::BG));
            else
                painter.draw_basic_box(box(), palette().color(Palette::BG),
                                       palette().color(Palette::BG));
        }

        painter.set_color(palette().color(Palette::TEXT));
        painter.set_font(m_font);
        painter.draw_text(box(), m_text, m_text_align, 5);
    }

    Label::~Label()
    {}

    CheckBox::CheckBox(const std::string& text,
                       const Rect& rect)
        : Label(text, rect),
          m_checked(false)
    {}

    int CheckBox::handle(int event)
    {
        switch (event)
        {
        case EVT_MOUSE_DOWN:
            check(!checked());
            return 1;
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
            painter.draw_basic_box(r, palette().color(Palette::BORDER),
                                   palette().color(Palette::HIGHLIGHT));

            static const int OFFSET = 5;
            auto cr = painter.context();

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
            painter.draw_gradient_box(r, palette().color(Palette::BORDER));
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

        painter.draw_basic_box(box(), palette().color(Palette::BORDER),
                               palette().color(Palette::BG));

        if (checked())
        {
            Rect rect = box();
            rect.w /= 2;
            rect.x += rect.w;
            painter.draw_basic_box(rect, palette().color(Palette::BORDER),
                                   palette().color(Palette::HIGHLIGHT));
        }
        else
        {
            Rect rect = box();
            rect.w /= 2;
            painter.draw_basic_box(rect, palette().color(Palette::BORDER),
                                   palette().color(Palette::MID));
        }
    }

    SlidingCheckBox::~SlidingCheckBox()
    {}

    ImageLabel::ImageLabel(const std::string& image,
                           const std::string& text,
                           const Rect& rect,
                           const Font& font)
        : Label(text, rect, alignmask::LEFT | alignmask::CENTER, font),
          m_image(image_cache.get(image, 1.0))
    {
        assert(cairo_surface_status(m_image.get()) == CAIRO_STATUS_SUCCESS);
    }

    void ImageLabel::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        // image
        painter.draw_image(m_image, box(), alignmask::LEFT | alignmask::CENTER);

        // text
        //Label::draw(rect);

        auto width = cairo_image_surface_get_width(m_image.get());
        painter.draw_text(m_text, box(), palette().color(Palette::TEXT),
                          alignmask::LEFT | alignmask::CENTER, width + 5, m_font);

    }

    ImageLabel::~ImageLabel()
    {
    }

}
