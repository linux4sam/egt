/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_LIST_H
#define EGT_LIST_H

/**
 * @file
 * @brief ListBox definition.
 */

#include <egt/widget.h>
#include <egt/painter.h>
#include <egt/palette.h>
#include <egt/frame.h>
#include <vector>

namespace egt
{
    class ListBox;

    /**
     * An item in a ListBox.
     */
    class ListBoxItem
    {
    public:
        virtual void draw(Painter& painter, const Rect& rect, bool selected, ListBox& listbox);
    };

    /**
     * A specialized ListBoxItem that holds a simple string.
     */
    class StringItem : public ListBoxItem
    {
    public:

        /**
         * Construct a List item.
         */
        // cppcheck-suppress noExplicitConstructor
        StringItem(const char* text)
            : m_text(text)
        {}

        /**
         * Construct a List item.
         */
        // cppcheck-suppress noExplicitConstructor
        StringItem(const std::string& text)
            : m_text(text)
        {}

        virtual void draw(Painter& painter, const Rect& rect, bool selected, ListBox& listbox) override;

        /**
        * Set the font of the items.
        */
        virtual void font(const Font& font) { m_font = font; }

        virtual ~StringItem()
        {}

    protected:

        std::string m_text;
        Font m_font;
    };

    /**
     * ListBox that manages a selectable list of items.
     *
     * Items are derived from type ListBoxItem. Only one item may be selected at
     * a time.
     *
     * @image html widget_listbox.png
     * @image latex widget_listbox.png "widget_listbox" width=5cm
     */
    class ListBox : public Widget
    {
    public:
        using item_array = std::vector<ListBoxItem*>;

        explicit ListBox(const Rect& rect = Rect())
            : Widget(rect)
        {
        }

        explicit ListBox(Frame& parent,
                         const Rect& rect = Rect())
            : ListBox(rect)
        {
            parent.add(this);
        }

        template<class T>
        explicit ListBox(const std::vector<T>& items,
                         const Rect& rect = Rect())
            : ListBox(rect)
        {
            std::copy(items.begin(), items.end(), back_inserter(m_items));
        }

        template<class T>
        explicit ListBox(Frame& parent,
                         const std::vector<T>& items,
                         const Rect& rect = Rect())
            : ListBox(items, rect)
        {
            parent.add(this);
        }

        virtual int handle(eventid event) override;

        virtual void draw(Painter& painter, const Rect& rect) override;

        /**
         * Select an item by index.
         */
        virtual void set_select(uint32_t index);

        /**
         * Return the number of items in the list.
         */
        virtual size_t count() const { return m_items.size(); }

        /**
         * Get the currently selected index.
         */
        virtual uint32_t selected() const { return m_selected; }

        /**
         * Add a new item to the end of the list.
         */
        virtual void add_item(ListBoxItem* item)
        {
            m_items.push_back(item);
        }

        virtual ~ListBox();

    protected:

        Rect item_rect(uint32_t index) const;

        item_array m_items;
        uint32_t m_selected{0};
    };

    namespace experimental
    {
        /**
         * Combo box widget.
         */
        class ComboBox : public TextWidget
        {
        public:
            using item_array = std::vector<std::string>;

            ComboBox(const item_array& items = item_array(),
                     const Rect& rect = Rect(),
                     alignmask align = alignmask::CENTER | alignmask::LEFT,
                     const Font& font = Font(),
                     widgetmask flags = widgetmask::NO_BORDER) noexcept;

            virtual int handle(eventid event) override;

            virtual void set_select(uint32_t index);

            virtual uint32_t selected() const { return m_selected; }

            virtual void draw(Painter& painter, const Rect& rect) override;

            virtual ~ComboBox();

        protected:
            item_array m_items;
            uint32_t m_selected{0};
            Rect m_up_rect;
            Rect m_down_rect;
        };

    }

}

#endif
