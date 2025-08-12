/*
 * Copyright (C) 2024 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/cairoabstraction.h"
#include "detail/gpu.h"
#include "detail/egtlog.h"
#include "egt/color.h"
#include "egt/detail/meta.h"
#include "egt/detail/math.h"
#include "egt/surface.h"

#include <stdexcept>

namespace egt
{
inline namespace v1
{
namespace detail
{
static const std::pair<PixelFormat, enum m2d_pixel_format> m2d_formats[] =
{
    {PixelFormat::argb8888, M2D_PF_ARGB8888},
    {PixelFormat::rgb565, M2D_PF_RGB565},
    {PixelFormat::a8, M2D_PF_A8},
};

static enum m2d_pixel_format m2d_format(PixelFormat format)
{
    for (const auto& i : m2d_formats)
        if (i.first == format)
            return i.second;

    return M2D_PF_ARGB8888;
}

void gpu_init()
{
    m2d_init();
    m2d_blend_functions(M2D_FUNC_ADD, M2D_FUNC_ADD);
    m2d_blend_factors(M2D_BLEND_SRC_ALPHA, M2D_BLEND_ONE_MINUS_SRC_ALPHA,
                      M2D_BLEND_ONE, M2D_BLEND_ONE_MINUS_SRC_ALPHA);
}

void gpu_cleanup()
{
    m2d_cleanup();
}

GPUSurface::GPUObjectId GPUSurface::next_id = 0;

GPUSurface::GPUSurface(Surface* surface, int prime_fd)
    : m_id(next_id++),
      m_surface(surface)
{
    struct m2d_import_desc desc;

    EGTLOG_TRACE("importing GPU object {}: size={},format={}.",
                 m_id, m_surface->size(), m_surface->format());
    memset(&desc, 0, sizeof(desc));
    desc.width = m_surface->width();
    desc.height = m_surface->height();
    desc.format = m2d_format(m_surface->format());
    desc.stride = m_surface->stride();
    desc.fd = prime_fd;
    desc.cpu_addr = m_surface->data();
    m_buf.reset(m2d_import(&desc));
    if (!m_buf)
        throw std::runtime_error("libm2d: cannot import object from PRIME file descriptor.");
}

GPUSurface::GPUSurface(Surface* surface, void** data, DefaultDim* stride)
    : m_id(next_id++),
      m_surface(surface)
{
    EGTLOG_TRACE("allocating GPU object {}: size={},format={}.",
                 m_id, m_surface->size(), m_surface->format());
    m_buf.reset(m2d_alloc(m_surface->width(),
                          m_surface->height(),
                          m2d_format(m_surface->format()),
                          m_surface->stride()));
    if (!m_buf)
        throw std::runtime_error("libm2d: cannot allocate memory.");

    *data = m2d_get_data(m_buf.get());
    *stride = m2d_get_stride(m_buf.get());
}

m2d_buffer* GPUSurface::tmp_buffer()
{
    if (!m_tmp)
    {
        EGTLOG_TRACE("allocating side buffer for GPU object {}.", m_id);

        /* Force the pixel format to ARGB8888 to allow alpha blending. */
        const auto format = PixelFormat::argb8888;
        m_tmp.reset(m2d_alloc(m_surface->width(),
                              m_surface->height(),
                              m2d_format(format),
                              Surface::stride(format, m_surface->width())));
        if (!m_tmp)
            EGTLOG_WARN("failed to allocate side buffer for GPU object {}.", m_id);
    }

    return m_tmp.get();
}

void GPUSurface::sync_for_cpu()
{
    if (detail::change_if_diff(m_owned_by_gpu, false))
    {
        struct timespec timeout;

        EGTLOG_TRACE("synchronizing GPU object {} for CPU.", m_id);
        clock_gettime(CLOCK_MONOTONIC, &timeout);
        timeout.tv_sec += 1;
        m2d_sync_for_cpu(m_buf.get(), &timeout);
        cairo_surface_mark_dirty(m_surface->impl());
    }
}

void GPUSurface::sync_for_gpu()
{
    if (detail::change_if_diff(m_owned_by_gpu, true))
    {
        EGTLOG_TRACE("synchronizing GPU object {} for GPU.", m_id);
        cpu_flush();
        m2d_sync_for_gpu(m_buf.get());
    }
}

