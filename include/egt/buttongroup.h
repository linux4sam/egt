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
 * If the group is exclusive, only one button in the group can have its
 * checked state to true.
 * If the group is imperative, it ensures that at least one button is
 * checked.
 */
class ButtonGroup
{
public:
    explicit ButtonGroup(bool imperative = false, bool exclusive = true) noexcept;
    virtual void exclusive(bool exclusive);
    virtual bool exclusive() const;
    virtual void imperative(bool imperative);
    virtual bool imperative() const;
    virtual void add(Button& button);
    virtual void remove(Button& button);
    virtual void checked_state_change(Button& button, bool value) const;
    virtual ~ButtonGroup();
protected:
    using button_array = std::vector<Button*>;
    button_array m_buttons;
    bool m_exclusive;
    bool m_imperative;
};

}

}

#endif
