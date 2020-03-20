/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/utf8text.h"
#include "egt/canvas.h"
#include "egt/painter.h"
#include "egt/serialize.h"
#include "egt/textwidget.h"

namespace egt
{
inline namespace v1
{

TextWidget::TextWidget(const std::string& text,
                       const Rect& rect,
                       const AlignFlags& text_align) noexcept
    : Widget(rect),
      m_text_align(text_align),
      m_text(text)
{}

void TextWidget::clear()
{
    if (!m_text.empty())
    {
        m_text.clear();
        on_text_changed.invoke();
        damage();
    }
}

void TextWidget::text(const std::string& str)
{
    if (detail::change_if_diff<>(m_text, str))
    {
        on_text_changed.invoke();
        damage();
        parent_layout();
    }
}

size_t TextWidget::len() const
{
    return detail::utf8len(m_text);
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

        if (textext.width - textext.x_bearing < target.width() &&
            textext.height - textext.y_bearing < target.height())
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

void TextWidget::serialize(Serializer& serializer) const
{
    Widget::serialize(serializer);
    if (!text().empty())
        serializer.add_property("text", text());
}

void TextWidget::deserialize(const std::string& name, const std::string& value,
                             const std::map<std::string, std::string>& attrs)
{
    if (name == "text")
        text(value);
    else
        Widget::deserialize(name, value, attrs);
}

}
}