void GPUSurface::flush(bool claimed_by_cpu)
{
    if (m_owned_by_gpu)
        if (claimed_by_cpu)
            sync_for_cpu();
        else
            gpu_flush();
    else
        cpu_flush();
}

void GPUSurface::cpu_flush()
{
    EGTLOG_TRACE("CPU flushing GPU object {}.", m_id);
    cairo_surface_flush(m_surface->impl());
}

void GPUSurface::gpu_flush()
{
    struct timespec timeout;

    EGTLOG_TRACE("GPU flushing GPU object {}.", m_id);
    clock_gettime(CLOCK_MONOTONIC, &timeout);
    timeout.tv_sec += 1;
    m2d_wait(m_buf.get(), &timeout);
}

bool GPUPainter::is_translation(const cairo_matrix_t& matrix, Point& offset)
{
    if (!detail::float_equal(matrix.xx, 1.) ||
        !detail::float_equal(matrix.yx, 0.) ||
        !detail::float_equal(matrix.xy, 0.) ||
        !detail::float_equal(matrix.yy, 1.))
        return false;

    offset.x(matrix.x0);
    offset.y(matrix.y0);
    return true;
}

bool GPUPainter::get_transformation(Point& offset)
{
    /* Accept only translations (no rotation, no scale, ...). */
    cairo_matrix_t ctm;
    cairo_get_matrix(m_painter.context(), &ctm);
    if (!is_translation(ctm, offset))
    {
        EGTLOG_TRACE("CTM is not a translation.");
        return false;
    }

    return true;
}

/**
 * Gets the current clip region as a list of rectangles in user coordinates.
 *
 * @note cairo_copy_clip_rectangle_list() never returns NULL.
 */
std::unique_ptr<cairo_rectangle_list_t, decltype(cairo_rectangle_list_destroy)*>
GPUPainter::get_clip_region() const
{
    std::unique_ptr<cairo_rectangle_list_t, decltype(cairo_rectangle_list_destroy)*>
    clip(cairo_copy_clip_rectangle_list(m_painter.context()), cairo_rectangle_list_destroy);
    if (clip->status != CAIRO_STATUS_SUCCESS)
    {
        EGTLOG_TRACE("cannot get clip rectangle list.");
        clip.reset();
    }

    return clip;
}

bool GPUPainter::draw(Operator op)
{
    if (!m_painter.target().impl().is_gpu_capable())
    {
        EGTLOG_TRACE("target {} is not GPU capable.", (void*)&m_painter.target());
        return false;
    }

    auto& target = m_painter.target().impl().gpu_surface();

    Point offset;
    if (!get_transformation(offset))
        return false;

    const auto blend = m_painter.alpha_blending();

    double red, green, blue, alpha;
    GPUPainterSource* src = nullptr;

    cairo_t* cr = m_painter.context();
    cairo_pattern_t* pattern = cairo_get_source(cr);
    switch (cairo_pattern_get_type(pattern))
    {
    default:
        EGTLOG_TRACE("unsupported source type.");
        return false;

    case CAIRO_PATTERN_TYPE_SOLID:
        if (cairo_pattern_get_rgba(pattern, &red, &green, &blue, &alpha) != CAIRO_STATUS_SUCCESS)
        {
            EGTLOG_TRACE("cannot extract RGBA components of source solid pattern.");
            return false;
        }
        m2d_source_color(red * 255., green * 255., blue * 255., alpha * 255.);
        break;

    case CAIRO_PATTERN_TYPE_SURFACE:
        src = (GPUPainterSource*)cairo_pattern_get_user_data(pattern, &m_key);
        if (!src)
        {
            EGTLOG_TRACE("source surface is not GPU capable.");
            return false;
        }
        break;
    }

    auto clip = get_clip_region();
    if (!clip)
        return false;

    m_rects.clear();

    bool ret = false;
    switch (op)
    {
    case Operator::fill:
        ret = fill(*clip);
        break;

    case Operator::paint:
        ret = paint(*clip, offset, src);
        break;

    default:
        EGTLOG_TRACE("unknown operation.");
        break;
    }
    if (!ret)
        return false;

    if (m_rects.empty())
    {
        EGTLOG_TRACE("no rectangle to draw.");
        return true;
    }

    auto rects = prepare_m2d_rectangles(offset);
    if (!rects)
        return false;

    bool has_source = false;
    if (src)
    {
        auto& source = *src->surface;

        EGTLOG_TRACE("{} GPU object {} source at {} into GPU object {} destination.",
                     blend ? "blend" : "copy",
                     source.id(), src->origin, target.id());

        source.sync_for_gpu();
        m2d_set_source(M2D_SRC, source,
                       static_cast<dim_t>(src->origin.x()),
                       static_cast<dim_t>(src->origin.y()));
        has_source = true;
    }
    else
    {
        EGTLOG_TRACE("{} GPU object {} destination with color ({},{},{},{}).",
                     blend ? "blend" : "fill",
                     target.id(),
                     (uint8_t)(red * 255.),
                     (uint8_t)(green * 255.),
                     (uint8_t)(blue * 255.),
                     (uint8_t)(alpha * 255.));
    }

    target.sync_for_gpu();
    m2d_set_target(target);

    m2d_blend_enable(blend);
    m2d_source_enable(M2D_SRC, has_source);
    m2d_draw_rectangles(rects.get(), m_rects.size());

    m2d_source_color(255, 255, 255, 255);

    return true;
}

