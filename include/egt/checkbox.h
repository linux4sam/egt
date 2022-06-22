/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_CHECKBOX_H
#define EGT_CHECKBOX_H

/**
 * @file
 * @brief Working with check boxes.
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
 * Boolean checkbox.
 *
 * @image html widget_checkbox1.png
 * @image latex widget_checkbox1.png "widget_checkbox1" width=5cm
 * @image html widget_checkbox2.png
 * @image latex widget_checkbox2.png "widget_checkbox2" width=5cm
 *
 * @todo This should be a ValueWidget<bool>.
 *
 * @ingroup controls
 */
class EGT_API CheckBox : public Button
{
public:

    /**
     * @param[in] text The text to display.
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit CheckBox(const std::string& text = {},
                      const Rect& rect = {}) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] text The text to display.
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit CheckBox(Frame& parent,
                      const std::string& text = {},
                      const Rect& rect = {}) noexcept;

    /**
     * @param[in] props list of widget argument and its properties.
     */
    explicit CheckBox(Serializer::Properties& props) noexcept
        : CheckBox(props, false)
    {
    }

protected:

    explicit CheckBox(Serializer::Properties& props, bool is_derived) noexcept;

    void serialize(Serializer& serializer) const override;

public:

    void handle(Event& event) override;

    void draw(Painter& painter, const Rect& rect) override;

    /**
     * Default draw method for the CheckBox.
     */
    static void default_draw(const CheckBox& widget, Painter& painter, const Rect& rect);

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

    using Button::min_size_hint;

    EGT_NODISCARD Size min_size_hint() const override;

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
     * Set the alignment of the checkbox relative to the text.
     *
     * @param[in] align Only left, right, top, and bottom alignments are supported.
     */
    void checkbox_align(const AlignFlags& align)
    {
        if (detail::change_if_diff<>(m_checkbox_align, align))
            damage();
    }

    /**
     * Get the image alignment.
     */
    EGT_NODISCARD AlignFlags checkbox_align() const { return m_checkbox_align; }

private:
    bool m_show_label{true};
    /// Alignment of the checkbox relative to the text.
    AlignFlags m_checkbox_align{AlignFlag::left};

    void deserialize(Serializer::Properties& props);
};

/**
 * CheckBox with a boolean slider style interface.
 *
 * @image html widget_slidingcheckbox.png
 * @image latex widget_slidingcheckbox.png "widget_slidingcheckbox" width=5cm
 *
 * @ingroup controls
 */
class EGT_API ToggleBox : public CheckBox
{
public:

    /**
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit ToggleBox(const Rect& rect = {}) noexcept;

    /**
    * @param[in] parent The parent Frame.
    * @param[in] rect Initial rectangle of the widget.
    */
    explicit ToggleBox(Frame& parent, const Rect& rect = {}) noexcept;

    /**
     * @param[in] props list of widget argument and its properties.
     */
    explicit ToggleBox(Serializer::Properties& props) noexcept
        : ToggleBox(props, false)
    {
    }

protected:

    explicit ToggleBox(Serializer::Properties& props, bool is_derived) noexcept;

public:

    void draw(Painter& painter, const Rect& rect) override;

    /**
     * Default draw method for the CheckBox.
     */
    static void default_draw(ToggleBox& widget, Painter& painter, const Rect& rect);

    using CheckBox::min_size_hint;

    EGT_NODISCARD Size min_size_hint() const override;

    /**
     * Set the optional "on" and "off" text.
     *
     * @param[in] off_text The "off" text to display.
     * @param[in] on_text The "on" text to display.
     */
    void toggle_text(const std::string& off_text,
                     const std::string& on_text)
    {
        bool off = detail::change_if_diff<>(m_off_text, off_text);
        bool on = detail::change_if_diff<>(m_on_text, on_text);

        if (on || off)
            damage();
    }

    /**
     * Get the "off" text.
     */
    EGT_NODISCARD const std::string& off_text() const { return m_off_text; }

    /**
     * Get the "on" text.
     */
    EGT_NODISCARD const std::string& on_text() const { return m_on_text; }

    /**
     * Get the enable/disable mode.
     */
    EGT_NODISCARD bool enable_disable() const { return m_enable_disable; }

    /**
     * Set the enable/disable mode.
     *
     * When true, the left side is in a disabled state and the right/second side
     * is an enabled state.  When false, both sides are always enabled.
     *
     * In either case, checked() true still means right/second.
     */
    void enable_disable(bool value)
    {
        if (detail::change_if_diff<>(m_enable_disable, value))
            damage();
    }

    void serialize(Serializer& serializer) const override;

private:

    void deserialize(Serializer::Properties& props);

protected:

    // this is particularly bad design that we don't even use text() here
    using TextWidget::text;

    /**
     * Optional "off" text.
     */
    std::string m_off_text;

    /**
     * Optional "on" text.
     */
    std::string m_on_text;

    /**
     * Enable/disable mode.
     */
    bool m_enable_disable{true};
};

}
}

#endif
