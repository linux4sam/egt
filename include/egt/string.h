/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_STRING_H
#define EGT_STRING_H

/**
 * @file
 * @brief StringItem definition.
 */

#include <egt/detail/meta.h>
#include <egt/frame.h>
#include <egt/label.h>
#include <egt/signal.h>
#include <egt/sizer.h>
#include <egt/view.h>
#include <string>

namespace egt
{
inline namespace v1
{

/**
 * List string helper.
 *
 * Light wrapper around a list item to make each item in the list a little
 * bit bigger.
 */
struct EGT_API StringItem : public ImageLabel
{
    /**
     * @param[in] image The image to display.
     * @param[in] text The text to display.
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] text_align Alignment for the text.
     */
    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    StringItem(const std::string& text = {},
               const Image& image = {},
               const Rect& rect = {},
               const AlignFlags& text_align = AlignFlag::center) noexcept
        : ImageLabel(image, text, rect, text_align)
    {
        fill_flags(Theme::FillFlag::blend);
    }

    /**
     * @param[in] text The text to display.
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] text_align Alignment for the text.
     */
    StringItem(const std::string& text,
               const Rect& rect,
               const AlignFlags& text_align = AlignFlag::center) noexcept
        : ImageLabel(Image(), text, rect, text_align)
    {
        fill_flags(Theme::FillFlag::blend);
    }

    /**
     * @param[in] parent The parent Frame.
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     */
    explicit StringItem(Frame& parent,
                        const std::string& text = {},
                        const AlignFlags& text_align = AlignFlag::center) noexcept
        : ImageLabel(parent, Image(), text, text_align)
    {
        fill_flags(Theme::FillFlag::blend);
    }

    /**
     * @param[in] parent The parent Frame.
     * @param[in] text The text to display.
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] text_align Alignment for the text.
     */
    StringItem(Frame& parent,
               const std::string& text,
               const Rect& rect,
               const AlignFlags& text_align = AlignFlag::center) noexcept
        : ImageLabel(parent, Image(), text, rect, text_align)
    {
        fill_flags(Theme::FillFlag::blend);
    }

    using ImageLabel::min_size_hint;

    EGT_NODISCARD Size min_size_hint() const override
    {
        if (!m_min_size.empty())
            return m_min_size;
        return {100, 40};
    }
};

}
}

#endif
