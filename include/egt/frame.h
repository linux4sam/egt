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

#include <cassert>
#include <egt/detail/alignment.h>
#include <egt/detail/meta.h>
#include <egt/screen.h>
#include <egt/widget.h>
#include <exception>
#include <memory>
#include <string>
#include <vector>

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

    /**
     * @param[in] parent Parent Frame of the Widget.
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] flags Widget flags.
     */
    Frame(Frame& parent, const Rect& rect,
          const Flags& flags = {}) noexcept;

    /**
     * @param[in] props array of widget arguments and its properties.
     */
    Frame(Serializer::Properties& props) noexcept
        : Frame(props, false)
    {
    }

protected:

    Frame(Serializer::Properties& props, bool is_derived) noexcept;

public:

    Frame(const Frame&) = delete;
    Frame& operator=(const Frame&) = delete;
    Frame(Frame&&) noexcept = default;
    Frame& operator=(Frame&&) noexcept = default;

    ~Frame() noexcept override;

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
    virtual void add(const std::shared_ptr<Widget>& widget);

    /**
     * Utility wrapper around add()
     *
     * @param widget The widget.
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
     * @param widget The widget.
     *
     * @warning This does not manage the lifetime of Widget. It is up to the
     * caller to make sure this Widget is available for as long as the instance
     * of this class is around.
     */
    void add(Widget& widget)
    {
        // Nasty, but it gets the job done.  If a widget is passed in as a
        // reference, we don't own it, so create a "pointless" shared_ptr that
        // will not delete it.
        auto w = std::shared_ptr<Widget>(&widget, [](Widget*) {});
        add(w);
    }

    /**
     * Returns true if the child exists.
     */
    bool is_child(Widget* widget) const;

    EGT_NODISCARD virtual Point to_child(const Point& p) const
    {
        return Widget::to_subordinate(p);
    }

    EGT_NODISCARD Rect to_child(Rect rect) const
    {
        return Widget::to_subordinate(rect);
    }

    using Widget::remove;

    /**
     * Remove all child widgets.
     */
    void remove_all();

    using Widget::children;

    /**
     * Get the number of children widgets.
     */
    EGT_NODISCARD size_t count_children() const { return children().size(); }

    /**
     * Get a child widget at a specific index.
     */
    EGT_NODISCARD std::shared_ptr<Widget> child_at(size_t index) const
    {
        if (index >= children().size())
            return nullptr;
        else
            return *std::next(children().begin(), index);
    }

    /**
     * Return true if this is a top level frame, with no parent.
     */
    EGT_NODISCARD bool top_level() const override
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

        auto i = std::find_if(children().begin(), children().end(),
                              [&name](const auto & obj)
        {
            return obj->name() == name;
        });

        // just return first one
        if (i != children().end())
            return std::dynamic_pointer_cast<T>(*i);

        i = std::find_if(children().begin(), children().end(),
                         [](const auto & obj)
        {
            return obj->frame();
        });

        for (; i != children().end(); ++i)
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

    void walk(const WalkCallback& callback, int level = 0) override;

    /**
     * Save the entire frame surface to a file.
     */
    void paint_to_file(const std::string& filename = {}) override;

    /**
     * Paint individual children to file.
     */
    void paint_children_to_file();

    using Widget::zorder;
    using Widget::zorder_down;
    using Widget::zorder_up;
    using Widget::zorder_bottom;
    using Widget::zorder_top;

    void show() override
    {
        if (visible())
            return;

        Widget::show();
        layout();
    }

    /**
     * Get the widget under the given DisplayPoint.
     *
     * @return The widget pointer or nullptr if not found.
     */
    Widget* hit_test(const DisplayPoint& point);

    /**
     * Create a child widget of the specified type.
     *
     * This allocates a widget of the specified type and automatically adds it
     * as a child to this Frame.
     *
     * @code{.cpp}
     * egt::TopWindow win;
     * auto widget = win.spawn<egt::TextBox>("hello world");
     * @endcode
     *
     * @return a std::shared_ptr of the allocated widget.
     */
    template<typename T, typename... Args>
    std::shared_ptr<T> spawn(Args&& ... args)
    {
        auto w = std::make_shared<T>(std::forward<Args>(args)...);
        add(w);
        return w;
    }

    using Widget::special_child_draw_callback;

    void serialize(Serializer& serializer) const override;

    void serialize_children(Serializer& serializer) const override;

    void deserialize_children(const Deserializer& deserializer) override;

    /// Overridden to be called recursively on all children.
    void on_screen_resized() override;

private:

    void remove_all_basic();
};

}
}

#endif
