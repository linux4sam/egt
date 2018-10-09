/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_TEXT_H
#define MUI_TEXT_H

/**
 * @file
 * @brief Working with text input.
 */

#include <mui/widget.h>

namespace mui
{

    /**
     * Input text box.
     */
    class TextBox : public Widget
    {
    public:
        TextBox(const std::string& text = std::string(),
                const Point& point = Point(),
                const Size& size = Size());

        virtual int handle(int event);

        virtual void draw(Painter& painter, const Rect& rect);

        virtual ~TextBox();

    protected:
        std::string m_text;
    };

}

#endif
