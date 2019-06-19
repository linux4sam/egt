/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/utf8text.h"
#include "egt/canvas.h"
#include "egt/detail/textwidget.h"
#include "egt/painter.h"

using namespace std;

namespace egt
{
inline namespace v1
{
namespace detail
{

TextWidget::TextWidget(const std::string& text,
                       const Rect& rect,
                       alignmask text_align) noexcept
    : Widget(rect),
      m_text_align(text_align),
      m_text(text)
{}

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

size_t TextWidget::len() const
{
    return utf8len(m_text);
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

        if (textext.width - textext.x_bearing < target.width &&
            textext.height - textext.y_bearing < target.height)
            return nfont;

        nfont.set_size(nfont.size() - 1);
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
}
