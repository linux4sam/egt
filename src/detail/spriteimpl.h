/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SRC_DETAIL_SPRITEIMPL_H
#define EGT_SRC_DETAIL_SPRITEIMPL_H

#include <egt/geometry.h>
#include <egt/image.h>
#include <vector>

namespace egt
{
inline namespace v1
{
class Painter;

namespace detail
{

/**
 * Base class for sprite functionality.
 */
class SpriteImpl
{
public:
    SpriteImpl(Image image, const Size& frame_size,
               int framecount, const Point& frame_point)
        : m_image(std::move(image)),
          m_frame(frame_size),
          m_index(0)
    {
        m_strip = add_strip(framecount, frame_point);
    }

    // special functions deleted because they are never used
    SpriteImpl(const SpriteImpl&) = delete;
    SpriteImpl& operator=(const SpriteImpl&) = delete;
    SpriteImpl(SpriteImpl&&) noexcept = delete;
    SpriteImpl& operator=(SpriteImpl&&) noexcept = delete;

    /**
     * Jump to the specified frame index.
     */
    virtual void show_frame(int index) = 0;

    virtual void draw(Painter& painter, const Rect& rect) = 0;

    /**
     * Advance to the next frame in the strip.
     */
    virtual void advance()
    {
        auto index = m_index;
        if (++index >= m_strips[m_strip].framecount)
            index = 0;

        show_frame(index);
    }

    /**
     * Returns true if the current frame is the last frame.
     */
    virtual bool is_last_frame() const
    {
        return m_index >= m_strips[m_strip].framecount - 1;
    }

    /**
     * Returns the number of frames in the current strip.
     */
    virtual uint32_t frame_count() const
    {
        return m_strips[m_strip].framecount;
    }

    /**
     * Information about a single sprite strip.
     */
    struct strip
    {
        int framecount{0};
        Point point;
    };

    /**
     * Change the strip to the specified id.
     */
    virtual void change_strip(uint32_t id)
    {
        if (id < m_strips.size() && id != m_strip)
        {
            m_strip = id;
            m_index = 0;
        }
    }

    /**
     * Add a new strip.
     */
    uint32_t add_strip(int framecount, const Point& point)
    {
        strip s;
        s.framecount = framecount;
        s.point = point;
        m_strips.push_back(s);
        return m_strips.size() - 1;
    }

    virtual ~SpriteImpl() = default;

protected:

    /**
     * Get the frame origin.
     */
    virtual Point get_frame_origin(int index) const
    {
        Point origin;
        if (m_image.empty())
            return origin;

        auto imagew = m_image.size().width();
        origin.x(m_strips[m_strip].point.x() + (index * m_frame.width()));
        origin.y(m_strips[m_strip].point.y());

        // support sheets that have frames on multiple rows
        if (origin.x() + m_frame.width() > imagew)
        {
            auto x = m_strips[m_strip].point.x() + (index * m_frame.width());

            origin.x((static_cast<uint32_t>(x) % static_cast<uint32_t>(imagew))); //NOLINT
            origin.y(((x / imagew) * m_strips[m_strip].point.y()) + (x / imagew) * m_frame.height());
        }

        return origin;
    }

    /**
     * The sprite sheet image.
     */
    Image m_image;

    /**
     * The size of an individual frame.
     */
    Size m_frame;

    /**
     * The current frame index.
     */
    int m_index{-1};

    using strip_array = std::vector<strip>;

    /**
     * Configuration for each of the available strips.
     */
    strip_array m_strips;

    /**
     * The current strip being used.
     */
    uint32_t m_strip{0};
};

}
}
}

#endif
