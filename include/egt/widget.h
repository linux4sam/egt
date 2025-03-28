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

#include <egt/app.h>
#include <egt/detail/enum.h>
#include <egt/detail/meta.h>
#include <egt/detail/range.h>
#include <egt/event.h>
#include <egt/flags.h>
#include <egt/font.h>
#include <egt/geometry.h>
#include <egt/imagegroup.h>
#include <egt/object.h>
#include <egt/palette.h>
#include <egt/screen.h>
#include <egt/serialize.h>
#include <egt/signal.h>
#include <egt/theme.h>
#include <egt/widgetflags.h>
#include <iosfwd>
#include <memory>
#include <string>

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
class EGT_API Widget : public Object
{
public:

    /**
     * Common flags used for various widget properties.
     */
    enum class Flag : uint32_t
    {
        /**
         * This is an overlay plane window.
         */
        plane_window = detail::bit(0),

        /**
         * This is a window widget.
         */
        window = detail::bit(1),

        /**
         * This is a frame.
         */
        frame = detail::bit(2),

        /**
         * When set, the widget is disabled.
         *
         * Typically, when a widget is disabled it will not accept input.
         *
         * This may change how the widget behaves or is drawn.
         */
        disabled = detail::bit(3),

        /**
         * When set, the widget will not receive input events.
         */
        readonly = detail::bit(4),

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
        active = detail::bit(5),

        /**
         * When set, the widget is not visible.
         */
        invisible = detail::bit(6),

        /**
         * Grab related mouse events.
         *
         * For example, if a button receives the EventId::raw_pointer_down
         * event, make sure the button also gets subsequent mouse events,
         * including the EventId::raw_pointer_up event.
         */
        grab_mouse = detail::bit(7),

        /**
         * Don't clip the child to drawing only in its box.
         *
         * Use this with caution, it's probably not what you want.
         */
        no_clip = detail::bit(8),

        /**
         * Tell any parent not to perform layout on this widget.
         */
        no_layout = detail::bit(9),

        /**
         * Do not automatically resize a widget to meet the minimal size hint.
         */
        no_autoresize = detail::bit(10),

        /**
         * Is the widget in a checked state.
         */
        checked = detail::bit(11),

        /**
         * Is the widget a component.
         */
        component = detail::bit(12),

        /**
         * Has the user enabled 'pointer_drag*' events (if not internally
         * enabled by the widget itself).
         */
        user_drag = detail::bit(13),

        /**
         * has the user enabled 'pointer_drag*' events to be tracked when they
         * cross the widget boundaries.
         */
        user_track_drag = detail::bit(14),
    };

    /// Widget flags
    using Flags = egt::Flags<Widget::Flag>;

    /**
     * Event signal.
     * @{
     */
    /// Invoked when the checked state of the widget changes.
    Signal<> on_checked_changed;

    /// Invoked when the widget gains focus.
    Signal<> on_gain_focus;

    /// Invoked when the widget loses focus.
    Signal<> on_lost_focus;

    /// Invoked when a widget is shown.
    Signal<> on_show;

    /// Invoked when a widget is hidden.
    Signal<> on_hide;
    /** @} */

    /**
     * @param[in] rect Initial rectangle of the Widget.
     * @param[in] flags Widget flags.
     */
    explicit Widget(const Rect& rect = {},
                    const Widget::Flags& flags = {}) noexcept;

    /**
     * @param[in] parent Parent Frame of the Widget.
     * @param[in] rect Initial rectangle of the Widget.
     * @param[in] flags Widget flags.
     */
    explicit Widget(Frame& parent, const Rect& rect = {},
                    const Widget::Flags& flags = {}) noexcept;

    /**
     * @param[in] props list of widget argument and its properties.
     */
    explicit Widget(Serializer::Properties& props) noexcept
        : Widget(props, false)
    {
    }

protected:

    explicit Widget(Serializer::Properties& props, bool is_derived) noexcept;

public:

    Widget(const Widget&) = delete;
    Widget& operator=(const Widget&) = delete;
    Widget(Widget&&) noexcept = default;
    Widget& operator=(Widget&&) noexcept = default;

    /**
     * Draw the widget.
     *
     * To change how a widget is drawn, this function can be overloaded and
     * changed in a derived class, or it can be changed dynamically with the
     * Theme.
     *
     * To optimize drawing, a Widget may use the Rect parameter to limit
     * what needs to be redrawn, which may be smaller than the widget's box(),
     * but will never be outside the widget's box().
     *
     * Painter will always be supplied in a default state to this function,
     * so there is no need to do any cleanup or state saving inside this
     * draw() function.
     *
     * @warning Normally this should not be called directly and instead the
     * EventLoop will call this function with an already established
     * Painter when the Widget needs to be drawn.
     *
     * @see @ref colors and @ref draw
     *
     * @param[in] painter Instance of the Painter for the Screen.
     * @param[in] rect The rectangle to draw.
     *
     */
    virtual void draw(Painter& painter, const Rect& rect);

    /**
     * Handle an event.
     *
     * This handles global events that usually do not originate at the widget.
     * For example, a pointer event.  All events are defined by egt::EventId.
     *
     * The default implementation in the Widget class, will dispatch the
     * event to any third party handlers that have been registered with
     * on_event(). What this means is if you expect other handlers to receive
     * the events then this must be called from derived classes. Or, manually
     * call invoke_handlers().
     *
     * Override this function in a derived class to handle events.
     *
     * @see @ref events
     *
     * @param event The Event that occurred.
     */
    virtual void handle(Event& event);

    /**
     * Handle 'pointer_drag' and 'pointer_drag_stop' events.
     * Called from Input::dispatch().
     *
     * @param event The drag event that occured.
     */
    void continue_drag(Event& event);

    /**
     * Resize the widget.
     *
     * Changes the width and height of the Widget.
     *
     * @param[in] size The new size of the Widget.
     * @note This will cause a redraw of the Widget.
     */
    virtual void resize(const Size& size);