bool GPUPainter::parse_path_move_to(const Point& p)
{
    if (m_path_state != PathState::empty && m_path_state != PathState::start)
        return false;

    m_path_current_point = p;
    m_path_size.clear();
    m_path_state = PathState::start;
    EGTLOG_TRACE("move to {} (state=start).", p);
    return true;
}

bool GPUPainter::parse_path_line_to(const Point& p)
{
    const auto& c = m_path_current_point;
    auto& s = m_path_size;

    switch (m_path_state)
    {
    case PathState::start:
        if (p.x() != c.x() && p.y() != c.y())
            return false;

        if (p.x() > c.x()) /* right */
        {
            m_path_state = PathState::r;
            s.width(p.x() - c.x());
            EGTLOG_TRACE("move right to {} (width={}, state=r).", p, s.width());
        }
        else if (p.x() < c.x()) /* left */
        {
            m_path_state = PathState::l;
            s.width(c.x() - p.x());
            EGTLOG_TRACE("move left to {} (width={}, state=l).", p, s.width());
        }
        else if (p.y() > c.y()) /* down */
        {
            m_path_state = PathState::d;
            s.height(p.y() - c.y());
            EGTLOG_TRACE("move down to {} (height={}, state=d).", p, s.height());
        }
        else if (p.y() < c.y()) /* up */
        {
            m_path_state = PathState::u;
            s.height(c.y() - p.y());
            EGTLOG_TRACE("move up to {} (height={}, state=u).", p, s.height());
        }
        else /* p == c*/
        {
            m_path_state = PathState::x;
            EGTLOG_TRACE("stay at {} (state=x).", p);
        }
        break;

    case PathState::x:
        if (p.x() != c.x() && p.y() != c.y())
            return false;

        if (p.x() > c.x()) /* right */
        {
            m_path_state = PathState::dr;
            s.width(p.x() - c.x());
            EGTLOG_TRACE("move down then right to {} (size={}, state=dr).", p, s);
        }
        else if (p.x() < c.x()) /* left */
        {
            m_path_state = PathState::dl;
            s.width(c.x() - p.x());
            EGTLOG_TRACE("move down then left to {} (size={}, state=dl).", p, s);
        }
        else if (p.y() > c.y()) /* down */
        {
            m_path_state = PathState::rd;
            s.height(p.y() - c.y());
            EGTLOG_TRACE("move right then down to {} (size={}, state=rd).", p, s);
        }
        else if (p.y() < c.y()) /* up */
        {
            m_path_state = PathState::ru;
            s.height(c.y() - p.y());
            EGTLOG_TRACE("move right then up to {} (size={}, state=ru).", p, s);
        }
        else /* p == c*/
        {
            m_path_state = PathState::rd;
            EGTLOG_TRACE("move right then down to {} (size={}, state=rd).", p, s);
        }
        break;

    case PathState::r:
        if (p.x() != c.x())
            return false;

        if (p.y() > c.y()) /* down */
        {
            m_path_state = PathState::rd;
            s.height(p.y() - c.y());
            EGTLOG_TRACE("move down to {} (height={}, state=rd).", p, s.height());
        }
        else /* up */
        {
            m_path_state = PathState::ru;
            s.height(c.y() - p.y());
            EGTLOG_TRACE("move up to {} (height={}, state=ru).", p, s.height());
        }
        break;

    case PathState::l:
        if (p.x() != c.x())
            return false;

        if (p.y() > c.y()) /* down */
        {
            m_path_state = PathState::ld;
            s.height(p.y() - c.y());
            EGTLOG_TRACE("move down to {} (height={}, state=ld).", p, s.height());
        }
        else /* up */
        {
            m_path_state = PathState::lu;
            s.height(c.y() - p.y());
            EGTLOG_TRACE("move up to {} (height={}, state=lu).", p, s.height());
        }
        break;

    case PathState::d:
        if (p.y() != c.y())
            return false;

        if (p.x() > c.x()) /* right */
        {
            m_path_state = PathState::dr;
            s.width(p.x() - c.x());
            EGTLOG_TRACE("move right to {} (width={}, state=dr).", p, s.width());
        }
        else /* left */
        {
            m_path_state = PathState::dl;
            s.width(c.x() - p.x());
            EGTLOG_TRACE("move left to {} (width={}, state=dl).", p, s.width());
        }
        break;

    case PathState::u:
        if (p.y() != c.y())
            return false;

        if (p.x() > c.x()) /* right */
        {
            m_path_state = PathState::ur;
            s.width(p.x() - c.x());
            EGTLOG_TRACE("move right to {} (width={}, state=ur).", p, s.width());
        }
        else /* left */
        {
            m_path_state = PathState::ul;
            s.width(c.x() - p.x());
            EGTLOG_TRACE("move left to {} (width={}, state=ul).", p, s.width());
        }
        break;

    case PathState::rd:
        if (p != (c - Point(s.width(), 0)))
            return false;

        /* left */
        m_path_state = PathState::rdl;
        EGTLOG_TRACE("move left to {} (state=rdl).", p);
        break;

    case PathState::ru:
        if (p != (c - Point(s.width(), 0)))
            return false;

        /* left */
        m_path_state = PathState::rul;
        EGTLOG_TRACE("move left to {} (state=rul).", p);
        break;

    case PathState::ld:
        if (p != (c + Point(s.width(), 0)))
            return false;

        /* right */
        m_path_state = PathState::ldr;
        EGTLOG_TRACE("move right to {} (state=ldr).", p);
        break;

    case PathState::lu:
        if (p != (c + Point(s.width(), 0)))
            return false;

        /* right */
        m_path_state = PathState::lur;
        EGTLOG_TRACE("move right to {} (state=lur).", p);
        break;

    case PathState::dr:
        if (p != (c - Point(0, s.height())))
            return false;

        /* up */
        m_path_state = PathState::dru;
        EGTLOG_TRACE("move up to {} (state=dru).", p);
        break;

    case PathState::dl:
        if (p != (c - Point(0, s.height())))
            return false;

        /* up */
        m_path_state = PathState::dlu;
        EGTLOG_TRACE("move up to {} (state=dlu).", p);
        break;

    case PathState::ur:
        if (p != (c + Point(0, s.height())))
            return false;

        /* down */
        m_path_state = PathState::urd;
        EGTLOG_TRACE("move down to {} (state=urd).", p);
        break;

    case PathState::ul:
        if (p != (c + Point(0, s.height())))
            return false;

        /* down */
        m_path_state = PathState::uld;
        EGTLOG_TRACE("move down to {} (state=uld).", p);
        break;

    default:
        return false;
    }

    m_path_current_point = p;
    return true;
}

