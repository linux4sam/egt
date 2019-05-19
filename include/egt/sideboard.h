/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SIDEBOARD_H
#define EGT_SIDEBOARD_H

/**
 * @file
 * @brief Sideboard widget.
 */

#include <chrono>
#include <egt/animation.h>
#include <egt/easing.h>
#include <egt/window.h>

namespace egt
{
inline namespace v1
{

/**
 * SideBoard widget for a sliding board that slides on and off the screen.
 *
 * This is a widget that manages a Window that slides on and off the screen,
 * with only a small portion of it shown so that sliding it out can be
 * initiated by default.
 */
class SideBoard : public Window
{
public:

    constexpr static const auto HANDLE_WIDTH = 50;

    enum class flags
    {
        left = 1 << 0,
        right = 1 << 1,
        top = 1 << 2,
        bottom = 1 << 3,
    };

    explicit SideBoard(flags f = flags::left,
                       std::chrono::milliseconds open_duration = std::chrono::milliseconds(1000),
                       easing_func_t open_func = easing_cubic_easeinout,
                       std::chrono::milliseconds close_duration = std::chrono::milliseconds(1000),
                       easing_func_t close_func = easing_circular_easeinout);

    virtual void handle(Event& event) override;

    virtual void close();

    virtual void open();

    virtual ~SideBoard() = default;

protected:

    inline bool is_set(flags f) const;

    /**
     * Sideboard flags.
     */
    flags m_side_flags{flags::left};

    /**
     * Open animation.
     */
    PropertyAnimator m_oanim;

    /**
     * Close animation.
     */
    PropertyAnimator m_canim;

    /**
     * State of the current direction.
     */
    bool m_dir{false};
};

ENABLE_BITMASK_OPERATORS(SideBoard::flags)

bool SideBoard::is_set(flags f) const
{
    return ((m_side_flags & f) == f);
}

}
}

#endif
