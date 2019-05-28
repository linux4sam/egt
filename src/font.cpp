/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/font.h"
#include "egt/utils.h"
#include <cairo-ft.h>
#include <cassert>
#include <map>

namespace egt
{
inline namespace v1
{

const char* Font::DEFAULT_FACE = "Free Sans";
const Font::weightid Font::DEFAULT_WEIGHT = Font::weightid::normal;
const int Font::DEFAULT_SIZE = 18;
const Font::slantid Font::DEFAULT_SLANT = Font::slantid::normal;

// more ideas: https://searchfox.org/mozilla-central/source/gfx/2d/ScaledFontFontconfig.cpp

static cairo_status_t create_scaled_font(cairo_t* cr,
        cairo_scaled_font_t** out,
        const Font& font)
{
    FcPattern* pattern, *resolved;
    FcResult result;
    cairo_font_face_t* font_face;
    cairo_scaled_font_t* scaled_font;
    cairo_font_options_t* font_options;
    cairo_matrix_t font_matrix, ctm;
    cairo_status_t status;
    double pixel_size;
    font_options = cairo_font_options_create();
    cairo_get_font_options(cr, font_options);
    pattern = FcPatternCreate();
    if (!pattern)
        return CAIRO_STATUS_NO_MEMORY;

    FcPatternAddString(pattern, FC_FAMILY, (FcChar8*) font.face().c_str());
    FcPatternAddDouble(pattern, FC_SIZE, font.size());

    int weight = FC_WEIGHT_NORMAL;
    switch (font.weight())
    {
    case Font::weightid::normal:
        break;
    case Font::weightid::bold:
        weight = FC_WEIGHT_BOLD;
        break;
    }
    FcPatternAddInteger(pattern, FC_WEIGHT, weight);

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
    FcPatternAddInteger(pattern, FC_SLANT, slant);

    FcConfigSubstitute(nullptr, pattern, FcMatchPattern);
    cairo_ft_font_options_substitute(font_options, pattern);
    FcDefaultSubstitute(pattern);
    resolved = FcFontMatch(nullptr, pattern, &result);
    if (!resolved)
    {
        FcPatternDestroy(pattern);
        return CAIRO_STATUS_NO_MEMORY;
    }
    FcPatternGetDouble(resolved, FC_PIXEL_SIZE, 0, &pixel_size);
    font_face = cairo_ft_font_face_create_for_pattern(resolved);
    cairo_matrix_init_identity(&font_matrix);
    cairo_matrix_scale(&font_matrix, pixel_size, pixel_size);
    cairo_get_matrix(cr, &ctm);
    scaled_font = cairo_scaled_font_create(font_face,
                                           &font_matrix,
                                           &ctm,
                                           font_options);
    cairo_font_options_destroy(font_options);
    cairo_font_face_destroy(font_face);
    FcPatternDestroy(pattern);
    FcPatternDestroy(resolved);
    status = cairo_scaled_font_status(scaled_font);
    if (status)
    {
        cairo_scaled_font_destroy(scaled_font);
        return status;
    }
    *out = scaled_font;
    return CAIRO_STATUS_SUCCESS;
}

Font::Font()
    : m_face(DEFAULT_FACE),
      m_size(DEFAULT_SIZE),
      m_weight(DEFAULT_WEIGHT),
      m_slant(DEFAULT_SLANT)
{}

Font::Font(const std::string& face, int size)
    : m_face(face),
      m_size(size),
      m_weight(DEFAULT_WEIGHT),
      m_slant(DEFAULT_SLANT)
{}

Font::Font(const std::string& face, int size, weightid weight, slantid slant)
    : m_face(face),
      m_size(size),
      m_weight(weight),
      m_slant(slant)
{}

Font::Font(int size)
    : m_face(DEFAULT_FACE),
      m_size(size),
      m_weight(DEFAULT_WEIGHT),
      m_slant(DEFAULT_SLANT)
{}

Font::Font(int size, weightid weight)
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

struct FontCache
{
    struct FontCompare
    {
        bool operator()(const Font& lhs, const Font& rhs) const
        {
            return lhs.face() < rhs.face() &&
                   lhs.size() < rhs.size() &&
                   lhs.weight() < rhs.weight() &&
                   lhs.slant() < rhs.slant();
        }
    };

    std::map<Font, shared_cairo_scaled_font_t, FontCompare> cache;

    shared_cairo_scaled_font_t scaled_font(const Font& font)
    {
        auto i = cache.find(font);
        if (i != cache.end())
            return i->second;

        DBG("creating scaled font " << font);

        auto cr = shared_cairo_t(cairo_create(nullptr), cairo_destroy);
        cairo_scaled_font_t* out = nullptr;
        cairo_status_t status = create_scaled_font(cr.get(), &out, font);

        assert(!status && out);
        if (status || !out)
            return nullptr;

        auto scaled_font = shared_cairo_scaled_font_t(out, cairo_scaled_font_destroy);
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
