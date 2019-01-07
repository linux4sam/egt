/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/imagecache.h"
#include "egt/kmsscreen.h"
#include "egt/painter.h"
#include "egt/sprite.h"
#include <iostream>
#ifdef HAVE_LIBPLANES
#include <planes/plane.h>
#endif

using namespace std;

namespace egt
{

    /**
     * This pulls out a frame into its own surface.
     */
    static shared_cairo_surface_t frame_surface(const Rect& rect, const Image& image)
    {
        // cairo_surface_create_for_rectangle() would work here with one
        // exception - the resulting image has no width and height

        shared_cairo_surface_t copy =
            shared_cairo_surface_t(cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                   rect.w,
                                   rect.h),
                                   cairo_surface_destroy);

        shared_cairo_t cr = shared_cairo_t(cairo_create(copy.get()), cairo_destroy);
        cairo_set_source_surface(cr.get(), image.surface().get(), -rect.x, -rect.y);
        cairo_rectangle(cr.get(), 0, 0, rect.w, rect.h);
        cairo_set_operator(cr.get(), CAIRO_OPERATOR_SOURCE);
        cairo_fill(cr.get());

        return copy;
    }

#ifdef HAVE_LIBPLANES
    HardwareSprite::HardwareSprite(const Image& image, const Size& frame_size,
                                   int framecount, const Point& frame_point,
                                   const Point& point)
        : Window(Size(), widgetmask::WINDOW_DEFAULT | widgetmask::NO_BACKGROUND,
                 pixel_format::argb8888),
          ISpriteBase(image, frame_size, framecount, frame_point),
          m_label(*this, image)
    {
        do_resize(m_image.size());

        allocate_screen();

        KMSOverlay* s = reinterpret_cast<KMSOverlay*>(screen());
        plane_set_pan_pos(s->s(), m_strips[m_strip].point.x, m_strips[m_strip].point.y);
        plane_set_pan_size(s->s(), m_frame.w, m_frame.h);

        // hack to change the size because the screen size and the box size are different
        move(point);
        m_box = Rect(point, frame_size);

        damage(box());
    }

    void HardwareSprite::show_frame(int index)
    {
        if (index != m_index)
        {
            m_index = index;

            Point origin = get_frame_origin(m_index);

            if (visible())
            {
                KMSOverlay* s = reinterpret_cast<KMSOverlay*>(screen());
                plane_set_pan_pos(s->s(), origin.x, origin.y);
                plane_set_pan_size(s->s(), m_frame.w, m_frame.h);
                plane_apply(s->s());
            }
        }
    }

    shared_cairo_surface_t HardwareSprite::surface() const
    {
        Point origin = get_frame_origin(m_index);
        return frame_surface(Rect(origin, Size(m_frame.w, m_frame.h)), m_image);
    }

    void HardwareSprite::paint(Painter& painter)
    {
        painter.draw_image(point(), Image(surface()));
    }

    HardwareSprite::~HardwareSprite()
    {}
#endif

    SoftwareSprite::SoftwareSprite(const Image& image, const Size& frame_size,
                                   int framecount, const Point& frame_point,
                                   const Point& point)
        : Widget(Rect(point, frame_size)),
          ISpriteBase(image, frame_size, framecount, frame_point)
    {
        m_box = Rect(point, frame_size);
    }

    void SoftwareSprite::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        Point origin = get_frame_origin(m_index);

        painter.draw_image(Rect(origin.x, origin.y, m_frame.w, m_frame.h),
                           box().point(), m_image);
    }

    void SoftwareSprite::show_frame(int index)
    {
        if (index != m_index)
        {
            m_index = index;
            damage();
        }
    }

    shared_cairo_surface_t SoftwareSprite::surface() const
    {
        Point origin = get_frame_origin(m_index);
        return frame_surface(Rect(origin, Size(m_frame.w, m_frame.h)), m_image);
    }

    void SoftwareSprite::paint(Painter& painter)
    {
        painter.draw_image(point(), Image(surface()));
    }

    SoftwareSprite::~SoftwareSprite()
    {
    }

}
