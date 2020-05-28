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

void Label::draw(Painter& painter, const Rect& rect)
{
    Drawer<Label>::draw(*this, painter, rect);
}

void Label::default_draw(Label& widget, Painter& painter, const Rect&)
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
      m_image(image)
{
    name("ImageLabel" + std::to_string(m_widgetid));

    if (text.empty())
        image_align(AlignFlag::center);
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

void ImageLabel::draw(Painter& painter, const Rect& rect)
{
    Drawer<ImageLabel>::draw(*this, painter, rect);
}

void ImageLabel::default_draw(ImageLabel& widget, Painter& painter, const Rect& rect)
{
    detail::ignoreparam(rect);

    widget.draw_box(painter, Palette::ColorId::label_bg, Palette::ColorId::border);

    if (!widget.text().empty())
    {
        std::string text;
        if (widget.show_label())
            text = widget.text();

        if (!widget.image().empty())
        {
            detail::draw_text(painter,
                              widget.content_area(),
                              text,
                              widget.font(),
                              TextBox::TextFlags({TextBox::TextFlag::multiline, TextBox::TextFlag::word_wrap}),
                              widget.text_align(),
                              Justification::middle,
                              widget.color(Palette::ColorId::label_text),
                              widget.image_align(),
                              widget.image());
        }
        else
        {
            detail::draw_text(painter,
                              widget.content_area(),
                              text,
                              widget.font(),
                              TextBox::TextFlags({TextBox::TextFlag::multiline, TextBox::TextFlag::word_wrap}),
                              widget.text_align(),
                              Justification::middle,
                              widget.color(Palette::ColorId::label_text));
        }
    }
    else if (!widget.image().empty())
    {
        const auto target = detail::align_algorithm(widget.m_image.size(),
                            widget.content_area(),
                            widget.image_align());

        const auto hs = static_cast<float>(target.width()) /
                        static_cast<float>(widget.image().size_orig().width());
        const auto vs = static_cast<float>(target.height()) /
                        static_cast<float>(widget.image().size_orig().height());
        widget.image().scale(hs, vs);

        painter.draw(target.point());
        painter.draw(widget.image());
    }
}

Size ImageLabel::min_size_hint() const
{
    if (!m_min_size.empty())
        return m_min_size;

    Rect size = Label::min_size_hint() - Size(moat() * 2, moat() * 2);

    if (!m_image.size().empty())
    {
        if (image_align().is_set(AlignFlag::left) ||
            image_align().is_set(AlignFlag::right))
        {
            size += Size(m_image.width(), 0);
        }
        else if (image_align().is_set(AlignFlag::top) ||
                 image_align().is_set(AlignFlag::bottom))
        {
            size += Size(0, m_image.height());
        }

        size = Rect::merge(size, m_image.size());
    }

    auto res = size.size() + Size(moat() * 2, moat() * 2);
    return res;
}

void ImageLabel::do_set_image(const Image& image)
{
    if (size().empty() && !image.empty())
        resize(image.size() + Size(moat() * 2, moat() * 2));

    m_image = image;
    damage();
}

void ImageLabel::image(const Image& image)
{
    do_set_image(image);
}

void ImageLabel::show_label(bool value)
{
    if (detail::change_if_diff<>(m_show_label, value))
        damage();
}

void ImageLabel::serialize(Serializer& serializer) const
{
    Label::serialize(serializer);

    serializer.add_property("showlabel", show_label());
    if (!m_image.empty())
        m_image.serialize("image", serializer);

    // TODO m_image_align
}

void ImageLabel::deserialize(const std::string& name, const std::string& value,
                             const Serializer::Attributes& attrs)
{
    // TODO proper loading of all image properties

    switch (detail::hash(name))
    {
    case detail::hash("showlabel"):
        show_label(std::stoul(value));
        break;
    case detail::hash("image"):
        m_image.deserialize(name, value, attrs);
        break;
    default:
        Label::deserialize(name, value, attrs);
        break;
    }
}

}
}
