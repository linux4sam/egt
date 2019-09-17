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
 *
 * @ingroup controls
 */
class Scrollwheel : public StaticGrid
{
public:
    using item_array = std::vector<std::string>;

    /**
     * @param[in] items Array of items to build the list.
     * @param[in] reversed Display the latest item instead of the first one.
     */
    explicit Scrollwheel(const item_array& items = {},
                         bool reversed = false) noexcept;

    /**
     * @param[in] rect Rectangle for the Widget.
     * @param[in] items Array of items to build the list.
     * @param[in] reversed Display the latest item instead of the first one.
     */
    explicit Scrollwheel(const Rect& rect,
                         const item_array& items = {},
                         bool reversed = false) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] items Array of items to build the list.
     * @param[in] reversed Display the latest item instead of the first one.
     */
    explicit Scrollwheel(Frame& parent,
                         const item_array& items = {},
                         bool reversed = false) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Rectangle for the Widget.
     * @param[in] items Array of items to build the list.
     * @param[in] reversed Display the latest item instead of the first one.
     */
    explicit Scrollwheel(Frame& parent,
                         const Rect& rect,
                         const item_array& items = {},
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
     * Change the images used for up and down buttons.
     */
    virtual void set_image(const Image& down_image, const Image& up_image);

    /**
     * Get the orientation.
     */
    inline orientation orient() const { return m_orient; }

    /**
     * Set the orientation.
     */
    void set_orient(orientation orient);

    /**
     * Returns a string of the item selected.
     */
    std::string value() const;

    /**
     * Add an item at the end of the array.
     */
    virtual void add_item(const std::string& item);

    /**
     * Get the index of the item selected.
     */
    virtual size_t selected() const;

    /**
     * Set the index to select a specific item.
     */
    virtual void set_selected(size_t index);

    /**
     * Return the number of items.
     */
    virtual size_t item_count() const { return m_items.size(); }

protected:
    void init(bool reversed);
    item_array m_items;
    size_t m_selected{0};
    std::shared_ptr<ImageButton> m_button_up;
    std::shared_ptr<ImageButton> m_button_down;
    std::shared_ptr<Label> m_label;
    orientation m_orient{orientation::vertical};
};

}
}

#endif
