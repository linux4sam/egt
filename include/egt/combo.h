/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_COMBO_H
#define EGT_COMBO_H

/**
 * @file
 * @brief ComboBox definition.
 */

#include <egt/detail/meta.h>
#include <egt/list.h>
#include <egt/popup.h>
#include <egt/signal.h>
#include <egt/string.h>
#include <egt/widget.h>
#include <memory>
#include <string>
#include <vector>

namespace egt
{
inline namespace v1
{
class ComboBox;
class Serializer;

namespace detail
{
class ComboBoxPopup;
}

/**
 * Combo box widget.
 *
 * This manages a list of selectable items, but otherwise just shows only what
 * is selected.
 *
 * @ingroup controls
 */
class EGT_API ComboBox : public Widget
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
    /** @} */

    /// Item array type
    using ItemArray = std::vector<std::shared_ptr<StringItem>>;

    /**
     * @param[in] items Array of items to insert into the list.
     */
    explicit ComboBox(const ItemArray& items = ItemArray()) noexcept;

    /**
     * @param[in] rect Rectangle for the widget.
     */
    explicit ComboBox(const Rect& rect) noexcept;

    /**
     * @param[in] items Array of items to insert into the list.
     * @param[in] rect Rectangle for the widget.
     */
    ComboBox(const ItemArray& items, const Rect& rect) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] items Array of items to insert into the list.
     * @param[in] rect Rectangle for the widget.
     */
    explicit ComboBox(Frame& parent,
                      const ItemArray& items = {},
                      const Rect& rect = {}) noexcept;

    void handle(Event& event) override;

    void resize(const Size& s) override;

    void move(const Point& point) override;

    void draw(Painter& painter, const Rect& rect) override;

    using Widget::min_size_hint;

    EGT_NODISCARD Size min_size_hint() const override;

    /**
     * Default draw method for the ComboBox.
     */
    static void default_draw(ComboBox& widget, Painter& painter, const Rect& rect);

    /**
     * Set the index of the selected item.
     */
    void selected(size_t index);

    /**
     * Get the index of the selected item.
     */
    EGT_NODISCARD ssize_t selected() const { return m_list.selected(); }

    /**
     * Append a new item to the ComboBox.
     */
    void add_item(const std::shared_ptr<StringItem>& item);

    /**
     * Remove an item from a ComboBox.
     */
    void remove_item(StringItem* item);

    /**
     * Get an item at the specified index.
     */
    EGT_NODISCARD std::shared_ptr<StringItem> item_at(size_t index) const
    {
        return m_list.item_at(index);
    }

    /**
     * Return the number of items in the list.
     */
    EGT_NODISCARD size_t item_count() const
    {
        return m_list.item_count();
    }

    /**
     * Show the ComboBox popup.
     */
    void show_popup() const;

    /**
     * Hide the ComboBox popup.
     */
    void hide_popup() const;

    /**
     * Remove all items from the list.
     */
    void clear();

    void serialize(Serializer& serializer) const override;

    void deserialize(const std::string& name, const std::string& value,
                     const Serializer::Attributes& attrs) override;

protected:

    void set_parent(Frame* parent) override;

    /// Internal list.
    ListBox m_list;

    /// Popup associated with the ComboBox.
    std::shared_ptr<detail::ComboBoxPopup> m_popup;

    friend class detail::ComboBoxPopup;
};

}
}

#endif
