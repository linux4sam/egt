/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "image.h"
#include "imagecache.h"
#include "painter.h"

using namespace std;

namespace mui
{

    Image::Image(const string& filename, int x, int y)
        : m_filename(filename),
          m_hscale(1.0),
          m_vscale(1.0)
    {
        m_image = image_cache.get(filename, 1.0);
        assert(m_image.get());

        m_box = Rect(x, y,
                     cairo_image_surface_get_width(m_image.get()),
                     cairo_image_surface_get_height(m_image.get()));
    }

    void Image::scale(double hscale, double vscale)
    {
        if (m_hscale != hscale || m_vscale != vscale)
        {
            damage();

            m_image = image_cache.get(m_filename, hscale, vscale, false);

            size(cairo_image_surface_get_width(m_image.get()),
                 cairo_image_surface_get_height(m_image.get()));
            m_hscale = hscale;
            m_vscale = vscale;

            damage();
        }
    }

    void Image::draw(const Rect& rect)
    {
        // TODO: this needs to apply to all draw() calls.  Don't give a widget a
        // rectangle that is outside of its own box.
        //Rect r = Rect::intersect(rect, box());

        Painter painter(screen()->context());
        painter.draw_image(Rect(rect.point() - box().point(), rect.size()), rect.point(), m_image);
    }

    Image::~Image()
    {

    }

}