    /**
     * Scale the current size of the Widget given the ratio.
     *
     * There is no automatic undo for this operation.  Each call to this function
     * uses the current size of the Widget.
     *
     * @param[in] hratio Horizontal ratio of width(), with 100 being 100%.
     * @param[in] vratio Vertical ratio of height(), with 100 being 100%.
     *
     * @see resize()
     */
    void resize_by_ratio(DefaultDim hratio, DefaultDim vratio);

    /**
     * Scale the current size of the Widget given the ratio.
     *
     * @param[in] ratio Horizontal and vertical ratio of size(), with 100 being 100%.
     *
     * @see resize()
     */
    void resize_by_ratio(DefaultDim ratio)
    {
        resize_by_ratio(ratio, ratio);
    }

    /**
     * Set the scale of the widget.
     *
     * @param[in] hscale Horizontal scale, with 1.0 being 100%.
     * @param[in] vscale Vertical scale, with 1.0 being 100%.
     *
     * @warning This is experimental.
     */
    virtual void scale(float hscale, float vscale)
    {
        detail::ignoreparam(hscale);
        detail::ignoreparam(vscale);
    }

    /**
     * Set the scale of the widget.
     *
     * @param[in] scale Vertical and horizontal scale, with 1.0 being 100%.
     *
     * @warning This is experimental.
     */
    void scale(float scale)
    {
        this->scale(scale, scale);
    }

    /**
     * Change the width.
     *
     * @param[in] w The new width of the Widget.
     */
    void width(DefaultDim w)
    {
        if (!parent_in_layout() && !in_layout())
            m_user_requested_box.width(w);

        resize(Size(w, height()));
    }

    /**
     * Change the height.
     *
     * @param[in] h The new height of the Widget.
     */
    void height(DefaultDim h)
    {
        if (!parent_in_layout() && !in_layout())
            m_user_requested_box.height(h);

        resize(Size(width(), h));
    }

    /**
     * Move the Widget to a new position.
     *
     * Changes the X and Y position of the Widget.
     *
     * @param[in] point The new origin point for the widget relative to its parent.
     * @note This will cause a redraw of the Widget.
     */
    virtual void move(const Point& point);

    /**
     * Set the X coordinate of the box.
     *
     * @param[in] x The new origin X value for the widget relative to its parent.
     */
    void x(DefaultDim x)
    {
        if (!parent_in_layout() && !in_layout())
            m_user_requested_box.x(x);

        move(Point(x, y()));
    }

    /**
     * Set the Y coordinate of the box.
     *
     * @param[in] y The new origin Y value for the widget relative to its parent.
     */
    void y(DefaultDim y)
    {
        if (!parent_in_layout() && !in_layout())
            m_user_requested_box.y(y);

        move(Point(x(), y));
    }

    /**
     * Move the widget to the specified center point.
     *
     * Changes the X and Y position of the widget relative to the center point.
     *
     * @note This will cause a redraw of the widget.
     */
    void move_to_center(const Point& point);

    /**
     * Move the widget to the center of its parent.
     *
     * Changes the X and Y position of the widget relative to the center point
     * of its parent.
     *
     * @note This will cause a redraw of the widget.
     */
    void move_to_center();

    /**
     * Hide the Widget.
     *
     * A widget that is not visible will receive no draw() calls.
     *
     * This changes the visible() state of the Widget.
     */
    virtual void hide();

    /**
     * Show the Widget.
     *
     * This changes the visible() state of the Widget.
     */
    virtual void show();

    /**
     * Get the visible state of the widget.
     *
     * @return true if the widget is visible.
     */
    EGT_NODISCARD bool visible() const
    {
        return !flags().is_set(Widget::Flag::invisible);
    }

    /**
     * Set the visible state.
     */
    void visible(bool value);

    /**
     * Toggle the visibility state.
     *
     * @see show() and hide() and visible()
     */
    void visible_toggle()
    {
        if (visible())
            hide();
        else
            show();
    }

    /**
     * Get the active state.
     *
     * The meaning of active is largely up to the derived implementation.
     */
    EGT_NODISCARD bool active() const;

    /**
     * Set the active state.
     *
     * The meaning of active is largely up to the derived implementation.
     */
    void active(bool value);

    /**
     * Get the readonly state of the widget.
     *
     * @return true if the widget is readonly.
     */
    EGT_NODISCARD bool readonly() const
    {
        return flags().is_set(Widget::Flag::readonly);
    }

    /**
     * Set the readonly state of the widget.
     *
     * When a widget is readonly, it typically works as normal, except it does
     * not accept any user input.  Depending on the widget, the color scheme may
     * change.
     *
     * @param value Readonly when true.
     */
    void readonly(bool value);

    /**
     * Toggle the readonly state.
     *
     * @see readonly()
     */
    void readonly_toggle()
    {
        readonly(!readonly());
    }

    /**
     * Set the disabled state to true.
     */
    void disable();

    /**
     * Set the disabled state to false.
     */
    void enable();

    /**
     * Return the disabled state of the widget.
     *
     * When a widget is disabled, it does not receive events. Depending on the
     * widget, the color scheme may change.
     */
    EGT_NODISCARD bool disabled() const
    {
        return flags().is_set(Widget::Flag::disabled);
    }

    /**
     * Toggle the disabled state.
     *
     * @see disable() and enable()
     */
    void disable_toggle()
    {
        if (disabled())
            enable();
        else
            disable();
    }

    /**
     * Set the disabled state.
     *
     * @param[in] value When true, the Widget is disabled.
     *
     * @see enable() and disable()
     */
    void disabled(bool value)
    {
        if (value)
            disable();
        else
            enable();
    }

    EGT_NODISCARD bool user_drag() const
    {
        return flags().is_set(Widget::Flag::user_drag);
    }

    void user_drag(bool value)
    {
        if (flags().is_set(Widget::Flag::user_drag) != value)
        {
            if (value)
                flags().set(Widget::Flag::user_drag);
            else
                flags().clear(Widget::Flag::user_drag);
        }
    }

