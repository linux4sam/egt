/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_FRAME_H
#define EGT_FRAME_H

/**
 * @file
 * @brief Working with frames.
 */

#include <deque>
#include <egt/screen.h>
#include <egt/widget.h>
#include <egt/detail/alignment.h>
#include <exception>
#include <memory>
#include <string>

namespace egt
{
inline namespace v1
{
/**
 * A Frame is a Widget that has children widgets.
 *
 * This base class basically contains the functionality for dealing with
 * children widgets.  A widget alone is a leaf node in the widget tree.
 * Frames may or may not be leaf nodes depending on whether they have
 * children.
 *
 * This involves everything from event handling down to widgets and drawing
 * children widgets. So, a frame is a special kind of widget that can
 * compose other widgets.
 *
 * Child widget coordinates have an origin at the top left of their parent
 * frame.  In other words, child widgets are drawn respective to and inside
 * of their parent frame.
 */
class Frame : public Widget
{
public:

    /**
     * @param[in] rect Initial rectangle of the Frame.
     * @param[in] flags Widget flags.
     */
    explicit Frame(const Rect& rect = Rect(),
                   const Widget::flags_type& flags = Widget::flags_type()) noexcept;

    /**
     * @param[in] parent Parent Frame of the Frame.
     * @param[in] rect Initial rectangle of the Frame.
     * @param[in] flags Widget flags.
     */
    explicit Frame(Frame& parent, const Rect& rect,
                   const Widget::flags_type& flags = Widget::flags_type()) noexcept;

    Frame(const Frame& rhs) noexcept;

    Frame(Frame&& rhs) noexcept;

    Frame& operator=(const Frame& rhs) noexcept;

    Frame& operator=(Frame&& rhs) noexcept;

    virtual std::unique_ptr<Widget> clone() override
    {
        return std::unique_ptr<Widget>(make_unique<Frame>(*this).release());
    }

    virtual int handle(eventid event) override;

    /**
     * Add a child widget.
     *
     * This will take a reference to the shared_ptr and manage the lifetime of
     * the widget.
     *
     * The inverse of this call is Frame::remove().
     *
     * The z-order of a widget is based on the order it is added.  First in
     * is bottom, or zorder 0.
     *
     * @todo Create the idea of layers by moving m_children to a 2d array.
     * Then add a layer index (x-order) here to add widgets to different
     * layers for drawing.
     */
    virtual void add(const std::shared_ptr<Widget>& widget)
    {
        if (!widget)
            return;

        if (widget.get() == this)
            throw std::runtime_error("cannot add a widget to itself");

        auto i = std::find_if(m_children.begin(), m_children.end(),
                              [&widget](const std::shared_ptr<Widget>& ptr)
        {
            return ptr.get() == widget.get();
        });

        if (i == m_children.end())
        {
            widget->set_parent(this);
            m_children.push_back(widget);
            layout();
        }
    }

    /**
     * Utility wrapper around add()
     */
    template<class T>
    void add(const std::shared_ptr<T>& widget)
    {
        auto p = std::dynamic_pointer_cast<Widget>(widget);
        add(p);
    }

    /**
     * Add a child widget.
     *
     * Adds a reference to a widget instance that the caller will manage.
     *
     * The inverse of this call is Frame::remove().
     *
     * @warning This does not manage the lifetime of Widget. It is up to the
     * caller to make sure this Widget is available for as long as the instance
     * of this class is around.
     */
    virtual void add(Widget& widget)
    {
        // Nasty, but it gets the job done.  If a widget is passed in as a
        // reference, we don't own it, so create a "pointless" shared_ptr that
        // will not delete it.
        add(std::shared_ptr<Widget>(&widget, [](Widget*) {}));
    }

    /**
     * Remove a child widget.
     *
     * The inverse of this call is Frame::add().
     */
    virtual void remove(Widget* widget);

    /**
     * Remove all child widgets.
     */
    void remove_all();

    inline size_t count_children() const { return m_children.size(); }

    Widget* child_at(size_t index)
    {
        return m_children[index].get();
    }

    /**
     * Return true if this is a top level frame, with no parent.
     */
    virtual bool top_level() const override
    {
        return !m_parent;
    }

    /**
     * Find a child Widget in the entire tree by name.
     *
     * @see Widget::name()
     *
     * If you're trying to find a Widget in the entire application, you need
     * to start at any top level frame. This function will only search down
     * from where it's called.
     */
    template <class T>
    std::shared_ptr<T> find_child(const std::string& name)
    {
        if (name.empty())
            return nullptr;

        auto i = std::find_if(m_children.begin(), m_children.end(),
                              [&name](const std::shared_ptr<Widget>& obj)
        {
            return obj->name() == name;
        });

        // just return first one
        if (i != m_children.end())
            return *i;

        i = std::find_if(m_children.begin(), m_children.end(),
                         [&name](const std::shared_ptr<Widget>& obj)
        {
            return obj->flags().is_set(Widget::flag::frame);
        });

        for (; i != m_children.end(); ++i)
        {
            auto frame = dynamic_cast<Frame*>((*i).get());
            if (frame)
            {
                auto w = frame->find_child<T>(name);
                if (w)
                    return w;
            }
        }

        return nullptr;
    }

    /**
     * Damage the rectangle of the entire Frame.
     */
    virtual void damage() override
    {
        damage(m_box);
    }

    /**
     * This will merge the damaged area with any already existing damaged area
     * that it overlaps with into a super rectangle. Then, the whole array has
     * to be checked again to make sure the new rectangle doesn't conflict with
     * another existing rectangle.
     */
    virtual void damage(const Rect& rect) override;

    virtual void damage_from_child(const Rect& rect)
    {
        damage(rect);
    }

    virtual void draw(Painter& painter, const Rect& rect) override;

    /**
     * Cause the frame to draw itself and all of its children.
     *
     * @warning Normally this should not be called directly and instead the
     * event loop will call this function.
     */
    virtual void top_draw()
    {
        assert(0);
    }

    virtual void dump(std::ostream& out, int level = 0) override;

    virtual void walk(walk_callback_t callback, int level = 0) override;

    /**
     * Save the entire frame surface to a file.
     */
    virtual void paint_to_file(const std::string& filename = std::string()) override;

    /**
     * Paint individual children to file.
     */
    virtual void paint_children_to_file();

    virtual void show() override
    {
        if (visible())
            return;

        Widget::show();
        layout();
    }

    /**
     * @note Remember that when overriding this function as a Frame, you must call
     * layout on each child Frame to propagate the layout.
     */
    virtual void layout() override;

    /**
     * Convert a point with an origin of the current frame to child origin.
     */
    virtual Point to_child(const Point& p) const
    {
        return p - point();
    }

    /**
     * @see Frame::to_child();
     */
    inline Rect to_child(Rect rect) const
    {
        rect.point(to_child(rect.point()));
        return rect;
    }

    /**
     * Convert a local point to the coordinate system of the current panel.
     *
     * In other words, work towards a parent that has the panel so we can get
     * this point relative to the origin of the panel we are drawing to.
     */
    virtual Point to_panel(const Point& p);

    virtual void zorder_down() override
    {
        Widget::zorder_down();
    }

    virtual void zorder_up() override
    {
        Widget::zorder_up();
    }

    virtual void zorder_down(Widget* widget)
    {
        auto i = std::find_if(m_children.begin(), m_children.end(),
                              [widget](const std::shared_ptr<Widget>& ptr)
        {
            return ptr.get() == widget;
        });
        if (i != m_children.end() && i != m_children.begin())
        {
            auto to = std::prev(i);
            (*i)->damage();
            (*to)->damage();
            std::iter_swap(i, to);
        }
    }

    virtual void zorder_up(Widget* widget)
    {
        auto i = std::find_if(m_children.begin(), m_children.end(),
                              [widget](const std::shared_ptr<Widget>& ptr)
        {
            return ptr.get() == widget;
        });
        if (i != m_children.end())
        {
            auto to = std::next(i);
            if (to != m_children.end())
            {
                (*i)->damage();
                (*to)->damage();
                std::iter_swap(i, to);
            }
        }
    }

    virtual Widget* child_hit_test(const DisplayPoint& point)
    {
        for (auto& child : detail::reverse_iterate(m_children))
        {
            Point pos = to_child(from_display(point));
            if (Rect::point_inside(pos, child->box()))
            {
                if (child->flags().is_set(Widget::flag::frame))
                {
                    auto frame = static_cast<Frame*>(child.get());
                    return frame->child_hit_test(point);
                }
                else
                {
                    return child.get();
                }
            }
        }

        Point pos = from_display(point);
        if (Rect::point_inside(pos, box()))
            return this;

        return nullptr;
    }

    /**
     * Add damage to the damage array.
     *
     * Damage rects added here must have origin at point() of this frame. This
     * is done for several reasons, including if this frame is moved, all of the
     * damage rects are still valid.
     */
    virtual void add_damage(const Rect& rect);

    using special_child_draw_callback_t = std::function<void(Painter& painter, Widget* widget)>;

    inline special_child_draw_callback_t special_child_draw_callback() const
    {
        return m_special_child_draw_callback;
    }

    inline void set_special_child_draw_callback(special_child_draw_callback_t func)
    {
        m_special_child_draw_callback = func;
    }

    inline void special_child_draw(Painter& painter, Widget* widget)
    {
        if (m_special_child_draw_callback)
            m_special_child_draw_callback(painter, widget);
        else if (parent())
            parent()->special_child_draw(painter, widget);
    }

    virtual bool has_screen() const { return false; }

    virtual Frame* find_screen()
    {
        if (has_screen())
            return this;

        if (parent())
            return parent()->find_screen();

        return nullptr;

    }

    virtual ~Frame() noexcept;

protected:

    special_child_draw_callback_t m_special_child_draw_callback;

    using children_array = std::deque<std::shared_ptr<Widget>>;

    /**
     * Array of child widgets in the order they were added.
     */
    children_array m_children;

    /**
     * The damage array for this frame.
     */
    Screen::damage_array m_damage;

    bool m_in_draw{false};
    bool m_in_layout{false};
};

}
}

#endif
