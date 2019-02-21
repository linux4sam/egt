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

#include <egt/frame.h>
#include <egt/label.h>
#include <egt/sizer.h>
#include <egt/view.h>
#include <vector>

namespace egt
{
inline namespace v1
{

using StringItem = egt::Label;

/**
 * @brief ListBox that manages a selectable list of items.
 *
 * Only one item may be selected at a time. The items are based on Widget, so
 * any widget can be used.
 *
 * @image html widget_listbox.png
 * @image latex widget_listbox.png "widget_listbox" width=5cm
 */
class ListBox : public Frame
{
public:

    using item_array = std::vector<Widget*>;

    explicit ListBox(const Rect& rect = Rect());

    explicit ListBox(Frame& parent,
                     const Rect& rect = Rect());

    explicit ListBox(const item_array& items,
                     const Rect& rect = Rect());

    ListBox(Frame& parent,
            const item_array& items,
            const Rect& rect = Rect());

    virtual int handle(eventid event) override;

    virtual Rect child_area() const override;

    /**
     * Select an item by index.
     */
    virtual void set_select(uint32_t index);

    /**
     * Return the number of items in the list.
     */
    virtual size_t count() const { return m_sizer.count_children(); }

    /**
     * Get the currently selected index.
     */
    virtual uint32_t selected() const { return m_selected; }

    /**
     * Add a new item to the end of the list.
     */
    virtual void add_item(Widget* item);

    /**
     * Remove an item from the list.
     */
    virtual void remove_item(Widget* widget);

    static inline size_t item_height()
    {
        return 40;
    }

    virtual ~ListBox();

protected:

    Rect item_rect(uint32_t index) const;

    uint32_t m_selected{0};
    ScrolledView m_view;
    OrientationPositioner m_sizer;
};

}
}

#endif