bool GPUPainter::parse_path_close_path(const cairo_rectangle_list_t& clip_rectangles)
{
    auto& c = m_path_current_point;
    const auto& s = m_path_size;
    Point top_left;

    switch (m_path_state)
    {
    case PathState::empty:
    case PathState::start:
        /* do nothing but accept this sub-path anyway. */
        return true;

    case PathState::rdl:
        /* move up, back to top left */
        c.y(c.y() - s.height());
        top_left = c;
        EGTLOG_TRACE("move up to {} and close path.", c);
        break;

    case PathState::ldr:
        /* move up, back to top right */
        c.y(c.y() - s.height());
        top_left = c - Point(s.width(), 0);
        EGTLOG_TRACE("move up to {} and close path.", c);
        break;

    case PathState::rul:
        /* move down, back to bottom left */
        c.y(c.y() + s.height());
        top_left = c - Point(0, s.height());
        EGTLOG_TRACE("move down to {} and close path.", c);
        break;

    case PathState::lur:
        /* move down, back to bottom right */
        c.y(c.y() + s.height());
        top_left = c - Point(s.width(), s.height());
        EGTLOG_TRACE("move down to {} and close path.", c);
        break;

    case PathState::dru:
        /* move left, back to top left */
        c.x(c.x() - s.width());
        top_left = c;
        EGTLOG_TRACE("move left to {} and close path.", c);
        break;

    case PathState::urd:
        /* move left, back to bottom left */
        c.x(c.x() - s.width());
        top_left = c - Point(0, s.height());
        EGTLOG_TRACE("move left to {} and close path.", c);
        break;

    case PathState::dlu:
        /* move right, back to top right */
        c.x(c.x() + s.width());
        top_left = c - Point(s.width(), 0);
        EGTLOG_TRACE("move right to {} and close path.", c);
        break;

    case PathState::uld:
        /* move right, back to bottom right */
        c.x(c.x() + s.width());
        top_left = c - Point(s.width(), s.height());
        EGTLOG_TRACE("move right to {} and close path.", c);
        break;

    default:
        return false;
    }

    if (!m_path_size.empty())
    {
        const Rect rect(top_left, m_path_size);
        EGTLOG_TRACE("rectangle before clipping: {}.", rect);

        if (!clip_rectangles.num_rectangles)
        {
            m_rects.emplace_back(rect);
        }
        else
        {
            for (int i = 0; i < clip_rectangles.num_rectangles; ++i)
            {
                const auto& clip = clip_rectangles.rectangles[i];
                const Rect clip_rect(clip.x, clip.y, clip.width, clip.height);
                auto r = Rect::intersection(rect, clip_rect);
                if (!r.empty())
                    m_rects.emplace_back(r);
            }
        }

        m_path_size.clear();
    }

    m_path_state = PathState::start;
    return true;
}

