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
#include <librsvg/rsvg.h>

namespace egt
{
inline namespace v1
{

struct SvgImage::SvgImpl
{
    std::shared_ptr<RsvgHandle> rsvg;
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
    return do_render();
}

Image SvgImage::render(const std::string& id, const RectF& rect) const
{
    return do_render(id, rect);
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

        RsvgPositionData pos;
        if (rsvg_handle_get_position_sub(m_impl->rsvg.get(), &pos, id.c_str()))
        {
            result.point(PointF(pos.x * hfactor, pos.y * vfactor));
        }

        RsvgDimensionData dim{};
        if (rsvg_handle_get_dimensions_sub(m_impl->rsvg.get(), &dim, id.c_str()))
        {
            /*
             * This +1 here in both dimensions is a hack of a workaround.  The
             * existing API in librsvg uses integers for position and dimension
             * data.  Of course this is a problem because the SVG can have
             * floating point values.  They fixed this in around 2.46 of librsvg
             * with a new API.
             *
             * See https://gitlab.gnome.org/GNOME/librsvg/blob/master/librsvg/rsvg.h#L208
             */
            result.size(SizeF((dim.em + 1) * hfactor, (dim.ex + 1) * vfactor));
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
    std::string path;
    auto type = detail::resolve_path(m_uri, path);

    switch (type)
    {
    case detail::SchemeType::resource:
    {
        auto data = ResourceManager::instance().data(path.c_str());
        if (!data)
            throw std::runtime_error("resource not found: " + path);

        auto handle = rsvg_handle_new_from_data(data,
                                                ResourceManager::instance().size(path.c_str()),
                                                nullptr);

        if (!handle)
            throw std::runtime_error("unable to load svg resource: " + m_uri);

        m_impl->rsvg = std::shared_ptr<RsvgHandle>(handle,
        [](RsvgHandle * r) { g_object_unref(r); });

        // this is a somewhat expensive operation, so do it once
        rsvg_handle_get_dimensions(m_impl->rsvg.get(), &m_impl->dim);
        break;
    }
    case detail::SchemeType::filesystem:
    {
        if (!detail::exists(path))
            throw std::runtime_error("file not found: " + path);

        auto handle = rsvg_handle_new_from_file(path.c_str(), nullptr);

        if (!handle)
            throw std::runtime_error("unable to load svg file: " + m_uri);

        m_impl->rsvg = std::shared_ptr<RsvgHandle>(handle,
        [](RsvgHandle * r) { g_object_unref(r); });

        // this is a somewhat expensive operation, so do it once
        rsvg_handle_get_dimensions(m_impl->rsvg.get(), &m_impl->dim);
        break;
    }
    default:
    {
        throw std::runtime_error("unsupported uri: " + m_uri);
    }
    }
}

shared_cairo_surface_t SvgImage::do_render(const std::string& id, const RectF& rect) const
{
    auto s = size();
    if (!rect.empty())
        s = rect.size();

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

        const auto scaled = size() / SizeF(m_impl->dim.width, m_impl->dim.height);
        cairo_scale(cr, scaled.width(), scaled.height());

        /* To avoid getting the edge pixels blended with 0 alpha, which would
         * occur with the default EXTEND_NONE. Use EXTEND_PAD for 1.2 or newer (2)
         */
        cairo_pattern_set_extend(cairo_get_source(cr), CAIRO_EXTEND_PAD);

        /* Replace the destination with the source instead of overlaying */
        cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);

        if (id.empty())
            rsvg_handle_render_cairo(m_impl->rsvg.get(), cr);
        else
            rsvg_handle_render_cairo_sub(m_impl->rsvg.get(), cr, id.c_str());

    });

    return canvas.surface();
}

SvgImage::~SvgImage() noexcept = default;

}
}
