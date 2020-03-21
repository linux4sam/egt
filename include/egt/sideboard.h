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
#include <egt/detail/meta.h>
#include <egt/easing.h>
#include <egt/window.h>
#include <iosfwd>

namespace egt
{
inline namespace v1
{

/**
 * SideBoard Window for a sliding board that slides on and off the screen.
 *
 * This is a widget that manages a Window that slides on and off the screen,
 * with only a small portion of it, the "handle", shown so that sliding it out
 * can be initiated by default.
 */
class EGT_API SideBoard : public Window
{
public:

    /// Default width shown when the SideBoard is closed.
    constexpr static const int HANDLE_WIDTH = 50;

    /// Position flag of the SideBoard.
    enum class PositionFlag : uint32_t
    {
        /// Left side.
        left,
        /// Right side.
        right,
        /// Top side.
        top,
        /// Botom side.
        bottom,
    };

    /**
     * @param[in] position Position of the SideBoard.
     * @param[in] open_duration The duration of the open animation.
     * @param[in] open_func Open easing function.
     * @param[in] close_duration Duration of the close animation.
     * @param[in] close_func Close easing function.
     * @param[in] hint Requested Window type. This only applies if this Window
     *            will be responsible for creating a backing screen.  This is
     *            only a hint.
     */
    explicit SideBoard(PositionFlag position = PositionFlag::left,
                       std::chrono::milliseconds open_duration = std::chrono::milliseconds(1000),
                       EasingFunc open_func = easing_cubic_easeinout,
                       std::chrono::milliseconds close_duration = std::chrono::milliseconds(1000),
                       EasingFunc close_func = easing_circular_easeinout,
                       WindowHint hint = WindowHint::automatic);

    /**
     * @param[in] position Position of the SideBoard.
     * @param[in] hint Requested Window type. This only applies if this Window
     *            will be responsible for creating a backing screen.  This is
     *            only a hint.
     */
    SideBoard(PositionFlag position, WindowHint hint);

    virtual void handle(Event& event) override;

    /**
     * Set the position of the SideBoard.
     *
     * @param[in] position Position of the SideBoard.
     */
    virtual void position(PositionFlag position);

    /// Get the position of the SideBoard.
    PositionFlag position() const { return m_position; }

    /// Move to a closed state.
    virtual void close();

    /// Move to an open state.
    virtual void open();

    virtual void serialize(Serializer& serializer) const override;

    virtual void deserialize(const std::string& name, const std::string& value,
                             const Serializer::Attributes& attrs) override;

    virtual ~SideBoard() = default;

protected:

    /// Reset animation start/end values.
    void reset_animations();

    /// SideBoard flags.
    PositionFlag m_position{PositionFlag::left};

    /// Open animation.
    PropertyAnimator m_oanim;

    /// Close animation.
    PropertyAnimator m_canim;

    /// State of the current direction.
    bool m_dir{false};
};

/// Overloaded std::ostream insertion operator
EGT_API std::ostream& operator<<(std::ostream& os, const SideBoard::PositionFlag& flag);

}
}

#endif
