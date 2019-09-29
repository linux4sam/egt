/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_BUTTONGROUP_H
#define EGT_BUTTONGROUP_H

/**
 * @file
 * @brief Working with button groups.
 */

#include <functional>
#include <memory>
#include <vector>

namespace egt
{
inline namespace v1
{

class Button;

/**
 * Button group.
 *
 * Several buttons can be added to a group. A button must belong to
 * only one group.
 *
 * If the group is exclusive, only one button in the group can have its
 * checked state to true.
 *
 * If the group is imperative, it ensures that at least one button is
 * checked.
 */
class ButtonGroup
{
public:

    /**
     * @param imperative When true, one button must always be checked.
     * @param exclusive When true, only one button can be checked at a time.
     */
    explicit ButtonGroup(bool imperative = false, bool exclusive = true) noexcept;

    /**
     * Set the exclusive property of the group.
     *
     * @param exclusive When true, only one button can be checked at a time.
     *
     * @note Changing this after adding buttons is undefined.
     */
    virtual void set_exclusive(bool exclusive);

    /**
     * Get the exclusive property.
     */
    virtual bool exclusive() const;

    /**
     * Set the imperative property of the group.
     *
     * @param imperative When true, one must must always be checked.
     *
     * @note Changing this after adding buttons is undefined.
     */
    virtual void set_imperative(bool imperative);

    /**
     * Get the imperative property.
     */
    virtual bool imperative() const;

    /**
     * Add a button to the group.
     */
    virtual void add(const std::shared_ptr<Button>& button);

    /**
     * Remove a button from the group.
     */
    virtual void remove(Button* button);


    virtual void checked_state_change(Button& button, bool checked) const;

    using foreach_checked_callback_t = std::function<void (Button& button)>;

    /**
     * Run a function on each checked button.
     *
     * If the exclusive property is true, only one button will even be
     * invoked.
     *
     * @param callback The callback to invoke.
     */
    virtual void foreach_checked(const foreach_checked_callback_t& callback);

    virtual ~ButtonGroup();

protected:

    using button_array = std::vector<std::weak_ptr<Button>>;

    /**
     * Buttons in the group.
     */
    button_array m_buttons;

    /**
     * Only one button can be checked().
     */
    bool m_exclusive{false};

    /**
     * At least one button must be checked().
     */
    bool m_imperative{true};
};

}

}

#endif