bool GPUPainter::fill(const cairo_rectangle_list_t& clip_rectangles)
{
    cairo_t* cr = m_painter.context();

    std::unique_ptr<cairo_path_t, decltype(cairo_path_destroy)*>
    path(cairo_copy_path(cr), cairo_path_destroy);
    if (path->status != CAIRO_STATUS_SUCCESS)
    {
        EGTLOG_TRACE("cannot copy cairo path.");
        return false;
    }

    m_path_state = PathState::empty;

    for (int i = 0; i < path->num_data; i += path->data[i].header.length)
    {
        cairo_path_data_t* data = &path->data[i];

        switch (data->header.type)
        {
        case CAIRO_PATH_MOVE_TO:
            if (!parse_path_move_to(Point(data[1].point.x, data[1].point.y)))
            {
                EGTLOG_TRACE("invalid cairo move_to.");
                return false;
            }
            break;

        case CAIRO_PATH_LINE_TO:
            if (!parse_path_line_to(Point(data[1].point.x, data[1].point.y)))
            {
                EGTLOG_TRACE("invalid cairo line_to.");
                return false;
            }
            break;

        case CAIRO_PATH_CURVE_TO:
            EGTLOG_TRACE("cairo curve_to not supported.");
            return false;

        case CAIRO_PATH_CLOSE_PATH:
            if (!parse_path_close_path(clip_rectangles))
            {
                EGTLOG_TRACE("invalid cairo close path.");
                return false;
            }
            break;
        }
    }

    return (m_path_state == PathState::empty) || (m_path_state == PathState::start);
}

bool GPUPainter::paint(const cairo_rectangle_list_t& clip_rectangles,
                       const Point& offset, GPUPainterSource* src)
{
    Rect src_rect;

    if (src)
    {
        const auto& surface = src->surface->owner();
        src_rect = Rect(src->origin - offset, surface.size());
    }

    for (int i = 0; i < clip_rectangles.num_rectangles; ++i)
    {
        const auto& clip = clip_rectangles.rectangles[i];
        Rect r(clip.x, clip.y, clip.width, clip.height);

        if (src)
            r = Rect::intersection(r, src_rect);

        if (!r.empty())
            m_rects.emplace_back(r);
    }

    return true;
}

