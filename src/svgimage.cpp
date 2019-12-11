/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/canvas.h"
#include "egt/detail/meta.h"
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
};

SvgImage::SvgImage()
    : m_impl(new SvgImage::SvgImpl)
{

}

SvgImage::SvgImage(const std::string& respath, const SizeF& size)
    : m_impl(new SvgImage::SvgImpl),
      m_size(size),
      m_respath(respath)
{
    load();
}

SvgImage::operator Image() const
{
    return render();
}

Image SvgImage::id(const std::string& id, const RectF& rect) const
{
    return render(id, rect);
}

RectF SvgImage::id_box(const std::string& id) const
{
    RectF result;

    if (m_impl->rsvg)
    {
        auto s = size();

        RsvgDimensionData dim;
        rsvg_handle_get_dimensions(m_impl->rsvg.get(), &dim);
        auto hfactor = s.width() / dim.width;
        auto vfactor = s.height() / dim.height;

        RsvgPositionData pos;
        if (rsvg_handle_get_position_sub(m_impl->rsvg.get(), &pos, id.c_str()))
        {
            result.set_point(PointF(pos.x * hfactor, pos.y * vfactor));
        }

        if (rsvg_handle_get_dimensions_sub(m_impl->rsvg.get(), &dim, id.c_str()))
        {
            /*
             * This +1 here in both dimensions is a hack of a workaround.  The
             * existing API in librsvg uses integers for position and dimension
             * data.  Of course this is a problem because the SVG can have
             * floating point values.  They fixes this in around 2.46 of librsvg
             * with a new API.
             *
             * See https://gitlab.gnome.org/GNOME/librsvg/blob/master/librsvg/rsvg.h#L208
             */
            result.set_size(SizeF(dim.em * hfactor + 1, dim.ex * vfactor + 1));
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

void SvgImage::set_respath(const std::string& respath)
{
    if (detail::change_if_diff<>(m_respath, respath))
        load();
}

SizeF SvgImage::size() const
{
    auto result = m_size;

    if (m_impl->rsvg)
    {
        RsvgDimensionData dim;
        rsvg_handle_get_dimensions(m_impl->rsvg.get(), &dim);

        if (m_size.width() <= 0 && m_size.height() > 0)
        {
            auto factor = m_size.height() / dim.height;
            result.set_width(dim.width * factor);
        }
        else if (m_size.height() <= 0 && m_size.width() > 0)
        {
            auto factor = m_size.width() / dim.width;
            result.set_height(dim.height * factor);
        }

        if (result.empty())
            result = SizeF(dim.width, dim.height);
    }

    return result;
}

void SvgImage::load()
{
    // TODO: this only supports file paths, not all respaths

    GError* error = nullptr;
    auto handle = rsvg_handle_new_from_file(resolve_file_path(m_respath).c_str(), &error);

    if (!handle || error)
        throw std::runtime_error("unable to load svg file: " + m_respath);

    m_impl->rsvg = std::shared_ptr<RsvgHandle>(handle,
    [](RsvgHandle * r) { g_object_unref(r); });
}

shared_cairo_surface_t SvgImage::render(const std::string& id, const RectF& rect) const
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

        RsvgDimensionData dim;
        rsvg_handle_get_dimensions(m_impl->rsvg.get(), &dim);

        const auto scaled = size() / SizeF(dim.width, dim.height);

        /* Scale *before* setting the source surface (1) */
        cairo_scale(cr,
                    scaled.width(),
                    scaled.height());

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

SvgImage::~SvgImage() = default;

}
}
