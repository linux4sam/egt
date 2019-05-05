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
#include <egt/detail/object.h>
#include <egt/flags.h>
#include <egt/font.h>
#include <egt/geometry.h>
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

        /**
         * Tell any parent not to perform layout on this widget.
         */
        no_layout,
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
    explicit Widget(Frame& parent, const Rect& rect = Rect(),
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
     * Scale the current size of the Widget given the ratio.
     *
     * There is no automatic undo for this operation.  Each call to this function
     * uses the current size of the widget.
     *
     * @param[in] hratio Horizontal ratio of w().
     * @param[in] vratio Vertical ratio of h().
     *
     * @see Widget::resize()
     */
    virtual void scale(int hratio, int vratio);

    /**
     * @param[in] ratio Horizontal and vertical ratio of size().
     *
     * @see Widget::scale()
     */
    inline void scale(int ratio)
    {
        scale(ratio, ratio);
    }

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

    inline void toggle_visible()
    {
        if (visible())
            hide();
        else
            show();
    }

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
     * Set the widget Palette.
     *
     * @todo Does this have to be a complete palette?  Should it be merged or
     * just replace completely?
     *
     * @param palette The new palette to assign to the widget.
     * @note This will overwrite the entire widget Palette.
     */
    virtual void set_palette(const Palette& palette);

    /**
     * Reset the widget's palette to a default state.
     */
    virtual void reset_palette();

    /**
     * Get a Widget color.
     *
     * This will return a color for the Palette::GroupId reflecting the current
     * state of the Widget when the call is made.  To get a specific
     * Palette::GroupId color, use the overloaded function.
     *
     * @param id Palette::ColorId to get.
     */
    Palette::pattern_type color(Palette::ColorId id) const;

    /**
     * Get a Widget color.
     */
    Palette::pattern_type color(Palette::ColorId id, Palette::GroupId group) const;

    /**
     * Set a Widget color.
     *
     * @param id Palette::ColorId to set.
     * @param color Color to set.
     * @param group Palette::GroupId to set.
     */
    void set_color(Palette::ColorId id,
                   const Palette::pattern_type& color,
                   Palette::GroupId group = Palette::GroupId::normal);

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
     */
    virtual void set_align(alignmask a);

    /**
     * Get the alignment of the widget.
     */
    inline alignmask align() const { return m_align; }

    /**
     * Set the alignment padding.
     */
    inline void set_padding(default_dim_type padding)
    {
        if (detail::change_if_diff<>(m_padding, padding))
        {
            damage();
            layout();
        }
    }

    /**
     * Return the alignment padding.
     */
    inline default_dim_type padding() const { return m_padding; }

    inline void set_margin(default_dim_type margin)
    {
        if (detail::change_if_diff<>(m_margin, margin))
        {
            damage();
            layout();
        }
    }

    inline default_dim_type margin() const { return m_margin; }

    inline void set_border(default_dim_type border)
    {
        if (detail::change_if_diff<>(m_border, border))
        {
            damage();
            layout();
        }
    }

    inline default_dim_type border() const { return m_border; }

    inline void set_ratio(default_dim_type ratio)
    {
        set_ratio(ratio, ratio);
    }

    inline void set_ratio(default_dim_type horizontal,
                          default_dim_type vertical)
    {
        auto a = detail::change_if_diff<>(m_horizontal_ratio, horizontal);
        auto b = detail::change_if_diff<>(m_vertical_ratio, vertical);
        if (a || b)
            parent_layout();
    }

    inline void set_vertical_ratio(default_dim_type vertical)
    {
        if (detail::change_if_diff<>(m_vertical_ratio, vertical))
            parent_layout();
    }

    inline default_dim_type vertical_ratio() const { return m_vertical_ratio; }

    inline void set_horizontal_ratio(default_dim_type horizontal)
    {
        if (detail::change_if_diff<>(m_horizontal_ratio, horizontal))
            parent_layout();
    }

    inline default_dim_type horizontal_ratio() const { return m_horizontal_ratio; }

    inline void set_yratio(default_dim_type yratio)
    {
        if (detail::change_if_diff<>(m_yratio, yratio))
            parent_layout();
    }

    inline default_dim_type yratio() const { return m_yratio; }

    inline void set_xratio(default_dim_type xratio)
    {
        if (detail::change_if_diff<>(m_xratio, xratio))
            parent_layout();
    }

    inline default_dim_type xratio() const { return m_xratio; }

    /**
     * Get a minimum size hint for the Widget.
     *
     * This is used by sizers to pick minimum and default dimensions when no
     * other force is used.
     */
    virtual Size min_size_hint() const;

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

    using walk_callback_t = std::function<bool(Widget* widget, int level)>;

    /**
     * Walk the Widget tree and call @b callback with each Widget.
     *
     * @param callback Function to call for each widget.
     * @param level The current level of the widget hierarchy.
     */
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

    inline Theme::boxtype boxtype() const
    {
        return m_boxtype;
    }

    /**
     * Get the Widget Theme.
     *
     * If a custom theme was set for the instance, it will be returned.
     * Otherwise, if this widget has a parent it will return the first parent in
     * the widget hierarchy that has a theme.  If no theme is found, the default
     * global theme will be returned.
     */
    const Theme& theme() const;

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
    * Convert a point with a local origin to a parent origin.
    */
    Point to_parent(const Point& r) const;

    /**
     * @see Widget::to_parent().
     */
    inline Rect to_parent(const Rect& r) const
    {
        return Rect(to_parent(r.point()), r.size());
    }

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

    /**
     * Return the area that content is allowed to be positioned into.
     *
     * In most cases, the normal box() area needs to be shrunk by the
     * margin + padding + border.  However, some widgets reserve more for
     * themselves, and only allow their children to be positioned in the
     * rest.
     */
    virtual Rect content_area() const;

    /**
     * Perform layout of the widget.
     */
    virtual void layout();

    /**
     * Helper function to draw this widget's box using the appropriate
     * theme.
     */
    void draw_box(Painter& painter, Palette::ColorId bg,
                  Palette::ColorId border) const;

    virtual ~Widget() noexcept;

protected:

    /**
     * Is this widget a top level widget?
     */
    virtual bool top_level() const { return false; }

    /**
     * Set this widget's parent.
     */
    virtual void set_parent(Frame* parent);

    virtual const Palette& default_palette() const;

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

    /**
     * Call our parent to do a layout.
     */
    void parent_layout();

private:

    DisplayPoint to_display_back(const Point& p);
    Point from_display_back(const DisplayPoint& p);

    /**
     * Flags for the widget.
     */
    flags_type m_widget_flags{};

    /**
     * Palette for the widget.
     *
     * This may or may not be a complete palette.  If a color does not exist in
     * this instance, it will refer to the default_palette().
     *
     * @note This should not be accessed directly.  Always use the accessor
     * functions because this may not be a complete Palette.
     */
    std::unique_ptr<Palette> m_palette;

    /**
     * Alignment hint for this widget within its parent.
     */
    alignmask m_align{alignmask::none};

    /**
     * Alignment padding.
     */
    default_dim_type m_padding{0};

    /**
     * Alignment border.
     */
    default_dim_type m_border{0};

    /**
     * Alignment margin.
     */
    default_dim_type m_margin{0};

    /**
     * Alignment X ratio.
     */
    default_dim_type m_xratio{0};

    /**
     * Alignment Y ratio.
     */
    default_dim_type m_yratio{0};

    /**
     * Horizontal alignment ratio.
     */
    default_dim_type m_horizontal_ratio{0};

    /**
     * Vertical alignment ratio.
     */
    default_dim_type m_vertical_ratio{0};

    /**
     * Focus state.
     */
    bool m_focus{false};

    /**
     * The boxtype of the widget.
     */
    Theme::boxtype m_boxtype{Theme::boxtype::none};

    /**
     * Instance theme for the widget.
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