    EGT_NODISCARD bool user_track_drag() const
    {
        return flags().is_set(Widget::Flag::user_track_drag);
    }

    void user_track_drag(bool value)
    {
        if (flags().is_set(Widget::Flag::user_track_drag) != value)
        {
            if (value)
                flags().set(Widget::Flag::user_track_drag);
            else
                flags().clear(Widget::Flag::user_track_drag);
        }
    }

    /**
     * Returns true if the widget is capable of handling an event.
     */
    bool can_handle_event() const
    {
        return !m_widget_flags.is_set(Flag::readonly) &&
               !m_widget_flags.is_set(Flag::invisible) &&
               !m_widget_flags.is_set(Flag::disabled);
    }

    /**
     * Returns true if the DisplayPoint is within the widget box.
     */
    EGT_NODISCARD bool hit(const DisplayPoint& point) const
    {
        auto pos = display_to_local(point);
        return local_box().intersect(pos);
    }

    /**
     * Set the grab_mouse state.
     *
     * @param[in] value When true, grab mouse events.
     *
     * By default, this state is false.
     */
    void grab_mouse(bool value);

    /**
     * Return the grab_mouse state of the widget.
     */
    EGT_NODISCARD bool grab_mouse() const;

    /**
     * Return the plane_window state of the widget.
     */
    EGT_NODISCARD bool plane_window() const;

    /**
     * Set the autoresize state.
     *
     * @param[in] value When true, the widget can be automatically resized.
     *
     * By default, this state is true;
     */
    void autoresize(bool value);

    /**
     * Return the autoresize state of the widget.
     */
    EGT_NODISCARD bool autoresize() const;

    /**
     * Return the frame state of the widget.
     */
    EGT_NODISCARD bool frame() const;

    /**
     * Return the clip state of the widget.
     */
    EGT_NODISCARD bool clip() const;

    /**
     * Set the no_layout state.
     *
     * @param[in] value When true, parent won't perform layout for the widget.
     *
     * By default, this state is false.
     */
    void no_layout(bool value);

    /**
     * Return the no_layout state of the widget.
     */
    EGT_NODISCARD bool no_layout() const;

    /**
     * Get the alpha property.
     *
     * @return Alpha property in the range 0.0 to 1.0.
     */
    EGT_NODISCARD float alpha() const
    {
        return m_alpha;
    }

    /**
     * Set the alpha property.
     *
     * This controls the alpha property of the entire widget and all of its
     * children.
     *
     * @param[in] alpha Widget alpha component in range 0.0 - 1.0.
     */
    void alpha(float alpha);

    /**
     * Damage the box() of the widget and cause a redraw.
     *
     * This is the same as calling damage(box()) in most cases.
     *
     * @warning damage() cannot be called while in or below a Widget::draw()
     * method.
     */
    virtual void damage();

    /**
     * Mark the specified rect as a damaged area of the widget.
     *
     * @note This call will propagate to a top level parent widget that owns a
     * Screen.
     *
     * @warning damage() cannot be called while in or below a Widget::draw()
     * method.
     *
     * @param rect The rectangle to save for damage.
     */
    /**
     * This will merge the damaged area with any already existing damaged area
     * that it overlaps with into a super rectangle. Then, the whole array has
     * to be checked again to make sure the new rectangle doesn't conflict with
     * another existing rectangle.
     */
    virtual void damage(const Rect& rect);

    /**
     * Bounding box for the Widget.
     *
     * This is the rectangle that defines where this widget is positioned
     * relative to its parent and also the size of the widget.
     *
     * @see @ref layout_box
     */
    EGT_NODISCARD const Rect& box() const
    {
        return m_box;
    }

    /**
     * Change the bounding box of the widget.
     *
     * This is the same as calling move() and resize() at the
     * same time.
     *
     * @see @ref layout_box
     */
    void box(const Rect& rect);

    /**
     * Get the box corresponding to the user requested one, not the actual one.
     */
    EGT_NODISCARD const Rect& user_requested_box() const
    {
        return m_user_requested_box;
    }

    /**
     * Get the size of the widget's box().
     */
    EGT_NODISCARD const Size& size() const
    {
        return box().size();
    }

    /**
     * Get the origin of the widget's box().
     */
    EGT_NODISCARD const Point& point() const
    {
        return box().point();
    }

    /**
     * Width of the widget's box().
     */
    EGT_NODISCARD DefaultDim width() const { return m_box.width(); }

    /**
     * Height of the widget's box().
     */
    EGT_NODISCARD DefaultDim height() const { return m_box.height(); }

    /**
     * X coordinate of the widget's box().
     */
    EGT_NODISCARD DefaultDim x() const { return m_box.x(); }

    /**
     * Y coordinate of the widget's box().
     */
    EGT_NODISCARD DefaultDim y() const { return m_box.y(); }

    /**
     * Get the center point of the widget's box().
     */
    EGT_NODISCARD Point center() const { return box().center(); }

    /**
     * Get the current Palette::GroupId depending on the widget's state.
     */
    EGT_NODISCARD Palette::GroupId group() const;

    /**
     * Set the widget instance Palette.
     *
     * This will replace any Palette instance currently owned by the Widget.
     * This does not have to be a complete Palette, in which case the widget
     * will default to the Theme palette if a Color is not found in this
     * palette.
     *
     * @param palette The new palette to assign to the widget.
     * @note This will overwrite the entire widget Palette.
     */
    void palette(const Palette& palette);

    /**
     * Get the widget palette.
     *
     * This will return any palette set on the widget instance, or default to the
     * global palette.
     */
    EGT_NODISCARD const Palette& palette() const;

    /**
     * Clear the widget instance palette.
     *
     * This will clear the widget instance's palette.  Meaning, for all colors
     * the widget will now use default_palette().
     */
    void reset_palette();

    /**
     * Check whether the widget has a custom palette.
     */
    EGT_NODISCARD bool has_palette() const { return (bool)m_palette; }

