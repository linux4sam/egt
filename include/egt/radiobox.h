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
class EGT_API RadioBox : public Switch
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

    using Switch::Switch;

    /**
     * Set the alignment of the image relative to the text.
     *
     * @param[in] align Only left, right, top, and bottom alignments are supported.
     */
    void radiobox_align(const AlignFlags& align)
    {
        switch_align(align);
    }

    /**
     * Get the image alignment.
     */
    EGT_NODISCARD AlignFlags radiobox_align() const { return switch_align(); }

    void draw(Painter& painter, const Rect& rect) override;

    /// Default draw method for the widget.
    static void default_draw(const RadioBox& widget, Painter& painter, const Rect& rect);
};

}
}

#endif
