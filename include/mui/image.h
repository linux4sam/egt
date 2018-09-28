/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_IMAGE_H
#define MUI_IMAGE_H

/**
 * @file
 * @brief Working with images.
 */

#include <mui/widget.h>

namespace mui
{

    /**
     * Basic widget that displays an image.
     */
    class Image : public Widget
    {
    public:
        Image(const std::string& filename, const Point& point = Point());

        virtual void draw(const Rect& rect);

        virtual void scale(double hscale, double vscale);

        // hmm, should resize really mean scale like this?
        virtual void resize(const Size& size)
        {
            if (box().size() != size)
            {
                double hs = (double)box().w / (double)size.w;
                double vs = (double)box().h / (double)size.h;
                scale(hs, vs);
            }
        }

        double hscale() const
        {
            return m_hscale;
        }

        double vscale() const
        {
            return m_vscale;
        }

        shared_cairo_surface_t surface() const
        {
            return m_image;
        }

        virtual ~Image();

    protected:

        Image() {}

        shared_cairo_surface_t m_image;
        std::string m_filename;
        double m_hscale;
        double m_vscale;
    };

}

#endif
