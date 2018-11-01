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
        explicit Image(const std::string& filename, const Point& point = Point());

        explicit Image(Frame& parent,
                       const std::string& filename, const Point& point = Point());

        virtual void draw(Painter& painter, const Rect& rect) override;

        /**
         * Scale the image.
         *
         * Change the size of the widget, similar to calling resize().
         *
         * @param[in] hscale Horizontal scale, with 1.0 being 100%.
         * @param[in] vscale Vertical scale, with 1.0 being 100%.
         * @param[in] approximate Approximate the scale to increase image cache
         *            hit efficiency.
         */
        virtual void scale(double hscale, double vscale, bool approximate = false);

        virtual void resize(const Size& size) override
        {
            if (box().size() != size)
            {
                double hs = (double)size.w / (double)m_orig_size.w;
                double vs = (double)size.h / (double)m_orig_size.h;
                scale(hs, vs);
            }
        }

        /**
         * Get the horizontal scale value.
         */
        double hscale() const { return m_hscale; }

        /**
         * Get the vertical scale value.
         */
        double vscale() const { return m_vscale; }

        shared_cairo_surface_t surface() const { return m_image; }

        virtual ~Image();

    protected:

        std::string m_filename;
        double m_hscale{1.0};
        double m_vscale{1.0};
        shared_cairo_surface_t m_image;

        /**
         * Original image size, used for scaling through a resize() call.
         */
        Size m_orig_size;

    private:

        Image() = delete;
    };

    /**
     * @todo Very similar to ImageLabel, but works a bit different.  Need to reconcile.
     */
    class ImageText : public Image
    {
    public:
        ImageText(const std::string& image,
                  const std::string& text = std::string(),
                  const Point& point = Point(),
                  const Font& font = Font());

        /**
         * Get the text of the label.
         */
        virtual const std::string& text() const { return m_text; }

        /**
         * Set the font of the label.
         */
        virtual void font(const Font& font) { m_font = font; }

        virtual void draw(Painter& painter, const Rect& rect) override;

        virtual void label_enabled(bool value);

        virtual ~ImageText();

    protected:
        std::string m_text;
        Font m_font;
        bool m_label;
    };
}

#endif
