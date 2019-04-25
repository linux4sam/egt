/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_WIDGET_H
#define EGT_WIDGET_H

/**
 * @file
 * @brief Base class Widget definition.
 */

#include <cairo.h>
#include <cassert>
#include <cstdint>
#include <egt/detail/object.h>
#include <egt/flags.h>
#include <egt/font.h>
#include <egt/geometry.h>
#include <egt/input.h>
#include <egt/palette.h>
#include <egt/theme.h>
#include <egt/utils.h>
#include <egt/widgetflags.h>
#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

namespace egt
{
inline namespace v1
{

class Painter;
class Frame;
class Screen;

/**
 * Base Widget class.
 *
 * This is the base class for all widgets. A Widget is a thing, a UI component,
 * with a basic set of properties.  In this case, it has a bounding rectangle,
 * some flags, and some states - and these properties can be manipulated. A
 * Widget can handle events, draw itself, and more.  However, the specifics
 * of what it means to handle an event or draw the Widget is implemented in
 * classes that are derived from this one, like a Button or a Label.
 */
class Widget : public detail::Object
{
public:

    /**
     * Common flags used for various widget properties.
     */
    enum class flag
    {
        /**
         * This is an overlay plane window.
         */
        plane_window,

        /**
         * This is a window widget.
         */
        window,

        /**
         * This is a frame.
         */
        frame,

        /**
         * When set, the widget is disabled.
         *
         * Typically, when a widget is disabled it will not accept input.
         *
         * This may change how the widget behaves or is drawn.
         */
        disabled,

        /**
         * When set, the widget will not receive input events.
         */
        readonly,

        /**
         * When true, the widget is active.
         *
         * The active state of a widget is usually a momentary state, unlike
         * focus, which exists until focus is changed. For example, when a button
         * is currently being held down, it its implementation may consider this
         * the active state and choose to draw the button differently.
         *
         * This may change how the widget behaves or is draw.
         */
        active,

        /**
         * When set, the widget is not visible.
         */
        invisible,

        /**
         * Grab related mouse events.
         *
         * For example, if a button is pressed with the eventid::MOUSE_DOWN
         * event, make sure the button gets subsequent mouse events, including
         * the eventid::MOUSE_UP event.
         */
        grab_mouse,

        /**
         * Don't clip the child to drawing only in its box.
         *
         * Use this with caution, it's probably not what you want.
         */
        no_clip,
    };

    using flags_type = Flags<flag>;

    /**
     * @param[in] rect Initial rectangle of the Widget.
     * @param[in] flags Widget flags.
     */
    Widget(const Rect& rect = Rect(),
           const flags_type& flags = flags_type()) noexcept;

    /**
     * @param[in] parent Parent Frame of the widget.
     * @param[in] rect Initial rectangle of the Widget.
     * @param[in] flags Widget flags.
     */
    Widget(Frame& parent, const Rect& rect = Rect(),
           const flags_type& flags = flags_type()) noexcept;

    Widget(const Widget& rhs) noexcept;

    Widget(Widget&& rhs) noexcept;

    Widget& operator=(const Widget& rhs) noexcept;

    Widget& operator=(Widget&& rhs) noexcept;

    /**
     * Perform a deep copy of the widget.
     */
    virtual std::unique_ptr<Widget> clone() = 0;

    /**
     * Draw the widget.
     *
     * To change how a widget is drawn, this function can be overloaded and
     * changed in a derived class, or it can be changed dynamically with the
     * Theme.
     *
     * To optimize drawing, a Widget may use the Rect parameter to limit
     * what needs to be redrawn, which may be smaller than the widget's box(),
     * but will never be outside of the widget's box().
     *
     * Painter will always be supplied in a default state to this function,
     * so there is no need to do any cleanup or state saving inside this
     * draw() function.
     *
     * @warning Normally this should not be called directly and instead the
     * EventLoop will call this function with an already established
     * Painter when the Widget needs to be drawn.
     *
     * @param[in] painter Instance of the Painter for the Screen.
     * @param[in] rect The rectangle to draw.
     *
     */
    virtual void draw(Painter& painter, const Rect& rect) = 0;

    /**
     * Handle an event.
     * Override this function in a derived class to handle events.
     *
     * Only the event ID is passed to this function. To get data associated
     * with the event, you have to call other functions.
     *
     * The default implementation in the Widget class, will dispatch the
     * event to any third party handlers that have been registered. What
     * this means is if you expect other handlers to receive the events
     * then this must be called from derived classes.  Or, manually call
     * Widget::invoke_handlers().
     *
     * @return A return value of non-zero stops the propagation of the event.
     */
    virtual int handle(eventid event);

    /**
     * Resize the widget.
     *
     * Changes the width and height of the widget.
     *
     * @param[in] s The new size of the Widget.
     * @note This will cause a redraw of the widget.
     */
    virtual void resize(const Size& s);

    /**
     * Change the width of the widget.
     *
     * @param[in] w The new width of the widget.
     */
    inline void set_width(default_dim_type w) { resize(Size(w, h())); }

    /**
     * Change the height of the widget.
     * @param[in] h The new height of the widget.
     */
    inline void set_height(default_dim_type h) { resize(Size(w(), h)); }

    /**
     * Move the widget.
     *
     * Changes the x and y position of the widget.
     *
     * @param[in] point The new origin point for the widget relative to its parent.
     * @note This will cause a redraw of the widget.
     */
    virtual void move(const Point& point);

    /**
     * Set the x coordinate of the box.
     *
     * @param[in] x The new origin X value for the widget relative to its parent.
     */
    inline void set_x(default_dim_type x) { move(Point(x, y())); }

    /**
     * Set the y coordinate of the box.
     *
     * @param[in] y The new origin Y value for the widget relative to its parent.
     */
    inline void set_y(default_dim_type y) { move(Point(x(), y)); }

    /**
     * Move the widget to the specified center point.
     *
     * Changes the x and y position of the widget relative to the center point.
     *
     * @note This will cause a redraw of the widget.
     */
    virtual void move_to_center(const Point& point);

    /**
     * Change the box of the widget.
     * This is the same as calling move() and resize() at the same time.
     */
    virtual void set_box(const Rect& rect);

    /**
     * Hide the widget.
     *
     * A widget that is not visible will receive no draw() calls.
     *
     * @note This changes the visible() property of the widget.
     */
    virtual void hide();

    /**
     * Show the widget.
     *
     * @note This changes the visible() property of the widget.
     */
    virtual void show();

    /**
     * Return true if the widget is visible.
     */
    virtual bool visible() const;

    /**
     * Return true if the widget is active.
     *
     * The meaning of active is largely up to the derived implementation.
     */
    virtual bool active() const;

    /**
     * Set the active property of the widget.
     */
    virtual void set_active(bool value);

    /**
     * Return true if the widget is readonly.
     */
    virtual bool readonly() const;

    /**
     * Set the readonly property of the widget.
     */
    virtual void set_readonly(bool value);

    /**
     * Return the disabled status of the widget.
     *
     * When a widget is disabled, it does not receive events. Also, the
     * color scheme may change when a widget is disabled.
     */
    virtual bool disabled() const;

    /**
     * Set the disabled status of the widget to true.
     */
    virtual void disable();

    /**
     * Set the enable status of the widget to true.
     */
    virtual void enable();

    /**
     * Damage the box() of the widget.
     *
     * This is the same as calling damage(box()) in most cases.
     */
    virtual void damage();

    /**
     * Mark the specified rect as a damaged area of the widget.
     *
     * This call will propagate to a top level parent frame that owns a Screen.
     *
     * @param rect The rectangle to save for damage.
     */
    virtual void damage(const Rect& rect);

    /**
     * Bounding box for the widget.
     */
    virtual const Rect& box() const;

    /**
     * Get the size of the Widget.
     * @see Widget::box()
     */
    virtual Size size() const;

    /**
     * Get the origin point of the Widget.
     * @see Widget::box()
     */
    virtual Point point() const;

    /**
     * @{
     * Bounding box dimensions.
     */
    inline default_dim_type w() const { return m_box.w; }
    inline default_dim_type h() const { return m_box.h; }
    inline default_dim_type x() const { return m_box.x; }
    inline default_dim_type y() const { return m_box.y; }
    /** @} */

    /**
     * Get the center point of the widget.
     */
    inline Point center() const { return box().center(); }

    /**
     * Get the widget Palette.
     */
    Palette& palette();

    /**
     * Get the widget Palette.
     */
    const Palette& palette() const;

    /**
     * Set the widget Palette.
     *
     * @param palette The new palette to assign to the widget.
     * @note This will overwrite the entire widget Palette.
     */
    void set_palette(const Palette& palette);

    /**
     * Reset the widget's palette to a default state.
     */
    void reset_palette();

    /**
     * Get a pointer to the parent Frame, or nullptr if none exists.
     */
    Frame* parent();

    /**
     * Get a pointer to the parent Frame, or nullptr if none exists.
     */
    const Frame* parent() const;

    /**
     * Get a pointer to the Screen instance, using using a parent as
     * necessary.
     *
     * @return An Screen if available, or nullptr.
     */
    virtual Screen* screen();

    /**
     * Get a const ref of the flags.
     */
    inline const flags_type& flags() const { return m_widget_flags; }

    /**
     * Get a modifiable ref of the flags.
     */
    inline flags_type& flags() { return m_widget_flags; }

    /**
     * @todo Need a better solution to force non-const call of flags() when
     * necessary.
     */
    inline flags_type& ncflags() { return m_widget_flags; }

    /**
     * Align the widget.
     *
     * This will align the widget relative to the box of its parent frame.
     *
     * @param[in] a The alignmask.
     * @param[in] margin Optional margin size used for alignment.
     */
    virtual void set_align(alignmask a, int margin = 0);

    /**
     * Get the alignment of the widget.
     */
    inline alignmask align() const { return m_align; }

    /**
     * Return the alignment margin.
     */
    inline int margin() const { return m_margin; }

    /**
     * Paint the Widget using Painter.
     *
     * paint() is not part of the normal draw path.  This is a utility
     * function to get the widget to draw its contents to another
     * surface provided with a Painter.
     */
    virtual void paint(Painter& painter);

    /**
     * Draw the widget to a file.
     *
     * @param[in] filename Optional filename to save to.
     */
    virtual void paint_to_file(const std::string& filename = std::string());

    /**
     * Dump the state of the Widget.
     *
     * Dump the state of the Widget to the specified ostream.
     * @param[in,out] out The output stream.
     * @param[in] level Indentation level.
     */
    virtual void dump(std::ostream& out, int level = 0);

    using walk_callback_t = std::function<void(Widget* widget, int level)>;

    virtual void walk(walk_callback_t callback, int level = 0);

    /**
     * Get the current focus state of the widget.
     */
    virtual bool focus() const { return m_focus; }

    /**
     * Set the widget's theme to a new theme.
     */
    void set_theme(const Theme& theme);

    /**
     * Reset the widget's Theme to the default Theme.
     */
    void reset_theme();

    /**
     * Set the boxtype of the widget.
     */
    inline void set_boxtype(const Theme::boxtype& type)
    {
        if (detail::change_if_diff<>(m_boxtype, type))
            damage();
    }

    /**
     * Get the Widget Theme, or the default Theme if none is set.
     */
    Theme& theme();

    /**
     * Move this widgets zorder down relative to other widgets with the same
     * parent.
     */
    virtual void zorder_down();

    /**
     * Move this widgets zorder up relative to other widgets with the same
     * parent.
     */
    virtual void zorder_up();

    /**
     * Detach this widget from its parent.
     */
    void detatch();

    /**
    * Convert a child rectangle to a parent rectangle.
    */
    Rect to_parent(const Rect& r);

    /**
     * Convert a local point to the coordinate system of the display.
     *
     * In other words, work towards the entire display so we can get
     * this point relative to the origin of the display.
     */
    virtual DisplayPoint to_display(const Point& p);

    /**
     * Convert a display point to a local point.
     *
     * In other words, walk towards the current widget to we can
     * get a point relative to the widget's own box.
     */
    virtual Point from_display(const DisplayPoint& p);

    virtual ~Widget() noexcept;

protected:

    /**
     * Helper function to draw this widget's box using the appropriate
     * theme.
     */
    void draw_box(Painter& painter, const Rect& rect = Rect());

    /**
     * Is this widget a top level widget?
     */
    virtual bool top_level() const { return false; }

    /**
     * Set this widget's parent.
     */
    virtual void set_parent(Frame* parent)
    {
        // cannot already have a parent
        assert(!m_parent);

        m_parent = parent;
        damage();
    }

    /**
     * Bounding box.
     */
    Rect m_box;

    /**
     * Pointer to this widget's parent.
     *
     * The parent is a Frame, which is capable of managing children.
     */
    Frame* m_parent{nullptr};

    /**
     * Unique ID of this widget.
     */
    int m_widgetid{0};

private:

    DisplayPoint to_display_back(const Point& p);
    Point from_display_back(const DisplayPoint& p);

    /**
     * Flags for the widget.
     */
    flags_type m_widget_flags{};

    /**
     * Current palette for the widget.
     *
     * @note This should not be accessed directly.  Always use the accessor
     * functions because this is not set until it is modified.
     */
    std::unique_ptr<Palette> m_palette;

    /**
     * Alignment hint for this widget within its parent.
     */
    alignmask m_align{alignmask::none};

    /**
     * Alignment margin.
     */
    int m_margin{0};

    /**
     * Focus state.
     */
    bool m_focus{false};

    /**
     * The boxtype of the widget.
     */
    Theme::boxtype m_boxtype{Theme::boxtype::none};

    /**
     * Current theme for the widget.
     *
     * @note This should not be accessed directly.  Always use the accessor
     * functions because this is not set until it is modified.
     */
    std::unique_ptr<Theme> m_theme;

    friend class Frame;
};

std::ostream& operator<<(std::ostream& os, const Widget::flag& flag);
std::ostream& operator<<(std::ostream& os, const Widget::flags_type& flags);

}
}

#endif
