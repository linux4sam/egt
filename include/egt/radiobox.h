/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_RADIOBOX_H
#define EGT_RADIOBOX_H

/**
 * @file
 * @brief Working with radio boxes.
 */

#include <egt/button.h>
#include <egt/detail/meta.h>
#include <string>

namespace egt
{
inline namespace v1
{

class Painter;
class Frame;

/**
 * Boolean RadioBox.
 *
 * @ingroup controls
 */
class EGT_API RadioBox : public Button
{
public:

    /**
     * @param[in] text The text to display.
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit RadioBox(const std::string& text = {},
                      const Rect& rect = {}) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] text The text to display.
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit RadioBox(Frame& parent,
                      const std::string& text = {},
                      const Rect& rect = {}) noexcept;

    void handle(Event& event) override;

    void draw(Painter& painter, const Rect& rect) override;

    /// Default draw method for the widget.
    static void default_draw(RadioBox& widget, Painter& painter, const Rect& rect);

    using Button::min_size_hint;

    EGT_NODISCARD Size min_size_hint() const override;
};

}
}

#endif
