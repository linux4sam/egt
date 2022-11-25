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

void Label::set_parent(Frame* parent)
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

ImageLabel::ImageLabel(ImageLabel&& rhs) noexcept
    : Label(std::move(static_cast<Label&>(rhs))),
      ImageHolder(static_cast<TextWidget&>(*this), std::move(rhs))
{}

ImageLabel::ImageLabel(const std::string& text,
                       const AlignFlags& text_align) noexcept
    : ImageLabel(Image(), text, {}, text_align)
{
}

ImageLabel::ImageLabel(const Image& image,
                       const std::string& text,
                       const AlignFlags& text_align) noexcept
    : ImageLabel(image, text, {}, text_align)
{
}

ImageLabel::ImageLabel(const Image& image,
                       const std::string& text,
                       const Rect& rect,
                       const AlignFlags& text_align) noexcept
    : Label(text, rect, text_align),
      ImageHolder(static_cast<TextWidget&>(*this))
{
    name("ImageLabel" + std::to_string(m_widgetid));

    if (text.empty())
    {
        show_label(false);
        image_align(AlignFlag::center | AlignFlag::expand);
    }
    do_set_image(image);
}

ImageLabel::ImageLabel(Frame& parent,
                       const Image& image,
                       const std::string& text,
                       const AlignFlags& text_align) noexcept
    : ImageLabel(image, text, text_align)
{
    parent.add(*this);
}

ImageLabel::ImageLabel(Frame& parent,
                       const Image& image,
                       const std::string& text,
                       const Rect& rect,
                       const AlignFlags& text_align) noexcept
    : ImageLabel(image, text, rect, text_align)
{
    parent.add(*this);
}

ImageLabel::ImageLabel(Serializer::Properties& props, bool is_derived) noexcept
    : Label(props, true),
      ImageHolder(static_cast<TextWidget&>(*this))
{
    deserialize(props);

    if (!is_derived)
        deserialize_leaf(props);
}

void ImageLabel::draw(Painter& painter, const Rect& rect)
{
    Drawer<ImageLabel>::draw(*this, painter, rect);
}

void ImageLabel::default_draw(ImageLabel& widget, Painter& painter, const Rect& rect)
{
    widget.ImageHolder::default_draw(painter,
                                     rect,
                                     Palette::ColorId::label_bg,
                                     Palette::ColorId::border,
                                     Palette::ColorId::label_text);
}

Size ImageLabel::min_size_hint() const
{
    return ImageHolder::min_size_hint(Label::min_size_hint());
}

void ImageLabel::serialize(Serializer& serializer) const
{
    Label::serialize(serializer);
    ImageHolder::serialize(serializer);
}

void ImageLabel::deserialize(Serializer::Properties& props)
{
    ImageHolder::deserialize(props);
}

}
}