bool GPUPainter::fill_rectangle(const cairo_rectangle_list_t& clip_rectangles,
                                const Rect& rect)
{
    if (!clip_rectangles.num_rectangles)
    {
        if (!rect.empty())
            m_rects.emplace_back(rect);
    }
    else
    {
        for (int i = 0; i < clip_rectangles.num_rectangles; ++i)
        {
            const auto& c = clip_rectangles.rectangles[i];
            Rect clipped_rect(c.x, c.y, c.width, c.height);
            if (!rect.empty())
                clipped_rect = Rect::intersection(rect, clipped_rect);

            if (!clipped_rect.empty())
                m_rects.emplace_back(clipped_rect);
        }
    }

    return true;
}

void GPUPainter::sync_for_cpu(bool skip_source)
{
    if (m_painter.target().impl().is_gpu_capable())
        m_painter.target().impl().gpu_surface().sync_for_cpu();

    if (skip_source)
        return;

    GPUPainterSource* src = nullptr;

    cairo_t* cr = m_painter.context();
    cairo_pattern_t* source = cairo_get_source(cr);
    switch (cairo_pattern_get_type(source))
    {
    case CAIRO_PATTERN_TYPE_SURFACE:
        src = (GPUPainterSource*)cairo_pattern_get_user_data(source, &m_key);
        break;
    default:
        break;
    }

    if (src)
        src->surface->sync_for_cpu();
}

void GPUPainter::source(const Surface& surface, const Point& point)
{
    if (!surface.impl().is_gpu_capable())
        return;

    Point offset;
    if (!get_transformation(offset))
        return;

    auto* src = alloc_source();
    if (!src)
        return;

    auto& surf = surface.impl().gpu_surface();

    src->origin = offset + point;
    src->surface = &surf;

    EGTLOG_TRACE("set source from GPU object {} at {} ({} + {}).",
                 surf.id(), offset + point, offset, point);

    cairo_t* cr = m_painter.context();
    cairo_pattern_t* pattern = cairo_get_source(cr);
    cairo_pattern_set_user_data(pattern, &m_key, (void*)src, source_destroy);
}

std::deque<GPUPainter::GPUPainterSource> GPUPainter::m_source_cache;
GPUPainter::GPUPainterSource* GPUPainter::m_free_list = nullptr;

GPUPainter::GPUPainterSource* GPUPainter::alloc_source()
{
    if (!m_free_list)
    {
        try
        {
            m_source_cache.emplace_back();
            auto& new_src = m_source_cache.back();
            new_src.next = m_free_list;
            m_free_list = &new_src;
        }
        catch (...)
        {
        }
    }

    if (!m_free_list)
        return nullptr;

    auto* src = m_free_list;
    m_free_list = m_free_list->next;

    return src;
}

void GPUPainter::free_source(GPUPainter::GPUPainterSource* src)
{
    if (!src)
        return;

    src->next = m_free_list;
    m_free_list = src;
}

