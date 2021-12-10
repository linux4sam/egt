/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/canvas.h"
#include "egt/detail/alignment.h"
#include "egt/detail/image.h"
#include "egt/detail/imagecache.h"
#include "egt/image.h"
#include "egt/serialize.h"

namespace egt
{
inline namespace v1
{

Image::Image(const std::string& uri, float scale)
    : Image(uri, scale, scale)
{
}

Image::Image(const std::string& uri,
             float hscale, float vscale)
    : m_uri(uri)
{
    if (!uri.empty())
    {
        m_surface = detail::image_cache().get(uri, hscale, vscale, false);
        handle_surface_changed();
    }
}

Image::Image(shared_cairo_surface_t surface)
    : m_surface(std::move(surface))
{
    handle_surface_changed();
}

Image::Image(cairo_surface_t* surface)
    : m_surface(cairo_image_surface_create(cairo_image_surface_get_format(surface),
                                           cairo_image_surface_get_width(surface),
                                           cairo_image_surface_get_height(surface)),
                cairo_surface_destroy)
{
    handle_surface_changed();
}

Image::Image(const unsigned char* data, size_t len)
    : m_surface(detail::load_image_from_memory(data, len))
{
    handle_surface_changed();
}

void Image::handle_surface_changed()
{
    assert(cairo_surface_status(m_surface.get()) == CAIRO_STATUS_SUCCESS);

    m_surface_local.reset();
    m_orig_size = Size(std::ceil(cairo_image_surface_get_width(m_surface.get()) / m_hscale),
                       std::ceil(cairo_image_surface_get_height(m_surface.get()) / m_vscale));
}

void Image::load(const std::string& uri, float hscale, float vscale, bool approximate)
{
    bool do_update = false;

    do_update |= detail::change_if_diff<>(m_uri, uri);
    do_update |= detail::change_if_diff<>(m_hscale, hscale);
    do_update |= detail::change_if_diff<>(m_vscale, vscale);
    if (do_update)
    {
        if (!uri.empty())
        {
            m_surface = detail::image_cache().get(uri, hscale, vscale, approximate);
            handle_surface_changed();
        }
        else
        {
            m_surface.reset();
            m_surface_local.reset();
            m_orig_size = Size();
        }
        m_pattern.reset();
    }
}

void Image::scale(float hscale, float vscale, bool approximate)
{
    load(m_uri, hscale, vscale, approximate);
}

Rect Image::align(const Rect& bounding, const AlignFlags& align)
{
    auto target = detail::align_algorithm(size(), bounding, align);

    /*
     * Better to do it with target rect. Depending on the align flags, the
     * target size can be different from the size().
     */
    if (align.is_set(AlignFlag::expand))
    {
        const auto hs = static_cast<float>(target.width()) /
                        static_cast<float>(size_orig().width());
        const auto vs = static_cast<float>(target.height()) /
                        static_cast<float>(size_orig().height());

        // This check avoid rounding issues.
        if (size() != target.size())
        {
            if (keep_image_ratio())
            {
                if (hs < vs)
                    scale(hs);
                else
                    scale(vs);
                /*
                 * Need to update the alignment as the image size has probably
                 * changed due to the scaling with ratio preservation.
                 * Also clear the AlignFlag::expand since the image should not
                 * be streched.
                 */
                AlignFlags align2 = align;
                align2.clear(AlignFlag::expand);
                target = detail::align_algorithm(size(), bounding, align2);
            }
            else
            {
                scale(hs, vs);
            }
        }
    }

    return target;
}

void Image::copy()
{
    if (!m_surface_local.get())
    {
        auto canvas = Canvas(surface());
        m_surface_local = canvas.surface();
    }
}

Image Image::crop(const RectF& rect)
{
    Canvas canvas(rect.size());
    canvas.copy(m_surface, rect);
    return canvas.surface();
}

void Image::serialize(const std::string& name, Serializer& serializer) const
{
    Serializer::Attributes attrs;

    if (!detail::float_equal(hscale(), 1.0f))
        attrs.emplace_back("hscale", std::to_string(hscale()));

    if (!detail::float_equal(vscale(), 1.0f))
        attrs.emplace_back("vscale", std::to_string(vscale()));

    if (!keep_image_ratio())
        attrs.emplace_back("keep_image_ratio", detail::to_string(keep_image_ratio()));

    if (!m_uri.empty())
        serializer.add_property(name, m_uri, attrs);

    // TODO: serialize raw image data without uri
}

void Image::deserialize(const std::string& name, const std::string& value,
                        const Serializer::Attributes& attrs)
{
    detail::ignoreparam(name);

    float hscale = 1.0;
    float vscale = 1.0;

    const auto h = std::find_if(attrs.begin(), attrs.end(),
    [](const auto & element) { return element.first == "hscale";});
    if (h != attrs.end())
        hscale = std::stod(h->second);

    const auto v = std::find_if(attrs.begin(), attrs.end(),
    [](const auto & element) { return element.first == "vscale";});
    if (v != attrs.end())
        vscale = std::stod(v->second);

    const auto r = std::find_if(attrs.begin(), attrs.end(),
    [](const auto & element) { return element.first == "keep_image_ratio";});
    if (r != attrs.end())
        keep_image_ratio(egt::detail::from_string(r->second));

    load(value, hscale, vscale);
}

}
}
