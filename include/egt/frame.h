/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef EGT_FRAME_H
#define EGT_FRAME_H

/**
 * @file
 * @brief Working with frames.
 */

#include <egt/widget.h>
#include <deque>
#include <string>

namespace egt
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
         * Construct a Frame.
         *
         * @param[in] rect Initial rectangle of the Frame.
         * @param[in] flags Widget flags.
         */
        explicit Frame(const Rect& rect,
                       widgetmask flags = widgetmask::FRAME);

        /**
         * Construct a Frame.
         *
         * @param[in] parent Parent Frame of the Frame.
         * @param[in] rect Initial rectangle of the Frame.
         * @param[in] flags Widget flags.
         */
        explicit Frame(Frame& parent, const Rect& rect,
                       widgetmask flags = widgetmask::FRAME);

        virtual int handle(eventid event) override;

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

        virtual Widget& add(Widget& widget);

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

        virtual void focus(Widget* widget) override;

        virtual bool focus() const override
        {
            return !!m_focus_widget;
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
         * Save the entire frame surface to a file.
         */
        virtual void save_to_file(const std::string& filename) override;

        void save_children_to_file()
        {
            for (auto& child : m_children)
            {
                std::ostringstream ss;
                ss << child->name() << ".png";
                child->save_to_file(ss.str().c_str());
            }
        }

        virtual ~Frame()
        {}

    protected:

        bool child_hit_test(const Point& point)
        {
            for (auto& child : m_children)
            {
                if (child->disabled())
                    continue;

                Point pos = screen_to_frame(point);

                if (Rect::point_inside(pos, child->box()))
                {
                    return true;
                }
            }

            return false;
        }

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
