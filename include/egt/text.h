/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_TEXT_H
#define EGT_TEXT_H

/**
 * @file
 * @brief Working with text input.
 */

#include <egt/detail/textwidget.h>
#include <egt/flags.h>
#include <egt/font.h>
#include <egt/timer.h>
#include <memory>
#include <string>
#include <vector>

namespace egt
{
inline namespace v1
{

/**
 * Input text box.
 *
 * Supported Features:
 * - UTF-8 encoding
 * - Cursor movement
 * - Selection Copy/Delete
 * - Multi-line
 *
 * @b Example
 * @code{.cpp}
 * TextBox textbox;
 * textbox.text_flags().set({TextBox::flag::multiline, TextBox::flag::word_wrap});
 * @endcode
 *
 * @image html widget_textbox.png
 * @image latex widget_textbox.png "Widget Textbox" height=5cm
 *
 * @ingroup controls
 */
class TextBox : public detail::TextWidget
{
public:

    enum class flag
    {
        /**
         * When not multiline, only allow max length to be what can fit.
         */
        fit_to_width,

        /**
         * Enable multi-line text.
         */
        multiline,

        /**
         * Wrap at word boundaries instead of character boundaries.  Must be flag::multiline.
         */
        word_wrap,

        /**
         * Do not display a virtual keyboard when focus is gained.
         */
        no_virt_keyboard,
    };

    using flags_type = Flags<flag>;

    using validator_callback_t = std::function<bool(std::string)>;

    static const alignmask default_align;

    /**
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     * @param[in] flags TextBox flags.
     */
    explicit TextBox(const std::string& text = {},
                     alignmask text_align = default_align,
                     const flags_type::flags& flags = {}) noexcept;

    /**
     * @param[in] text The text to display.
     * @param[in] rect Rectangle for the widget.
     * @param[in] text_align Alignment for the text.
     * @param[in] flags TextBox flags.
     */
    TextBox(const std::string& text,
            const Rect& rect,
            alignmask text_align = default_align,
            const flags_type::flags& flags = {}) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     * @param[in] flags TextBox flags.
     */
    explicit TextBox(Frame& parent,
                     const std::string& text = {},
                     alignmask text_align = default_align,
                     const flags_type::flags& flags = {}) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] text The text to display.
     * @param[in] rect Rectangle for the widget.
     * @param[in] text_align Alignment for the text.
     * @param[in] flags TextBox flags.
     */
    TextBox(Frame& parent,
            const std::string& text,
            const Rect& rect,
            alignmask text_align = default_align,
            const flags_type::flags& flags = {}) noexcept;

    virtual void handle(Event& event) override;

    virtual void draw(Painter& painter, const Rect& rect) override;

    virtual void set_text(const std::string& str) override;

    virtual void clear() override;

    virtual Size min_size_hint() const override;

    /**
     * Set the maximum allowed length of the text.
     *
     * @param len The maximum length of the text.
     */
    virtual void set_max_length(size_t len);

    /**
     * Get the max length of allowed text.
     */
    inline size_t max_length() const { return m_max_len;}

    /**
     * Get a const ref of the flags.
     */
    inline const flags_type& text_flags() const { return m_text_flags; }

    /**
     * Get a modifiable ref of the flags.
     */
    inline flags_type& text_flags() { return m_text_flags; }

    /**
     * Move the cursor to the end and insert.
     *
     * The is the same as doing:
     * @code{.cpp}
     * cursor_set_end();
     * insert(str);
     * @endcode
     *
     * @param str The string to append.
     */
    virtual size_t append(const std::string& str);

    /**
     * Insert text at the cursor.
     *
     * @param str The string to insert.
     * @return The number of characters inserted.
     */
    virtual size_t insert(const std::string& str);

    /**
     * Get the cursor position.
     */
    virtual size_t cursor() const;

    /**
     * Move the cursor to the beginning.
     */
    virtual void cursor_set_begin();

    /**
     * Move the cursor to the end.
     */
    virtual void cursor_set_end();

    /**
     * Move the cursor forward by the specified count from the current
     * position.
     *
     * @param count The number of characters to move.
     */
    virtual void cursor_forward(size_t count = 1);

    /**
     * Move the cursor backward by the specified count from the current
     * position.
     *
     * @param count The number of characters to move.
     */
    virtual void cursor_backward(size_t count = 1);

    /**
     * Set the cursor to the specific position.
     *
     * @param pos The new cursor position.
     */
    virtual void cursor_set(size_t pos);

    /**
     * Select all of the text.
     */
    virtual void set_selection_all();

    /**
     * Set the selection of text.
     *
     * @param pos The starting position.
     * @param length The length of the selection.
     */
    virtual void set_selection(size_t pos, size_t length);

    /**
     * Clear any selection.
     *
     * @note This does not delete text, it just clears the selection. To delete
     * the selected text call delete_selection().
     */
    virtual void clear_selection();

    /**
     * Get, or copy, the selected text.
     *
     * @return The selected text, or an empty string if there is no selection.
     */
    std::string selected_text() const;

    /**
     * Delete the selected text.
     */
    virtual void delete_selection();

    /**
     * Enable or disable input validation.
     *
     * Invoke the validator callbacks. If one of them returns false, the input
     * is rejected.
     */
    virtual void set_input_validation_enabled(bool enabled);

    /**
     * Add a callback to be invoked to validate the input.
     */
    virtual void add_validator_function(validator_callback_t callback);

    virtual ~TextBox() noexcept = default;

protected:

    using validator_callback_array = std::vector<validator_callback_t>;

    /**
     * Show/enable the visibility of the cursor.
     */
    void show_cursor();

    /**
     * Hide/disable the visibility of the cursor.
     */
    void hide_cursor();

    /**
     * Process key events.
     */
    virtual void handle_key(const Key& key);

    /**
     * Validate the input against the validator pattern.
     */
    bool validate_input(const std::string& str);

    /**
     * Timer for blinking the cursor.
     */
    PeriodicTimer m_timer;

    /**
     * The current position of the cursor.
     * This is a utf-8 offset.
     */
    size_t m_cursor_pos{0};

    /**
     * Selection start position.
     * This is a utf-8 offset.
     */
    size_t m_select_start{0};

    /**
     * Selection length.
     */
    size_t m_select_len{0};

    /**
     * Validation of the input.
     */
    bool m_validate_input{false};

    /**
     * Callbacks invoked to validate the input.
     */
    validator_callback_array m_validator_callbacks;

    size_t width_to_len(const std::string& text) const;

private:

    /**
     * Callback for the cursor timeout.
     */
    void cursor_timeout();

    /**
     * The current visible state of the cursor.
     */
    bool m_cursor_state{false};

    /**
     * TextBox flags.
     */
    flags_type m_text_flags;

    /**
     * Maximum text length, or zero.
     */
    size_t m_max_len{0};
};

}
}

#endif
