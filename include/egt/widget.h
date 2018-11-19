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

#include <egt/geometry.h>
#include <egt/object.h>
#include <egt/input.h>
#include <egt/font.h>
#include <egt/palette.h>
#include <egt/screen.h>
#include <egt/utils.h>
#include <cairo.h>
#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace egt
{
    class Painter;

    /**
     * Flags used for various widget properties.
     */
    enum class widgetmask : uint32_t
    {
        NONE = 0,

        /**
         * Do not draw the background color.
         *
         * The background color is usually drawn by default. A background will
         * not be drawn if this flag is set. If something else is not drawn,
         * instead (like a child widget), this can result in unintended side
         * effects. Not drawing a background is an optimization to reduce
         * unecessary drawing.
         */
        NO_BACKGROUND = (1 << 0),

        /**
         * This is an overlay plane window.
         */
        PLANE_WINDOW = (1 << 1),

        /**
         * Don't draw any border.
         */
        NO_BORDER = (1 << 2),

        /**
         * Widget always requires a full redraw.
         */
        FULL_REDRAW = (1 << 3),

        /**
         * This is a window widget.
         */
        WINDOW = (1 << 4),

        /**
         * This is a frame window.
         */
        FRAME = (1 << 5),

        /**
         * Don't allow the widget to have focus.
         */
        NO_FOCUS = (1 << 6),

        /**
         * Enable transparent background.
         */
        TRANSPARENT_BACKGROUND = (1 << 7),

        /**
         * Default window flags.
         */
        WINDOW_DEFAULT = WINDOW | NO_BORDER,
    };

    constexpr widgetmask operator&(widgetmask a, widgetmask b)
    {
        return static_cast<widgetmask >(
                   static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    constexpr widgetmask operator|(widgetmask a, widgetmask b)
    {
        return static_cast<widgetmask >(
                   static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    constexpr widgetmask operator^(widgetmask a, widgetmask b)
    {
        return static_cast<widgetmask >(
                   static_cast<uint32_t>(a) ^ static_cast<uint32_t>(b));
    }

    constexpr widgetmask operator~(widgetmask a)
    {
        return static_cast<widgetmask >(~static_cast<uint32_t>(a));
    }

    widgetmask& operator&=(widgetmask& a, widgetmask Y);
    widgetmask& operator|=(widgetmask& a, widgetmask Y);
    widgetmask& operator^=(widgetmask& a, widgetmask Y);

    class Widget;

    /**
     * Alignment flags.
     */
    enum class alignmask : uint32_t
    {
        /** No alignment. */
        NONE = 0,
        /**
         * Center alignment is a weak alignment both horizontal and
         * vertical. To break one of those dimensions to another
         * alignment, specify it in addiiton to ALIGN_CENTER.  If both
         * are broken, ALIGN_CENTER has no effect.
         */
        CENTER = (1 << 0),
        /** Horizontal alignment. */
        LEFT = (1 << 1),
        /** Horizontal alignment. */
        RIGHT = (1 << 2),
        /** Vertical alignment. */
        TOP = (1 << 3),
        /** Vertical alignment. */
        BOTTOM = (1 << 4),
        /** Don't align, expand. */
        EXPAND = (1 << 5),
    };

    constexpr alignmask operator&(alignmask a, alignmask b)
    {
        return static_cast<alignmask >(
                   static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    constexpr alignmask operator|(alignmask a, alignmask b)
    {
        return static_cast<alignmask >(
                   static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    constexpr alignmask operator^(alignmask a, alignmask b)
    {
        return static_cast<alignmask >(
                   static_cast<uint32_t>(a) ^ static_cast<uint32_t>(b));
    }

    constexpr alignmask operator~(alignmask a)
    {
        return static_cast<alignmask >(~static_cast<uint32_t>(a));
    }

    alignmask& operator&=(alignmask& a, alignmask b);
    alignmask& operator|=(alignmask& a, alignmask b);
    alignmask& operator^=(alignmask& a, alignmask b);

    class Frame;

    /**
     * Base widget class.
     *
     * This is the base class for all widgets. A Widget is a thing, with a basic
     * set of properties.  In this case, it has a bounding rectangle,
     * some flags, and some states - and these properties can be manipulated. A
     * Widget can handle events, draw itself, and more.  However, the specifics
     * of what it means to handle an event or draw the widget is implemented in
     * classes that are derived from this one, like a Button or a Label.
     */
    class Widget : public detail::Object
    {
    public:

        /**
         * Given an item size, and a bounding box, and an alignment parameter,
         * return the rectangle the item box should be move/resized to.
         */
        static Rect align_algorithm(const Size& size, const Rect& bounding,
                                    alignmask align, int margin = 0)
        {
            assert(align != alignmask::NONE);

            if ((align & alignmask::EXPAND) == alignmask::EXPAND)
                return bounding;

            Point p;

            if ((align & alignmask::CENTER) == alignmask::CENTER)
            {
                p.x = bounding.x + (bounding.w / 2) - (size.w / 2);
                p.y = bounding.y + (bounding.h / 2) - (size.h / 2);
            }

            if ((align & alignmask::LEFT) == alignmask::LEFT)
                p.x = bounding.x + margin;
            else if ((align & alignmask::RIGHT) == alignmask::RIGHT)
                p.x = bounding.x + bounding.w - size.w - margin;

            if ((align & alignmask::TOP) == alignmask::TOP)
                p.y = bounding.y + margin;
            else if ((align & alignmask::BOTTOM) == alignmask::BOTTOM)
                p.y = bounding.y + bounding.h - size.h - margin;

            return Rect(p, size);
        }

        /**
         * Construct a widget.
         *
         * @param[in] rect Initial rectangle of the Widget.
         * @param[in] flags Widget flags.
         */
        Widget(const Rect& rect = Rect(),
               widgetmask flags = widgetmask::NONE) noexcept;

        /**
         * Construct a widget.
         *
         * @param[in] parent Parent Frame of the widget.
         * @param[in] rect Initial rectangle of the Widget.
         * @param[in] flags Widget flags.
         */
        Widget(Frame& parent, const Rect& rect = Rect(),
               widgetmask flags = widgetmask::NONE) noexcept;

        /**
         * Draw the widget.
         *
         * To change how a widget is drawn, this function can be overloaded and
         * changed.
         *
         * @warning Normally this should not be called directly and instead the
         * event loop will call this function with an already established
         * Painter when the widget needs to be redrawn.
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
         * event to any third party handleres that have been registered. What
         * this means is if you expect other handlers to receive the events
         * then this must be called from derived classes.  Or, manually call
         * invoke_handlers().
         */
        virtual int handle(eventid event);

        /**
         * Resize the widget.
         * Changes the width and height of the widget.
         *
         * @note This will cause a redraw of the widget.
         */
        virtual void resize(const Size& s);

        inline void set_width(int w)
        {
            resize(Size(w, h()));
        }

        inline void set_height(int h)
        {
            resize(Size(w(), h));
        }

        /**
         * Move the widget.
         * Changes the x and y position of the widget.
         *
         * @note This will cause a redraw of the widget.
         */
        virtual void move(const Point& point);
        inline void set_x(int x) { move(Point(x, y())); }
        inline void set_y(int y) { move(Point(x(), y)); }

        /**
         * Move the widget to the specified center point.
         * Changes the x and y position of the widget relative to the center point.
         *
         * @note This will cause a redraw of the widget.
         */
        virtual void move_to_center(const Point& point);

        /**
         * Change the box of the widget.
         * This is the same as calling move() and resize() at the same time.
         */
        virtual void set_box(const Rect& rect)
        {
            move(rect.point());
            resize(rect.size());
        }

        /**
         * Hide the widget.
         *
         * A widget that is not visible will receive no draw() calls.
         *
         * @note This changes the visible() property of the widget.
         */
        virtual void hide()
        {
            if (!m_visible)
                return;
            // careful attention to ordering
            damage();
            m_visible = false;
        }

        /**
         * Show the widget.
         *
         * @note This changes the visible() property of the widget.
         */
        virtual void show()
        {
            if (m_visible)
                return;
            // careful attention to ordering
            m_visible = true;
            damage();
        }

        /**
         * Return true if the widget is visible.
         */
        virtual bool visible() const { return m_visible; }

        /**
         * Return true if the widget is in focus.
         */
        virtual bool focus() const { return m_focus; }

        virtual void focus(Widget* widget)
        {
            ignoreparam(widget);
        }

        /**
         * Set the focus property of the widget.
         */
        virtual void focus(bool value);

        /**
         * Return true if the widget is active.
         *
         * The meaning of active is largely up to the derived implementation.
         */
        virtual bool active() const { return m_active; }
        /**
         * Set the active property of the widget.
         */
        virtual void set_active(bool value)
        {
            if (m_active != value)
            {
                m_active = value;
                damage();
            }
        }

        /**
         * Return the disabled status of the widget.
         *
         * When a widget is disabled, it does not receive events. Also, the
         * color scheme may change when a widget is disabled.
         */
        virtual bool disabled() const { return m_disabled; }

        /**
         * Set the disabled status of the widget.
         */
        virtual void disable(bool value)
        {
            if (m_disabled != value)
                damage();
            m_disabled = value;
        }

        /**
         * Damage the box() of the widget.
         *
         * @note This is just a utility wrapper around damage(box()) in most cases.
         */
        virtual void damage();

        /**
         * Mark the specified rect as a damaged area.
         *
         * This call will propagate to a top level parent frame.
         */
        virtual void damage(const Rect& rect);

        /**
         * Bounding box for the widget.
         */
        virtual const Rect& box() const { return m_box; }

        /**
         * Get the size of the widget.
         */
        virtual Size size() const { return m_box.size(); }

        /**
         * Get the origin point of the widget.
         */
        virtual Point point() const { return m_box.point(); }

        /**
         * @{
         * Bounding box dimensions.
         */
        inline int w() const { return m_box.w; }
        inline int h() const { return m_box.h; }
        inline int x() const { return m_box.x; }
        inline int y() const { return m_box.y; }
        /** @} */

        /**
         * Get the center point of the widget.
         */
        inline Point center() const { return box().center(); }

        /**
         * Get the widget Palette.
         */
        Palette& palette()
        {
            if (!m_palette.get())
                m_palette.reset(new Palette(global_palette()));

            return *m_palette.get();
        }

        /**
         * Get the widget Palette.
         */
        const Palette& palette() const
        {
            if (m_palette.get())
                return *m_palette.get();

            return global_palette();
        }

        /**
         * Set the widget Palette.
         *
         * @note This will overwrite the entire widget Palette.
         */
        void set_palette(const Palette& palette)
        {
            m_palette.reset(new Palette(palette));
        }

        void reset_palette()
        {
            m_palette.reset();
        }

        Frame* parent()
        {
            //assert(m_parent);
            return m_parent;
        }

        const Frame* parent() const
        {
            //assert(m_parent);
            return m_parent;
        }

        virtual IScreen* screen();

	virtual shared_cairo_surface_t surface() const
	{
		/** @todo */
		return nullptr;
	}

        /**
         * Test if the specified Widget flag(s) is/are set.
         * @param flag Bitmask of flags.
         */
        inline bool is_flag_set(widgetmask flag) const
        {
            return (m_flags & flag) == flag;
        }

        /**
         * Set the specified widget flag(s).
         * @param flag Bitmask of flags.
         */
        inline void flag_set(widgetmask flag) { m_flags |= flag; }

        /**
         * Clear, or unset, the specified widget flag(s).
         * @param flag Bitmask of flags.
         */
        inline void flag_clear(widgetmask flag) { m_flags &= ~flag; }

        /**
         * Align the widget.
         *
         * This will align the widget relative to the box of its parent frame.
         */
        void set_align(alignmask a, int margin = 0);

        /**
         * Get the alignment of the widget.
         */
        inline alignmask align() const { return m_align; }

        /**
         * Return the alignment margin.
         */
        inline int margin() const { return m_margin; }

        /**
         * Get the name of the widget.
         */
        const std::string& name() const { return m_name; }

        /**
         * Set the name of the widget.
         */
        inline void set_name(const std::string& name) { m_name = name; }

        /**
         * Draw the widget to a file.
         */
        virtual void save_to_file(const std::string& filename);

        virtual ~Widget();

    protected:

        /**
         * Convert screen coordinates to frame coordinates.
         */
        Point screen_to_frame(const Point& p);

        virtual Point frame_to_screen(const Point& p);

        /**
         * Convert screen rectangle to frame coordinates.
         */
        Rect screen_to_frame(const Rect& r);

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

    private:

        /**
         * When true, the widget is visible.
         */
        bool m_visible{true};

        /**
         * When true, the widget has focus.
         */
        bool m_focus{false};

        /**
         * When true, the widget is active.
         *
         * The active state of a widget is usually a momentary state, unlike
         * focus, which exists until focu is changed. For example, when a button
         * is currently being held down, it its implementation may consider this
         * the active state and choose to draw the button diffeerently.
         *
         * This may change how the widget behaves or is draw.
         */
        bool m_active{false};

        /**
         * When true, the widget is disabled.
         *
         * Typically, when a widget is disabled it will not accept input.
         *
         * This may change how the widget behaves or is draw.
         */
        bool m_disabled{false};

        /**
         * Flags for the widget.
         */
        widgetmask m_flags{widgetmask::NONE};

        /**
         * Current palette for the widget.
         *
         * @note This should not be accessed directly.  Always use the accessor
         * functions because this is not set until it is modified.
         */
        std::shared_ptr<Palette> m_palette;

        /**
         * A user defined name for the widget.
         */
        std::string m_name;

        /**
         * Alignment hint for this widget within its parent.
         */
        alignmask m_align{alignmask::NONE};

        int m_margin{0};

        Widget(const Widget&) = delete;
        Widget& operator=(const Widget&) = delete;

        friend class Frame;
    };

    /**
     * A widget that has text and properties associated with text.
     *
     * This is not meant to be used directly as it does not implement at least a
     * draw() method.
     */
    class TextWidget : public Widget
    {
    public:

        explicit TextWidget(const std::string& text = std::string(),
                            const Rect& rect = Rect(),
                            alignmask align = alignmask::CENTER,
                            const Font& font = Font(),
                            widgetmask flags = widgetmask::NO_BORDER) noexcept;

        /**
         * Set the text of the label.
         */
        virtual void set_text(const std::string& str);

        /**
         * Clear the text value.
         */
        virtual void clear();

        /**
         * Get the text of the Label.
         */
        virtual const std::string& get_text() const { return m_text; }

        /**
         * Set the alignment of the Label.
         */
        void text_align(alignmask align) { m_text_align = align; }

        /**
         * Get the widget Font.
         */
        Font& font()
        {
            if (!m_font.get())
                m_font.reset(new Font(global_font()));

            return *m_font.get();
        }

        /**
         * Get the widget Font.
         */
        const Font& font() const
        {
            if (m_font.get())
                return *m_font.get();

            return global_font();
        }

        /**
         * Set the widget Font.
         *
         * @note This will overwrite the entire widget Font.
         */
        void set_font(const Font& font)
        {
            m_font.reset(new Font(font));
        }

        virtual ~TextWidget()
        {}

    protected:
        alignmask m_text_align{alignmask::CENTER};
        std::string m_text;

    private:
        std::shared_ptr<Font> m_font;
    };

    namespace experimental
    {
        /**
         * Combo box widget.
         */
        class Combo : public Widget
        {
        public:
            Combo(const std::string& label = std::string(),
                  const Rect& rect = Rect());

            virtual int handle(eventid event) override;

            virtual void draw(Painter& painter, const Rect& rect) override;

            virtual ~Combo();

        protected:
            std::string m_label;
        };

        class ScrollWheel : public Widget
        {
        public:
            explicit ScrollWheel(const Rect& rect = Rect());

            virtual int handle(eventid event) override;

            virtual void draw(Painter& painter, const Rect& rect) override;

            int position() const
            {
                return m_pos;
            }

            inline void position(int pos)
            {
                if (pos < (int)m_values.size())
                {
                    m_pos = pos;
                    damage();
                }
            }

            void values(const std::vector<std::string>& v)
            {
                m_values = v;
            }

        protected:
            std::vector<std::string> m_values;

            int m_pos{0};
            int m_moving_x{0};
            int m_start_pos{0};
        };

    }

}

#endif
