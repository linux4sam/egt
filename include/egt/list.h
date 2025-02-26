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
#include <list>
#include <string>

namespace egt
{
inline namespace v1
{

/**
 * ListBoxBase is an abstract class that can be used as a base class to
 * implement list boxes.
 */
class EGT_API ListBoxBase : public Widget
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
     * Invoked when items are added or removed.
     */
    Signal<> on_items_changed;
    /** @} */

    /// Item array type
    using ItemArray = std::vector<std::shared_ptr<StringItem>>;

protected:

    /**
     * @param[in] items Array of items to insert into the list.
     */
    explicit ListBoxBase(const ItemArray& items = ItemArray()) noexcept;

    /**
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit ListBoxBase(const Rect& rect) noexcept;

    /**
     * @param[in] items Array of items to insert into the list.
     * @param[in] rect Initial rectangle of the widget.
     */
    ListBoxBase(const ItemArray& items, const Rect& rect) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] items Array of items to insert into the list.
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit ListBoxBase(Frame& parent, const ItemArray& items = {}, const Rect& rect = {}) noexcept;

    explicit ListBoxBase(Serializer::Properties& props) noexcept;

public:

    virtual ~ListBoxBase() = default;

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
    virtual void selected(size_t index) = 0;

    /**
     * Return the number of items in the list.
     */
    EGT_NODISCARD size_t item_count() const { return m_sizer.count_children(); }

    /**
     * Add a new item to the end of the list.
     */
    void add_item(const std::shared_ptr<StringItem>& item);

    /**
     * Insert a new item to the list at the specified position.
     */
    void add_item_at(const std::shared_ptr<StringItem>& item, size_t pos);

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
     * Insert a new item to the list at the specified position.
     *
     * If the position is incorrect, the item is added at the end.
     *
     * @param item The item.
     * @param pos The index where to insert the item.
     *
     * @warning This does not manage the lifetime of StringItem. It is up to
     * the caller to make sure this StringItem is available for as long as the
     * instance of this class is around.
     */
    void add_item_at(StringItem& item, size_t pos)
    {
        // Nasty, but it gets the job done.  If a widget is passed in as a
        // reference, we don't own it, so create a "pointless" shared_ptr that
        // will not delete it.
        auto i = std::shared_ptr<StringItem>(&item, [](StringItem*) {});
        add_item_at(i, pos);
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
     * Remove the item at the specified position.
     *
     * If the position is incorrect, no item is removed.
     *
     * If the item to remove is selected, selection is left unchanged, i.e. no
     * other item is automatically selected even if the item was the only one
     * selected.
     *
     * @param pos The index of the item.
     */
    void remove_item_at(size_t pos);

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

    void add_item_private(const std::shared_ptr<StringItem>& item, ssize_t pos = -1);

    void deserialize(Serializer::Properties& props);
};

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
class EGT_API ListBox : public ListBoxBase
{
public:

    /**
     * Event signal.
     * @{
     */
    /**
     * Invoked when an item is selected with the index of the item selected.
     */
    Signal<size_t> on_selected;
    /** @} */


    /**
     * @param[in] items Array of items to insert into the list.
     */
    explicit ListBox(const ItemArray& items = ItemArray()) noexcept
        : ListBoxBase(items)
    {
    }

    /**
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit ListBox(const Rect& rect) noexcept
        : ListBoxBase(rect)
    {
    }

    /**
     * @param[in] items Array of items to insert into the list.
     * @param[in] rect Initial rectangle of the widget.
     */
    ListBox(const ItemArray& items, const Rect& rect) noexcept
        : ListBoxBase(items, rect)
    {
    }

    /**
     * @param[in] parent The parent Frame.
     * @param[in] items Array of items to insert into the list.
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit ListBox(Frame& parent, const ItemArray& items = {}, const Rect& rect = {}) noexcept
        : ListBoxBase(parent, items, rect)
    {
    }

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

    /**
     * Select an item by index.
     */
    void selected(size_t index) override;