    /**
     * Get a Widget color.
     *
     * This will return a color for the Palette::GroupId reflecting the current
     * state of the Widget when the call is made.  To get a specific
     * Palette::GroupId color, use the overloaded function.
     *
     * If the id and group exist in the widget instance palette, that will be
     * used.  Otherwise, the default_palette() will be used.
     *
     * @param id Palette::ColorId to get.
     */
    EGT_NODISCARD const Pattern& color(Palette::ColorId id) const;

    /**
     * Get a Widget color.
     *
     * If the id and group exist in the widget instance palette, that will be
     * used.  Otherwise, the default_palette() will be used.
     *
     * @param id Palette::ColorId to get.
     * @param group Palette::GroupId to get.
     */
    EGT_NODISCARD const Pattern& color(Palette::ColorId id, Palette::GroupId group) const;

    /**
     * Add a color to the widget's instance palette.
     *
     * This will add the specified id, color, and group to the widget's instance
     * palette.
     *
     * @param id Palette::ColorId to set.
     * @param color Color to set.
     * @param group Palette::GroupId to set.
     */
    void color(Palette::ColorId id,
               const Pattern& color,
               Palette::GroupId group = Palette::GroupId::normal);

    /**
     * Get the background image, if any, for the current group.
     *
     * @param allow_fallback If true, return the 'normal' image if no image
     *                       is set for the current Palette::GroupId.
     */
    EGT_NODISCARD Image* background(bool allow_fallback = false) const;

    /**
     * Get the background image, if any, for the given group.
     *
     * @param group Palette::GroupId to get.
     * @param allow_fallback If true, return the 'normal' image if no image
     *                       is set for the requested group.
     */
    EGT_NODISCARD Image* background(Palette::GroupId group,
                                    bool allow_fallback = false) const;

    /**
     * Add an image background to the widget's box.
     *
     * @param image Image to set.
     * @param group Palette::GroupId to set.
     */
    void background(const Image& image,
                    Palette::GroupId group = Palette::GroupId::normal);

    /**
     * Remove an image background from the widget's box.
     *
     * @param group Palette::GroupId to set.
     */
    void reset_background(Palette::GroupId group = Palette::GroupId::normal);

    /**
     * Get a pointer to the parent Widget, or nullptr if none exists.
     */
    Widget* parent();

    /**
     * Get a pointer to the parent Widget, or nullptr if none exists.
     */
    EGT_NODISCARD const Widget* parent() const;

    /**
     * Get a pointer to the Screen instance, using using a parent as
     * necessary.
     *
     * @return An Screen if available, or nullptr.
     */
    EGT_NODISCARD virtual Screen* screen() const;

    /**
     * Does this Widget have a screen?
     */
    EGT_NODISCARD virtual bool has_screen() const { return false; }

    /**
     * Align the widget.
     *
     * This will align the widget relative to the box of its parent widget.
     *
     * @param[in] a The AlignFlags.
     */
    void align(const AlignFlags& a);

    /**
     * Get the alignment.
     */
    EGT_NODISCARD const AlignFlags& align() const { return m_align; }

    /**
     * Get the alignment.
     */
    AlignFlags& align() { return m_align; }

    /**
     * Set the padding width.
     *
     * @see @ref layout_box
     */
    void padding(DefaultDim padding)
    {
        if (detail::change_if_diff<>(m_padding, padding))
        {
            damage();
            parent_layout();
        }
    }

    /**
     * Return the padding width.
     *
     * @see @ref layout_box
     */
    EGT_NODISCARD DefaultDim padding() const { return m_padding; }

    /**
     * Set the margin width.
     *
     * @see @ref layout_box
     */
    void margin(DefaultDim margin)
    {
        if (detail::change_if_diff<>(m_margin, margin))
        {
            damage();
            parent_layout();
        }
    }

    /**
     * Get the margin width.
     *
     * @see @ref layout_box
     */
    EGT_NODISCARD DefaultDim margin() const { return m_margin; }

    /**
     * Set the border width.
     *
     * @see @ref layout_box
     */
    void border(DefaultDim border)
    {
        if (detail::change_if_diff<>(m_border, border))
        {
            damage();
            parent_layout();
        }
    }

    /**
     * Get the border width.
     *
     * @see @ref layout_box
     */
    EGT_NODISCARD DefaultDim border() const { return m_border; }

    /**
     * Set the border radius.
     */
    void border_radius(float radius)
    {
        if (detail::change_if_diff<>(m_border_radius, radius))
            damage();
    }

    /**
     * Get the border radius.
     */
    EGT_NODISCARD float border_radius() const { return m_border_radius; }

    /**
     * Set the border flags.
     *
     * @see Theme::BorderFlag
     */
    void border_flags(const Theme::BorderFlags& flags)
    {
        if (detail::change_if_diff<>(m_border_flags, flags))
            damage();
    }

    /**
     * Get the border flags.
     *
     * @see Theme::BorderFlag
     */
    EGT_NODISCARD Theme::BorderFlags border_flags() const { return m_border_flags; }

    /**
     * Set the size ratio relative to parent.
     *
     * @param[in] ratio Horizontal and vertical ratio parent size, with 100 being 100%.
     *
     * @note This is the same as calling ratio(ratio, ratio).
     */
    void ratio(DefaultDim ratio)
    {
        this->ratio(ratio, ratio);
    }

    /**
     * Set the size ratio relative to parent.
     *
     * @param[in] horizontal Horizontal ratio of parent width, with 100 being 100%.
     * @param[in] vertical Vertical ratio of parent height, with 100 being 100%.
     */
    void ratio(DefaultDim horizontal,
               DefaultDim vertical)
    {
        auto a = detail::change_if_diff<>(m_horizontal_ratio, horizontal);
        auto b = detail::change_if_diff<>(m_vertical_ratio, vertical);
        if (a || b)
            parent_layout();
    }

    /**
     * Set the vertical ratio relative to parent.
     *
     * @param[in] vertical Vertical ratio of parent height, with 100 being 100%.
     */
    void vertical_ratio(DefaultDim vertical)
    {
        if (detail::change_if_diff<>(m_vertical_ratio, vertical))
            parent_layout();
    }

