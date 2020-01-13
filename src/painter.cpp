/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/image.h"
#include "egt/painter.h"
#include <cairo.h>
#include <deque>

namespace egt
{
inline namespace v1
{

Painter::Painter(shared_cairo_t cr) noexcept
    : m_cr(std::move(cr))
{
}

void Painter::save()
{
    cairo_save(m_cr.get());
}

void Painter::restore()
{
    cairo_restore(m_cr.get());
}

void Painter::push_group()
{
    cairo_push_group(m_cr.get());
}

void Painter::pop_group()
{
    cairo_pop_group_to_source(m_cr.get());
}

Painter& Painter::set(const Color& color)
{
    cairo_set_source_rgba(m_cr.get(),
                          color.redf(),
                          color.greenf(),
                          color.bluef(),
                          color.alphaf());

    return *this;
}

Painter& Painter::set(const Font& font)
{
    cairo_set_scaled_font(m_cr.get(), font.scaled_font());

    return *this;
}

Painter& Painter::line_width(float width)
{
    cairo_set_line_width(m_cr.get(), width);

    return *this;
}

Painter& Painter::draw(const Image& image)
{
    assert(!image.empty());
    if (image.empty())
        return *this;

    AutoSaveRestore sr(*this);

    if (!cairo_has_current_point(m_cr.get()))
        return *this;

    double x, y;
    cairo_get_current_point(m_cr.get(), &x, &y);
    cairo_set_source_surface(m_cr.get(), image.surface().get(), x, y);
    cairo_rectangle(m_cr.get(), x, y, image.size().width(), image.size().height());

    /// @todo no fill here
    fill();

    return *this;
}

Painter& Painter::draw(const Rect& rect, const Image& image)
{
    double x, y;

    assert(!image.empty());
    if (image.empty())
        return *this;

    if (!cairo_has_current_point(m_cr.get()))
        return *this;

    cairo_get_current_point(m_cr.get(), &x, &y);
    cairo_set_source_surface(m_cr.get(), image.surface().get(),
                             x - rect.x(), y - rect.y());
    cairo_rectangle(m_cr.get(), x, y, rect.width(), rect.height());

    /// @todo no fill here
    fill();

    return *this;
}

Painter& Painter::draw(const std::string& str, const TextDrawFlags& flags)
{
    if (str.empty())
        return *this;

    if (!cairo_has_current_point(m_cr.get()))
        return *this;

    double x, y;
    cairo_text_extents_t textext;
    cairo_text_extents(m_cr.get(), str.c_str(), &textext);

    cairo_get_current_point(m_cr.get(), &x, &y);

    if (flags.is_set(TextDrawFlag::shadow))
    {
        AutoSaveRestore sr(*this);

        cairo_move_to(m_cr.get(), x - textext.x_bearing + 5.,
                      y - textext.y_bearing + 5.);
        cairo_set_source_rgba(m_cr.get(), 0, 0, 0, 0.2);
        cairo_show_text(m_cr.get(), str.c_str());
        cairo_stroke(m_cr.get());
    }

    AutoSaveRestore sr(*this);

    cairo_move_to(m_cr.get(), x - textext.x_bearing,
                  y - textext.y_bearing);
    cairo_show_text(m_cr.get(), str.c_str());
    cairo_stroke(m_cr.get());

    return *this;
}

Size Painter::text_size(const std::string& text)
{
    cairo_text_extents_t textext;
    cairo_text_extents(m_cr.get(), text.c_str(), &textext);
    return {static_cast<Size::dim_type>(std::ceil(textext.width)),
            static_cast<Size::dim_type>(std::ceil(textext.height))};
}

Size Painter::font_size(const std::string& text)
{
    cairo_font_extents_t fe;
    cairo_font_extents(m_cr.get(), &fe);
    return {text_size(text).width(),
            static_cast<Size::dim_type>(std::ceil(fe.height))};
}

Painter& Painter::clip()
{
    cairo_clip(m_cr.get());

    return *this;
}

Painter& Painter::fill()
{
    cairo_fill(m_cr.get());

    return *this;
}

Painter& Painter::paint()
{
    cairo_paint(m_cr.get());

    return *this;
}

Painter& Painter::paint(float alpha)
{
    cairo_paint_with_alpha(m_cr.get(), alpha);

    return *this;
}

Painter& Painter::stroke()
{
    cairo_stroke(m_cr.get());

    return *this;
}

static inline Color get32(const unsigned char* data, size_t stride, const Point& point)
{
    return Color::pixel32(*reinterpret_cast<const uint32_t*>(data + point.y() * stride + 4 * point.x()));
}

static inline Color get24(const unsigned char* data, size_t stride, const Point& point)
{
    return Color::pixel24(*reinterpret_cast<const uint32_t*>(data + point.y() * stride + 4 * point.x()));
}

static inline Color get16(const unsigned char* data, size_t stride, const Point& point)
{
    return Color::pixel16(*reinterpret_cast<const uint16_t*>(data + point.y() * stride + 2 * point.x()));
}

static inline Color getc(cairo_surface_t* image, const Point& point)
{
    const auto data = cairo_image_surface_get_data(image);
    const auto format = cairo_image_surface_get_format(image);
    const auto stride = cairo_image_surface_get_stride(image);
    switch (format)
    {
    case CAIRO_FORMAT_ARGB32:
        return get32(data, stride, point);
    case CAIRO_FORMAT_RGB24:
        return get24(data, stride, point);
    case CAIRO_FORMAT_RGB16_565:
        return get16(data, stride, point);
    case CAIRO_FORMAT_RGB30:
    case CAIRO_FORMAT_A8:
    case CAIRO_FORMAT_A1:
    case CAIRO_FORMAT_INVALID:
    default:
        break;
    }

    return {};
}

static inline void set32(unsigned char* data, size_t stride, const Point& point, const Color& color)
{
    *reinterpret_cast<uint32_t*>(data + point.y() * stride + 4 * point.x()) = color.pixel32();
}

static inline void set24(unsigned char* data, size_t stride, const Point& point, const Color& color)
{
    *reinterpret_cast<uint32_t*>(data + point.y() * stride + 4 * point.x()) = color.pixel24();
}

static inline void set16(unsigned char* data, size_t stride, const Point& point, const Color& color)
{
    *reinterpret_cast<uint16_t*>(data + point.y() * stride + 2 * point.x()) = color.pixel16();
}

static inline void setc(cairo_surface_t* image, const Point& point, const Color& color)
{
    auto data = cairo_image_surface_get_data(image);
    const auto stride = cairo_image_surface_get_stride(image);
    const auto format = cairo_image_surface_get_format(image);
    switch (format)
    {
    case CAIRO_FORMAT_ARGB32:
        set32(data, stride, point, color);
        break;
    case CAIRO_FORMAT_RGB24:
        set24(data, stride, point, color);
        break;
    case CAIRO_FORMAT_RGB16_565:
        set16(data, stride, point, color);
        break;
    case CAIRO_FORMAT_RGB30:
    case CAIRO_FORMAT_A8:
    case CAIRO_FORMAT_A1:
    case CAIRO_FORMAT_INVALID:
    default:
        break;
    }
}

void Painter::color_at(cairo_surface_t* image, const Point& point, const Color& color)
{
    const auto size = surface_to_size(image);
    if (!Rect(Point(), size - Size(1, 1)).intersect(point))
        return;

    cairo_surface_flush(image);

    setc(image, point, color);

    cairo_surface_mark_dirty(image);
}

void Painter::color_at(const Point& point, const Color& color)
{
    color_at(cairo_get_target(m_cr.get()), point, color);
}

Color Painter::color_at(const Point& point)
{
    return color_at(cairo_get_target(m_cr.get()), point);
}

Color Painter::color_at(cairo_surface_t* image, const Point& point)
{
    const auto size = surface_to_size(image);
    if (!Rect(Point(), size - Size(1, 1)).intersect(point))
        return {};

    cairo_surface_flush(image);

    return getc(image, point);
}

static inline bool check(const Size& size, cairo_format_t format,
                         const unsigned char* data,
                         size_t stride, const Point& point,
                         const Color& target_color, const Color& color)
{
    if (!Rect(Point(), size).intersect(point))
        return false;

    switch (format)
    {
    case CAIRO_FORMAT_ARGB32:
    {
        const auto c = get32(data, stride, point);
        return c != color && c == target_color;
    }
    case CAIRO_FORMAT_RGB24:
    {
        const auto c = get24(data, stride, point);
        return c != color && c == target_color;
    }
    case CAIRO_FORMAT_RGB16_565:
    {
        const auto c = get16(data, stride, point);
        return c != color && c == target_color;
    }
    case CAIRO_FORMAT_RGB30:
    case CAIRO_FORMAT_A8:
    case CAIRO_FORMAT_A1:
    case CAIRO_FORMAT_INVALID:
    default:
        break;
    }

    return false;
}

Painter& Painter::flood(const Point& point, const Color& color)
{
    flood(cairo_get_target(m_cr.get()), point, color);
    return *this;
}

void Painter::flood(cairo_surface_t* image,
                    const Point& point, const Color& color)
{
    auto size = surface_to_size(image);
    if (!Rect(Point(), size - Size(1, 1)).intersect(point))
        return;

    cairo_surface_flush(image);

    auto data = cairo_image_surface_get_data(image);
    const auto stride = cairo_image_surface_get_stride(image);
    const auto format = cairo_image_surface_get_format(image);
    const Color target_color = color_at(image, point);

    // non-recursive breadth first search

    std::deque<Point> q;
    q.push_back(point);

    while (!q.empty())
    {
        auto p = q.back();
        q.pop_back();

        switch (format)
        {
        case CAIRO_FORMAT_ARGB32:
            set32(data, stride, p, color);
            break;
        case CAIRO_FORMAT_RGB24:
            set24(data, stride, p, color);
            break;
        case CAIRO_FORMAT_RGB16_565:
            set16(data, stride, p, color);
            break;
        case CAIRO_FORMAT_RGB30:
        case CAIRO_FORMAT_A8:
        case CAIRO_FORMAT_A1:
        case CAIRO_FORMAT_INVALID:
        default:
            break;
        }

        if (check(size, format, data, stride, p + Point(1, 0), target_color, color))
            q.push_back(p + Point(1, 0));
        if (check(size, format, data, stride, p - Point(1, 0), target_color, color))
            q.push_back(p - Point(1, 0));
        if (check(size, format, data, stride, p + Point(0, 1), target_color, color))
            q.push_back(p + Point(0, 1));
        if (check(size, format, data, stride, p - Point(0, 1), target_color, color))
            q.push_back(p - Point(0, 1));
    }

    cairo_surface_mark_dirty(image);
}

}
}
