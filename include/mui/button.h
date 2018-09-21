/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_BUTTON_H
#define MUI_BUTTON_H

/**
 * @file
 * @brief Working with buttons.
 */

#include <mui/widget.h>

namespace mui
{

    /**
     * Basic button widget.
     */
    class Button : public Widget
    {
    public:

        Button(const std::string& label, const Point& point = Point(),
               const Size& size = Size());

        int handle(int event);

        virtual void draw(const Rect& rect);

        virtual ~Button();

    protected:
        std::string m_label;
    };

    /**
     * A button widget that can have an image and/or a label on it.
     */
    class ImageButton : public Button
    {
    public:
        ImageButton(const std::string& image,
                    const std::string& label = "",
                    const Point& point = Point(),
                    const Size& size = Size(),
                    bool border = true);

        virtual void draw(const Rect& rect);

        virtual void font(const Font& font)
        {
            m_font = font;
        }

        virtual void set_image(const std::string& image);

        virtual void set_image_align(uint32_t align)
        {
            m_image_align = align;
        }

        virtual void set_label_align(uint32_t align)
        {
            m_label_align = align;
        }

        virtual ~ImageButton();

    protected:
        shared_cairo_surface_t m_image;
        Font m_font;
        bool m_border;
        uint32_t m_image_align;
        uint32_t m_label_align;
    };

}

#endif
