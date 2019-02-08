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

#include <egt/palette.h>
#include <egt/geometry.h>
#include <functional>
#include <iostream>

namespace egt
{
inline namespace v1
{

class Widget;
class Painter;

/**
 * Drawable function object.
 *
 * This class defines what is necessary to implement the draw() function of a
 * widget type.
 */
template<class T>
class Drawable
{
public:

    /**
     * Definition of the draw function.
     */
    using draw_t = std::function<void(T& widget, Painter& painter, const Rect& rect)>;

    /**
     * Implementation of the actual draw function.
     */
    virtual void operator()(T& widget, Painter& painter, const Rect& rect) = 0;
};

/**
 * Manager of the Drawable for each widget type.
 *
 * The Button draw() method calls the drawable like:
 * @code
 * Drawer<Button>::draw(*this, painter, rect);
 * @endcode
 *
 *
 * Observing code can change the Button Drawable by calling:
 * @code
 * Drawer<Button>::set_draw([](Button & widget, Painter & painter, const Rect & rect)
 * {
 *     ...
 * });
 * @endcode
 */
template<class T>
class Drawer
{
public:

    /**
     * Set the default Drawable for all buttons.
     */
    static void set_draw(typename Drawable<T>::draw_t d)
    {
        m_drawable = d;
    }

    /**
     * Call the Drawable.
     */
    static void draw(T& widget, Painter& painter, const Rect& rect)
    {
        m_drawable(widget, painter, rect);
    }

private:
    static typename Drawable<T>::draw_t m_drawable;
};

template<class T>
typename Drawable<T>::draw_t Drawer<T>::m_drawable = T::default_draw;

class Color;

/**
 * Theme implements common charactistics of drawing widgets, used mostly
 * by the theme.
 *
 * This can be overloaded and supplied with a different renderer in addiiton to
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
        bottom_border,
        rounded_border,
        borderfill,
        rounded_borderfill,
        rounded_fill,
        rounded_gradient,
        rounded_border_gradient,
    };

    static float DEFAULT_ROUNDED_RADIUS;
    static float DEFAULT_BORDER_WIDTH;

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

    virtual void draw_bottom_border_box(Painter& painter,
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

    virtual void draw_rounded_fill_box(Painter& painter,
                                       const Rect& rect,
                                       const Color& bg);

    virtual void draw_rounded_gradient_box(Painter& painter,
                                           const Rect& rect,
                                           const Color& bg);

    virtual void draw_rounded_border_gradient_box(Painter& painter,
            const Rect& rect,
            const Color& border,
            const Color& bg);

    virtual ~Theme()
    {}

};

/**
 * Get the global theme.
 *
 * This is the point to get/set the global theme.  Keep in mind that
 * themes can be set on individual widgets, in which case, they are
 * disconnected from the global theme.
 */
Theme& global_theme();

void set_global_theme(Theme* theme);

}
}

#endif
