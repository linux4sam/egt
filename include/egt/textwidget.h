/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_TEXTWIDGET_H
#define EGT_TEXTWIDGET_H

/**
 * @file
 * @brief Base class TextWidget definition.
 */

#include <egt/widget.h>

namespace egt
{
inline namespace v1
{

/**
 * A widget that has text and properties associated with text.
 *
 * This is not meant to be used directly as it does not implement at least a
 * draw() method.
 */
class TextWidget : public Widget
{
public:

    explicit TextWidget(const std::string& text = std::string(),
                        const Rect& rect = Rect(),
                        alignmask align = alignmask::CENTER,
                        const Font& font = Font(),
                        const widgetflags& flags = widgetflags()) noexcept;

    /**
     * Set the text of the label.
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
     * Set the alignment of the Label.
     */
    void set_text_align(alignmask align)
    {
        if (m_text_align != align)
        {
            m_text_align = align;
            damage();
        }
    }

    alignmask text_align() const { return m_text_align; }

    /**
     * Get the widget Font.
     */
    Font& font()
    {
        if (!m_font.get())
            m_font.reset(new Font);

        return *m_font.get();
    }

    /**
     * Get the widget Font.
     */
    const Font& font() const
    {
        if (!m_font.get())
            m_font.reset(new Font);

        return *m_font.get();
    }

    /**
     * Set the widget Font.
     *
     * @note This will overwrite the entire widget Font.
     */
    void set_font(const Font& font)
    {
        m_font.reset(new Font(font));
        damage();
    }

    virtual ~TextWidget()
    {}

    /**
     * Given a Font, text, and a target Size, scale the font size so that
     * the text will will fit and return the new Font.
     */
    static Font scale_font(const Size& target, const std::string& text, const Font& font);

protected:

    Size text_size();

    alignmask m_text_align{alignmask::CENTER};
    std::string m_text;

private:
    mutable std::unique_ptr<Font> m_font;
};

}
}

#endif
