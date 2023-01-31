/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/image.h"
#include "egt/painter.h"
#include <cairo.h>
#include <deque>
#include <sstream>

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

Painter& Painter::set(const Pattern& pattern)
{
    cairo_set_source(m_cr.get(), pattern.pattern());
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

    double x;
    double y;
    cairo_get_current_point(m_cr.get(), &x, &y);

    cairo_translate(m_cr.get(), x, y);
    cairo_set_source(m_cr.get(), image.pattern());

    /// @todo no paint here
    paint();

    return *this;
}

Painter& Painter::mask(const Image& image, const Point& point)
{
    cairo_mask_surface(m_cr.get(), image.surface().get(), point.x(), point.y());

    return *this;
}

Painter& Painter::draw(const Rect& rect, const Image& image)
{
    double x;
    double y;

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

    double x;
    double y;
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
    /*
     * cairo_text_extents deals with glyphs and doesn't handle multilines
     * strings so the text size has to be computed manually.
     * The line recommended height is used here as adding the line height
     * returned by the text extents won't work. getline removes the line return
     * sequence whose glyph is bigger than letters.
     */
    cairo_font_extents_t fontext;
    cairo_font_extents(m_cr.get(), &fontext);
    const double line_recommended_height = fontext.height;

    std::istringstream lines(text);
    std::string line;
    unsigned int n = 0;
    double line_max_width = 0;
    while (std::getline(lines, line))
    {
        cairo_text_extents_t textext;
        cairo_text_extents(m_cr.get(), line.c_str(), &textext);
        line_max_width = std::max(line_max_width, textext.width);
        ++n;
    }

    return {static_cast<Size::DimType>(std::floor(line_max_width + 1.0)),
            static_cast<Size::DimType>(std::floor(n * line_recommended_height + 1.0))};
}

Size Painter::font_size(const std::string& text)
{
    cairo_font_extents_t fe;
    cairo_font_extents(m_cr.get(), &fe);
    return {text_size(text).width(),
            static_cast<Size::DimType>(std::floor(fe.height + 1.0))};
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

static inline Color getc(cairo_surface_t* image, const Point& point) noexcept
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

static inline void set32(unsigned char* data, size_t stride, const Point& point, const Color& color) noexcept
{
    *reinterpret_cast<uint32_t*>(data + point.y() * stride + 4 * point.x()) = color.pixel32();
}

static inline void set24(unsigned char* data, size_t stride, const Point& point, const Color& color) noexcept
{
    *reinterpret_cast<uint32_t*>(data + point.y() * stride + 4 * point.x()) = color.pixel24();
}

static inline void set16(unsigned char* data, size_t stride, const Point& point, const Color& color) noexcept
{
    *reinterpret_cast<uint16_t*>(data + point.y() * stride + 2 * point.x()) = color.pixel16();
}

static inline void setc(cairo_surface_t* image, const Point& point, const Color& color) noexcept
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

void Painter::color_at(cairo_surface_t* image, const Point& point, const Color& color) noexcept
{
    const auto size = surface_to_size(image);
    if (!Rect(Point(), size - Size(1, 1)).intersect(point))
        return;

    cairo_surface_flush(image);

    setc(image, point, color);

    cairo_surface_mark_dirty(image);
}

void Painter::color_at(const Point& point, const Color& color) noexcept
{
    color_at(cairo_get_target(m_cr.get()), point, color);
}

Color Painter::color_at(const Point& point) noexcept
{
    return color_at(cairo_get_target(m_cr.get()), point);
}

Color Painter::color_at(cairo_surface_t* image, const Point& point) noexcept
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
                         const Color& target_color, const Color& color) noexcept
{
    if (!Rect(Point(), size - Size(1, 1)).intersect(point))
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
    q.emplace_back(point);

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
            q.emplace_back(p + Point(1, 0));
        if (check(size, format, data, stride, p - Point(1, 0), target_color, color))
            q.emplace_back(p - Point(1, 0));
        if (check(size, format, data, stride, p + Point(0, 1), target_color, color))
            q.emplace_back(p + Point(0, 1));
        if (check(size, format, data, stride, p - Point(0, 1), target_color, color))
            q.emplace_back(p - Point(0, 1));
    }

    cairo_surface_mark_dirty(image);
}

}
}