    /**
     * Get the currently selected index.
     *
     * @return The selected index, or -1 if there is no selection.
     */
    EGT_NODISCARD ssize_t selected() const;
};

/**
 * ListBoxMulti a listbox that manages a selectable list of items and allows
 * multi selection.
 *
 * Several items may be selected at a time. The items are based on StringItem.
 *
 * Selected and deselected methods set the status of all items. For example,
 * selected({1, 3, 5}) selects item at indexes 1, 3, 5, while deselecting
 * all other items.
 *
 * Select, deselect and toggle methods update the status of for the specified
 * indexes without affecting the status of other items.
 *
 * Events are triggered only when there is a change in the status of an item.
 * For instance, selecting an item that is  already selected will not generate
 * an event.
 *
 * @ingroup controls
 *
 * @note This interface only supports a vertical Orientation.
 */
class EGT_API ListBoxMulti : public ListBoxBase
{
public:

    /**
     * Event signal.
     * @{
     */
    /**
     * Invoked when items are selected with the list of the indexes of the
     * items selected.
     */
    Signal<std::list<size_t>> on_selected;

    /**
     * Invoked when items are deselected with the list of the indexes of the
     * items deselected.
     */
    Signal<std::list<size_t>> on_deselected;
    /** @} */


    /**
     * @param[in] items Array of items to insert into the list.
     */
    explicit ListBoxMulti(const ItemArray& items = ItemArray()) noexcept;

    /**
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit ListBoxMulti(const Rect& rect) noexcept;

    /**
     * @param[in] items Array of items to insert into the list.
     * @param[in] rect Initial rectangle of the widget.
     */
    ListBoxMulti(const ItemArray& items, const Rect& rect) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] items Array of items to insert into the list.
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit ListBoxMulti(Frame& parent, const ItemArray& items = {}, const Rect& rect = {}) noexcept;

    /**
     * @param[in] props list of widget argument and its properties.
     */
    explicit ListBoxMulti(Serializer::Properties& props) noexcept
        : ListBoxMulti(props, false)
    {
    }

protected:

    explicit ListBoxMulti(Serializer::Properties& props, bool is_derived) noexcept;

public:

    void handle(Event& event) override;

    /**
     * Set the selected item, other items are deselected.
     */
    void selected(size_t index) override;

    /**
     * Set a list of selected items, other items are deselected.
     */
    void selected(const std::list<size_t>& indexes);

    /**
     * Set a range [start_index, end_index) of selected items, other items
     * are deselected.
     */
    void selected(size_t start_index, size_t end_index);

    /**
     * Set the deselected item, other items are selected.
     */
    void deselected(size_t index);

    /**
     * Set a list of deselected items, other items are selected.
     */
    void deselected(const std::list<size_t>& indexes);

    /**
     * Set a range [start_index, end_index) of deselected items, other items
     * are selected.
     */
    void deselected(size_t start_index, size_t end_index);

    /**
     * Select an item by index, others items are left unchanged.
     */
    void select(size_t index);

    /**
     * Select a list of items by indexes, others items are left unchanged.
     */
    void select(const std::list<size_t>& indexes);

    /**
     * Select a range [start_index, end_index) of items, others items are
     * left unchanged.
     */
    void select(size_t start_index, size_t end_index);

    /**
     * Select all the items.
     */
    void select_all();

    /**
     * Deselect an item by index, others items are left unchanged.
     */
    void deselect(size_t index);

    /**
     * Deselect a list of items by indexes, others items are left unchanged.
     */
    void deselect(const std::list<size_t>& indexes);

    /**
     * Deselect a range [start_index, end_index) of items, others items are
     * left unchanged.
     */
    void deselect(size_t start_index, size_t end_index);

    /**
     * Deselect all the items.
     */
    void deselect_all();

    /**
     * Toggle an item by index.
     */
    void toggle(size_t index);

    /**
     * Toggle a list of items by indexes.
     */
    void toggle(const std::list<size_t>& indexes);

    /**
     * Toggle a range [start_index, end_index) of items.
     */
    void toggle(size_t start_index, size_t end_index);

    /**
     * Toggle all the items.
     */
    void toggle_all();

    /**
     * Get the currently selected indexes.
     *
     * @return A list of selected index, an empty list if any.
     */
    EGT_NODISCARD std::list<size_t> selected() const;

    /**
     * Get the currently deselected indexes.
     *
     * @return A list of deselected index, an empty list if any.
     */
    EGT_NODISCARD std::list<size_t> deselected() const;

protected:

    bool selected(const std::list<size_t>& indexes,
                  bool selected,
                  std::list<size_t>& selected_update,
                  std::list<size_t>& deselected_update);
    bool select(const std::list<size_t>& indexes,
                bool selected,
                std::list<size_t>& selected_update,
                std::list<size_t>& deselected_update);
};

}
}

#endif
