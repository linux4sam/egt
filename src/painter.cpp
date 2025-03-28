/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/cairoabstraction.h"
#include "detail/gpu.h"
#include "detail/painter.h"
#include "egt/app.h"
#include "egt/detail/enum.h"
#include "egt/image.h"
#include "egt/painter.h"
#include "egt/surface.h"
#include <cairo.h>
#include <sstream>
#include <string.h>

namespace egt
{
inline namespace v1
{
namespace detail
{

Painter& dummy_painter()
{
    static uint32_t data;
    static Surface target(&data, nullptr, Size(1, 1), PixelFormat::argb8888, sizeof(data));
    static Painter painter(target);

    return painter;
}

}

#ifdef HAVE_LIBM2D
#define gpu_painter() m_cr->gpu_painter()

static inline bool gpu_enabled()
{
    return Application::instance().gpu_enabled();
}
#endif

Painter::Painter(Surface& surface) noexcept
    : m_surface(surface)
{
    m_cr = std::make_unique<detail::InternalContext>(*this, cairo_create(surface.impl()));
}

Painter::~Painter()
{
    m_surface.flush();
}

void Painter::save()
{
    cairo_save(*m_cr);
}

void Painter::restore()
{
    cairo_restore(*m_cr);
}

void Painter::low_fidelity()
{
    // font
    cairo_font_options_t* cfo = cairo_font_options_create();
    cairo_font_options_set_antialias(cfo, CAIRO_ANTIALIAS_FAST);
    cairo_font_options_set_hint_style(cfo, CAIRO_HINT_STYLE_NONE);
    cairo_set_font_options(*m_cr, cfo);
    cairo_font_options_destroy(cfo);

    // shapes
    cairo_set_antialias(*m_cr, CAIRO_ANTIALIAS_FAST);
}

void Painter::high_fidelity()
{
    // font
    cairo_font_options_t* cfo = cairo_font_options_create();
    cairo_font_options_set_antialias(cfo, CAIRO_ANTIALIAS_GOOD);
    cairo_font_options_set_hint_style(cfo, CAIRO_HINT_STYLE_MEDIUM);
    cairo_set_font_options(*m_cr, cfo);
    cairo_font_options_destroy(cfo);

    // shapes
    cairo_set_antialias(*m_cr, CAIRO_ANTIALIAS_GOOD);
}

bool Painter::filter_subordinate(const Widget& subordinate) const
{
    if (m_subordinate_filter)
        return m_subordinate_filter(subordinate);

    return false;
}

Painter::SubordinateFilter Painter::set_subordinate_filter(const SubordinateFilter& subordinate_filter)
{
    auto previous = m_subordinate_filter;
    m_subordinate_filter = subordinate_filter;
    return previous;
}

void Painter::restore_subordinate_filter(SubordinateFilter&& subordinate_filter)
{
    m_subordinate_filter = std::move(subordinate_filter);
}

void Painter::push_group()
{
    cairo_push_group(*m_cr);
}

void Painter::pop_group()
{
    cairo_pop_group_to_source(*m_cr);
}

Painter& Painter::set(const Pattern& pattern)
{
    cairo_set_source(*m_cr, pattern.pattern());
    return *this;
}

Painter& Painter::set(const Font& font)
{
    cairo_set_scaled_font(*m_cr, font.scaled_font());
    return *this;
}

Painter& Painter::line_width(float width)
{
    cairo_set_line_width(*m_cr, width);

    return *this;
}

Painter& Painter::line_cap(Painter::LineCap value)
{
    cairo_set_line_cap(*m_cr, detail::cairo_line_cap(value));
    return *this;
}

Painter::LineCap Painter::line_cap() const
{
    return detail::egt_line_cap(cairo_get_line_cap(*m_cr));
}

Painter& Painter::set_dash(const double* dashes, size_t num_dashes, double offset)
{
    cairo_set_dash(*m_cr, dashes, num_dashes, offset);
    return *this;
}

Painter& Painter::antialias(Painter::AntiAlias value)
{
    cairo_set_antialias(*m_cr, detail::cairo_antialias(value));
    return *this;
}

Painter::AntiAlias Painter::antialias() const
{
    return detail::egt_antialias(cairo_get_antialias(*m_cr));
}

Painter& Painter::alpha_blending(bool enabled)
{
    if (enabled)
        cairo_set_operator(*m_cr, CAIRO_OPERATOR_OVER);
    else
        cairo_set_operator(*m_cr, CAIRO_OPERATOR_SOURCE);

    return *this;
}

bool Painter::alpha_blending() const
{
    return cairo_get_operator(*m_cr) != CAIRO_OPERATOR_SOURCE;
}

Painter& Painter::draw(const Image& image)
{
    assert(!image.empty());
    if (image.empty())
        return *this;

    if (!cairo_has_current_point(*m_cr))
        return *this;

    double x;
    double y;
    cairo_get_current_point(*m_cr, &x, &y);

    return draw(image, Point(x, y));
}

Painter& Painter::source(const Pattern& pattern)
{
    return set(pattern);
}

Painter& Painter::source(const Color& color)
{
    cairo_set_source_rgba(*m_cr,
                          color.redf(),
                          color.greenf(),
                          color.bluef(),
                          color.alphaf());
    return *this;
}

Painter& Painter::source(const Surface& surface, const PointF& point)
{
    cairo_set_source_surface(*m_cr, surface.impl(), point.x(), point.y());
#ifdef HAVE_LIBM2D
    gpu_painter().source(surface, point);
#endif
    return *this;
}

Painter& Painter::source(const Image& image, const PointF& point)
{
    return source(*image.surface(), point);
}

Painter& Painter::mask(const Surface& surface, const PointF& point)
{
#ifdef HAVE_LIBM2D
    gpu_painter().sync_for_cpu();
#endif
    cairo_mask_surface(*m_cr, surface.impl(), point.x(), point.y());
    return *this;
}

Painter& Painter::mask(const Image& image, const Point& point)
{
    return mask(*image.surface(), point);
}

Painter& Painter::draw(const Surface& surface, const PointF& point, const RectF& rect)
{
    if (surface.empty())
        return *this;

#ifdef HAVE_LIBM2D
    if (gpu_enabled() && gpu_painter().draw(surface, point, rect))
        return *this;
#endif

    source(surface, point);
    if (rect.empty())
    {
        paint();
    }
    else
    {
        draw(rect);
        fill();
    }
    return *this;
}

Painter& Painter::draw(const Image& image, const PointF& point, const RectF& rect)
{
    return draw(*image.surface(), point, rect);
}

Painter& Painter::draw(const std::string& str, const TextDrawFlags& flags)
{
    if (str.empty())
        return *this;

    if (!cairo_has_current_point(*m_cr))
        return *this;

#ifdef HAVE_LIBM2D
    gpu_painter().sync_for_cpu();
#endif

    double x;
    double y;
    cairo_text_extents_t textext;
    cairo_text_extents(*m_cr, str.c_str(), &textext);

    cairo_get_current_point(*m_cr, &x, &y);

    if (flags.is_set(TextDrawFlag::shadow))
    {
        AutoSaveRestore sr(*this);

        cairo_move_to(*m_cr, x - textext.x_bearing + 5.,
                      y - textext.y_bearing + 5.);
        cairo_set_source_rgba(*m_cr, 0, 0, 0, 0.2);
        cairo_show_text(*m_cr, str.c_str());
        cairo_stroke(*m_cr);
    }

    AutoSaveRestore sr(*this);

    cairo_move_to(*m_cr, x - textext.x_bearing,
                  y - textext.y_bearing);
    cairo_show_text(*m_cr, str.c_str());
    cairo_stroke(*m_cr);

    return *this;
}

Font::FontExtents Painter::extents() const
{
    cairo_font_extents_t fe;
    cairo_font_extents(*m_cr, &fe);

    Font::FontExtents font_extents;
    font_extents.ascent = fe.ascent;
    font_extents.descent = fe.descent;
    font_extents.height = fe.height;
    font_extents.max_x_advance = fe.max_x_advance;
    font_extents.max_y_advance = fe.max_y_advance;
    return font_extents;
}

Font::TextExtents Painter::extents(const std::string& text) const
{
    cairo_text_extents_t te;
    cairo_text_extents(*m_cr, text.c_str(), &te);

    Font::TextExtents text_extents;
    text_extents.x_bearing = te.x_bearing;
    text_extents.y_bearing = te.y_bearing;
    text_extents.width = te.width;
    text_extents.height = te.height;
    text_extents.x_advance = te.x_advance;
    text_extents.y_advance = te.y_advance;
    return text_extents;
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
    cairo_font_extents(*m_cr, &fontext);
    const double line_recommended_height = fontext.height;

    unsigned int n = 0;
    double line_max_width = 0;

    char* lines = strdup(text.c_str());
    char* line = lines;
    while (line)
    {
        char* eol = strchr(line, '\n');
        char* next = nullptr;

        if (eol)
        {
            *eol = '\0';
            next = eol + 1;
        }

        cairo_text_extents_t textext;
        cairo_text_extents(*m_cr, line, &textext);
        line_max_width = std::max(line_max_width, textext.width);
        ++n;

        line = next;
    }
    free(lines);

    return {static_cast<Size::DimType>(std::floor(line_max_width + 1.0)),
            static_cast<Size::DimType>(std::floor(n * line_recommended_height + 1.0))};
}

Painter& Painter::clip()
{
    cairo_clip(*m_cr);

    return *this;
}

Painter& Painter::fill()
{
#ifdef HAVE_LIBM2D
    if (gpu_enabled() && gpu_painter().fill())
    {
        cairo_new_path(*m_cr);
        return *this;
    }

    gpu_painter().sync_for_cpu();
#endif
    cairo_fill(*m_cr);

    return *this;
}

Painter& Painter::fill_preserve()
{
#ifdef HAVE_LIBM2D
    if (gpu_enabled() && gpu_painter().fill())
        return *this;

    gpu_painter().sync_for_cpu();
#endif
    cairo_fill_preserve(*m_cr);

    return *this;
}

Painter& Painter::paint()
{
#ifdef HAVE_LIBM2D
    if (gpu_enabled() && gpu_painter().paint())
        return *this;

    gpu_painter().sync_for_cpu();
#endif
    cairo_paint(*m_cr);

    return *this;
}

Painter& Painter::paint(float alpha)
{
#ifdef HAVE_LIBM2D
    gpu_painter().sync_for_cpu();
#endif
    cairo_paint_with_alpha(*m_cr, alpha);

    return *this;
}

Painter& Painter::stroke()
{
#ifdef HAVE_LIBM2D
    gpu_painter().sync_for_cpu();
#endif
    cairo_stroke(*m_cr);

    return *this;
}

Painter& Painter::move_to(const PointF& point)
{
    cairo_move_to(*m_cr, point.x(), point.y());
    return *this;
}

Painter& Painter::line_to(const PointF& point)
{
    cairo_line_to(*m_cr, point.x(), point.y());
    return *this;
}

Painter& Painter::rectangle(const RectF& rect)
{
    if (!rect.empty())
        cairo_rectangle(*m_cr, rect.x(), rect.y(), rect.width(), rect.height());
    return *this;
}

Painter& Painter::arc(const ArcF& arc)
{
    if (!arc.empty())
        cairo_arc(*m_cr, arc.center().x(), arc.center().y(),
                  arc.radius(), arc.angle1(), arc.angle2());
    return *this;
}

Painter& Painter::translate(const PointF& point)
{
    if (!detail::float_equal(point.x(), 0) ||
        !detail::float_equal(point.y(), 0))
        cairo_translate(*m_cr, point.x(), point.y());

    return *this;
}

Painter& Painter::translate(const Point& point)
{
    if (point.x() || point.y())
        cairo_translate(*m_cr, point.x(), point.y());

    return *this;
}

Painter& Painter::scale(float sx, float sy)
{
    cairo_scale(*m_cr, sx, sy);
    return *this;
}

Painter& Painter::rotate(float angle)
{
    if (!detail::float_equal(angle, 0))
        cairo_rotate(*m_cr, angle);

    return *this;
}

Painter& Painter::show_text(const char* utf8)
{
    cairo_show_text(*m_cr, utf8);
    return *this;
}

void Painter::color_at(const Point& point, const Color& color) noexcept
{
    m_surface.color_at(point, color);
}

Color Painter::color_at(const Point& point) noexcept
{
    return m_surface.color_at(point);
}

Painter& Painter::flood(const Point& point, const Color& color)
{
    m_surface.flood(point, color);
    return *this;
}

void Painter::sync_for_cpu([[maybe_unused]] bool skip_source) const
{
#ifdef HAVE_LIBM2D
    gpu_painter().sync_for_cpu(skip_source);
#endif
}

template<>
const std::pair<Painter::LineCap, char const*> detail::EnumStrings<Painter::LineCap>::data[] =
{
    {Painter::LineCap::butt, "butt"},
    {Painter::LineCap::round, "round"},
    {Painter::LineCap::square, "square"},
};

std::ostream& operator<<(std::ostream& os, const Painter::LineCap& cap)
{
    return os << detail::enum_to_string(cap);
}

template<>
const std::pair<Painter::AntiAlias, char const*> detail::EnumStrings<Painter::AntiAlias>::data[] =
{
    {Painter::AntiAlias::system, "system"},
    {Painter::AntiAlias::none, "none"},
    {Painter::AntiAlias::gray, "gray"},
    {Painter::AntiAlias::subpixel, "subpixel"},
    {Painter::AntiAlias::fast, "fast"},
    {Painter::AntiAlias::good, "good"},
    {Painter::AntiAlias::best, "best"},
};

std::ostream& operator<<(std::ostream& os, const Painter::AntiAlias& antialias)
{
    return os << detail::enum_to_string(antialias);
}

}
}