bool GPUPainter::mask(const Surface& surface, const Point& point, const Rect& rect)
{
    /* user_rect is in user coordinates. */
    Rect user_rect(point, surface.size());
    if (!rect.empty())
        user_rect = Rect::intersection(user_rect, rect);
    if (user_rect.empty())
        return true;

    if (!m_painter.target().impl().is_gpu_capable())
    {
        EGTLOG_TRACE("target {} is not GPU capable.", (void*)&m_painter.target());
        return false;
    }
    auto& target = m_painter.target().impl().gpu_surface();

    if (!surface.impl().is_gpu_capable())
    {
        EGTLOG_TRACE("source surface is not GPU capable.");
        return false;
    }
    auto& mask = surface.impl().gpu_surface();

    double red, green, blue, alpha;
    GPUPainterSource* src = nullptr;

    cairo_t* cr = m_painter.context();
    cairo_pattern_t* pattern = cairo_get_source(cr);
    switch (cairo_pattern_get_type(pattern))
    {
    default:
        EGTLOG_TRACE("unsupported source type.");
        return false;

    case CAIRO_PATTERN_TYPE_SOLID:
        if (cairo_pattern_get_rgba(pattern, &red, &green, &blue, &alpha) != CAIRO_STATUS_SUCCESS)
        {
            EGTLOG_TRACE("cannot extract RGBA components of source solid pattern.");
            return false;
        }
        break;

    case CAIRO_PATTERN_TYPE_SURFACE:
        src = (GPUPainterSource*)cairo_pattern_get_user_data(pattern, &m_key);
        if (!src)
        {
            EGTLOG_TRACE("source surface is not GPU capable.");
            return false;
        }
        break;
    }

    Point offset;
    if (!get_transformation(offset))
        return false;

    if (src)
    {
        auto& source = *src->surface;

        /*
         * Clip the user rectangle with the source rectangle (both in user
         * coordinates).
         */
        Rect source_rect(src->origin - offset, source.owner().size());
        user_rect = Rect::intersection(user_rect, source_rect);
        if (user_rect.empty())
            return true;
    }

    auto clip = get_clip_region();
    if (!clip)
        return false;

    m_rects.clear();
    if (!fill_rectangle(*clip, user_rect))
        return false;

    if (m_rects.empty())
    {
        EGTLOG_TRACE("no rectangle to draw.");
        return true;
    }

    auto rects = prepare_m2d_rectangles(offset);
    if (!rects)
        return false;

    auto* tmp = target.tmp_buffer();
    if (!tmp)
        return false;

    bool has_source = false;
    if (src)
    {
        auto& source = *src->surface;

        EGTLOG_TRACE("paint GPU object {} source with GPU object {} mask into GPU object {} destination.",
                     source.id(), mask.id(), target.id());

        source.sync_for_gpu();

        m2d_set_source(M2D_SRC, source,
                       static_cast<dim_t>(src->origin.x()),
                       static_cast<dim_t>(src->origin.y()));
        has_source = true;
    }
    else
    {
        EGTLOG_TRACE("paint color ({},{},{},{}) with GPU object {} mask into GPU object {} destination.",
                     (uint8_t)(red * 255.),
                     (uint8_t)(green * 255.),
                     (uint8_t)(blue * 255.),
                     (uint8_t)(alpha * 255.),
                     mask.id(), target.id());

        m2d_source_color(red * 255., green * 255., blue * 255., alpha * 255.);
    }

    mask.sync_for_gpu();
    target.sync_for_gpu();

    m2d_source_enable(M2D_SRC, has_source);
    m2d_source_enable(M2D_DST, true);
    m2d_blend_enable(true);

    const Point mask_origin = offset + point;
    m2d_set_source(M2D_DST, mask,
                   static_cast<dim_t>(mask_origin.x()),
                   static_cast<dim_t>(mask_origin.y()));
    m2d_set_target(tmp);

    /*
     * Keep the source color but multiply the source alpha with the
     * destination alpha:
     * Cr =  1 * Cs + 0 * Cd
     * Ar = Ad * As + 0 * Ad
     */
    m2d_blend_factors(M2D_BLEND_ONE, M2D_BLEND_ZERO,
                      M2D_BLEND_DST_ALPHA, M2D_BLEND_ZERO);

    m2d_draw_rectangles(rects.get(), m_rects.size());

    m2d_source_color(255, 255, 255, 255);
    m2d_source_enable(M2D_SRC, true);
    m2d_source_enable(M2D_DST, false);
    m2d_set_source(M2D_SRC, tmp, 0, 0);
    m2d_set_target(target);

    /*
     * Reset classical blend parameters:
     * Cr = As * Cs + (1 - As) * Cd
     * Ar =  1 * As + (1 - As) * Ad
     */
    m2d_blend_factors(M2D_BLEND_SRC_ALPHA, M2D_BLEND_ONE_MINUS_SRC_ALPHA,
                      M2D_BLEND_ONE, M2D_BLEND_ONE_MINUS_SRC_ALPHA);

    m2d_draw_rectangles(rects.get(), m_rects.size());

    return true;
}

