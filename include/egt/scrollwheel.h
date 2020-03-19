/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SCROLLWHEEL_H
#define EGT_SCROLLWHEEL_H

/**
 * @file
 * @brief Scrollwheel definition.
 *
 */

#include <egt/button.h>
#include <egt/detail/meta.h>
#include <egt/detail/signal.h>
#include <egt/grid.h>
#include <egt/label.h>
#include <string>
#include <vector>

namespace egt
{
inline namespace v1
{
/**
 * Scrollwheel widget.
 *
 * Manages a list of selectable items. Only the one selected is shown.
 * Navigation through the list is done with the top and bottom arrows.
 *
 * @ingroup controls
 */
class EGT_API Scrollwheel : public StaticGrid
{
public:

    /**
     * Event signal.
     * @{
     */
    /// Invoked when the value changes.
    detail::Signal<> on_value_changed;
    /** @} */

    /// Item array type.
    using ItemArray = std::vector<std::string>;

    /**
     * @param[in] items Array of items to build the list.
     */
    explicit Scrollwheel(const ItemArray& items = {}) noexcept;

    /**
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] items Array of items to build the list.
     */
    explicit Scrollwheel(const Rect& rect,
                         const ItemArray& items = {}) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] items Array of items to build the list.
     */
    explicit Scrollwheel(Frame& parent,
                         const ItemArray& items = {}) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] items Array of items to build the list.
     */
    explicit Scrollwheel(Frame& parent,
                         const Rect& rect,
                         const ItemArray& items = {}) noexcept;

    /**
     * @param[in] min The range minimum value.
     * @param[in] max The range maximum value.
     * @param[in] step The value of step to create a list from the minimum value to the maximum one.
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit Scrollwheel(const Rect& rect, int min, int max, int step) noexcept;

    /**
     * Change the images used for up and down buttons.
     */
    virtual void image(const Image& down_image, const Image& up_image);

    /**
     * Get the Orientation.
     */
    Orientation orient() const { return m_orient; }

    /**
     * Set the Orientation.
     */
    void orient(Orientation orient);

    /**
     * Returns a string of the item selected.
     */
    std::string value() const;

    /**
     * Add an item at the end of the array.
     */
    virtual void add_item(const std::string& item);

    /**
     * Remove an item from the array.
     */
    virtual bool remove_item(const std::string& item);

    /**
     * Remove all the items.
     */
    virtual void clear_items();

    /**
     * Get the index of the item selected.
     */
    virtual size_t selected() const;

    /**
     * Set the index to select a specific item.
     */
    virtual void selected(size_t index);

    /**
     * Return the number of items.
     */
    virtual size_t item_count() const { return m_items.size(); }

    /**
     * Enable or disable the reversed mode. When enabled the behavior of
     * the up and down button is swapped.
     */
    virtual void reversed(bool enabled);

protected:
    /// @private
    void init();
    /// Array of items.
    ItemArray m_items;
    /// Currently selected index.
    size_t m_selected{0};
    /// Up button.
    ImageButton m_button_up;
    /// Down button.
    ImageButton m_button_down;
    /// Label for the value.
    Label m_label;
    /// Reversed direction of up/down.
    bool m_reversed{false};
    /// Orientation of the Scrollwheel.
    Orientation m_orient{Orientation::vertical};
};

}
}

#endif
