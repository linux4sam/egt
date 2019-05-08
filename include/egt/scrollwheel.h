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
#include <egt/grid.h>
#include <egt/label.h>
#include <memory>
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
 */
class Scrollwheel : public StaticGrid
{
public:
    using item_array = std::vector<std::string>;

    /**
     * @param[in] rect Rectangle for the Widget.
     * @param[in] items Array of items to build the list.
     * @param[in] reversed Display the latest item instead of the first one.
     */
    explicit Scrollwheel(const Rect& rect = Rect(),
                         const item_array& items = item_array(),
                         bool reversed = false) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Rectangle for the Widget.
     * @param[in] items Array of items to build the list.
     * @param[in] reversed Display the latest item instead of the first one.
     */
    explicit Scrollwheel(Frame& parent, const Rect& rect = Rect(),
                         const item_array& items = item_array(),
                         bool reversed = false) noexcept;

    /**
     * @param[in] min The range minimum value.
     * @param[in] max The range maximum value.
     * @param[in] step The value of step to create a list from the minimum value to the maximum one.
     * @param[in] reversed Display the latest item instead of the first one.
     * @param[in] rect Rectangle for the Widget.
     */
    explicit Scrollwheel(const Rect& rect, int min, int max, int step, bool reversed = false) noexcept;

    /**
     * Returns a string of the item selected.
     */
    std::string get_value() const;

    /**
     * Add an item at the end of the array.
     */
    void add_item(std::string item);

    /**
     * Get the index of the item selected.
     */
    unsigned int get_index() const;

    /**
     * Set the index to select a specific item.
     */
    void set_index(unsigned int index);

protected:
    void init(bool reversed);
    item_array m_items;
    unsigned int m_selected{0};
    std::shared_ptr<Button> m_button_up;
    std::shared_ptr<Button> m_button_down;
    std::shared_ptr<Label> m_label;
};

}
}

#endif
