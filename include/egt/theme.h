/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_THEME_H
#define EGT_THEME_H

/**
 * @file
 * @brief Working with themes.
 */

#include <egt/geometry.h>

namespace egt
{
    class Widget;
    class Painter;
    class Color;

    /**
     * Theme implements common charactistics of the widgets.  For example,
     * drawing different box types.
     *
     * This can be overloaded and supplied with a different theme in addiiton to
     * setting up a custom palette.
     */
    class Theme
    {
    public:

        /**
         * Box types used to characterize how a widget's box should be draw.
         */
        enum class boxtype
        {
            none,
            fill,
            border,
            rounded_border,
            borderfill,
            rounded_borderfill,
            rounded_gradient,
        };

        virtual void draw_box(Painter& painter,
                              Widget& widget,
                              boxtype type = boxtype::borderfill,
                              const Rect& rect = Rect());

        virtual void draw_fill_box(Painter& painter,
                                   const Rect& rect,
                                   const Color& bg);

        virtual void draw_border_box(Painter& painter,
                                     const Rect& rect,
                                     const Color& border);

        virtual void draw_border_fill_box(Painter& painter,
                                          const Rect& rect,
                                          const Color& border,
                                          const Color& bg);

        virtual void draw_rounded_border_box(Painter& painter,
                                             const Rect& rect,
                                             const Color& border);

        virtual void draw_rounded_borderfill_box(Painter& painter,
                const Rect& rect,
                const Color& border,
                const Color& bg);

        virtual void draw_rounded_gradient_box(Painter& painter,
                                               const Rect& rect,
                                               const Color& border,
                                               const Color& bg);

        virtual ~Theme()
        {}

    };

    /**
     * Get the default Theme pointer.
     */
    Theme& default_theme();
}

#endif