    /**
     * Get the vertical ratio relative to parent.
     */
    EGT_NODISCARD DefaultDim vertical_ratio() const { return m_vertical_ratio; }

    /**
     * Set the horizontal ratio relative to parent.
     *
     * @param[in] horizontal Horizontal ratio of parent width, with 100 being 100%.
     */
    void horizontal_ratio(DefaultDim horizontal)
    {
        if (detail::change_if_diff<>(m_horizontal_ratio, horizontal))
            parent_layout();
    }

    /**
     * Get the horizontal ratio relative to parent.
     */
    EGT_NODISCARD DefaultDim horizontal_ratio() const { return m_horizontal_ratio; }

    /**
     * Set the Y position ratio relative to parent.
     *
     * @param[in] yratio Y ratio of parent height, with 100 being 100%.
     */
    void yratio(DefaultDim yratio)
    {
        if (detail::change_if_diff<>(m_yratio, yratio))
            parent_layout();
    }

    /**
     * Get the Y position ratio relative to parent.
     */
    EGT_NODISCARD DefaultDim yratio() const { return m_yratio; }

    /**
     * Set the X position ratio relative to parent.
     *
     * @param[in] xratio X ratio of parent width, with 100 being 100%.
     */
    void xratio(DefaultDim xratio)
    {
        if (detail::change_if_diff<>(m_xratio, xratio))
            parent_layout();
    }

    /**
     * Get the X position ratio relative to parent.
     */
    EGT_NODISCARD DefaultDim xratio() const { return m_xratio; }

    /**
     * Get a minimum size hint for the Widget.
     *
     * This is used by sizers to pick minimum and default dimensions when no
     * other @b force is used.
     */
    EGT_NODISCARD virtual Size min_size_hint() const;

    /**
     * Set the minimum size hint for the Widget.
     *
     * This is used by sizers to pick minimum and default dimensions when no
     * other force is used.  When this is set to a non-zero value, this will
     * override any computation done by min_size_hint().
     *
     * @param size The minimum size.
     */
    void min_size_hint(const Size& size)
    {
        if (detail::change_if_diff<>(m_min_size, size))
            parent_layout();
    }

    /**
     * Paint the Widget using a Painter.
     *
     * paint() is not part of the normal draw path.  This is a utility
     * function to get the widget to draw its contents to another
     * surface provided with a Painter.
     */
    void paint(Painter& painter);

    /**
     * Draw the widget to a file.
     *
     * @param[in] filename Optional filename to save to.
     */
    virtual void paint_to_file(const std::string& filename = {});

    /**
     * Callback definition used by walk().
     *
     * In the callback, return true to continue or false to stop.
     */
    using WalkCallback = std::function<bool(Widget* widget, int level)>;

    /**
     * Walk the Widget tree and call @b callback with each Widget.
     *
     * @param callback Function to call for each widget.
     * @param level The current level of the widget hierarchy starting with the
     *              level originally passed in.
     *
     * @code{.cpp}
     * widget->walk([](Widget* widget, int level) {
     *     // do something
     * });
     * @endcode
     */
    virtual void walk(const WalkCallback& callback, int level = 0);

    /**
     * Set the focus state.
     *
     * @param value State of the focus.
     */
    void focus(bool value);

    /**
     * Get the current focus state.
     *
     * @return true if in focus.
     */
    EGT_NODISCARD bool focus() const { return m_focus; }

    /**
     * Set the fill flags.
     *
     * The fill flags of a widget are used to set predefined draw operators
     * for a widget. This provides a level of customization that is primarily
     * for performance optimizations.
     *
     * Setting a fill type of empty means that no background will be drawn no
     * matter what the background color of the widget is.
     *
     * @see Theme::FillFlag
     */
    void fill_flags(const Theme::FillFlags& flags)
    {
        if (detail::change_if_diff<>(m_fill_flags, flags))
            damage();
    }

    /**
     * Get the fill flags.
     *
     * @see Theme::FillFlag
     */
    EGT_NODISCARD const Theme::FillFlags& fill_flags() const
    {
        return m_fill_flags;
    }

    /**
     * Get a modifiable fill flags reference.
     *
     * @see Theme::FillFlag
     */
    Theme::FillFlags& fill_flags()
    {
        return m_fill_flags;
    }

    /**
     * Get the Widget Theme.
     *
     * A global_theme() will be returned.
     *
     * @see @ref colors_themes
     */
    EGT_NODISCARD const Theme& theme() const;

    /**
     * Move this widgets zorder down relative to other widgets with the same
     * parent.
     *
     * @see @ref draw_zorder
     */
    virtual void zorder_down();

    /**
     * Move this widgets zorder up relative to other widgets with the same
     * parent.
     *
     * @see @ref draw_zorder
     */
    virtual void zorder_up();

    /**
     * Move the widget to the bottom.
     *
     * @see @ref draw_zorder
     */
    virtual void zorder_bottom();

    /**
     * Move the widget to the top.
     *
     * @see @ref draw_zorder
     */
    virtual void zorder_top();

    /**
     * Get the zorder of the widget.
     *
     * @see @ref draw_zorder
     */
    EGT_NODISCARD virtual size_t zorder() const;

    /**
     * Set the zorder of the widget.
     *
     * @param rank The rank in the zorder.
     *
     * @see @ref draw_zorder
     */
    virtual void zorder(size_t rank);

    /**
     * Detach this widget from its parent.
     *
     * @warning This can result in immediately deleting the instance of the
     * widget if its lifetime is managed by a smart pointer and there are no
     * more references.
     */
    void detach();

    /**
     * Convert a point with a local origin to a parent origin.
     */
    EGT_NODISCARD Point to_parent(const Point& r) const;

    /**
     * Convert a point in a rect a local origin to a parent origin.
     *
     * @see to_parent().
     */
    EGT_NODISCARD Rect to_parent(const Rect& r) const
    {
        return Rect(to_parent(r.point()), r.size());
    }

