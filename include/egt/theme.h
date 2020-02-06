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

#include <egt/detail/enum.h>
#include <egt/detail/flags.h>
#include <egt/detail/meta.h>
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
    using DrawFunction = std::function<void(T& widget, Painter& painter, const Rect& rect)>;

    /**
     * Implementation of the actual draw function.
     */
    virtual void operator()(T& widget, Painter& painter, const Rect& rect) = 0;
};

/**
 * Keeps track of what's been changed with Drawer::draw() changes.
 *
 * This provides a utility to reset or undo any of Drawer changes, specifically
 * when switching themes.
 */
class EGT_API DrawerReset
{
public:
    using ResetFunction = std::function<void()>;

    static inline void add(const ResetFunction& func)
    {
        m_reset_list.push_back(func);
    }

    static inline void reset()
    {
        for (auto& x : m_reset_list)
            x();
        m_reset_list.clear();
    }

private:

    static std::vector<ResetFunction> m_reset_list;
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
     * Set the default Drawable for all widgets of type T.
     */
    static void draw(typename Drawable<T>::DrawFunction d)
    {
        DrawerReset::add([]()
        {
            Drawer<T>::draw(T::default_draw);
        });

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
    static typename Drawable<T>::DrawFunction m_drawable;
};

template<class T>
typename Drawable<T>::DrawFunction Drawer<T>::m_drawable = T::default_draw;

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

    /**
     * Fill flags are used to characterize how a widget's background should be
     * drawn.
     *
     * This is an optimization to control whether a solid fill is done, it's
     * blended, or if nothing is set - nothing will be drawn.
     */
    enum class FillFlag : uint32_t
    {
        /**
         * Overwrite and don't blend.
         * @note This is the same as CAIRO_OPERATOR_SOURCE.
         */
        solid = detail::bit(0),
        /// perform a blend
        blend = detail::bit(1),
    };

    /// Fill flags.
    using FillFlags = detail::Flags<FillFlag>;

    /**
     * Border flags allow, when drawing a rectangle, control over what sides of
     * the rectangle are drawn.
     *
     * If no flags are specified, the default assumption is that all flags are
     * drawn.
     */
    enum class BorderFlag : uint32_t
    {
        top = detail::bit(0),
        right = detail::bit(1),
        bottom = detail::bit(2),
        left = detail::bit(3),
    };

    using BorderFlags = detail::Flags<BorderFlag>;

    Theme() = default;

    Theme(const Theme& rhs)
    {
        if (rhs.m_palette)
            m_palette = detail::make_unique<Palette>(*rhs.m_palette);

        if (rhs.m_font)
            m_font = detail::make_unique<Font>(*rhs.m_font);
    }

    /**
     * Get a reference to the theme Palette.
     */
    Palette& palette()
    {
        assert(m_palette);
        return *m_palette;
    }

    /**
     * Get a const reference to the theme Palette.
     */
    const Palette& palette() const
    {
        assert(m_palette);
        return *m_palette;
    }

    /**
     * Set the theme palette.
     */
    void palette(Palette& palette)
    {
        assert(m_palette);
        *m_palette = palette;
    }

    /**
     * Get a reference to the theme Font.
     */
    Font& font()
    {
        assert(m_font);
        return *m_font;
    }

    /**
     * Get a const reference to the theme Font.
     */
    const Font& font() const
    {
        assert(m_font);
        return *m_font;
    }

    /**
     * Set the theme Font.
     */
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
                          const FillFlags& type,
                          const Rect& rect,
                          const Pattern& border,
                          const Pattern& bg,
                          DefaultDim border_width = 0,
                          DefaultDim margin_width = 0,
                          float border_radius = 0.0,
                          const BorderFlags& border_flags = {}) const;

    virtual void draw_circle(Painter& painter,
                             const Widget& widget,
                             Palette::ColorId bg,
                             Palette::ColorId border) const;

    virtual void draw_circle(Painter& painter,
                             const FillFlags& type,
                             const Rect& rect,
                             const Pattern& border,
                             const Pattern& bg,
                             DefaultDim border_width = 0,
                             DefaultDim margin_width = 0) const;

    virtual DefaultDim default_border() const
    {
        return 2;
    }

    virtual float default_border_radius() const
    {
        return 4.0;
    }

    /**
     * Apply the Theme.
     *
     * Automatically called by global_theme() when setting a new theme.
     */
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

    /**
     * Setup for initializing the palette.
     *
     * Called by apply().
     */
    virtual void init_palette();

    /**
     * Setup for initializing the font.
     *
     * Called by apply().
     */
    virtual void init_font();

    /**
     * Setup for initializing drawing.
     *
     * Called by apply().
     */
    virtual void init_draw();
};

template<>
EGT_API const std::map<Theme::FillFlag, char const*> detail::EnumStrings<Theme::FillFlag>::data;

template<>
EGT_API const std::map<Theme::BorderFlag, char const*> detail::EnumStrings<Theme::BorderFlag>::data;

/**
 * Get the global theme.
 *
 * This is the point to get and modify the global theme.
 *
 * @warning Keep in mind that themes can be set on individual widgets, in which
 * case, they are disconnected from the global theme.
 *
 * @return Reference to the current Theme.
 */
EGT_API Theme& global_theme();

/**
 * Set the global theme.
 *
 * This function will call Theme::apply() on the passed in Theme.
 *
 * @note This will destroy any pre-existing theme instance.
 */
EGT_API void global_theme(std::shared_ptr<Theme> theme);

}
}

#endif
