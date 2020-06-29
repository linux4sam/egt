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

#include <egt/detail/meta.h>
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
class EGT_API ButtonGroup
{
public:

    /**
     * @param imperative When true, one button must always be checked.
     * @param exclusive When true, only one button can be checked at a time.
     */
    explicit ButtonGroup(bool imperative = false, bool exclusive = true) noexcept;

    ButtonGroup(const ButtonGroup&) = delete;
    ButtonGroup& operator=(const ButtonGroup&) = delete;
    ButtonGroup(ButtonGroup&&) = default;
    ButtonGroup& operator=(ButtonGroup&&) = default;

    /**
     * Set the exclusive property of the group.
     *
     * @param exclusive When true, only one button can be checked at a time.
     *
     * @note Changing this after adding buttons is undefined.
     */
    void exclusive(bool exclusive);

    /**
     * Get the exclusive property.
     */
    EGT_NODISCARD bool exclusive() const;

    /**
     * Set the imperative property of the group.
     *
     * @param imperative When true, one must must always be checked.
     *
     * @note Changing this after adding buttons is undefined.
     */
    void imperative(bool imperative);

    /**
     * Get the imperative property.
     */
    EGT_NODISCARD bool imperative() const;

    /**
     * Add a button to the group.
     */
    void add(const std::shared_ptr<Button>& button);

    /**
     * Remove a button from the group.
     */
    void remove(Button* button);

    /// @see foreach_checked()
    using CheckedCallback = std::function<void (Button& button)>;

    /**
     * Run a function on each checked button.
     *
     * If the exclusive property is true, only one button will even be
     * invoked.
     *
     * @param callback The callback to invoke.
     */
    void foreach_checked(const CheckedCallback& callback);

    ~ButtonGroup() noexcept;

protected:

    /// @private
    void checked_state_change(Button& button, bool checked) const;

    /// @private
    using ButtonArray = std::vector<std::weak_ptr<Button>>;

    /**
     * Buttons in the group.
     */
    ButtonArray m_buttons;

    /**
     * Only one button can be checked().
     */
    bool m_exclusive{false};

    /**
     * At least one button must be checked().
     */
    bool m_imperative{true};

    friend class Button;
};

}

}

#endif
