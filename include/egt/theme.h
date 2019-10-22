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

#include <egt/detail/meta.h>
#include <egt/bitmask.h>
#include <egt/font.h>
#include <egt/geometry.h>
#include <egt/palette.h>
#include <functional>
#include <memory>

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
 * Drawer<Button>::draw([](Button & widget, Painter & painter, const Rect & rect)
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
    static void draw(typename Drawable<T>::draw_t d)
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
 * Customizable characteristics for drawing widgets.
 *
 * Theme implements common characteristics of drawing widgets. This can be
 * overloaded and supplied with different properties like Palette and Font
 * to change how Widget are rendered globally.
 */
class EGT_API Theme
{
public:

    using pattern_type = Pattern;

    /**
     * Box types used to characterize how a widget's box should be drawn.
     */
    enum class boxtype : uint32_t
    {
        /// do not draw a box
        none = 0,

        /**
         * Overwrite and don't blend.
         * @note This is the same as CAIRO_OPERATOR_SOURCE.
         */
        solid = 1 << 0,

        /// type of fill, if none is set, no fill will be done
        fill = 1 << 1,

        /// type of border, border will always be drawn if border_width > 0
        border_rounded = 1 << 2,
        /// type of border, border will always be drawn if border_width > 0
        border_bottom = 1 << 3,

        fill_rounded = fill | border_rounded,
    };

    static float DEFAULT_ROUNDED_RADIUS;

    Theme() = default;

    Theme(const Theme& rhs)
    {
        if (rhs.m_palette)
            m_palette = detail::make_unique<Palette>(*rhs.m_palette);

        if (rhs.m_font)
            m_font = detail::make_unique<Font>(*rhs.m_font);
    }

    Palette& palette()
    {
        assert(m_palette);
        return *m_palette;
    }

    const Palette& palette() const
    {
        assert(m_palette);
        return *m_palette;
    }

    void palette(Palette& palette)
    {
        assert(m_palette);
        *m_palette = palette;
    }

    Font& font()
    {
        assert(m_font);
        return *m_font;
    }

    const Font& font() const
    {
        assert(m_font);
        return *m_font;
    }

    void font(const Font& font)
    {
        assert(m_font);
        *m_font = font;
    }

    /**
     * Draw a box using properties directly from the widget.
     */
    virtual void draw_box(Painter& painter,
                          const Widget& widget,
                          Palette::ColorId bg,
                          Palette::ColorId border) const;

    /**
     * Draw a box specifying the properties directly.
     */
    virtual void draw_box(Painter& painter,
                          boxtype type,
                          const Rect& rect,
                          const pattern_type& border,
                          const pattern_type& bg,
                          default_dim_type border_width = 0,
                          default_dim_type margin_width = 0) const;

    virtual void draw_circle(Painter& painter,
                             const Widget& widget,
                             Palette::ColorId bg,
                             Palette::ColorId border) const;

    virtual void draw_circle(Painter& painter,
                             boxtype type,
                             const Rect& rect,
                             const pattern_type& border,
                             const pattern_type& bg,
                             default_dim_type border_width = 0,
                             default_dim_type margin_width = 0) const;

    virtual default_dim_type default_border() const
    {
        return 2;
    }

    virtual void apply()
    {
        init_palette();
        init_font();
        init_draw();
    }

    virtual ~Theme() = default;

protected:

    /**
     * Palette instance used by the theme.
     */
    std::unique_ptr<Palette> m_palette{detail::make_unique<Palette>()};

    /**
     * Default font instance used by the theme.
     */
    std::unique_ptr<Font> m_font{detail::make_unique<Font>()};

    virtual void init_palette();

    virtual void init_font();

    virtual void init_draw();
};

ENABLE_BITMASK_OPERATORS(Theme::boxtype);


/**
 * Get the global theme.
 *
 * This is the point to get and modify the global theme.
 *
 * @warning Keep in mind that themes can be set on individual widgets, in which
 * case, they are disconnected from the global theme.
 */
EGT_API Theme& global_theme();

/**
 * Set the global theme.
 *
 * @note This will destroy any pre-existing theme instance.
 */
EGT_API void global_theme(std::shared_ptr<Theme> theme);

}
}

#endif
