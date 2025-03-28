/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "detail/egtlog.h"
#include "detail/painter.h"
#include "egt/app.h"
#include "egt/detail/enum.h"
#include "egt/font.h"
#include "egt/respath.h"
#include "egt/screen.h"
#include "egt/serialize.h"
#include <cairo-ft.h>
#include <map>
#include <memory>

namespace egt
{
inline namespace v1
{

constexpr const char* Font::DEFAULT_FACE;
constexpr Font::Weight Font::DEFAULT_WEIGHT;
constexpr Font::Slant Font::DEFAULT_SLANT;

static FT_Library ftlib{nullptr};

static std::unique_ptr<Font> the_global_font;

const Font* global_font()
{
    return the_global_font.get();
}

void global_font(std::unique_ptr<Font>&& font)
{
    the_global_font = std::move(font);
}

void reset_global_font()
{
    the_global_font.reset(nullptr);
}

static bool init_freetype()
{
    if (!ftlib)
    {
        FT_Error status = FT_Init_FreeType(&ftlib);
        if (status != 0)
        {
            detail::error("error initializing FreeType library");
            return false;
        }
    }
    return true;
}

static void ft_done_face_uncached(void* closure)
{
    auto face = static_cast<FT_Face>(closure);
    FT_Done_Face(face);
}

static shared_cairo_scaled_font_t create_ft_font(cairo_t* cr,
        FT_Face& face,
        const Font& font)
{
    std::unique_ptr<cairo_font_face_t, decltype(cairo_font_face_destroy)*>
    font_face(cairo_ft_font_face_create_for_ft_face(face, FT_LOAD_NO_HINTING | FT_LOAD_NO_BITMAP),
              cairo_font_face_destroy);

    static const cairo_user_data_key_t key{};
    if (cairo_font_face_set_user_data(font_face.get(), &key, face, ft_done_face_uncached))
        return nullptr;

    std::unique_ptr<cairo_font_options_t, decltype(cairo_font_options_destroy)*>
    font_options(cairo_font_options_create(), cairo_font_options_destroy);
    cairo_get_font_options(cr, font_options.get());
    cairo_font_options_set_hint_style(font_options.get(), CAIRO_HINT_STYLE_NONE);
    cairo_font_options_set_hint_metrics(font_options.get(), CAIRO_HINT_METRICS_OFF);

    cairo_matrix_t size_matrix{};
    cairo_matrix_t identity_matrix{};
    cairo_matrix_init_scale(&size_matrix, font.size(), font.size());
    cairo_matrix_init_identity(&identity_matrix);

    shared_cairo_scaled_font_t scaled_font(cairo_scaled_font_create(font_face.get(),
                                           &size_matrix,
                                           &identity_matrix,
                                           font_options.get()),
                                           cairo_scaled_font_destroy);

    if (!scaled_font || cairo_scaled_font_status(scaled_font.get()))
        return nullptr;

    return scaled_font;
}

static shared_cairo_scaled_font_t create_ft_scaled_font(cairo_t* cr,
        const char* path,
        const Font& font)
{
    EGTLOG_DEBUG("allocating font using FreeType: {}", font.face());

    if (!init_freetype())
        return nullptr;

    FT_Face face{};
    FT_Error status = FT_New_Face(ftlib, path, 0, &face);
    if (status != 0)
    {
        detail::error("error opening font {}", path);
        return nullptr;
    }

    return create_ft_font(cr, face, font);
}

static shared_cairo_scaled_font_t create_ft_scaled_font(cairo_t* cr,
        const unsigned char* data,
        size_t len, const Font& font)
{
    EGTLOG_DEBUG("allocating memory font using FreeType: {}", font.face());

    if (!init_freetype())
        return nullptr;

    FT_Face face{};
    FT_Error status = FT_New_Memory_Face(ftlib, static_cast<const FT_Byte*>(data),
                                         len, 0, &face);
    if (status)
        return nullptr;

    return create_ft_font(cr, face, font);
}

#ifdef HAVE_FONTCONFIG
static shared_cairo_scaled_font_t create_scaled_font(cairo_t* cr, const Font& font)
{
    EGTLOG_DEBUG("allocating font using Fontconfig: {}", font.face());

    std::unique_ptr<cairo_font_options_t, decltype(cairo_font_options_destroy)*>
    font_options(cairo_font_options_create(), cairo_font_options_destroy);
    cairo_get_font_options(cr, font_options.get());

    std::unique_ptr<FcPattern, decltype(FcPatternDestroy)*>
    pattern(FcPatternCreate(), FcPatternDestroy);
    if (!pattern)
        return nullptr;

    // NOLINTNEXTLINE
    FcPatternAddString(pattern.get(), FC_FAMILY, (const FcChar8*)(font.face().c_str()));
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

    char* face = nullptr;
    if (FcPatternGetString(resolved.get(), FC_FULLNAME, 0, reinterpret_cast<FcChar8**>(&face)) == FcResultMatch)
    {
        if (font.face() != std::string(face))
            EGTLOG_DEBUG("Font \"{}\" not found: using default {} font", font.face(), face);
    }

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

Font::Font()
    : m_size(default_font_size())
{}

// NOLINTNEXTLINE(modernize-pass-by-value)
Font::Font(const std::string& face)
    : m_face(face),
      m_size(default_font_size())
{}

Font::Font(const unsigned char* data, size_t len,
           Font::Size size)
    : m_face("MEMORY"),
      m_size(size <= 0 ? default_font_size() : size),
      m_data(data),
      m_len(len),
      m_use_default_size(size <= 0)
{
    direct_allocate();
}

// NOLINTNEXTLINE(modernize-pass-by-value)
Font::Font(const std::string& face, Font::Size size, Font::Weight weight, Font::Slant slant)
    : m_face(face),
      m_size(size),
      m_weight(weight),
      m_slant(slant),
      m_use_default_size(false)
{}

Font::Font(Font::Size size)
    : m_size(size),
      m_use_default_size(false)
{}

Font::Font(Font::Size size, Font::Weight weight)
    : m_size(size),
      m_weight(weight),
      m_use_default_size(false)
{}

Font::Font(Font::Weight weight)
    : m_size(default_font_size()),
      m_weight(weight)
{}

Font::Font(Font::Slant slant)
    : m_size(default_font_size()),
      m_slant(slant)
{}

Size Font::text_size(const std::string& text) const
{
    return detail::dummy_painter().set(*this).text_size(text);
}

Font::Size Font::default_font_size()
{
    auto ssize = egt::Application::instance().screen()->size();
    return ssize.width() * 0.02;
}

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
            detail::warn("unhandled attribute {}", a.first);
            break;
        }
    }
}

