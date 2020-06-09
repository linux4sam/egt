/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/canvas.h"
#include "egt/detail/enum.h"
#include "egt/font.h"
#include "egt/respath.h"
#include "egt/serialize.h"
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

constexpr const char* Font::DEFAULT_FACE;
constexpr Font::Weight Font::DEFAULT_WEIGHT;
constexpr Font::Size Font::DEFAULT_SIZE;
constexpr Font::Slant Font::DEFAULT_SLANT;

static shared_cairo_scaled_font_t create_ft_scaled_font(cairo_t* cr, const char* path, const Font& font)
{
    SPDLOG_DEBUG("allocating font using FreeType: {}", font.face());

    FT_Library value;
    FT_Error status = FT_Init_FreeType(&value);
    if (status != 0)
    {
        spdlog::error("error initializing FreeType library");
        return nullptr;
    }

    FT_Face face;
    status = FT_New_Face(value, path, 0, &face);
    if (status != 0)
    {
        spdlog::error("error opening font {}", path);
        return nullptr;
    }

    std::unique_ptr<cairo_font_face_t, decltype(cairo_font_face_destroy)*>
    font_face(cairo_ft_font_face_create_for_ft_face(face, FT_LOAD_NO_HINTING | FT_LOAD_NO_BITMAP),
              cairo_font_face_destroy);

    static const cairo_user_data_key_t key{};
    auto s = cairo_font_face_set_user_data(font_face.get(), &key,
                                           face, reinterpret_cast<cairo_destroy_func_t>(FT_Done_Face));
    if (s)
    {
        spdlog::error("error creating cairo font face from FreeType font {}", font.face());
        return nullptr;
    }

    std::unique_ptr<cairo_font_options_t, decltype(cairo_font_options_destroy)*>
    font_options(cairo_font_options_create(), cairo_font_options_destroy);
    cairo_get_font_options(cr, font_options.get());
    cairo_font_options_set_hint_style(font_options.get(), CAIRO_HINT_STYLE_NONE);
    cairo_font_options_set_hint_metrics(font_options.get(), CAIRO_HINT_METRICS_OFF);

    cairo_matrix_t size_matrix;
    cairo_matrix_t identity_matrix;
    cairo_matrix_init_scale(&size_matrix, font.size(), font.size());
    cairo_matrix_init_identity(&identity_matrix);

    shared_cairo_scaled_font_t scaled_font(cairo_scaled_font_create(font_face.get(),
                                           &size_matrix,
                                           &identity_matrix,
                                           font_options.get()),
                                           cairo_scaled_font_destroy);

    s = cairo_scaled_font_status(scaled_font.get());
    if (s)
        return nullptr;

    return scaled_font;
}

#if CAIRO_HAS_FC_FONT
static shared_cairo_scaled_font_t create_scaled_font(cairo_t* cr, const Font& font)
{
    SPDLOG_DEBUG("allocating font using Fontconfig: {}", font.face());

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
    case Font::Weight::normal:
        break;
    case Font::Weight::bold:
        weight = FC_WEIGHT_BOLD;
        break;
    }
    FcPatternAddInteger(pattern.get(), FC_WEIGHT, weight);

    int slant = FC_SLANT_ROMAN;
    switch (font.slant())
    {
    case Font::Slant::normal:
        break;
    case Font::Slant::italic:
        slant = FC_SLANT_ITALIC;
        break;
    case Font::Slant::oblique:
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
#endif

// NOLINTNEXTLINE(modernize-pass-by-value)
Font::Font(const std::string& face)
    : m_face(face)
{}

// NOLINTNEXTLINE(modernize-pass-by-value)
Font::Font(const std::string& face, Font::Size size, Font::Weight weight, Font::Slant slant)
    : m_face(face),
      m_size(size),
      m_weight(weight),
      m_slant(slant)
{}

Font::Font(Font::Size size)
    : m_size(size)
{}

Font::Font(Font::Size size, Font::Weight weight)
    : m_size(size),
      m_weight(weight)
{}

Font::Font(Font::Weight weight)
    : m_weight(weight)
{}

Font::Font(Font::Slant slant)
    : m_slant(slant)
{}

template<>
const std::pair<Font::Weight, char const*> detail::EnumStrings<Font::Weight>::data[] =
{
    {Font::Weight::normal, "normal"},
    {Font::Weight::bold, "bold"},
};

template<>
const std::pair<Font::Slant, char const*> detail::EnumStrings<Font::Slant>::data[] =
{
    {Font::Slant::normal, "normal"},
    {Font::Slant::italic, "italic"},
    {Font::Slant::oblique, "oblique"},
};

void Font::serialize(const std::string& name, Serializer& serializer) const
{
    const Serializer::Attributes attrs =
    {
        {"weight", detail::enum_to_string(weight())},
        {"slant", detail::enum_to_string(slant())},
        {"size", std::to_string(size())},
    };

    serializer.add_property(name, face(), attrs);
}

void Font::deserialize(const std::string& name, const std::string& value,
                       const Serializer::Attributes& attrs)
{
    detail::ignoreparam(name);

    face(value);

    for (auto& a : attrs)
    {
        switch (detail::hash(a.first))
        {
        case detail::hash("size"):
            size(std::stoi(a.second));
            break;
        case detail::hash("weight"):
            weight(detail::enum_from_string<Font::Weight>(a.second));
            break;
        case detail::hash("slant"):
            slant(detail::enum_from_string<Font::Slant>(a.second));
            break;
        default:
            spdlog::warn("unhandled attribute {}", a.first);
            break;
        }
    }
}

struct FontCache
{
    struct FontCompare
    {
        bool operator()(const Font& lhs, const Font& rhs) const
        {
            if (lhs.face() != rhs.face())
                return lhs.face() < rhs.face();
            if (!detail::float_equal(lhs.size(), rhs.size()))
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

        shared_cairo_scaled_font_t scaled_font;

        std::string path;
        auto type = detail::resolve_path(font.face(), path);

        switch (type)
        {
        case detail::SchemeType::filesystem:
        {
            scaled_font = create_ft_scaled_font(cr.get(), path.c_str(), font);
            break;
        }
        case detail::SchemeType::unknown:
#if CAIRO_HAS_FC_FONT
        {
            scaled_font = create_scaled_font(cr.get(), font);
            break;
        }
#endif
        case detail::SchemeType::resource:
        case detail::SchemeType::network:
        default:
        {
            throw std::runtime_error("unsupported uri: " + font.face());
            break;
        }
        }

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

void Font::reset_font_cache()
{
    font_cache.cache.clear();
}

void Font::shutdown_fonts()
{
    reset_font_cache();
    FcFini();
}

}
}
