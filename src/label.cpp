/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/utf8text.h"
#include "egt/detail/alignment.h"
#include "egt/detail/imagecache.h"
#include "egt/frame.h"
#include "egt/label.h"
#include "egt/painter.h"

namespace egt
{
inline namespace v1
{
static const auto DEFAULT_LABEL_SIZE = Size(100, 50);

const alignmask Label::default_align = alignmask::center | alignmask::left;

Label::Label(const std::string& text, alignmask text_align) noexcept
    : Label(text, {}, text_align)
{
}

Label::Label(const std::string& text, const Rect& rect, alignmask text_align) noexcept
    : TextWidget(text, rect, text_align)
{
    set_name("Label" + std::to_string(m_widgetid));

    set_boxtype(Theme::boxtype::none);
    set_padding(5);
}

Label::Label(Frame& parent, const std::string& text, alignmask text_align) noexcept
    : Label(text, text_align)
{
    parent.add(*this);
}

Label::Label(Frame& parent, const std::string& text, const Rect& rect,
             alignmask text_align) noexcept
    : Label(text, rect, text_align)
{
    parent.add(*this);
}

void Label::set_text(const std::string& text)
{
    if (m_text != text)
    {
        bool doresize = m_text.empty();
        TextWidget::set_text(text);
        if (doresize)
        {
            first_resize();
        }
    }
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
                      TextBox::flags_type({TextBox::flag::multiline, TextBox::flag::word_wrap}),
                      widget.text_align(),
                      justification::middle,
                      widget.color(Palette::ColorId::label_text).color());
}

void Label::set_parent(Frame* parent)
{
    TextWidget::set_parent(parent);
    first_resize();
}

Size Label::min_size_hint() const
{
    if (!m_text.empty())
    {
        auto s = text_size(m_text);
        s += Widget::min_size_hint();
        return s;
    }

    return DEFAULT_LABEL_SIZE;
}

void Label::first_resize()
{
    if (box().size().empty())
    {
        resize(min_size_hint());
    }
}

ImageLabel::ImageLabel(const std::string& text,
                       alignmask text_align) noexcept
    : ImageLabel(Image(), text, {}, text_align)
{
}

ImageLabel::ImageLabel(const Image& image,
                       const std::string& text,
                       alignmask text_align) noexcept
    : ImageLabel(image, text, {}, text_align)
{
}

ImageLabel::ImageLabel(const Image& image,
                       const std::string& text,
                       const Rect& rect,
                       alignmask text_align) noexcept
    : Label(text, rect, text_align),
      m_image(image)
{
    set_name("ImageLabel" + std::to_string(m_widgetid));

    set_padding(0);

    if (text.empty())
        set_image_align(alignmask::center);
    do_set_image(image);
}

ImageLabel::ImageLabel(Frame& parent,
                       const Image& image,
                       const std::string& text,
                       alignmask text_align) noexcept
    : ImageLabel(image, text, text_align)
{
    parent.add(*this);
}

ImageLabel::ImageLabel(Frame& parent,
                       const Image& image,
                       const std::string& text,
                       const Rect& rect,
                       alignmask text_align) noexcept
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
    ignoreparam(rect);

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
                              TextBox::flags_type({TextBox::flag::multiline, TextBox::flag::word_wrap}),
                              widget.text_align(),
                              justification::middle,
                              widget.color(Palette::ColorId::label_text).color(),
                              widget.image_align(),
                              widget.image());
        }
        else
        {
            detail::draw_text(painter,
                              widget.content_area(),
                              text,
                              widget.font(),
                              TextBox::flags_type({TextBox::flag::multiline, TextBox::flag::word_wrap}),
                              widget.text_align(),
                              justification::middle,
                              widget.color(Palette::ColorId::label_text).color());
        }
    }
    else if (!widget.image().empty())
    {
        const auto b = widget.content_area();
        const auto target = detail::align_algorithm(widget.m_image.size(),
                            b,
                            widget.m_image_align);

        const auto hs = static_cast<double>(target.width()) /
                        static_cast<double>(widget.image().size_orig().width());
        const auto vs = static_cast<double>(target.height()) /
                        static_cast<double>(widget.image().size_orig().height());
        widget.scale_image(hs, vs);

        painter.draw(target.point());
        painter.draw(widget.image());
    }
}

Size ImageLabel::min_size_hint() const
{
    // if we are expanding the image, don't use it for min size hint
    if ((m_image_align & alignmask::expand_horizontal) == alignmask::expand_horizontal ||
        (m_image_align & alignmask::expand_vertical) == alignmask::expand_vertical)
        return Widget::min_size_hint();

    if (!m_text.empty())
        return m_image.size() + Label::min_size_hint();

    return m_image.size() + Widget::min_size_hint();
}

void ImageLabel::do_set_image(const Image& image)
{
    if (size().empty() && !image.empty())
        resize(image.size());

    m_image = image;
    damage();
}

void ImageLabel::set_image(const Image& image)
{
    do_set_image(image);
}

void ImageLabel::set_show_label(bool value)
{
    if (detail::change_if_diff<>(m_show_label, value))
        damage();
}

void ImageLabel::first_resize()
{
    if (size().empty())
    {
        resize(m_image.size());
    }
}

}
}
