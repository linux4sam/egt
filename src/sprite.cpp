/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "mui/imagecache.h"
#include "mui/kmsscreen.h"
#include "mui/painter.h"
#include "mui/sprite.h"
#include <iostream>
#ifdef HAVE_LIBPLANES
#include <planes/plane.h>
#endif

using namespace std;

namespace mui
{

    shared_cairo_surface_t ISpriteBase::surface() const
    {
        int panx;
        int pany;
        get_frame_offsets(m_index, panx, pany);

        // cairo_surface_create_for_rectangle() would work here with one
        // exception - the resulting image has no width and height

        shared_cairo_surface_t copy =
            shared_cairo_surface_t(cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                   m_frame.w,
                                   m_frame.h),
                                   cairo_surface_destroy);

        shared_cairo_t cr = shared_cairo_t(cairo_create(copy.get()), cairo_destroy);
        cairo_set_source_surface(cr.get(), m_image.surface().get(), -panx, -pany);
        cairo_rectangle(cr.get(), 0, 0, m_frame.w, m_frame.h);
        cairo_set_operator(cr.get(), CAIRO_OPERATOR_SOURCE);
        cairo_fill(cr.get());

        return copy;
    }

#ifdef HAVE_LIBPLANES
    HardwareSprite::HardwareSprite(const std::string& filename, int framew,
                                   int frameh, int framecount, int framex,
                                   int framey, const Point& point)
        : PlaneWindow(Size(), widgetmask::WINDOW_DEFAULT | widgetmask::NO_BACKGROUND),
          ISpriteBase(filename, framew, frameh, framecount, framex, framey)
    {
        add(&m_image);

        resize(m_image.size());

        KMSOverlayScreen* s = reinterpret_cast<KMSOverlayScreen*>(screen());
        plane_set_pan_pos(s->s(), m_strips[m_strip].framex, m_strips[m_strip].framey);
        plane_set_pan_size(s->s(), m_frame.w, m_frame.h);

        // hack to change the size because the screen size and the box size are different
        move(point);
        m_box = Rect(point.x, point.y, framew, frameh);

        damage();
    }

    void HardwareSprite::show_frame(int index)
    {
        if (index != m_index)
        {
            m_index = index;

            int panx;
            int pany;
            get_frame_offsets(m_index, panx, pany);

            KMSOverlayScreen* s = reinterpret_cast<KMSOverlayScreen*>(screen());
            plane_set_pan_pos(s->s(), panx, pany);
            plane_set_pan_size(s->s(), m_frame.w, m_frame.h);
            plane_apply(s->s());
        }
    }

    HardwareSprite::~HardwareSprite()
    {}
#endif

    SoftwareSprite::SoftwareSprite(const std::string& filename, int framew, int frameh,
                                   int framecount, int framex, int framey,
                                   const Point& point)
        : Widget(Rect(point, Size(framew, frameh))),
          ISpriteBase(filename, framew, frameh, framecount, framex, framey)
    {
        m_box = Rect(point.x, point.y, framew, frameh);
    }

    void SoftwareSprite::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        int panx;
        int pany;
        get_frame_offsets(m_index, panx, pany);

        painter.draw_image(Rect(panx, pany, m_frame.w, m_frame.h),
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
