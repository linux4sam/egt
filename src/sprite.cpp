/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
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

    shared_cairo_surface_t ISpriteBase::surface() const
    {
        Point origin = get_frame_origin(m_index);

        // cairo_surface_create_for_rectangle() would work here with one
        // exception - the resulting image has no width and height

        shared_cairo_surface_t copy =
            shared_cairo_surface_t(cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                   m_frame.w,
                                   m_frame.h),
                                   cairo_surface_destroy);

        shared_cairo_t cr = shared_cairo_t(cairo_create(copy.get()), cairo_destroy);
        cairo_set_source_surface(cr.get(), m_image.surface().get(), -origin.x, -origin.y);
        cairo_rectangle(cr.get(), 0, 0, m_frame.w, m_frame.h);
        cairo_set_operator(cr.get(), CAIRO_OPERATOR_SOURCE);
        cairo_fill(cr.get());

        return copy;
    }

#ifdef HAVE_LIBPLANES
    HardwareSprite::HardwareSprite(const std::string& filename, const Size& frame_size,
                                   int framecount, const Point& frame_point,
                                   const Point& point)
        : PlaneWindow(Size(), widgetmask::WINDOW_DEFAULT | widgetmask::NO_BACKGROUND,
                      DRM_FORMAT_ARGB8888),
          ISpriteBase(filename, frame_size, framecount, frame_point)
    {
        add(&m_image);

        resize(m_image.size());

        KMSOverlayScreen* s = reinterpret_cast<KMSOverlayScreen*>(screen());
        plane_set_pan_pos(s->s(), m_strips[m_strip].point.x, m_strips[m_strip].point.y);
        plane_set_pan_size(s->s(), m_frame.w, m_frame.h);

        // hack to change the size because the screen size and the box size are different
        move(point);
        m_box = Rect(point, frame_size);

        damage();
    }

    void HardwareSprite::show_frame(int index)
    {
        if (index != m_index)
        {
            m_index = index;

            Point origin = get_frame_origin(m_index);

            if (visible())
            {
                KMSOverlayScreen* s = reinterpret_cast<KMSOverlayScreen*>(screen());
                plane_set_pan_pos(s->s(), origin.x, origin.y);
                plane_set_pan_size(s->s(), m_frame.w, m_frame.h);
                plane_apply(s->s());
            }
        }
    }

    HardwareSprite::~HardwareSprite()
    {}
#endif

    SoftwareSprite::SoftwareSprite(const std::string& filename, const Size& frame_size,
                                   int framecount, const Point& frame_point,
                                   const Point& point)
        : Widget(Rect(point, frame_size)),
          ISpriteBase(filename, frame_size, framecount, frame_point)
    {
        m_box = Rect(point, frame_size);
    }

    void SoftwareSprite::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        Point origin = get_frame_origin(m_index);

        painter.draw_image(Rect(origin.x, origin.y, m_frame.w, m_frame.h),
                           box().point(), m_image.surface());
    }

    void SoftwareSprite::show_frame(int index)
    {
        if (index != m_index)
        {
            m_index = index;
            damage();
        }
    }

    SoftwareSprite::~SoftwareSprite()
    {
    }

}
