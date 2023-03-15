/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/utf8text.h"
#include "egt/detail/alignment.h"
#include "egt/detail/imagecache.h"
#include "egt/detail/meta.h"
#include "egt/frame.h"
#include "egt/label.h"
#include "egt/painter.h"
#include "egt/serialize.h"

namespace egt
{
inline namespace v1
{
static AlignFlags default_text_align_value{AlignFlag::center};

AlignFlags Label::default_text_align()
{
    return default_text_align_value;
}

void Label::default_text_align(const AlignFlags& align)
{
    default_text_align_value = align;
}

Label::Label(const std::string& text, const AlignFlags& text_align) noexcept
    : Label(text, {}, text_align)
{
}

Label::Label(const std::string& text, const Rect& rect, const AlignFlags& text_align) noexcept
    : TextWidget(text, rect, text_align)
{
    name("Label" + std::to_string(m_widgetid));
}

Label::Label(Frame& parent, const std::string& text, const AlignFlags& text_align) noexcept
    : Label(text, text_align)
{
    parent.add(*this);
}

Label::Label(Frame& parent, const std::string& text, const Rect& rect,
             const AlignFlags& text_align) noexcept
    : Label(text, rect, text_align)
{
    parent.add(*this);
}

Label::Label(Serializer::Properties& props, bool is_derived) noexcept
    : TextWidget(props, true)
{
    if (!is_derived)
        deserialize_leaf(props);
}

void Label::draw(Painter& painter, const Rect& rect)
{
    Drawer<Label>::draw(*this, painter, rect);
}

void Label::default_draw(const Label& widget, Painter& painter, const Rect&)
{
    widget.draw_box(painter, Palette::ColorId::label_bg, Palette::ColorId::border);

    detail::draw_text(painter,
                      widget.content_area(),
                      widget.text(),
                      widget.font(),
                      TextBox::TextFlags({TextBox::TextFlag::multiline, TextBox::TextFlag::word_wrap}),
                      widget.text_align(),
                      Justification::middle,
                      widget.color(Palette::ColorId::label_text));
}

void Label::set_parent(Widget* parent)
{
    TextWidget::set_parent(parent);
    layout();
}

Size Label::min_size_hint() const
{
    if (!m_min_size.empty())
        return m_min_size;

    if (!m_text.empty())
    {
        auto s = text_size(m_text);
        return s + Widget::min_size_hint();
    }

    return Widget::min_size_hint();
}

}
}
