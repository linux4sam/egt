/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "detail/cairoabstraction.h"
#include "detail/dump.h"
#include "egt/color.h"
#include "egt/palette.h"
#include "egt/screen.h"
#include "egt/types.h"
#include "egt/utils.h"
#include <cairo.h>
#include <cassert>
#include <cstring>
#include <numeric>
#include <string>

#ifdef HAVE_SIMD
#include "Simd/SimdLib.hpp"
#endif

namespace egt
{
inline namespace v1
{

Screen::Screen() noexcept
{
    if (getenv("EGT_SCREEN_ASYNC_FLIP"))
        m_async = true;
}

void Screen::flip(const DamageArray& damage)
{
    if (!damage.empty() && index() < m_buffers.size())
    {
        // save the damage to all buffers
        for (auto& b : m_buffers)
            for (const auto& d : damage)
                b.add_damage(d);

        detail::code_timer(false, "copy_to_buffer: ", [&]()
        {
            ScreenBuffer& buffer = m_buffers[index()];
            if ((m_format == PixelFormat::rgb565) ||
                (m_format == PixelFormat::argb8888) ||
                (m_format == PixelFormat::xrgb8888))
            {
                copy_to_buffer(buffer);
            }
            else
            {
                throw std::runtime_error("invalid pixelformat: cario supports only RGB formats");
            }
            // delete all damage from current buffer
            buffer.damage.clear();
        });

        schedule_flip();
    }
}

#ifdef HAVE_SIMD

using View = Simd::View<Simd::Allocator>;

static constexpr std::pair<cairo_format_t, View::Format> simd_formats[] =
{
    {CAIRO_FORMAT_RGB16_565, View::Int16},
    {CAIRO_FORMAT_ARGB32, View::Int32},
    {CAIRO_FORMAT_RGB24, View::Int32},
};

static constexpr View::Format simd_format(cairo_format_t format)
{
    for (const auto& i : simd_formats)
        if (format == i.first)
            return i.second;

    return View::None;
}

static void simd_copy(cairo_surface_t* src_surface,
                      cairo_surface_t* dst_surface,
                      const Screen::DamageArray& damage)
{
    cairo_surface_flush(src_surface);

    auto src = cairo_image_surface_get_data(src_surface);
    auto dst = cairo_image_surface_get_data(dst_surface);

    assert(src);
    assert(dst);

    auto src_format = cairo_image_surface_get_format(src_surface);
    auto dst_format = cairo_image_surface_get_format(dst_surface);

    assert(src_format == dst_format);

    auto src_width = cairo_image_surface_get_width(src_surface);
    auto src_height = cairo_image_surface_get_height(src_surface);

    auto dst_width = cairo_image_surface_get_width(dst_surface);
    auto dst_height = cairo_image_surface_get_height(dst_surface);

    assert(src_width == dst_width);
    assert(src_height == dst_height);

    View srcview(src_width, src_height,
                 cairo_format_stride_for_width(src_format, src_width),
                 simd_format(src_format), src);
    View dstview(dst_width, dst_height,
                 cairo_format_stride_for_width(dst_format, dst_width),
                 simd_format(dst_format), dst);

    if (damage.size() == 1 &&
        damage[0] == Rect(Point(), Size(src_width, src_height)))
    {
        memcpy(dst, src,
               src_width * dst_height * (src_format == CAIRO_FORMAT_RGB16_565 ? 2 : 4));
    }
    else
    {
        for (const auto& rect : damage)
        {
            Simd::Copy(srcview.Region(rect.x(), rect.y(), rect.right(), rect.bottom()),
                       dstview.Region(rect.x(), rect.y(), rect.right(), rect.bottom()).Ref());
        }
    }

    cairo_surface_mark_dirty(dst_surface);
}

void Screen::copy_to_buffer(ScreenBuffer& buffer)
{
    simd_copy(m_surface.impl(), buffer.surface.impl(), buffer.damage);
}
#else
void Screen::copy_to_buffer(ScreenBuffer& buffer)
{
    copy_to_buffer_software(buffer);
}
#endif

static inline bool wireframe_enable()
{
    static int value = 0;
    if (value == 0)
    {
        if (std::getenv("EGT_WIREFRAME_ENABLE"))
            value += 1;
        else
            value -= 1;
    }
    return value == 1;
}

static inline int wireframe_decay()
{
    static int value = -1;
    if (value == -1)
    {
        if (std::getenv("EGT_WIREFRAME_DECAY") && strlen(std::getenv("EGT_WIREFRAME_DECAY")))
            value = std::stoi(std::getenv("EGT_WIREFRAME_DECAY"));
        else
            value = 0;
    }
    return value;
}

static inline bool screen_bandwidth_enable()
{
    static int value = 0;
    if (value == 0)
    {
        if (std::getenv("EGT_SHOW_SCREEN_BANDWIDTH"))
            value += 1;
        else
            value -= 1;
    }
    return value == 1;
}

static std::vector<std::pair<std::chrono::steady_clock::time_point, Rect>> history;

class DamageBandwidth
{
public:

    DamageBandwidth() noexcept
    {
        start();
    }

    /**
     * Start/reset the counter.
     */
    void start()
    {
        m_start = std::chrono::steady_clock::now();
        m_frames.clear();
    }

    /**
     * Call at the end of every frame.
     */
    void end_frame(size_t bytes)
    {
        m_frames.push_back(bytes);

        const auto now = std::chrono::steady_clock::now();
        const auto diff = std::chrono::duration<double>(now - m_start).count();
        if (diff > 1.0)
        {
            auto total = std::accumulate(m_frames.begin(), m_frames.end(), 0ULL);
            m_bps = total / diff;
            m_ready = true;
            start();
        }
    }

    /**
     * Is any calculation ready?
     */
    EGT_NODISCARD bool ready() const { return m_ready; }

    /**
     * Retrieve the current FPS value.
     */
    std::string value()
    {
        m_ready = false;
        return pretty(m_bps);
    }

protected:

    static std::string pretty(size_t bytes)
    {
        const char* suffixes[] =
        {
            "B/s",
            "kB/s",
            "MB/s",
            "GB/s",
            "TB/s",
            "PB/s",
            "EB/s"
        };
        const auto div = 1000;
        size_t s = 0;
        double count = bytes;
        while (count >= div && s < 6)
        {
            s++;
            count /= div;
        }
        return fmt::format("{:.2f} {}", count, suffixes[s]);
    }

    /// Start time
    std::chrono::time_point<std::chrono::steady_clock> m_start{};

    /// Number of frames recorded since start time.
    std::vector<size_t> m_frames;

    /// Calculated Kbs
    float m_bps{0.};

    /// Is m_fps valid and ready?
    bool m_ready{false};
};

static DamageBandwidth bandwidth;

static size_t pixel_bytes(PixelFormat format)
{
    switch (format)
    {
    case PixelFormat::rgb565:
        return 2;
    case PixelFormat::argb8888:
    case PixelFormat::xrgb8888:
        return 4;
    default:
        break;
    }

    throw std::runtime_error("unable to convert format to bytes");
}

void Screen::copy_to_buffer_software(ScreenBuffer& buffer)
{
    // create a new context for each frame
    unique_cairo_t cr(cairo_create(buffer.surface.impl()));

    cairo_set_source_surface(cr.get(), m_surface.impl(), 0, 0);
    cairo_set_operator(cr.get(), CAIRO_OPERATOR_SOURCE);

    if (!wireframe_enable())
    {
        for (const auto& rect : buffer.damage)
        {
            cairo_rectangle(cr.get(), rect.x(), rect.y(), rect.width(), rect.height());
            if (screen_bandwidth_enable())
            {
                bandwidth.end_frame(rect.width() * rect.height() * pixel_bytes(m_format));
                if (bandwidth.ready())
                    fmt::print("screen bandwidth: {}\n", bandwidth.value());
            }
        }

        cairo_fill(cr.get());
    }
    else
    {
        const auto start = std::chrono::steady_clock::now();

        // paint whole source surface!
        cairo_paint(cr.get());

        cairo_set_line_width(cr.get(), 1);

        auto decay = wireframe_decay();
        if (decay)
        {
            cairo_set_source_rgba(cr.get(), .36, .67, .93, 1.0);

            for (auto i = history.begin(); i != history.end();)
            {
                bool ignore = false;
                for (const auto& rect : buffer.damage)
                {
                    if (rect == i->second)
                    {
                        ignore = true;
                        break;
                    }
                }

                const auto diff = start - i->first;
                if (ignore || diff > std::chrono::milliseconds(decay))
                {
                    i = history.erase(i);
                }
                else
                {
                    cairo_rectangle(cr.get(), i->second.x(), i->second.y(),
                                    i->second.width(), i->second.height());
                    ++i;
                }
            }
            cairo_stroke(cr.get());
        }

        cairo_set_source_rgba(cr.get(), .36, .99, .04, 1);

        for (const auto& rect : buffer.damage)
        {
            if (decay)
                history.emplace_back(std::make_pair(start, rect));
            cairo_rectangle(cr.get(), rect.x(), rect.y(), rect.width(), rect.height());
            if (screen_bandwidth_enable())
            {
                bandwidth.end_frame(rect.width() * rect.height() * pixel_bytes(m_format));
                if (bandwidth.ready())
                    fmt::print("screen bandwidth: {}\n", bandwidth.value());
            }
        }
        cairo_stroke(cr.get());
    }

    cairo_surface_flush(buffer.surface.impl());
}

void Screen::damage_algorithm(Screen::DamageArray& damage, Rect rect)
{
    if (rect.empty())
        return;

    // work backwards for a stronger hit chance for existing rectangles
    for (auto i = damage.rbegin(); i != damage.rend();)
    {
        // exact rectangle already exists; done
        if (*i == rect)
            return;

        // if this rectangle intersects an existing rectangle, then merge
        // the rectangles and start over
        if (rect.intersect(*i))
        {
            rect = Rect::merge(*i, rect);
            damage.erase(std::next(i).base());
            i = damage.rbegin();
            continue;
        }

        ++i;
    }

    // if we get here, no intersect found so add it
    damage.emplace_back(rect);
}

static inline bool no_composition_buffer()
{
    static int value = 0;
    if (value == 0)
    {
        if (std::getenv("EGT_NO_COMPOSITION_BUFFER"))
            value += 1;
        else
            value -= 1;
    }
    return value == 1;
}

void Screen::init(void** ptr, uint32_t count, const Size& size, PixelFormat format)
{
    m_size = size;

    m_buffers.clear();

    if (count == 1 && no_composition_buffer())
    {
        m_surface = Surface(ptr[0], nullptr, size, format,
                            Surface::stride(format, size.width()));
    }
    else
    {
        for (uint32_t x = 0; x < count; x++)
        {
            m_buffers.emplace_back(Surface(ptr[x], nullptr, size, format,
                                           Surface::stride(format, size.width())));
            m_buffers.back().damage.emplace_back(Point(), size);
        }

        m_surface = Surface(size, format);
    }

    m_painter = std::make_unique<Painter>(m_surface);

    m_format = format;
}

void Screen::low_fidelity()
{
    m_painter->low_fidelity();
}

void Screen::high_fidelity()
{
    m_painter->high_fidelity();
}


size_t Screen::max_brightness() const
{
    return 100;
}

size_t Screen::brightness() const
{
    return 100;
}

void Screen::brightness(size_t brightness)
{
    detail::ignoreparam(brightness);
}

}
}
