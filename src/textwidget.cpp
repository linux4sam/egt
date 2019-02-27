/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/canvas.h"
#include "egt/painter.h"
#include "egt/textwidget.h"

using namespace std;

namespace egt
{
inline namespace v1
{

TextWidget::TextWidget(const std::string& text, const Rect& rect,
                       alignmask align, const Font& font, const Widget::flags_type& flags) noexcept
    : Widget(rect, flags),
      m_text_align(align),
      m_text(text)
{
    set_font(font);
}

TextWidget::TextWidget(const TextWidget& rhs) noexcept
    : Widget(rhs),
      m_text_align(rhs.m_text_align),
      m_text(rhs.m_text)
{
    if (rhs.m_font)
        m_font = make_unique<Font>(*rhs.m_font.get());
}

TextWidget& TextWidget::operator=(const TextWidget& rhs) noexcept
{
    Widget::operator=(rhs);
    if (rhs.m_font)
        m_font = make_unique<Font>(*rhs.m_font.get());

    return *this;
}

void TextWidget::clear()
{
    if (!m_text.empty())
    {
        m_text.clear();
        damage();
    }
}

void TextWidget::set_text(const std::string& str)
{
    if (m_text != str)
    {
        m_text = str;
        damage();
    }
}

Font TextWidget::scale_font(const Size& target, const std::string& text, const Font& font)
{
    auto surface = shared_cairo_surface_t(cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 10, 10),
                                          cairo_surface_destroy);
    auto cr = shared_cairo_t(cairo_create(surface.get()), cairo_destroy);
    Painter painter(cr);

    auto nfont = font;
    while (true)
    {
        painter.set_font(nfont);

        cairo_text_extents_t textext;
        cairo_text_extents(cr.get(), text.c_str(), &textext);

        if (textext.width - textext.x_bearing < target.w &&
            textext.height - textext.y_bearing < target.h)
            return nfont;

        nfont.size(nfont.size() - 1);
        if (nfont.size() < 1)
            return font;
    }
    return nfont;
}

Size TextWidget::text_size(const std::string& text)
{
    if (m_parent)
    {
        Canvas canvas(Size(100, 100));
        Painter painter(canvas.context());
        painter.set_font(font());
        return painter.text_size(text);
    }

    return Size();
}

}
}
