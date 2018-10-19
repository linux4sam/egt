/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_WIDGET_H
#define MUI_WIDGET_H

/**
 * @file
 * @brief Base class Widget definition.
 */

#include <mui/font.h>
#include <mui/geometry.h>
#include <mui/input.h>
#include <mui/palette.h>
#include <mui/screen.h>
#include <mui/utils.h>
#include <cairo.h>
#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace mui
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
         * Default window flags.
         */
        WINDOW_DEFAULT = WINDOW | NO_BORDER,
    };

    constexpr widgetmask operator&(widgetmask X, widgetmask Y)
    {
        return static_cast<widgetmask >(
                   static_cast<uint32_t>(X) & static_cast<uint32_t>(Y));
    }

    constexpr widgetmask operator|(widgetmask X, widgetmask Y)
    {
        return static_cast<widgetmask >(
                   static_cast<uint32_t>(X) | static_cast<uint32_t>(Y));
    }

    constexpr widgetmask operator^(widgetmask X, widgetmask Y)
    {
        return static_cast<widgetmask >(
                   static_cast<uint32_t>(X) ^ static_cast<uint32_t>(Y));
    }

    constexpr widgetmask operator~(widgetmask X)
    {
        return static_cast<widgetmask >(~static_cast<uint32_t>(X));
    }

    widgetmask& operator&=(widgetmask& X, widgetmask Y);
    widgetmask& operator|=(widgetmask& X, widgetmask Y);
    widgetmask& operator^=(widgetmask& X, widgetmask Y);

    class Widget;

    class EventWidget
    {
    public:
        EventWidget() noexcept
        {}

        using handler_callback_t = std::function<void (EventWidget* widget, int event)>;

        virtual void add_handler(handler_callback_t handler)
        {
            m_handlers.push_back(handler);
        }

        virtual ~EventWidget()
        {}

    protected:

        virtual void invoke_handlers(int event)
        {
            for (auto x : m_handlers)
                x(this, event);
        }

        std::vector<handler_callback_t> m_handlers;
    };

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

    constexpr alignmask operator&(alignmask X, alignmask Y)
    {
        return static_cast<alignmask >(
                   static_cast<uint32_t>(X) & static_cast<uint32_t>(Y));
    }

    constexpr alignmask operator|(alignmask X, alignmask Y)
    {
        return static_cast<alignmask >(
                   static_cast<uint32_t>(X) | static_cast<uint32_t>(Y));
    }

    constexpr alignmask operator^(alignmask X, alignmask Y)
    {
        return static_cast<alignmask >(
                   static_cast<uint32_t>(X) ^ static_cast<uint32_t>(Y));
    }

    constexpr alignmask operator~(alignmask X)
    {
        return static_cast<alignmask >(~static_cast<uint32_t>(X));
    }

    alignmask& operator&=(alignmask& X, alignmask Y);
    alignmask& operator|=(alignmask& X, alignmask Y);
    alignmask& operator^=(alignmask& X, alignmask Y);

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
    class Widget : public EventWidget
    {
    public:

        /**
         * Given an item size, and a bounding box, and an alignment parameter,
         * return the rectangle the item box should be move/resized to.
         */
        static Rect align_algorithm(const Size& item, const Rect& bounding,
                                    alignmask align, int margin = 0)
        {
            assert(align != alignmask::NONE);

            if ((align & alignmask::EXPAND) == alignmask::EXPAND)
                return bounding;

            Point p;

            if ((align & alignmask::CENTER) == alignmask::CENTER)
            {
                p.x = bounding.x + (bounding.w / 2) - (item.w / 2);
                p.y = bounding.y + (bounding.h / 2) - (item.h / 2);
            }

            if ((align & alignmask::LEFT) == alignmask::LEFT)
                p.x = bounding.x + margin;
            else if ((align & alignmask::RIGHT) == alignmask::RIGHT)
                p.x = bounding.x + bounding.w - item.w - margin;

            if ((align & alignmask::TOP) == alignmask::TOP)
                p.y = bounding.y + margin;
            else if ((align & alignmask::BOTTOM) == alignmask::BOTTOM)
                p.y = bounding.y + bounding.h - item.h - margin;

            return Rect(p, item);
        }

        /**
         * Construct a widget.
         *
         * @param[in] point Point to position the widget at.
         * @param[in] point Initial size of the widget.
         * @param[in] flags Widget flags.
         */
        Widget(const Point& point = Point(),
               const Size& size = Size(),
               widgetmask flags = widgetmask::NONE) noexcept;

        Widget(Frame& parent, const Point& point = Point(),
               const Size& size = Size(),
               widgetmask flags = widgetmask::NONE) noexcept;

        /**
         * Draw the widget.
         *
         * This is called by the internal code to cause a redraw of the widget.
         * You must implement this function in a derived class to handle drawing
         * the widget.
         *
         * @warning Do not call this directly.
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
        virtual int handle(int event);

        /**
         * Resize the widget.
         * Changes the width and height of the widget.
         *
         * @note This will cause a redraw of the widget.
         */
        virtual void resize(const Size& s);
        inline void resizew(int w)
        {
            resize(Size(w, h()));
        }
        inline void resizeh(int h)
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
        inline void movex(int x)
        {
            move(Point(x, y()));
        }
        inline void movey(int y)
        {
            move(Point(x(), y));
        }

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
            m_visible = false;
            damage();
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
        virtual void active(bool value)
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
         * Bounding box for the widgets.
         */
        virtual const Rect& box() const { return m_box; }

        /**
         * Get the size of the widget.
         */
        virtual Size size() const { return m_box.size(); }

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
         * Get the widget palette().
         */
        Palette& palette()
        {
            if (!m_palette.get())
                m_palette.reset(new Palette(global_palette()));

            return *m_palette.get();
        }

        /**
         * Get the widget palette().
         */
        const Palette& palette() const
        {
            if (m_palette.get())
                return *m_palette.get();

            return global_palette();
        }

        /**
         * Set the widget palette().
         * @note This will overwrite the entire palette.
         */
        void set_palette(const Palette& palette)
        {
            m_palette.reset(new Palette(palette));
        }

        Frame* parent()
        {
            assert(m_parent);
            return m_parent;
        }

        const Frame* parent() const
        {
            assert(m_parent);
            return m_parent;
        }

        virtual IScreen* screen();

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
         * Get the name of the widget.
         */
        const std::string& name() const { return m_name; }

        /**
         * Align the widget.
         *
         * This will align the widget relative to the box of its parent frame.
         */
        void align(alignmask a, int margin = 0);

        /**
         * Read the alignment of the widget.
         */
        inline alignmask align() const { return m_align; }

        /**
         * Set the name of the widget.
         */
        inline void name(const std::string& name) { m_name = name; }

        virtual ~Widget();

    protected:

        /**
         * Convert screen coordinates to frame coordinates.
         */
        Point screen_to_frame(const Point& p);

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

        Widget(const Widget&) = delete;
        Widget& operator=(const Widget&) = delete;

        friend class Frame;
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
                  const Point& point = Point(),
                  const Size& size = Size());

            virtual int handle(int event);

            virtual void draw(Painter& painter, const Rect& rect);

            virtual ~Combo();

        protected:
            std::string m_label;
        };

        class ScrollWheel : public Widget
        {
        public:
            ScrollWheel(const Point& point = Point(), const Size& size = Size());

            virtual int handle(int event);

            virtual void draw(Painter& painter, const Rect& rect);

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

            int m_pos {0};
            int m_moving_x {0};
            int m_start_pos {0};
        };

    }

}

#endif
