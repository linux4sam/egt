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

#include <egt/detail/meta.h>
#include <egt/label.h>
#include <egt/signal.h>
#include <egt/sizer.h>
#include <egt/string.h>
#include <egt/view.h>
#include <egt/widget.h>
#include <string>

namespace egt
{
inline namespace v1
{

/**
 * ListBox that manages a selectable list of widgets.
 *
 * Only one item may be selected at a time. The items are based on Widget, so
 * any Widget can be used.
 *
 * @image html widget_listbox.png
 * @image latex widget_listbox.png "widget_listbox" width=5cm
 *
 * @ingroup controls
 *
 * @note This interface only supports a vertical Orientation.
 */
class EGT_API ListBox : public Widget
{
public:

    /**
     * Event signal.
     * @{
     */
    /**
     * Invoked when the selection changes.
     */
    Signal<> on_selected_changed;

    /**
     * Invoked when an item is selected with the index of the item selected.
     */
    Signal<size_t> on_selected;

    /**
     * Invoked when items are added or removed.
     */
    Signal<> on_items_changed;
    /** @} */

    /// Item array type
    using ItemArray = std::vector<std::shared_ptr<StringItem>>;

    /**
     * @param[in] items Array of items to insert into the list.
     */
    explicit ListBox(const ItemArray& items = ItemArray()) noexcept;

    /**
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit ListBox(const Rect& rect) noexcept;

    /**
     * @param[in] items Array of items to insert into the list.
     * @param[in] rect Initial rectangle of the widget.
     */
    ListBox(const ItemArray& items, const Rect& rect) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] items Array of items to insert into the list.
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit ListBox(Frame& parent, const ItemArray& items = {}, const Rect& rect = {}) noexcept;

    /**
     * @param[in] props list of widget argument and its properties.
     */
    explicit ListBox(Serializer::Properties& props) noexcept
        : ListBox(props, false)
    {
    }

protected:

    explicit ListBox(Serializer::Properties& props, bool is_derived) noexcept;

public:

    void handle(Event& event) override;

    void resize(const Size& s) override
    {
        if (s != size())
        {
            Widget::resize(s);
            auto carea = content_area();
            if (!carea.empty())
            {
                m_view.box(to_subordinate(carea));
                m_sizer.resize(carea.size());
            }
        }
    }

    /**
     * Select an item by index.
     */
    void selected(size_t index);

    /**
     * Get the currently selected index.
     *
     * @return The selected index, or -1 if there is no selection.
     */
    EGT_NODISCARD ssize_t selected() const;

    /**
     * Return the number of items in the list.
     */
    EGT_NODISCARD size_t item_count() const { return m_sizer.count_children(); }

    /**
     * Add a new item to the end of the list.
     */
    void add_item(const std::shared_ptr<StringItem>& item);

    /**
     * Add a new item to the end of the list.
     *
     * @param item The item.
     *
     * @warning This does not manage the lifetime of StringItem. It is up to
     * the caller to make sure this StringItem is available for as long as the
     * instance of this class is around.
     */
    void add_item(StringItem& item)
    {
        // Nasty, but it gets the job done.  If a widget is passed in as a
        // reference, we don't own it, so create a "pointless" shared_ptr that
        // will not delete it.
        auto i = std::shared_ptr<StringItem>(&item, [](StringItem*) {});
        add_item(i);
    }

    /**
     * Get the currently selected index item from list.
     */
    EGT_NODISCARD std::shared_ptr<StringItem> item_at(size_t index) const;

    /**
     * Remove an item from the list.
     */
    void remove_item(StringItem* item);

    /**
     * Remove all items from the list.
     */
    void clear();

    /**
     * Scroll all the way to the top of the list.
     */
    void scroll_top();

    /**
     * Scroll all the way to the bottom of the list.
     */
    void scroll_bottom();

    /**
     * Scroll all the way to the beginning of the list, either horizontally or
     * vertically.
     */
    void scroll_beginning();

    /**
     * Scroll all the way to the end of the list, either horizontally or
     * vertically.
     */
    void scroll_end();

    /**
     * Set the orientation of the list: either vertical or horizontal.
     */
    void orient(Orientation orient);

    /**
     * Get the orientation of the list: either vertical or horizontal.
     */
    EGT_NODISCARD Orientation orient() const
    {
        return m_orient;
    }

    void serialize(Serializer& serializer) const override;

protected:

    /// View used to contain the possible large sizer.
    ScrolledView m_view;

    /// Internal sizer used to layout items.
    BoxSizer m_sizer;

    /// List orientation, either vertical or horizontal.
    Orientation m_orient{Orientation::vertical};

private:

    void add_item_private(const std::shared_ptr<StringItem>& item);

    void deserialize(Serializer::Properties& props);
};

}
}

#endif
