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
#include <egt/detail/meta.h>
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
class EGT_API Frame : public Widget
{
public:

    /**
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] flags Widget flags.
     */
    explicit Frame(const Rect& rect = {},
                   const Flags& flags = {}) noexcept;

    virtual void handle(Event& event) override;

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
     * @param widget The widget.
     *
     * @todo Create the idea of layers by moving m_children to a 2d array.
     * Then add a layer index (x-order) here to add widgets to different
     * layers for drawing.
     */
    virtual void add(std::shared_ptr<Widget> widget);

    /**
     * Utility wrapper around add()
     *
     * @param widget The widget.
     */
    template<class T>
    inline void add(std::shared_ptr<T>& widget)
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
     * @param widget The widget.
     *
     * @warning This does not manage the lifetime of Widget. It is up to the
     * caller to make sure this Widget is available for as long as the instance
     * of this class is around.
     */
    inline void add(Widget& widget)
    {
        // Nasty, but it gets the job done.  If a widget is passed in as a
        // reference, we don't own it, so create a "pointless" shared_ptr that
        // will not delete it.
        add(std::shared_ptr<Widget>(&widget, [](Widget*) {}));
    }

    /**
     * Returns true if the child exists.
     */
    bool is_child(Widget* widget) const;

    /**
     * Remove a child widget.
     *
     * The inverse of this call is Frame::add().
     *
     * @param widget The widget.
     */
    virtual void remove(Widget* widget);

    /**
     * Remove all child widgets.
     */
    void remove_all();

    /**
     * Get the number of children widgets.
     */
    inline size_t count_children() const { return m_children.size(); }

    /**
     * Get a child widget at a specific index.
     */
    std::shared_ptr<Widget> child_at(size_t index)
    {
        return m_children.at(index);
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
            return std::dynamic_pointer_cast<T>(*i);

        i = std::find_if(m_children.begin(), m_children.end(),
                         [](const std::shared_ptr<Widget>& obj)
        {
            return obj->flags().is_set(Widget::Flag::frame);
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

    /**
     * Special variation of damage() that is to be called explicitly by child
     * widgets.
     */
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
    virtual void begin_draw()
    {
        assert(0);
    }

    virtual void dump(std::ostream& out, int level = 0) override;

    virtual void walk(const WalkCallback& callback, int level = 0) override;

    /**
     * Save the entire frame surface to a file.
     */
    virtual void paint_to_file(const std::string& filename = {}) override;

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
     * @note Remember that when overriding this function as a Frame, you must
     * call layout on each child Frame to propagate the layout.
     */
    virtual void layout() override;

    virtual void resize(const Size& size) override
    {
        if (size != this->size())
        {
            Widget::resize(size);
            layout();
        }
    }

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

    virtual void zorder_down() override;

    virtual void zorder_up() override;

    /**
     * Move the specified widget zorder down relative to other widgets with the
     * same parent.
     *
     * @param widget The widget.
     */
    virtual void zorder_down(const Widget* widget);

    /**
     * Move the specified widget zorder up relative to other widgets with the
     * same parent.
     *
     * @param widget The widget.
     */
    virtual void zorder_up(const Widget* widget);

    virtual void zorder_bottom() override;

    virtual void zorder_top() override;

    /**
     * Move the specified widget zorder to the bottom of the current list of widgets
     * with the same parent.
     *
     * @param widget The widget.
     */
    virtual void zorder_bottom(const Widget* widget);

    /**
     * Move the specified widget zorder to the top of the current list of widgets
     * with the same parent.
     *
     * @param widget The widget.
     */
    virtual void zorder_top(const Widget* widget);

    /**
     * Get the zorder of the widget.
     *
     * @param widget The widget.
     */
    virtual size_t zorder(const Widget* widget) const;

    virtual size_t zorder() const override;

    /**
     * Get the widget under the given DisplayPoint.
     *
     * @return The widget pointer or nullptr if not found.
     */
    virtual Widget* hit_test(const DisplayPoint& point);

    /**
     * Add damage to the damage array.
     *
     * Damage rects added here must have origin at point() of this frame. This
     * is done for several reasons, including if this frame is moved, all of the
     * damage rects are still valid.
     */
    virtual void add_damage(const Rect& rect);

    /**
     * Helper type that defines the special draw child callback.
     */
    using ChildDrawCallback = std::function<void(Painter& painter, Widget* widget)>;

    /**
     * Get the special child draw callback.
     */
    inline ChildDrawCallback special_child_draw_callback() const
    {
        return m_special_child_draw_callback;
    }

    /**
     * Set the special child draw callback.
     */
    inline void special_child_draw_callback(ChildDrawCallback func)
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
    inline void special_child_draw(Painter& painter, Widget* widget)
    {
        if (m_special_child_draw_callback)
            m_special_child_draw_callback(painter, widget);
        else if (parent())
            parent()->special_child_draw(painter, widget);
    }

    /**
     * Does this Frame have a screen?
     */
    virtual bool has_screen() const { return false; }

    /**
     * Starting from this Frame, find the Frame that has a Screen.
     *
     * This searches up the widget hierarchy from here.
     */
    virtual Frame* find_screen()
    {
        if (has_screen())
            return this;

        if (parent())
            return parent()->find_screen();

        return nullptr;
    }

    /**
     * Starting from this Frame, find the Frame that has a Screen.
     *
     * This searches up the widget hierarchy from here.
     */
    virtual const Frame* find_screen() const
    {
        if (has_screen())
            return this;

        if (parent())
            return parent()->find_screen();

        return nullptr;
    }

    /**
     * Create a child widget of the specified type.
     *
     * This allocated a widget of the specified type and automatically adds it
     * as a child to this Frame.
     *
     * @code{.cpp}
     * egt::TopWindow win;
     * auto widget = win.spawn<egt::TextBox>("hello world");
     * @endcode
     *
     * @return a std::unique_ptr of the allocated widget.
     */
    template<typename T, typename... Args>
    std::unique_ptr<T> spawn(Args&& ... args)
    {
        auto w = std::make_unique<T>(std::forward<Args>(args)...);
        add(w);
        return w;
    }

    virtual ~Frame() noexcept;

protected:

    /// @private
    void draw_child(Painter& painter, const Rect& crect, Widget* child);

    /**
     * Used internally for calling the special child draw function.
     */
    ChildDrawCallback m_special_child_draw_callback;

    /**
     * Helper type for an array of children.
     */
    using ChildrenArray = std::deque<std::shared_ptr<Widget>>;

    /**
     * Array of child widgets in the order they were added.
     */
    ChildrenArray m_children;

    /**
     * The damage array for this frame.
     */
    Screen::DamageArray m_damage;

    /**
     * Status for whether this frame is currently drawing.
     */
    bool m_in_draw{false};

    /**
     * Status for whether this frame is currently performing layout.
     */
    bool m_in_layout{false};

private:

    void remove_all_basic();
};

}
}

#endif