    /**
     * Get the display origin of the Widget.
     */
    DisplayPoint display_origin();

    /**
     * Get the sum of the margin(), padding(), and border() around the content
     * of the widget.
     */
    EGT_NODISCARD size_t moat() const;

    /**
     * Return the area that content is allowed to be positioned into.
     *
     * In most cases, the normal box() area needs to be shrunk by the
     * margin + padding + border.  However, some widgets reserve more for
     * themselves, and only allow their children to be positioned in the
     * rest.
     *
     * @see @ref layout_box
     */
    EGT_NODISCARD virtual Rect content_area() const;

    /**
     * Indicate if the Widget is computing the layout or not.
     */
    EGT_NODISCARD bool in_layout() const
    {
        return m_in_layout;
    }

    /**
     * Perform layout of the Widget.
     *
     * This will cause the widget to layout itself and any of its children.
     * This can mean, for example, the widget will resize() itself to respect
     * its min_size_hint().
     *
     * @note Remember that when overriding this function as a Frame, you must
     * call layout on each child Frame to propagate the layout.
     */
    virtual void layout();

    /**
     * Helper function to draw this widget's box using the appropriate
     * theme.
     */
    void draw_box(Painter& painter, Palette::ColorId bg,
                  Palette::ColorId border) const;

    /**
     * Helper function to draw this widget's circle using the appropriate
     * theme.
     */
    void draw_circle(Painter& painter, Palette::ColorId bg,
                     Palette::ColorId border) const;

    /**
     * Convert a local point to a display point.
     *
     * A display point has origin in the top left of the display. A local point
     * has its origin at the top left of the position of this widget.
     *
     * @see @ref geometry_coord
     */
    DisplayPoint local_to_display(const Point& p) const;

    /**
     * Convert a display point to a local point.
     *
     * A display point has origin in the top left of the display. A local point
     * has its origin at the top left of the position of this widget.
     *
     * @see @ref geometry_coord
     */
    virtual Point display_to_local(const DisplayPoint& p) const;

    /**
     * Get the widget Font.
     *
     * This will return any font set on the widget instance, or default to the
     * widget Theme font.
     */
    EGT_NODISCARD const Font& font() const;

    /**
     * Set the widget Font.
     *
     * Sets the widget font which means the widget instance will no longer use
     * its Theme font.
     */
    void font(const Font& font)
    {
        if (m_font && *m_font == font)
            return;

        m_font = std::make_unique<Font>(font);
        damage();
        layout();
        parent_layout();
    }

    /**
     * Reset the widget's Font.
     *
     * This is the inverse of setting a custom font for this widget instance
     * with font().
     */
    void reset_font()
    {
        if (m_font)
        {
            m_font.reset();
            damage();
            layout();
            parent_layout();
        }
    }

    /**
     * Check whether the widget has a custom Font.
     */
    bool has_font() const { return (bool)m_font; }

    /**
     * Get the boolean checked state of the a widget.
     *
     * @return true if checked.
     */
    EGT_NODISCARD bool checked() const
    {
        return flags().is_set(Widget::Flag::checked);
    }

    /**
     * Set the checked state of the widget.
     */
    virtual void checked(bool value);

    /**
     * Returns a string representation of the type of this widget.
     *
     * Should this need to be changed, override this function.
     */
    EGT_NODISCARD virtual std::string type() const;

    /**
     * Type used to identify a unique Widget ID.
     */
    using WidgetId = uint64_t;

    /**
     * Get the unique id of the widget.
     *
     * Every widget allocated is given a unique id.  This is used internally to
     * create a default name() of the widget, but can also be used to uniquely
     * identify widgets for other purposes.
     */
    EGT_NODISCARD WidgetId widgetid() const
    {
        return m_widgetid;
    }

    /**
     * Called from ComposerScreen::resize().
     *
     * @note Widgets can be copied, moved or destroyed. Therefore their
     * life cycle is more complex than static data (default font size, default
     * widget sizes). If the ComposerScreen::on_screen_resized signal had been
     * used to manage widgets when the screen is resized, then we would have to
     * register a hook from any constructor, forcing us to explicitly define
     * copy and move constructors instead of using default implementation of
     * those constructors. We would also have to register that hook from the
     * copy and move assignment operators and finally unregister it from the
     * widget destructor.
     *
     * This would have required too many changes to accurately handle all widget
     * life cycles, so instead we call this virtual method on the main window
     * from ComposerScreen::resize() to recursively scan and progress the whole
     * widget tree.
     */
    virtual void on_screen_resized();

    /**
     * Serialize the widget to the specified serializer.
     */
    virtual void serialize(Serializer& serializer) const;

    /**
     * Serialize the widget's children to the specified serializer.
     */
    virtual void serialize_children(Serializer& serializer) const { detail::ignoreparam(serializer); }

    /**
     * Deserialize the children of this widget.
     */
    virtual void deserialize_children(const Deserializer& deserializer) { detail::ignoreparam(deserializer); }

    /**
     * Resume deserializing of the widget after its children have been deserialized.
     */
    virtual void post_deserialize(Serializer::Properties& props) { detail::ignoreparam(props); }

    ~Widget() noexcept override;

protected:

    /**
     * Special variation of damage() that is to be called explicitly by
     * subordinate widgets.
     */
    virtual void damage_from_subordinate(const Rect& rect)
    {
        damage(rect);
    }

    virtual Point point_from_subordinate(const Widget& subordinate) const
    {
        detail::ignoreparam(subordinate);
        return point();
    }

    /**
     * Cause the widget to draw itself and all of its children.
     *
     * @warning Normally this should not be called directly and instead the
     * event loop will call this function.
     */
    virtual void begin_draw()
    {
        assert(0);
    }

    /**
     * Call the begin_draw() method of the parent.
     *
     * As begin_draw() is protected, a subclass of widget is not allowed to call
     * m_parent->begin_draw(), so use this method.
     */
    void begin_draw(Widget* parent)
    {
        parent->begin_draw();
    }

