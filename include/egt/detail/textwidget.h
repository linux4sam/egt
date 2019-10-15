/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_TEXTWIDGET_H
#define EGT_DETAIL_TEXTWIDGET_H

/**
 * @file
 * @brief Base class TextWidget definition.
 */

#include <egt/image.h>
#include <egt/widget.h>
#include <memory>
#include <string>

namespace egt
{
inline namespace v1
{
namespace detail
{

/**
 * A widget with text and text related properties.
 *
 * This is not meant to be used directly as it does not implement at least a
 * draw() method.
 */
class TextWidget : public Widget
{
public:

    /**
     * @param[in] text The text to display.
     * @param[in] rect Rectangle for the widget.
     * @param[in] text_align Alignment for the text.
     */
    explicit TextWidget(const std::string& text = {},
                        const Rect& rect = {},
                        alignmask text_align = alignmask::center) noexcept;

    /**
     * Set the text.
     *
     * @param str The text string to set.
     */
    virtual void set_text(const std::string& str);

    /**
     * Clear the text value.
     */
    virtual void clear();

    /**
     * Get the text of the Label.
     */
    virtual const std::string& text() const { return m_text; }

    /**
     * Set the text alignment within the Label.
     *
     * @param[in] align Alignment for the text.
     */
    void set_text_align(alignmask align)
    {
        if (detail::change_if_diff<>(m_text_align, align))
            damage();
    }

    /**
     * Get the text alignment within the Label.
     */
    alignmask text_align() const { return m_text_align; }

    /**
     * Get the length of the text.
     */
    virtual size_t len() const;

    /**
     * Given a Font, text, and a target Size, scale the font size so that
     * the text will will fit and return the new Font.
     */
    static Font scale_font(const Size& target, const std::string& text, const Font& font);

    virtual ~TextWidget() noexcept = default;

protected:

    /**
     * Get the size of the text.
     */
    Size text_size(const std::string& text) const;

    /**
     * Alignment of the text.
     */
    alignmask m_text_align{alignmask::center};

    /**
     * The text.
     */
    std::string m_text;
};

}
}
}

#endif
