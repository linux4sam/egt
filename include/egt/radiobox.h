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

    /**
     * @param[in] props list of widget argument and its properties.
     */
    explicit RadioBox(Serializer::Properties& props) noexcept
        : RadioBox(props, false)
    {
    }

    using Button::text;
    /**
     * Set the text.
     *
     * It sets show_label to true if the string is not empty, to false
     * otherwise.
     *
     * @param str The text string to set.
     */
    void text(const std::string& text) override;

    /**
     * Enable/disable showing the label text.
     *
     * @param[in] value When true, the label text is shown.
     */
    void show_label(bool value)
    {
        if (detail::change_if_diff<>(m_show_label, value))
            damage();
    }

    /**
     * Get the show label state.
     */
    EGT_NODISCARD bool show_label() const { return m_show_label; }

    /**
     * Set the alignment of the image relative to the text.
     *
     * @param[in] align Only left, right, top, and bottom alignments are supported.
     */
    void radiobox_align(const AlignFlags& align)
    {
        if (detail::change_if_diff<>(m_radiobox_align, align))
            damage();
    }

    /**
     * Get the image alignment.
     */
    EGT_NODISCARD AlignFlags radiobox_align() const { return m_radiobox_align; }

protected:

    explicit RadioBox(Serializer::Properties& props, bool is_derived) noexcept;

public:

    void handle(Event& event) override;

    void draw(Painter& painter, const Rect& rect) override;

    /// Default draw method for the widget.
    static void default_draw(const RadioBox& widget, Painter& painter, const Rect& rect);

    using Button::min_size_hint;

    EGT_NODISCARD Size min_size_hint() const override;

private:
    bool m_show_label{true};
    /// Alignment of the radiobox relative to the text.
    AlignFlags m_radiobox_align{AlignFlag::left};
};

}
}

#endif
