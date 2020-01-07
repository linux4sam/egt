/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/canvas.h"
#include "egt/font.h"
#include <cairo-ft.h>
#include <cassert>
#include <map>
#include <memory>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

namespace egt
{
inline namespace v1
{

const char* Font::DEFAULT_FACE = "Free Sans";
const Font::weightid Font::DEFAULT_WEIGHT = Font::weightid::normal;
const Font::fontsize Font::DEFAULT_SIZE = 18.f;
const Font::slantid Font::DEFAULT_SLANT = Font::slantid::normal;

static shared_cairo_scaled_font_t create_scaled_font(cairo_t* cr, const Font& font)
{
    std::unique_ptr<cairo_font_options_t, decltype(cairo_font_options_destroy)*>
    font_options(cairo_font_options_create(), cairo_font_options_destroy);
    cairo_get_font_options(cr, font_options.get());

    std::unique_ptr<FcPattern, decltype(FcPatternDestroy)*>
    pattern(FcPatternCreate(), FcPatternDestroy);
    if (!pattern)
        return nullptr;

    // NOLINTNEXTLINE
    FcPatternAddString(pattern.get(), FC_FAMILY, reinterpret_cast<const FcChar8*>(font.face().c_str()));
    FcPatternAddDouble(pattern.get(), FC_SIZE, font.size());

    int weight = FC_WEIGHT_NORMAL;
    switch (font.weight())
    {
    case Font::weightid::normal:
        break;
    case Font::weightid::bold:
        weight = FC_WEIGHT_BOLD;
        break;
    }
    FcPatternAddInteger(pattern.get(), FC_WEIGHT, weight);

    int slant = FC_SLANT_ROMAN;
    switch (font.slant())
    {
    case Font::slantid::normal:
        break;
    case Font::slantid::italic:
        slant = FC_SLANT_ITALIC;
        break;
    case Font::slantid::oblique:
        slant = FC_SLANT_OBLIQUE;
        break;
    }
    FcPatternAddInteger(pattern.get(), FC_SLANT, slant);

    FcConfigSubstitute(nullptr, pattern.get(), FcMatchPattern);
    cairo_ft_font_options_substitute(font_options.get(), pattern.get());
    FcDefaultSubstitute(pattern.get());
    FcResult result;
    std::unique_ptr<FcPattern, decltype(FcPatternDestroy)*>
    resolved(FcFontMatch(nullptr, pattern.get(), &result), FcPatternDestroy);
    if (!resolved)
        return nullptr;

    double pixel_size;
    FcPatternGetDouble(resolved.get(), FC_PIXEL_SIZE, 0, &pixel_size);

    std::unique_ptr<cairo_font_face_t, decltype(cairo_font_face_destroy)*>
    font_face(cairo_ft_font_face_create_for_pattern(resolved.get()),
              cairo_font_face_destroy);

    cairo_matrix_t font_matrix;
    cairo_matrix_init_identity(&font_matrix);

    cairo_matrix_scale(&font_matrix, pixel_size, pixel_size);
    cairo_matrix_t ctm;
    cairo_get_matrix(cr, &ctm);

    shared_cairo_scaled_font_t scaled_font(cairo_scaled_font_create(font_face.get(),
                                           &font_matrix,
                                           &ctm,
                                           font_options.get()),
                                           cairo_scaled_font_destroy);

    cairo_status_t status = cairo_scaled_font_status(scaled_font.get());
    if (status)
        return nullptr;

    return scaled_font;
}

Font::Font()
    : m_face(DEFAULT_FACE),
      m_size(DEFAULT_SIZE),
      m_weight(DEFAULT_WEIGHT),
      m_slant(DEFAULT_SLANT)
{}

Font::Font(const std::string& face)
    : m_face(face),
      m_size(DEFAULT_SIZE),
      m_weight(DEFAULT_WEIGHT),
      m_slant(DEFAULT_SLANT)
{}

Font::Font(const std::string& face, fontsize size)
    : m_face(face),
      m_size(size),
      m_weight(DEFAULT_WEIGHT),
      m_slant(DEFAULT_SLANT)
{}

Font::Font(const std::string& face, fontsize size, weightid weight, slantid slant)
    : m_face(face),
      m_size(size),
      m_weight(weight),
      m_slant(slant)
{}

Font::Font(fontsize size)
    : m_face(DEFAULT_FACE),
      m_size(size),
      m_weight(DEFAULT_WEIGHT),
      m_slant(DEFAULT_SLANT)
{}

Font::Font(fontsize size, weightid weight)
    : m_face(DEFAULT_FACE),
      m_size(size),
      m_weight(weight),
      m_slant(DEFAULT_SLANT)
{}

Font::Font(weightid weight)
    : m_face(DEFAULT_FACE),
      m_size(DEFAULT_SIZE),
      m_weight(weight),
      m_slant(DEFAULT_SLANT)
{}

Font::Font(slantid slant)
    : m_face(DEFAULT_FACE),
      m_size(DEFAULT_SIZE),
      m_weight(DEFAULT_WEIGHT),
      m_slant(slant)
{}

struct FontCache : detail::noncopyable
{
    struct FontCompare
    {
        bool operator()(const Font& lhs, const Font& rhs) const
        {
            if (lhs.face() != rhs.face())
                return lhs.face() < rhs.face();
            if (!detail::float_compare(lhs.size(), rhs.size()))
                return lhs.size() < rhs.size();
            if (lhs.weight() != rhs.weight())
                return lhs.weight() < rhs.weight();
            return lhs.slant() < rhs.slant();
        }
    };

    std::map<Font, shared_cairo_scaled_font_t, FontCompare> cache;

    shared_cairo_scaled_font_t scaled_font(const Font& font)
    {
        auto i = cache.find(font);
        if (i != cache.end())
            return i->second;

        SPDLOG_TRACE("creating scaled font {}", font);

        Canvas canvas(Size(100, 100));
        auto cr = canvas.context();
        auto scaled_font = create_scaled_font(cr.get(), font);
        if (scaled_font)
            cache.insert(std::make_pair(font, scaled_font));
        return scaled_font;
    }
};

static FontCache font_cache;

cairo_scaled_font_t* Font::scaled_font() const
{
    return font_cache.scaled_font(*this).get();
}

std::ostream& operator<<(std::ostream& os, const Font& font)
{
    os << font.face() << ", " << font.size() << ", " <<
       static_cast<int>(font.weight()) << ", " << static_cast<int>(font.slant());
    return os;
}

}
}
