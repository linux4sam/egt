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
#include <egt/detail/meta.h>
#include <egt/flags.h>
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
    /// Reset function type.
    using ResetFunction = std::function<void()>;

    /// Add a custom reset function.
    static inline void add(const ResetFunction& func)
    {
        m_reset_list.push_back(func);
    }

    /// Execute reset functions.
    static inline void reset()
    {
        for (auto& x : m_reset_list)
            x();
        m_reset_list.clear();
    }

private:

    /// Reset function array.
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
// NOLINTNEXTLINE(cert-err58-cpp)
typename Drawable<T>::DrawFunction Drawer<T>::m_drawable = T::default_draw;

class Color;

/**
 * Customizable characteristics for drawing widgets.
 *
 * Theme implements common characteristics of drawing widgets. This can be
 * overloaded and supplied with different properties like Palette and Font
 * to change how a Widget is rendered globally.
 */
class EGT_API Theme
{
public:

    /**
     * Fill flags are used to characterize how a widget's background and border
     * should be drawn.
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
    using FillFlags = Flags<FillFlag>;

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
        drop_shadow = detail::bit(4),
    };

    /// Border flags
    using BorderFlags = Flags<BorderFlag>;

    Theme();

    Theme(const Theme&) = default;
    Theme& operator=(const Theme&) = default;
    Theme(Theme&&) noexcept = default;
    Theme& operator=(Theme&&) noexcept = default;

    /// Get the name of the Object.
    EGT_NODISCARD const std::string& name() const { return m_name; }

    /**
     * Set the name of the Object.
     *
     * Assigns a human readable name to an Object that can then be used to
     * find timers by name or debug.
     *
     * @param[in] name Name to set for the Object.
     */
    void name(const std::string& name) { m_name = name; }

    /**
     * Get a reference to the theme Palette.
     */
    Palette& palette()
    {
        return m_palette;
    }

    /**
     * Get a const reference to the theme Palette.
     */
    const Palette& palette() const
    {
        return m_palette;
    }

    /**
     * Set the theme palette.
     */
    void palette(const Palette& palette)
    {
        m_palette = palette;
    }

    /**
     * Get a reference to the theme Font.
     */
    Font& font()
    {
        return m_font;
    }

    /**
     * Get a const reference to the theme Font.
     */
    const Font& font() const
    {
        return m_font;
    }

    /**
     * Set the theme Font.
     */
    void font(const Font& font)
    {
        m_font = font;
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

    /**
     * Draw a circle using properties directly from the widget.
     */
    virtual void draw_circle(Painter& painter,
                             const Widget& widget,
                             Palette::ColorId bg,
                             Palette::ColorId border) const;

    /**
     * Draw a circle specifying the properties directly.
     */
    virtual void draw_circle(Painter& painter,
                             const FillFlags& type,
                             const Rect& rect,
                             const Pattern& border,
                             const Pattern& bg,
                             DefaultDim border_width = 0,
                             DefaultDim margin_width = 0) const;

    /// Get the default border width
    virtual DefaultDim default_border() const
    {
        return 2;
    }

    /// Get the default border radius
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

    virtual ~Theme() noexcept = default;

protected:

    explicit Theme(const std::string& name);

    inline void rounded_box(Painter& painter, const Rect& box, float border_radius) const
    {
        rounded_box(painter, RectF(box.x(), box.y(), box.width(), box.height()), border_radius);
    }

    virtual void rounded_box(Painter& painter, const RectF& box, float border_radius) const;

    /// Palette instance used by the theme.
    Palette m_palette;

    /// A user defined name for the Object.
    std::string m_name;

    /// Default font instance used by the theme.
    Font m_font;

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

/// BorderFlags operator
inline Theme::BorderFlags operator|(Theme::BorderFlag lhs, Theme::BorderFlag rhs)
{
        return {lhs, rhs};
}

/// Enum string conversion map
template<>
EGT_API const std::pair<Theme::FillFlag, char const*> detail::EnumStrings<Theme::FillFlag>::data[2];

/// Enum string conversion map
template<>
EGT_API const std::pair<Theme::BorderFlag, char const*> detail::EnumStrings<Theme::BorderFlag>::data[5];

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
EGT_API void global_theme(std::unique_ptr<Theme>&& theme);

}
}

#endif
