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
                       alignmask align, const Font& font) noexcept
    : Widget(rect),
      m_text_align(align),
      m_text(text)
{
    set_font(font);
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
    if (detail::change_if_diff<>(m_text, str))
    {
        invoke_handlers(eventid::property_changed);
        damage();
    }
}

Font TextWidget::scale_font(const Size& target, const std::string& text, const Font& font)
{
    Canvas canvas(Size(10, 10));
    Painter painter(canvas.context());

    auto nfont = font;
    while (true)
    {
        painter.set(nfont);

        cairo_text_extents_t textext;
        cairo_text_extents(painter.context().get(), text.c_str(), &textext);

        if (textext.width - textext.x_bearing < target.w &&
            textext.height - textext.y_bearing < target.h)
            return nfont;

        nfont.size(nfont.size() - 1);
        if (nfont.size() < 1)
            return font;
    }
    return nfont;
}

Size TextWidget::text_size(const std::string& text) const
{
    Canvas canvas(Size(100, 100));
    Painter painter(canvas.context());
    painter.set(this->font());
    return painter.font_size(text);
}

}
}