struct FontCache : private detail::NonCopyable<FontCache>
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

        EGTLOG_TRACE("creating scaled font {}", font);

        auto cr = detail::dummy_painter().context().get();

        shared_cairo_scaled_font_t scaled_font;

        std::string path;
        auto type = detail::resolve_path(font.face(), path);

        switch (type)
        {
        case detail::SchemeType::filesystem:
        {
            scaled_font = create_ft_scaled_font(cr, path.c_str(), font);
            break;
        }
        case detail::SchemeType::unknown:
#ifdef HAVE_FONTCONFIG
        {
            scaled_font = create_scaled_font(cr, font);
            break;
        }
#endif
        case detail::SchemeType::resource:
        case detail::SchemeType::network:
        default:
            throw std::runtime_error("unable to load font uri: " + font.face());
        }

        if (scaled_font)
            cache.insert(std::make_pair(font, scaled_font));
        return scaled_font;
    }
};

static FontCache font_cache;

cairo_scaled_font_t* Font::scaled_font() const
{
    if (m_data && m_len && !m_scaled_font)
    {
        auto cr = detail::dummy_painter().context().get();
        m_scaled_font = create_ft_scaled_font(cr, m_data, m_len, *this);
    }

    if (m_scaled_font)
        return m_scaled_font.get();

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
#ifdef HAVE_FONTCONFIG
    FcFini();
#endif
}

void Font::direct_allocate()
{
    m_scaled_font.reset();
}

void Font::on_screen_resized()
{
    if (m_use_default_size)
    {
        m_size = default_font_size();
        direct_allocate();
    }
}

}
}
