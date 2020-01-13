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
 * with only a small portion of it shown so that sliding it out can be
 * initiated by default.
 */
class EGT_API SideBoard : public Window
{
public:

    constexpr static const int HANDLE_WIDTH = 50;

    enum class PositionFlag : uint32_t
    {
        left,
        right,
        top,
        bottom,
    };

    /**
     * @param[in] position Position of the SideBoard.
     * @param[in] open_duration The duration of the open animation.
     * @param[in] open_func Open easing function.
     * @param[in] close_duration Duration of the close animation.
     * @param[in] close_func Close easing function.
     */
    explicit SideBoard(PositionFlag position = PositionFlag::left,
                       std::chrono::milliseconds open_duration = std::chrono::milliseconds(1000),
                       EasingFunc open_func = easing_cubic_easeinout,
                       std::chrono::milliseconds close_duration = std::chrono::milliseconds(1000),
                       EasingFunc close_func = easing_circular_easeinout);

    virtual void handle(Event& event) override;

    /**
     * Set the position of the SideBoard.
     *
     * @param[in] position Position of the SideBoard.
     */
    virtual void position(PositionFlag position);

    /**
     * Get the position of the SideBoard.
     */
    inline PositionFlag position() const { return m_position; }

    /**
     * Move to a closed state.
     */
    virtual void close();

    /**
     * Move to an open state.
     */
    virtual void open();

    virtual void serialize(detail::Serializer& serializer) const override;

    virtual void deserialize(const std::string& name, const std::string& value,
                             const std::map<std::string, std::string>& attrs) override;

    virtual ~SideBoard() = default;

protected:

    /**
     * Reset animation start/end values.
     */
    void reset_animations();

    /**
     * Sideboard flags.
     */
    PositionFlag m_position{PositionFlag::left};

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

EGT_API std::ostream& operator<<(std::ostream& os, const SideBoard::PositionFlag& flag);

}
}

#endif