    /**
     * Convert a point with an origin of the current widget to subordinate origin.
     */
    EGT_NODISCARD virtual Point to_subordinate(const Point& p) const
    {
        return p - point();
    }

    /**
     * @see Widget::to_subordinate();
     */
    EGT_NODISCARD Rect to_subordinate(Rect rect) const
    {
        rect.point(to_subordinate(rect.point()));
        return rect;
    }

    /**
     * Convert a local point to the coordinate system of the current panel.
     *
     * In other words, work towards a parent that has the panel so we can get
     * this point relative to the origin of the panel we are drawing to.
     */
    virtual Point to_panel(const Point& p);

    /**
     * Move the specified widget zorder down relative to other widgets with the
     * same parent.
     *
     * @param widget The widget.
     */
    void zorder_down(const Widget* widget);

    /**
     * Move the specified widget zorder up relative to other widgets with the
     * same parent.
     *
     * @param widget The widget.
     */
    void zorder_up(const Widget* widget);

    /**
     * Move the specified widget zorder to the bottom of the current list of widgets
     * with the same parent.
     *
     * @param widget The widget.
     */
    void zorder_bottom(const Widget* widget);

    /**
     * Move the specified widget zorder to the top of the current list of widgets
     * with the same parent.
     *
     * @param widget The widget.
     */
    void zorder_top(const Widget* widget);

    /**
     * Get the zorder of the widget.
     *
     * @param widget The widget.
     */
    size_t zorder(const Widget* widget) const;

    /**
     * Set the zorder of the widget.
     *
     * @param widget The widget.
     * @param rank The rank in the zorder.
     */
    void zorder(const Widget* widget, size_t rank);

    /**
     * Add damage to the damage array.
     *
     * Damage rects added here must have origin at point() of this widget. This
     * is done for several reasons, including if this widget is moved, all of the
     * damage rects are still valid.
     */
    void add_damage(const Rect& rect);

    /**
     * Helper type that defines the special draw child callback.
     */
    using ChildDrawCallback = std::function<void(Painter& painter, Widget* widget)>;

    /**
     * Get the special child draw callback.
     */
    EGT_NODISCARD ChildDrawCallback special_child_draw_callback() const
    {
        return m_special_child_draw_callback;
    }

    /**
     * Get the child draw callback of the parent.
     *
     * As special_child_draw_callback() is protected, a subclass of widget is
     * not allowed to call m_parent->special_child_draw_callback(), so use this
     * method.
     */
    EGT_NODISCARD ChildDrawCallback special_child_draw_callback(Widget* parent) const
    {
        return parent->special_child_draw_callback();
    }

    /**
     * Set the special child draw callback.
     */
    void special_child_draw_callback(ChildDrawCallback func)
    {
        m_special_child_draw_callback = std::move(func);
    }

    /**
     * Special draw function that can be invoked when drawing each child.
     *
     * This is useful, for example, to draw a custom bounding box around
     * children or to modify how a child draws by overwriting it.
     *
     * @param painter An instance of the Painter to use.
     * @param widget The widget.
     */
    void special_child_draw(Painter& painter, Widget* widget)
    {
        if (m_special_child_draw_callback)
            m_special_child_draw_callback(painter, widget);
        else if (parent())
            parent()->special_child_draw(painter, widget);
    }

    /**
     * Starting from this Widget, find the Widget that has a Screen.
     *
     * This searches up the widget hierarchy from here.
     */
    Widget* find_screen()
    {
        if (has_screen())
            return this;

        if (parent())
            return parent()->find_screen();

        return nullptr;
    }

    /**
     * Starting from this Widget, find the Widget that has a Screen.
     *
     * This searches up the widget hierarchy from here.
     */
    EGT_NODISCARD const Widget* find_screen() const
    {
        if (has_screen())
            return this;

        if (parent())
            return parent()->find_screen();

        return nullptr;
    }

    /**
     * Get a const ref of the flags.
     */
    EGT_NODISCARD const Widget::Flags& flags() const { return m_widget_flags; }

    /**
     * Get a modifiable ref of the flags.
     *
     * @warning Directly modifying flags may result in inconsistent state of the
     * widget. Always use the appropriate member functions (i.e. readonly(),
     * checked()).
     */
    Widget::Flags& flags() { return m_widget_flags; }

    /**
     * Is this widget a top level widget?
     */
    EGT_NODISCARD virtual bool top_level() const { return false; }

    /**
     * Set this widget's parent.
     */
    virtual void set_parent(Widget* parent);

    /**
     * Get the local box which is the same size as box(), but with the
     * origin zeroed.
     */
    EGT_NODISCARD Rect local_box() const
    {
        return size();
    }

    /**
     * Handle 'pointer_drag_start' events and decide whether they are caught
     * by this widget.
     * Called from handle().
     *
     * @param event The drag event that occured.
     */
    void start_drag(Event& event);

    /**
     * Tell the start_drag() method whether the widget accepts
     * 'pointer_drag*' events.
     *
     * @return true if and only if the widget accepts 'pointer_drag*' events.
     */
    bool accept_drag() const { return internal_drag() || user_drag(); }

    virtual bool internal_drag() const { return false; }

    /**
     * Tell the continue_drag() method whether the widget tracks
     * 'pointer_drag*' events once they have crossed out the widget boundaries.
     * If not, a 'pointer_drag_stop' event is inserted wihtin the widget
     * boundaries.
     *
     * @return true if and only if the widget tracks 'pointer_drag*' events.
     */
    bool track_drag() const { return internal_track_drag() || user_track_drag(); }

    virtual bool internal_track_drag() const { return false; }

    /**
     * Bounding box.
     */
    Rect m_box;

    /**
     * Keep track of the box requested by the user.
     */
    Rect m_user_requested_box;

    /**
     * Pointer to this widget's parent.
     *
     * The parent is a Widget, which is capable of managing children.
     */
    Widget* m_parent{nullptr};

