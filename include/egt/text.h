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

#include <egt/canvas.h>
#include <egt/detail/meta.h>
#include <egt/flags.h>
#include <egt/font.h>
#include <egt/image.h>
#include <egt/slider.h>
#include <egt/textwidget.h>
#include <egt/timer.h>
#include <egt/types.h>
#include <list>
#include <memory>
#include <string>
#include <vector>

namespace egt
{
inline namespace v1
{


class TextRect
{
public:

    /// TextRect flags.
    enum class TextRectFlag : uint32_t
    {
        /// Text is selected
        selected = detail::bit(0),

        /// End of non-empty line
        eonel = detail::bit(1),

        /// Beginning of line
        bol = detail::bit(2),
    };

    /// TextRect flags.
    using TextRectFlags = egt::Flags<TextRectFlag>;

    TextRect(uint32_t behave,
             const Rect& rect,
             std::string text,
             const cairo_text_extents_t& te,
             const TextRectFlags& flags = {}) noexcept
        : m_text(std::move(text)),
          m_rect(rect),
          m_text_rect_flags(flags),
          m_behave(behave),
          m_te(te)
    {}

    const std::string& text(void) const { return m_text; }
    const Rect& rect(void) const { return m_rect; }
    void rect(const Rect& r) { m_rect = r; }
    void point(const Point& p) { m_rect.point(p); }
    const TextRectFlags& flags(void) const { return m_text_rect_flags; }
    uint32_t behave(void) const { return m_behave; }
    const cairo_text_extents_t& text_extents(void) const { return m_te; }
    void select(void) { m_text_rect_flags.set(TextRectFlag::selected); }
    void deselect(void) { m_text_rect_flags.clear(TextRectFlag::selected); }
    bool is_selected(void) const { return m_text_rect_flags.is_set(TextRectFlag::selected); }
    bool end_of_non_empty_line(void) const { return m_text_rect_flags.is_set(TextRectFlag::eonel); }
    void mark_beginning_of_line(void) { m_text_rect_flags.set(TextRectFlag::bol); }
    bool beginning_of_line(void) const { return m_text_rect_flags.is_set(TextRectFlag::bol); }

    bool can_consolidate(const TextRect& r) const noexcept
    {
        return ((m_text_rect_flags == r.m_text_rect_flags) &&
                (m_rect.y() == r.m_rect.y()) &&
                (m_rect.height() == r.m_rect.height()) &&
                ((m_rect.x() + m_rect.width()) == r.m_rect.x()) &&
                (r.m_text != "\n"));
    }

    size_t length(void) const noexcept;
    TextRect& consolidate(const TextRect& r, cairo_t* cr) noexcept;
    TextRect split(size_t pos, cairo_t* cr) noexcept;

private:

    /// Text to be displayed
    std::string m_text;

    /// Rectangle where to display the text
    Rect m_rect;

    /// Flags tuning how the text is displayed
    TextRectFlags m_text_rect_flags{};

    /// Behavior flags of the object.
    uint32_t m_behave{0};

    /// Cairo text extents
    cairo_text_extents_t m_te;
};

using TextRects = std::list<TextRect>;

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
 * textbox.text_flags().set({TextBox::TextFlag::multiline, TextBox::TextFlag::word_wrap});
 * @endcode
 *
 * @image html widget_textbox.png
 * @image latex widget_textbox.png "Widget Textbox" height=5cm
 *
 * @ingroup controls
 */
class EGT_API TextBox : public TextWidget
{
public:

    /// Text flags.
    enum class TextFlag : uint32_t
    {
        /// When not multiline, only allow max length to be what can fit.
        fit_to_width = detail::bit(0),

        /// Enable multi-line text. expand_vertical align flag must be set, be careful to not set top or bottom later.
        multiline = detail::bit(1),

        /// Wrap at word boundaries instead of character boundaries.  Must be flag::multiline.
        word_wrap = detail::bit(2),

        /// Do not display a virtual keyboard when focus is gained.
        no_virt_keyboard = detail::bit(3),

        /// Horizontal scrollable
        horizontal_scrollable = detail::bit(4),

        /// Vertical scrollable
        vertical_scrollable = detail::bit(5),
    };

    /// Text flags.
    using TextFlags = egt::Flags<TextFlag>;

    /**
     * Validator callback type.
     * @see add_validator_function()
     */
    using ValidatorCallback = std::function<bool(const std::string&)>;

    /// Change text align.
    static AlignFlags default_text_align();
    /// Change default text align.
    static void default_text_align(const AlignFlags& align);

    /**
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     * @param[in] flags TextBox flags.
     */
    explicit TextBox(const std::string& text = {},
                     const AlignFlags& text_align = default_text_align(),
                     const TextFlags& flags = {}) noexcept;

    /**
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     * @param[in] flags TextBox flags.
     */
    explicit TextBox(const std::string& text,
                     const TextFlags& flags,
                     const AlignFlags& text_align = default_text_align()) noexcept;

    /**
     * @param[in] text The text to display.
     * @param[in] rect Rectangle for the widget.
     * @param[in] text_align Alignment for the text.
     * @param[in] flags TextBox flags.
     */
    TextBox(const std::string& text,
            const Rect& rect,
            const AlignFlags& text_align = default_text_align(),
            const TextFlags& flags = {}) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     * @param[in] flags TextBox flags.
     */
    explicit TextBox(Frame& parent,
                     const std::string& text = {},
                     const AlignFlags& text_align = default_text_align(),
                     const TextFlags& flags = {}) noexcept;

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
            const AlignFlags& text_align = default_text_align(),
            const TextFlags& flags = {}) noexcept;

    /**
     * @param[in] props list of widget argument and its properties.
     */
    explicit TextBox(Serializer::Properties& props) noexcept
        : TextBox(props, false)
    {
    }

protected:

    explicit TextBox(Serializer::Properties& props, bool is_derived) noexcept;

public:

    TextBox(const TextBox&) = delete;
    TextBox& operator=(const TextBox&) = delete;
    TextBox(TextBox&&) = default;
    TextBox& operator=(TextBox&&) = default;

    void handle(Event& event) override;

    void draw(Painter& painter, const Rect& rect) override;

    void resize(const Size& size) override;

    using TextWidget::text;

    void text(const std::string& str) override;

    void clear() override;

    using TextWidget::min_size_hint;

    EGT_NODISCARD Size min_size_hint() const override;

    /**
     * Set the maximum allowed length of the text.
     *
     * @param len The maximum length of the text.
     */
    void max_length(size_t len);

    /**
     * Get the max length of allowed text.
     */
    EGT_NODISCARD size_t max_length() const { return m_max_len;}

    /**
     * Set the text flags.
     *
     * @param[in] text_flags Text flags.
     */
    void text_flags(const TextFlags& text_flags)
    {
        if (detail::change_if_diff<>(m_text_flags, text_flags))
        {
            resize_sliders();
            damage();
        }
    }

    /**
     * Get a const ref of the flags.
     */
    EGT_NODISCARD const TextFlags& text_flags() const { return m_text_flags; }

    /**
     * Move the cursor to the end and insert.
     *
     * The is the same as doing:
     * @code{.cpp}
     * cursor_end();
     * insert(str);
     * @endcode
     *
     * @param str The string to append.
     */
    size_t append(const std::string& str);

    /**
     * Insert text at the cursor.
     *
     * @param str The string to insert.
     * @return The number of characters inserted.
     */
    size_t insert(const std::string& str);

    /**
     * Get the cursor position.
     */
    EGT_NODISCARD size_t cursor() const;

    /**
     * Move the cursor to the beginning.
     */
    void cursor_begin();

    /**
     * Move the cursor to the end.
     */
    void cursor_end();

    /**
     * Move the cursor forward by the specified count from the current
     * position.
     *
     * @param count The number of characters to move.
     */
    void cursor_forward(size_t count = 1);

    /**
     * Move the cursor backward by the specified count from the current
     * position.
     *
     * @param count The number of characters to move.
     */
    void cursor_backward(size_t count = 1);

    /**
     * Set the cursor to the specific position.
     *
     * @param pos The new cursor position.
     */
    void cursor_set(size_t pos);

    /**
     * Select all of the text.
     */
    void selection_all();

    /**
     * Set the selection of text.
     *
     * @param pos The starting position.
     * @param length The length of the selection.
     */
    void selection(size_t pos, size_t length);

    /**
     * Extend the selection to the right if the cursor is at its beginning,
     * otherwise reduce the selection from the left.
     * In both cases, the selection length is updated by the specified count.
     *
     * @param count The number of characters to move.
     */
    void selection_forward(size_t count = 1);

    /**
     * Extend the selection to the left if the cursor is at its end,
     * otherwise reduce the selection from the right.
     * In both cases, the selection length is updated by the specified count.
     *
     * @param count The number of characters to move.
     */
    void selection_backward(size_t count = 1);

    /**
     * Move the selection cursor to count character(s):
     * - to the left, if count is negative
     * - to the right, if count is positive
     * Do nothing if count is zero.
     *
     * Also, set the selection origin (its fixed end), if needed.
     * In all cases, the cursor is positioned to the new selection cursor.
     */
    void selection_move(size_t count, bool save_column = true);

    /**
     * Get the position of the moving end of the selection, as opposed to its
     * origin (the fixed end of the selection).
     * Set the selection origin, if needed.
     */
    EGT_NODISCARD size_t selection_cursor();

    /**
     * Get the start position of the selection.
     */
    EGT_NODISCARD size_t selection_start() const
    {
        return m_select_start;
    }

    /**
     * Get the length of the selection.
     */
    EGT_NODISCARD size_t selection_length() const
    {
        return m_select_len;
    }

    /**
     * Clear any selection.
     *
     * @note This does not delete text, it just clears the selection. To delete
     * the selected text call delete_selection().
     */
    void selection_clear();

    /**
     * Get, or copy, the selected text.
     *
     * @return The selected text, or an empty string if there is no selection.
     */
    EGT_NODISCARD std::string selected_text() const;

    /**
     * Delete the selected text.
     */
    void selection_delete();

    /**
     * Enable or disable input validation.
     *
     * Invoke the validator callbacks. If one of them returns false, the input
     * is rejected.
     */
    void input_validation_enabled(bool enabled);

    /**
     * Add a callback to be invoked to validate the input.
     *
     * Anytime the text is changed, the callback will be invoked with the new
     * text for validation.
     *
     * To enable this, you must set input_validation_enabled() to true.
     */
    void add_validator_function(ValidatorCallback callback);

    /**
     * Get the position offset of the drawn text as compared to the whole text.
     */
    EGT_NODISCARD Point text_offset() const;

    /**
     * Set the position offset of the drawn text as compared to the whole text.
     */
    void text_offset(const Point& p);

    void serialize(Serializer& serializer) const override;

    ~TextBox() noexcept override;

protected:

    /**
     * @override
     */
    bool on_drag_start(Event& event) override;

    /**
     * @override
     */
    void on_drag(Event& event) override;

    /// Get the rectangle of the text boundaries.
    EGT_NODISCARD Rect text_boundaries() const;

    /// Get the rectangle of a text.
    EGT_NODISCARD Rect text_rect() const;

    /// Get the size of a text.
    using TextWidget::text_size;
    EGT_NODISCARD Size text_size() const;

    /// Invalidate the cache used by text_rect()
    void invalidate_text_rect();

    /// Type array used for validator callbacks.
    using ValidatorCallbackArray = std::vector<ValidatorCallback>;

    /// Show/enable the visibility of the cursor.
    void show_cursor();

    /// If the cursor is shown, keep is shown some more.
    void continue_show_cursor();

    /// Hide/disable the visibility of the cursor.
    void hide_cursor();

    /// Set the cursor position.
    void cursor_set(size_t pos, bool save_column);

    /// Change the value of m_cursor_pos.
    void change_cursor(size_t pos, bool save_column = true);

    /// Convert point coordinates to position in text for cursor or selection.
    size_t point2pos(const Point& p) const;

    /// Get the position of the beginning of the line at cursor_pos.
    size_t beginning_of_line(size_t cursor_pos) const;

    /// Get the position of the beginning of the current line.
    size_t beginning_of_line() const { return beginning_of_line(m_cursor_pos); }

    /// Get the position of the end of the line at cursor_pos.
    size_t end_of_line(size_t cursor_pos) const;

    /// Get the position of the end of the current line.
    size_t end_of_line() const { return end_of_line(m_cursor_pos); }

    /// Get the position in the line befor cursor_pos at the same column, if possible.
    size_t up(size_t cursor_pos) const;

    /// Get the position in the previous line at the same column, if possible.
    size_t up() const { return up(m_cursor_pos); }

    /// Get the position in the line after cursor_pos at the same column, if possible.
    size_t down(size_t cursor_pos) const;

    /// Get the position in the next line at the same column, if possible.
    size_t down() const { return down(m_cursor_pos); }

    /// Process key events.
    virtual void handle_key(const Key& key);

    /// Validate the input against the validator pattern.
    bool validate_input(const std::string& str);

    /// Compute damage rectangles for selection updates
    void selection_damage();

    /// Get the cairo context to compute text extents, hence the text layout.
    cairo_t* context() const { return m_cr; }

    /// Split the text into atomic tokens that fill the TextRects parameter.
    void tokenize(TextRects& rects);

    /// Compute the text layout from the tokens contained in the TextRects.
    void compute_layout(TextRects& rects);

    /// Merge adjacent TextRect items, when possible.
    void consolidate(TextRects& rects);

    /// Clear the TextRectFlag::selected flag from all TextRects in rects.
    void clear_selection(TextRects& rects);

    /**
     * Add the TextRectFlag::selected flag to TextRects based on the
     * m_select_start and m_select_len values.
     *
     * Split TextRects if needed.
     */
    void set_selection(TextRects& rects);

    /**
     * Get the std::string and Rect from pos to the first follwing TextRect in
     * rects with a different height, or till the end of rects if any.
     */
    static void get_line(const TextRects& rects,
                         TextRects::iterator& pos,
                         std::string& line,
                         Rect& rect);

    /// Compute the longest common prefix between two strings.
    static std::string longest_prefix(const std::string& s1, const std::string& s2);

    /// Compute the longest common suffix between two strings.
    static std::string longest_suffix(const std::string& s1, const std::string& s2);

    /// Damage the merged rectangle of two text lines if they are not equal.
    void tag_default_aligned_line(TextRects& prev,
                                  TextRects::iterator& prev_pos,
                                  TextRects& next,
                                  TextRects::iterator& next_pos);

    /// Damage the merge rectangle of two text lines, excluding their common prefix.
    void tag_left_aligned_line(TextRects& prev,
                               TextRects::iterator& prev_pos,
                               TextRects& next,
                               TextRects::iterator& next_pos);

    /// Damage the merge rectangle of two text lines, excluding their common suffix.
    void tag_right_aligned_line(TextRects& prev,
                                TextRects::iterator& prev_pos,
                                TextRects& next,
                                TextRects::iterator& next_pos);

    /// Damage the differences between two text lines, based on the text alignment.
    void tag_line(TextRects& prev,
                  TextRects::iterator& prev_pos,
                  TextRects& next,
                  TextRects::iterator& next_pos);

    /// Damage the differences between two texts.
    void tag_text(TextRects& prev, TextRects& next);

    /// Compute the merged Rect for the selected TextRects on the same line as pos.
    static void get_line_selection(const TextRects& rects,
                                   TextRects::const_iterator& pos,
                                   Rect& rect);

    /// Damage the differences between two selected text lines.
    void tag_line_selection(const TextRects& prev,
                            TextRects::const_iterator& prev_pos,
                            const TextRects& next,
                            TextRects::const_iterator& next_pos);

    /// Damage the differences between two selected texts.
    void tag_text_selection(const TextRects& prev, const TextRects& next);

    /// Tokenize and compute the layout of a text; fill TextRects accordingly.
    void prepare_text(TextRects& rects);

    /// Update m_cursor_rect based on the current position of the cursor.
    void get_cursor_rect();

    /// Draw the text based on the m_rects TextRects.
    void draw_text(Painter& painter, const Rect& rect);

    /// Compute the new text layout and cursor rectangle.
    void refresh_text_area();

    /// Return the rectangle boundaries where the text can be drawn.
    EGT_NODISCARD Rect text_area() const;

    /// Init sliders.
    void init_sliders();

    /// Resize the sliders whenever the widget size changes.
    void resize_sliders();

    /// Return the lowest multiple of 'screen_size' / 2, greater than 'size'.
    static DefaultDim half_screens(DefaultDim size, DefaultDim screen_size);

    /// Update the horizontal slider.
    void update_hslider();

    /// Update the vertical slider.
    void update_vslider();

    /// Update the horizontal and vertical sliders as needed.
    void update_sliders();

    /// Move the horizontal slider, if needed, to track the cursor.
    void move_hslider();

    /// Move the vertical slider, if needed, to track the cursor.
    void move_vslider();

    /// Move sliders, if needed, to track the cursor.
    void move_sliders();

    /// Draw sliders.
    void draw_sliders(Painter& painter, const Rect& rect);

    /// Damage the text but only if visible.
    void damage_text(const Rect& rect)
    {
        damage(Rect::intersection(rect, text_area()));
    }

    /// Damage the cursor but only if visible.
    void damage_cursor()
    {
        /// The cursor may cross the content area; hence intersect with the box.
        damage(Rect::intersection(m_cursor_rect, box()));
    }

    /// Timer for blinking the cursor.
    PeriodicTimer m_timer;

    /**
     * The current position of the cursor.
     * This is a UTF-8 offset.
     */
    size_t m_cursor_pos{0};

    /**
     * Save the column of the cursor when it was moved for the last time, except
     * when the cursor is moved with the UP and DOWN keys.
     */
    size_t m_saved_column{0};

    /**
     * Selection start position.
     * This is a UTF-8 offset.
     */
    size_t m_select_start{0};

    /// Selection length.
    size_t m_select_len{0};

    /// Selection origin.
    size_t m_select_origin{0};

    /// Selection drag start.
    size_t m_select_drag_start{0};

    /// Validation of the input.
    bool m_validate_input{false};

    /// Callbacks invoked to validate the input.
    ValidatorCallbackArray m_validator_callbacks;

    /// The canvas that provides a cairo context to compute the text layout.
    Canvas m_canvas;

    /// The cache for text_rect().
    mutable Rect m_text_rect;
    mutable bool m_text_rect_valid{false};

    /**
     * Cairo context.
     */
    cairo_t* m_cr{nullptr};

    TextRects m_rects;
    Rect m_cursor_rect;

    /**
     * Given text, return the number of UTF8 characters that will fit on a
     * single line inside of the widget.
     */
    EGT_NODISCARD size_t width_to_len(const std::string& str) const;

private:
    class State
    {
    public:
        State() noexcept
        {}

        explicit State(const Font* font,
                       const Widget::Flags& flags,
                       const AlignFlags& text_align,
                       const TextFlags& text_flags) noexcept
            : m_font(font),
              m_flags(flags),
              m_text_align(text_align),
              m_text_flags(text_flags)
        {}

        State& operator=(const State& rhs) noexcept
        {
            m_font = rhs.m_font;
            m_flags = rhs.m_flags;
            m_text_align = rhs.m_text_align;
            m_text_flags = rhs.m_text_flags;
            return *this;
        }

        State& operator=(State&& rhs) noexcept
        {
            m_font = rhs.m_font;
            m_flags = std::move(rhs.m_flags);
            m_text_align = std::move(rhs.m_text_align);
            m_text_flags = std::move(rhs.m_text_flags);
            return *this;
        }

        bool operator==(const State& rhs) const noexcept
        {
            return m_font == rhs.m_font &&
                   m_flags == rhs.m_flags &&
                   m_text_align == rhs.m_text_align &&
                   m_text_flags == rhs.m_text_flags;
        }

        bool operator!=(const State& rhs) const noexcept
        {
            return !operator==(rhs);
        }

    private:
        const Font* m_font{nullptr};
        Widget::Flags m_flags;
        AlignFlags m_text_align;
        TextFlags m_text_flags;
    };

    /// Callback for the cursor timeout.
    void cursor_timeout();

    /// The current visible state of the cursor.
    bool m_cursor_state{false};

    /// TextBox flags.
    TextFlags m_text_flags{};

    /// Maximum text length, or zero.
    size_t m_max_len{0};

    State m_state;

    /// Horizontal slider shown when scrollable.
    Slider m_hslider;

    /// Vertical slider shown when scrollable.
    Slider m_vslider;

    /// Width/height of the slider when shown.
    DefaultDim m_slider_dim{8};

    /// Gain focus registration.
    Signal<>::RegisterHandle m_gain_focus_reg{};

    /// Lost focus registration.
    Signal<>::RegisterHandle m_lost_focus_reg{};

    void initialize(bool init_inherited_properties = true);

    void deserialize(Serializer::Properties& props);
};

namespace detail
{
/// Internal draw text function.
EGT_API void draw_text(Painter& painter,
                       const Rect& b,
                       const std::string& text,
                       const Font& font,
                       const TextBox::TextFlags& flags,
                       const AlignFlags& text_align,
                       Justification justify,
                       const Pattern& text_color,
                       const std::function<void(const Point& offset, size_t height)>& draw_cursor = nullptr,
                       size_t cursor_pos = 0,
                       const Pattern& highlight_color = {},
                       size_t select_start = 0,
                       size_t select_len = 0);

/// Internal draw text function with associated image.
EGT_API void draw_text(Painter& painter,
                       const Rect& b,
                       const std::string& text,
                       const Font& font,
                       const TextBox::TextFlags& flags,
                       const AlignFlags& text_align,
                       Justification justify,
                       const Pattern& text_color,
                       const AlignFlags& image_align,
                       const Image& image,
                       const std::function<void(const Point& offset, size_t height)>& draw_cursor = nullptr,
                       size_t cursor_pos = 0,
                       const Pattern& highlight_color = {},
                       size_t select_start = 0,
                       size_t select_len = 0);
}

}
}

#endif
