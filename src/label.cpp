/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/frame.h"
#include "egt/imagecache.h"
#include "egt/label.h"
#include "egt/painter.h"

using namespace std;

namespace egt
{
inline namespace v1
{
static const auto DEFAULT_LABEL_SIZE = Size(100, 50);

template<>
Drawable<Label>::draw_t Drawer<Label>::m_drawable = Label::default_draw;

Label::Label(const std::string& text, const Rect& rect,
             alignmask align, const Font& font, widgetmask flags) noexcept
    : TextWidget(text, rect, align, font, flags)
{
    set_boxtype(Theme::boxtype::fill);
}

Label::Label(Frame& parent, const std::string& text, const Rect& rect,
             alignmask align, const Font& font, widgetmask flags) noexcept
    : Label(text, rect, align, font, flags)
{
    parent.add(this);
}

Label::Label(Frame& parent, const std::string& text,
             alignmask align, const Font& font, widgetmask flags) noexcept
    : Label(parent, text, Rect(), align, font, flags)
{

}

void Label::set_text(const std::string& str)
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
    Drawer<Label>::draw(*this, painter, rect);
}

void Label::default_draw(Label& widget, Painter& painter, const Rect& rect)
{
    ignoreparam(rect);

    widget.draw_box(painter);

    painter.set_color(widget.palette().color(Palette::TEXT,
                      widget.active() ? Palette::GROUP_ACTIVE : Palette::GROUP_NORMAL));
    painter.set_font(widget.font());
    painter.draw_text(widget.box(), widget.text(), widget.text_align(), 5);
}

void Label::set_parent(Frame* parent)
{
    TextWidget::set_parent(parent);
    first_resize();
}

void Label::first_resize()
{
    if (box().size().empty())
    {
        if (!m_text.empty())
        {
            auto s = text_size();
            s += Size(5, 5);
            resize(s);
        }
        else
        {
            resize(DEFAULT_LABEL_SIZE);
        }
    }
}

Label::~Label()
{}

template<>
Drawable<ImageLabel>::draw_t Drawer<ImageLabel>::m_drawable = ImageLabel::default_draw;

ImageLabel::ImageLabel(const Image& image,
                       const std::string& text,
                       const Rect& rect,
                       const Font& font)
    : Label(text, rect, alignmask::RIGHT | alignmask::CENTER, font),
      m_image(image)
{
    set_boxtype(Theme::boxtype::none);

    if (rect.empty())
        m_box.size(image.size());

    if (text.empty())
        set_image_align(alignmask::CENTER);
}

ImageLabel::ImageLabel(const Image& image,
                       const Point& point)
    : ImageLabel(image, "", Rect(point, Size(0, 0)), Font())
{
}

ImageLabel::ImageLabel(const Image& image,
                       const std::string& text,
                       const Point& point,
                       const Font& font)
    : ImageLabel(image, text, Rect(point, Size(0, 0)), font)
{
}

ImageLabel::ImageLabel(Frame& parent,
                       const Image& image,
                       const std::string& text,
                       const Rect& rect,
                       const Font& font)
    : ImageLabel(image, text, rect, font)
{
    parent.add(this);
}

void ImageLabel::scale_image(double hscale, double vscale,
                             bool approximate)
{
    m_image.scale(hscale, vscale, approximate);
    m_box = Rect(m_box.point(), m_image.size());
}

void ImageLabel::scale_image(double s, bool approximate)
{
    scale_image(s, s, approximate);
}

#if 0
void ImageLabel::resize(const Size& size) override
{
    if (m_text.empty())
    {
        if (this->size() != size)
        {
            double hs = (double)size.w / (double)m_image.size_orig().w;
            double vs = (double)size.h / (double)m_image.size_orig().h;
            scale_image(hs, vs);
        }
    }
    else
    {
        Widget::resize(size);
    }
}
#endif

void ImageLabel::draw(Painter& painter, const Rect& rect)
{
    Drawer<ImageLabel>::draw(*this, painter, rect);
}

void ImageLabel::default_draw(ImageLabel& widget, Painter& painter, const Rect& rect)
{
    ignoreparam(rect);

    widget.draw_box(painter);

    if (!widget.text().empty())
    {
        auto text_size = widget.text_size();

        Rect tbox;
        Rect ibox;

        if (widget.m_position_image_first)
            Widget::double_align(widget.box(),
                                 widget.m_image.size(), widget.m_image_align, ibox,
                                 text_size, widget.m_text_align, tbox, 5);
        else
            Widget::double_align(widget.box(),
                                 text_size, widget.text_align(), tbox,
                                 widget.m_image.size(), widget.m_image_align, ibox, 5);

        painter.draw_image(ibox.point(), widget.m_image);

        if (widget.m_show_label)
        {
            painter.draw_text(widget.m_text, tbox, widget.palette().color(Palette::TEXT),
                              alignmask::CENTER, 0, widget.font());
        }
    }
    else
    {
        Rect target = Widget::align_algorithm(widget.m_image.size(),
                                              widget.box(), widget.m_image_align, 0);
        painter.draw_image(target.point(), widget.m_image);
    }
}

void ImageLabel::set_image(const Image& image)
{
    if (!image.empty())
    {
        m_image = image;
#if 0
        auto width = cairo_image_surface_get_width(m_image.get());
        auto height = cairo_image_surface_get_height(m_image.get());
        m_box.w = width;
        m_box.h = height;
#endif
        damage();
    }
}

void ImageLabel::label_enabled(bool value)
{
    if (m_show_label != value)
    {
        m_show_label = value;
        damage();
    }
}

ImageLabel::~ImageLabel()
{
}

void ImageLabel::first_resize()
{
    if (box().size().empty())
    {
        if (!m_text.empty())
        {
            auto text_size = this->text_size();

            Rect tbox;
            Rect ibox;

            if (m_position_image_first)
                Widget::double_align(box(),
                                     m_image.size(), m_image_align, ibox,
                                     text_size, m_text_align, tbox, 5);
            else
                Widget::double_align(box(),
                                     text_size, m_text_align, tbox,
                                     m_image.size(), m_image_align, ibox, 5);

            auto s = Rect::merge(tbox, ibox);
            resize(s.size() + Size(10, 10));
        }
        else
        {
            resize(m_image.size());
        }
    }
}

}
}
