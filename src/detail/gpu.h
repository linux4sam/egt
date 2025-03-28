/*
 * Copyright (C) 2024 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SRC_DETAIL_GPU_H
#define EGT_SRC_DETAIL_GPU_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/detail/meta.h"
#include "egt/geometry.h"
#include "egt/types.h"

#include <cairo.h>
#ifdef HAVE_LIBM2D
#include <m2d/m2d.h>
#endif
#include <deque>

namespace egt
{
inline namespace v1
{

class Painter;
class Surface;

namespace detail
{

#ifndef HAVE_LIBM2D
static inline void gpu_init() {}
static inline void gpu_cleanup() {}
#else
void gpu_init();
void gpu_cleanup();

class GPUSurface
{
public:

    GPUSurface(Surface* surface, int prime_fd);
    GPUSurface(Surface* surface, void** data);

    GPUSurface(const GPUSurface&) = delete;
    GPUSurface(GPUSurface&& rhs) noexcept = default;

    GPUSurface& operator=(const GPUSurface&) = delete;
    GPUSurface& operator=(GPUSurface&& rhs) noexcept = default;

    operator m2d_buffer* () const { return m_buf.get(); }

    m2d_buffer* tmp_buffer();

    using GPUObjectId = int64_t;
    EGT_NODISCARD GPUObjectId id() const { return m_id; }

    void sync_for_cpu();
    void sync_for_gpu();
    void flush(bool claimed_by_cpu);

    /* Only called from Surface(Surface&&) or Surface::operator=(Surface&&). */
    void owner(Surface* surface) { m_surface = surface; }
    EGT_NODISCARD Surface& owner() const { return *m_surface; }

private:

    struct m2d_buffer_deleter
    {
        void operator()(m2d_buffer* buf) { m2d_free(buf); }
    };

    void cpu_flush();
    void gpu_flush();

    GPUObjectId m_id{-1};
    Surface* m_surface{nullptr};
    std::unique_ptr<m2d_buffer, m2d_buffer_deleter> m_buf;
    std::unique_ptr<m2d_buffer, m2d_buffer_deleter> m_tmp;
    bool m_owned_by_gpu{true};

    static GPUObjectId next_id;
};

class GPUPainter
{
public:

    GPUPainter(Painter& painter)
        : m_painter(painter)
    {}

    bool fill() { return draw(Operator::fill); }
    bool paint() { return draw(Operator::paint); }

    void sync_for_cpu(bool skip_source = false);

    void source(const Surface& surface, const Point& point);

protected:

    Painter& m_painter;
    cairo_user_data_key_t m_key;

    std::deque<Rect> m_rects;

    enum class PathState
    {
        empty,
        start,
        x, /* the direction is unknown yet (width or height is 0) but keep going */
        r,
        rd,
        rdl,
        ru,
        rul,
        l,
        ld,
        ldr,
        lu,
        lur,
        d,
        dr,
        dru,
        dl,
        dlu,
        u,
        ur,
        urd,
        ul,
        uld,
    };

    Point m_path_current_point;
    Size m_path_size;
    PathState m_path_state{PathState::empty};
    bool parse_path_move_to(const Point& p);
    bool parse_path_line_to(const Point& p);
    bool parse_path_close_path(const cairo_rectangle_list_t& clip_rectangles);

    enum class Operator
    {
        fill,
        paint,
    };

    struct GPUPainterSource
    {
        Point origin;
        GPUSurface* surface{nullptr};
        GPUPainterSource* next{nullptr};
    };

    bool get_transformation(Point& offset);
    std::unique_ptr<cairo_rectangle_list_t, decltype(cairo_rectangle_list_destroy)*>
    get_clip_region() const;

    bool draw(Operator op);
    bool fill(const cairo_rectangle_list_t& clip_rectangles);
    bool paint(const cairo_rectangle_list_t& clip_rectangles,
               const Point& offset, GPUPainterSource* src);

    static constexpr size_t m_rectangle_array_size = 256u;
    static m2d_rectangle m_rectangle_array[m_rectangle_array_size];
    static void free_rectangles(m2d_rectangle* rects);
    std::unique_ptr<m2d_rectangle, decltype(GPUPainter::free_rectangles)*>
    prepare_m2d_rectangles(const Point& offset) const;

    static bool is_translation(const cairo_matrix_t& matrix, Point& offset);

    static std::deque<GPUPainterSource> m_source_cache;
    static GPUPainterSource* m_free_list;

    static GPUPainterSource* alloc_source();
    static void free_source(GPUPainterSource* src);
    static void source_destroy(void* src) { free_source(reinterpret_cast<GPUPainterSource*>(src)); }
};

#endif /* HAVE_LIBM2D */

}
}
}

#endif
