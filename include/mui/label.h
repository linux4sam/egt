/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_LABEL_H
#define MUI_LABEL_H

/**
 * @file
 * @brief Working with labels.
 */

#include <mui/widget.h>

namespace mui
{

    /**
     * A Label in its basic form is just some text.
     *
     * It handles no events, has a font and alignment, but otherwise is pretty
     * basic.
     */
    class Label : public Widget
    {
    public:
        explicit Label(const std::string& text = std::string(),
                       const Point& point = Point(),
                       const Size& size = Size(),
                       int align = ALIGN_CENTER,
                       const Font& font = Font());

        /**
         * Set the text of the label.
         */
        virtual void text(const std::string& str);

        /**
         * Get the text of the label.
         */
        virtual const std::string& text() const { return m_text; }

        /**
         * Set the font of the label.
         */
        virtual void font(const Font& font) { m_font = font; }

        /**
         * Set the alignment of the label.
         */
        virtual void align(uint32_t align) { m_align = align; }

        virtual void draw(Painter& painter, const Rect& rect);

        virtual ~Label();

    protected:
        int m_align;
        std::string m_text;
        Font m_font;
    };

    /**
     * Boolean checkbox.
     *
     * @todo This should be a ValueWidget<bool>.
     */
    class CheckBox : public Label
    {
    public:
        CheckBox(const std::string& text = std::string(),
                 const Point& point = Point(),
                 const Size& size = Size());

        bool checked() const
        {
            return m_checked;
        }
        void checked(bool c)
        {
            m_checked = c;
        }

        virtual int handle(int event);

        virtual void draw(Painter& painter, const Rect& rect);

        virtual ~CheckBox();
    protected:
        bool m_checked;
    };

    /**
     * A Label widget that also contains an image.
     */
    class ImageLabel : public Label
    {
    public:
        ImageLabel(const std::string& image,
                   const std::string& text = std::string(),
                   const Point& point = Point(),
                   const Size& size = Size(),
                   const Font& font = Font());

        virtual void draw(Painter& painter, const Rect& rect);

        virtual ~ImageLabel();

    protected:
        shared_cairo_surface_t m_image;
    };

}

#endif