    /**
     * Unique ID of this widget.
     */
    WidgetId m_widgetid{0};

    /**
     * Indicate if our parent is computing the layout.
     */
    EGT_NODISCARD bool parent_in_layout();

    /**
     * Call our parent to do a layout.
     */
    void parent_layout();

    /**
     * Minimum size of the widget when not an empty value.
     */
    Size m_min_size;

    /**
     * Status for whether this widget is currently performing layout.
     */
    bool m_in_layout{false};

    /**
     * Deserialize widget properties that require to call overridden methods.
     *
     * Should be called at the end of all
     * T::T(Serializer::Properties& props, bool is_derived) constructors,
     * if and only if 'is_derived' is 'false'.
     *
     * Indeed, we wait for an object to be entirely constructed so the expected
     * virtual/overridden methods are executed.
     */
    void deserialize_leaf(Serializer::Properties& props);

    /// @private
    void draw_subordinate(Painter& painter, const Rect& crect, Widget* child);

    /// Used internally for calling the special child draw function.
    ChildDrawCallback m_special_child_draw_callback;

    /// Helper type for an array of subordinate widgets.
    using SubordinatesArray = std::list<std::shared_ptr<Widget>>;

    /// Array of subordinates widgets split in child widgets and component widgets.
    SubordinatesArray m_subordinates;

    /// Return the array of child widgets.
    EGT_NODISCARD const detail::Range<SubordinatesArray>& children() const
    {
        return m_children;
    }

    /// Return the array of child widgets.
    EGT_NODISCARD detail::Range<SubordinatesArray>& children()
    {
        return m_children;
    }

    /// Array of child widgets in the order they were added.
    detail::Range<SubordinatesArray> m_children;

    /// Return the array of components widgets.
    EGT_NODISCARD const detail::Range<SubordinatesArray>& components() const
    {
        return m_components;
    }

    /// Return the array of components widgets.
    EGT_NODISCARD detail::Range<SubordinatesArray>& components()
    {
        return m_components;
    }

    /// Array of component widgets in the order they were added.
    detail::Range<SubordinatesArray> m_components;

    /**
     * Update the 'm_children' and 'm_components' members.
     *
     * @note Should be called any time 'm_components_begin',
     * m_subordonates.begin() or m_subordinates.end() have changed.
     */
    void update_subordinates_ranges()
    {
        m_children.begin(m_subordinates.begin());
        m_children.end(m_components_begin);
        m_components.begin(m_components_begin);
        m_components.end(m_subordinates.end());
    }

    /// Return either components() or children() depending on widget.component()
    EGT_NODISCARD detail::Range<SubordinatesArray>& range_from_widget(const Widget& widget)
    {
        return widget.component() ? components() : children();
    }

    /// Return either components() or children() depending on widget.component()
    EGT_NODISCARD const detail::Range<SubordinatesArray>& range_from_widget(const Widget& widget) const
    {
        return widget.component() ? components() : children();
    }

    /// Add a component.
    void add_component(Widget& widget);

    /// Remove a component.
    void remove_component(Widget* widget);

    /// Set the component flag.
    void component(bool value);

    /// Get the component status.
    EGT_NODISCARD bool component() const;
    /**
     * Iterator for the beginning of components which are positionned after
     * children in the subordinates array. Components are child widget as well,
     * but not added through the Frame interface. Components are composition
     * widgets and users must not be able to add extra ones or to remove them.
     */
    SubordinatesArray::iterator m_components_begin;

    /// The damage array for this widget.
    Screen::DamageArray m_damage;

    /// Status for whether this widget is currently drawing.
    bool m_in_draw{false};

private:

    /**
     * Palette for the widget.
     *
     * This may or may not be a complete palette.  If a color does not exist in
     * this instance, it will refer to the default_palette().
     *
     * @note This should not be accessed directly.  Always use the access
     * functions because this may not be a complete Palette.
     */
    std::unique_ptr<Palette> m_palette;

    /**
     * Optional background images.
     */
    ImageGroup m_backgrounds{"bg"};

    /**
     * Flags for the widget.
     */
    Widget::Flags m_widget_flags{};

    /**
     * Alignment hint for this widget within its parent.
     */
    AlignFlags m_align{};

    /**
     * Alignment padding.
     */
    DefaultDim m_padding{0};

    /**
     * Alignment border.
     */
    DefaultDim m_border{0};

    /**
     * Border radius.
     */
    float m_border_radius{};

    /**
     * Border flags.
     */
    Theme::BorderFlags m_border_flags{};

    /**
     * Alignment margin.
     */
    DefaultDim m_margin{0};

    /**
     * Alignment X ratio.
     */
    DefaultDim m_xratio{0};

    /**
     * Alignment Y ratio.
     */
    DefaultDim m_yratio{0};

    /**
     * Horizontal alignment ratio.
     */
    DefaultDim m_horizontal_ratio{0};

    /**
     * Vertical alignment ratio.
     */
    DefaultDim m_vertical_ratio{0};

    /**
     * Focus state.
     */
    bool m_focus{false};

    /**
     * Widget alpha.
     */
    float m_alpha{1.0f};

    /**
     * Fill flags.
     */
    Theme::FillFlags m_fill_flags{};

    /**
     * Common initializations done by all constructors.
     */
    void init(void);

    /**
     * Font instance for the widget.
     *
     * @note This should not be accessed directly.  Always use the access
     * functions because this is not set until it is modified.
     */
    std::unique_ptr<Font> m_font;

    /**
     * Deserialize widget properties.
     */
    void deserialize(Serializer::Properties& props);

    friend class Frame;
};

/// Enum string conversion map
template<>
EGT_API const std::pair<Widget::Flag, char const*> detail::EnumStrings<Widget::Flag>::data[15];

/// Overloaded std::ostream insertion operator
EGT_API std::ostream& operator<<(std::ostream& os, const Widget::Flag& flag);
/// Overloaded std::ostream insertion operator
EGT_API std::ostream& operator<<(std::ostream& os, const Widget::Flags& flags);

}
}

#endif
