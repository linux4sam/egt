/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_LABEL_H
#define MUI_LABEL_H

#include <mui/widget.h>

namespace mui
{

    class Label : public Widget
    {
    public:
        explicit Label(const std::string& text,
                       const Point& point = Point(),
                       const Size& size = Size(), int align = ALIGN_CENTER,
                       const Font& font = Font());

        void text(const std::string& str);

        void fgcolor(const Color& color)
        {
            m_fgcolor = color;
        }

        void font(const Font& font)
        {
            m_font = font;
        }

        virtual void draw(const Rect& rect);

        virtual ~Label();

    protected:
        int m_align;
        std::string m_text;
        Font m_font;
        Color m_fgcolor;
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

        int handle(int event);

        virtual void draw(const Rect& rect);

        virtual ~CheckBox();
    protected:
        bool m_checked;
    };

    class ImageLabel : public Label
    {
    public:
        ImageLabel(const std::string& image,
                   const std::string& text = std::string(),
                   const Point& point = Point(),
                   const Size& size = Size(),
                   const Font& font = Font());

        virtual void draw(const Rect& rect);

        virtual ~ImageLabel();
    protected:
        shared_cairo_surface_t m_image;
    };

}

#endif
