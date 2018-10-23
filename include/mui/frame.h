/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_FRAME_H
#define MUI_FRAME_H

/**
 * @file
 * @brief Working with frames.
 */

#include <mui/widget.h>
#include <deque>
#include <string>

namespace mui
{
    /**
     * A frame is a widget that has children widgets.
     *
     * This base class basically contains the functionality for dealing with
     * children widgets.  A widget alone is a leaf node in the widget tree.
     * Frames may or may not be leaf nodes depending on whether they have
     * children.
     *
     * This involves everything from event handling down to widgets and drawing
     * children widgets.
     *
     * Child widget coordinates have an origin at the top left of their parent
     * frame.  In other words, child widgets are drawn respective to and inside
     * of their parent frame.
     */
    class Frame : public Widget
    {
    public:

        /**
         * @param[in] point Point to position the widget at.
         * @param[in] point Initial size of the widget.
         * @param[in] flags Widget flags.
         */
        explicit Frame(const Point& point, const Size& size,
                       widgetmask flags = widgetmask::FRAME);

        explicit Frame(Frame& parent, const Point& point, const Size& size,
                       widgetmask flags = widgetmask::FRAME);

        virtual int handle(int event) override;

        /**
         * Add a child widget.
         *
         * The z-order of a widget is based on the order it is added.  First in
         * is bottom.
        *
         * @todo Create the idea of layers by moving m_children to a 2d array.
         * Then add a layer index (x-order) here to add widgets to different
         * layers for drawing.
               */
        virtual Widget* add(Widget* widget);

        /**
         * Insert a child widget at the specified index.
         *
         * The z-order of a widget is based on the order it is added.  First in
         * is bottom.
         */
        virtual Widget* insert(Widget* widget, uint32_t index = 0);

        /**
         * Remove a child widget.
         */
        virtual void remove(Widget* widget);

        /**
         * Remove all child widgets.
         */
        virtual void remove_all();

        virtual bool focus() const override
        {
            return !!m_focus_widget;
        }

        virtual void focus(Widget* widget) override
        {
            if (m_parent)
                m_parent->focus(widget);
            else
                m_focus_widget = widget;
        }

        /**
         * Return true if this is a top level frame, with no parent.
         */
        virtual bool top_level() const
        {
            return !m_parent;
        }

        /**
         * Find a child widget by name.
         *
         * @see Widget::name()
         *
         * If you're trying to find a widget in the entire application, you need
         * to start at any top level frame. This function will only search down
         * from where it's called.
         */
        template <class T>
        T find_child(const std::string& name)
        {
            if (name.empty())
                return nullptr;

            auto i = std::find_if(m_children.begin(), m_children.end(),
                                  [&name](const Widget * obj)
            {
                return obj->name() == name;
            });

            // just return first one
            if (i != m_children.end())
                return reinterpret_cast<T>(*i);

            i = std::find_if(m_children.begin(), m_children.end(),
                             [&name](const Widget * obj)
            {
                return obj->is_flag_set(widgetmask::FRAME);
            });

            for (; i != m_children.end(); ++i)
            {
                auto frame = dynamic_cast<Frame*>(*i);
                auto w = frame->find_child<T>(name);
                if (w)
                    return w;
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

        virtual void draw(Painter& painter, const Rect& rect) override;

        /**
         * @warning Do not call directly.
         */
        virtual void draw();

        /**
         * Save the frame contents to a file.
         */
        void save_to_file(const std::string& filename);

        virtual ~Frame()
        {}

    protected:

        using children_array = std::deque<Widget*>;

        /**
         * Add damage to the damage array.
         */
        virtual void add_damage(const Rect& rect);

        /**
         * Perform the draw starting from this frame.
         */
        virtual void do_draw();

        /**
         * Array of child widgets in the order they were added.
         */
        children_array m_children;

        /**
         * The damage array for this frame.
         */
        IScreen::damage_array m_damage;

        Widget* m_focus_widget{nullptr};
    };

}

#endif
