/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/dump.h"
#include "egt/canvas.h"
#include "egt/detail/filesystem.h"
#include "egt/detail/meta.h"
#include "egt/resource.h"
#include "egt/respath.h"
#include "egt/svgimage.h"
#define RSVG_DISABLE_DEPRECATION_WARNINGS /* for rsvg_handle_get_dimensions() */
#include <librsvg/rsvg.h>

namespace egt
{
inline namespace v1
{

struct RsvgHandle_deleter
{
    void operator()(RsvgHandle* handle) { g_object_unref(handle); }
};

struct SvgImage::SvgImpl
{
    std::unique_ptr<RsvgHandle, RsvgHandle_deleter> rsvg;
    RsvgDimensionData dim{};
};

SvgImage::SvgImage()
    : m_impl(std::make_unique<SvgImage::SvgImpl>())
{

}

// NOLINTNEXTLINE(modernize-pass-by-value)
SvgImage::SvgImage(const std::string& uri, const SizeF& size)
    : m_impl(std::make_unique<SvgImage::SvgImpl>()),
      m_size(size),
      m_uri(uri)
{
    load();
}

SvgImage::operator Image() const
{
    return Image(do_render(), m_uri);
}

Image SvgImage::render(const std::string& id, const RectF& rect) const
{
    return Image(do_render(id, rect), m_uri);
}

RectF SvgImage::id_box(const std::string& id) const
{
    RectF result;

    if (id.empty())
        return size();

    if (m_impl->rsvg)
    {
        auto s = size();
        auto hfactor = s.width() / m_impl->dim.width;
        auto vfactor = s.height() / m_impl->dim.height;

        RsvgRectangle viewport;
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = 1;
        viewport.height = 1;

        RsvgRectangle ink_r;
        if (rsvg_handle_get_geometry_for_layer(m_impl->rsvg.get(), id.c_str(),
                                               &viewport, &ink_r, nullptr,
                                               nullptr))
        {
            result.point(PointF(ink_r.x * hfactor, ink_r.y * vfactor));
            result.size(SizeF(ink_r.width * hfactor, ink_r.height * vfactor));
        }
    }

    return result;
}

bool SvgImage::id_exists(const std::string& id) const
{
    if (m_impl->rsvg)
        return rsvg_handle_has_sub(m_impl->rsvg.get(), id.c_str());

    return false;
}

void SvgImage::uri(const std::string& uri)
{
    if (detail::change_if_diff<>(m_uri, uri))
        load();
}

SizeF SvgImage::size() const
{
    auto result = m_size;

    if (m_impl->rsvg)
    {
        if (m_size.width() <= 0 && m_size.height() > 0)
        {
            auto factor = m_size.height() / m_impl->dim.height;
            result.width(m_impl->dim.width * factor);
        }
        else if (m_size.height() <= 0 && m_size.width() > 0)
        {
            auto factor = m_size.width() / m_impl->dim.width;
            result.height(m_impl->dim.height * factor);
        }

        if (result.empty())
            result = SizeF(m_impl->dim.width, m_impl->dim.height);
    }

    return result;
}

void SvgImage::load()
{
    RsvgHandle* handle = nullptr;
    std::string path;
    auto type = detail::resolve_path(m_uri, path);

    switch (type)
    {
    case detail::SchemeType::resource:
    {
        auto data = ResourceManager::instance().data(path.c_str());
        if (!data)
            throw std::runtime_error("resource not found: " + path);

        handle = rsvg_handle_new_from_data(data,
                                           ResourceManager::instance().size(path.c_str()),
                                           nullptr);

        if (!handle)
            throw std::runtime_error("unable to load svg resource: " + m_uri);

        break;
    }
    case detail::SchemeType::filesystem:
    {
        if (!detail::exists(path))
            throw std::runtime_error("file not found: " + path);

        handle = rsvg_handle_new_from_file(path.c_str(), nullptr);

        if (!handle)
            throw std::runtime_error("unable to load svg file: " + m_uri);

        break;
    }
    default:
    {
        throw std::runtime_error("unsupported uri: " + m_uri);
    }
    }

    m_impl->rsvg.reset(handle);

    /*
     * rsvg_handle_get_intrinsic_size_in_pixels() is not able to extract the
     * size in pixels directly from the intrinsic dimensions of the SVG document
     * if the `width` or `height` are in percentage units (or if they do not
     * exist, in which case the SVG spec mandates that they default to 100%),
     * whereas `rsvg_handle_get_dimensions()` is.
     *
     * This is a somewhat expensive operation, so do it once.
     */
    rsvg_handle_get_dimensions(m_impl->rsvg.get(), &m_impl->dim);
}

shared_cairo_surface_t SvgImage::do_render(const std::string& id, const RectF& rect) const
{
    auto s = size();

    RsvgRectangle viewport;
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = s.width();
    viewport.height = s.height();

    if (!rect.empty())
        s = rect.size();

    s.width(std::ceil(s.width()));
    s.height(std::ceil(s.height()));

    Canvas canvas(s);
    auto cr = canvas.context().get();

    detail::code_timer(false, "render " + id + ": ", [&]()
    {
        if (!rect.empty())
        {
            cairo_translate(cr,
                            -rect.x(),
                            -rect.y());

            cairo_rectangle(cr,
                            rect.x(),
                            rect.y(),
                            rect.width(),
                            rect.height());

            cairo_clip(cr);
        }

        if (id.empty())
            rsvg_handle_render_document(m_impl->rsvg.get(), cr, &viewport, nullptr);
        else
            rsvg_handle_render_layer(m_impl->rsvg.get(), cr, id.c_str(), &viewport, nullptr);

    });

    return canvas.surface();
}

SvgImage::SvgImage(SvgImage&&) noexcept = default;
SvgImage& SvgImage::operator=(SvgImage&&) noexcept = default;
SvgImage::~SvgImage() noexcept = default;

}
}