bool GPUPainter::draw(const Surface& surface, const Point& point, const Rect& rect)
{
    /* user_rect is in user coordinates. */
    Rect user_rect(point, surface.size());
    if (!rect.empty())
        user_rect = Rect::intersection(user_rect, rect);
    if (user_rect.empty())
        return true;

    if (!m_painter.target().impl().is_gpu_capable())
    {
        EGTLOG_TRACE("target {} is not GPU capable.", (void*)&m_painter.target());
        return false;
    }
    auto& target = m_painter.target().impl().gpu_surface();

    if (!surface.impl().is_gpu_capable())
    {
        EGTLOG_TRACE("source surface is not GPU capable.");
        return false;
    }
    auto& source = surface.impl().gpu_surface();

    Point offset;
    if (!get_transformation(offset))
        return false;

    auto clip = get_clip_region();
    if (!clip)
        return false;

    m_rects.clear();
    if (!fill_rectangle(*clip, user_rect))
        return false;

    if (m_rects.empty())
    {
        EGTLOG_TRACE("no rectangle to draw.");
        return true;
    }

    auto rects = prepare_m2d_rectangles(offset);
    if (!rects)
        return false;

    source.sync_for_gpu();
    target.sync_for_gpu();

    const auto blend = m_painter.alpha_blending();
    EGTLOG_TRACE("{} GPU object {} source at {} into GPU object {} destination ({}).",
                 blend ? "blend" : "copy",
                 source.id(), point, target.id(), user_rect);

    m2d_blend_enable(blend);
    m2d_source_enable(M2D_SRC, true);

    const Point source_origin = offset + point;
    m2d_set_source(M2D_SRC, source,
                   static_cast<dim_t>(source_origin.x()),
                   static_cast<dim_t>(source_origin.y()));

    m2d_set_target(target);
    m2d_draw_rectangles(rects.get(), m_rects.size());

    return true;
}

bool GPUPainter::draw(const Color& color, const Rect& rect)
{
    if (!m_painter.target().impl().is_gpu_capable())
    {
        EGTLOG_TRACE("target {} is not GPU capable.", (void*)&m_painter.target());
        return false;
    }
    auto& target = m_painter.target().impl().gpu_surface();

    const auto blend = (color.alpha() != 255u && m_painter.alpha_blending());

    Point offset;
    if (!get_transformation(offset))
        return false;

    auto clip = get_clip_region();
    if (!clip)
        return false;

    m_rects.clear();
    if (!fill_rectangle(*clip, rect))
        return false;

    if (m_rects.empty())
    {
        EGTLOG_TRACE("no rectangle to draw.");
        return true;
    }

    auto rects = prepare_m2d_rectangles(offset);
    if (!rects)
        return false;

    target.sync_for_gpu();

    EGTLOG_TRACE("{} GPU object {} destination with color {} ({}).",
                 blend ? "blend" : "fill", target.id(), color, rect);

    m2d_source_color(color.red(), color.green(), color.blue(), color.alpha());
    m2d_source_enable(M2D_SRC, false);
    m2d_blend_enable(blend);
    m2d_set_target(target);

    m2d_draw_rectangles(rects.get(), m_rects.size());

    m2d_source_color(255, 255, 255, 255);

    return true;
}

m2d_rectangle GPUPainter::m_rectangle_array[m_rectangle_array_size];

std::unique_ptr<m2d_rectangle, decltype(GPUPainter::free_rectangles)*>
GPUPainter::prepare_m2d_rectangles(const Point& offset) const
{
    m2d_rectangle* ptr;

    if (m_rects.size() <= m_rectangle_array_size)
        ptr = m_rectangle_array;
    else
        ptr = static_cast<m2d_rectangle*>(malloc(m_rects.size() * sizeof(m2d_rectangle)));

    if (ptr)
    {
        size_t pos = 0;
        for (const auto& rect : m_rects)
        {
            auto& r = ptr[pos++];
            r.x = static_cast<dim_t>(offset.x() + rect.x());
            r.y = static_cast<dim_t>(offset.y() + rect.y());
            r.w = static_cast<dim_t>(rect.width());
            r.h = static_cast<dim_t>(rect.height());
        }
    }
    else
    {
        EGTLOG_ERROR("failed to allocate memory for 'm2d_rectangle' array.");
    }

    return std::unique_ptr<m2d_rectangle, decltype(GPUPainter::free_rectangles)*>(
               ptr, GPUPainter::free_rectangles);
}

void GPUPainter::free_rectangles(m2d_rectangle* rects)
{
    if (rects != m_rectangle_array)
        free(rects);
}

}
}
}
