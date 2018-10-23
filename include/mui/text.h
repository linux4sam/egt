/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_TEXT_H
#define MUI_TEXT_H

/**
 * @file
 * @brief Working with text input.
 */

#include <mui/widget.h>
#include <mui/timer.h>
#include <string>

namespace mui
{

    /**
     * Input text box.
     *
     * This supports text on a single line.
     */
    class TextBox : public Widget
    {
    public:
        TextBox(const std::string& text = std::string(),
                const Rect& rect = Rect());

        virtual int handle(int event) override;

        virtual void draw(Painter& painter, const Rect& rect) override;

        virtual void focus(bool value);

        /**
         * Set the text value.
         */
        virtual void value(const std::string& text);

        /**
         * Append text to the existing contents.
         */
        virtual void append(const std::string& text);

        /**
         * Clear the text value.
         */
        virtual void clear();

        /**
         * Get the value of the text.
         */
        inline const std::string& value() const { return m_text; }

        /**
         * Set the font of the label.
         */
        virtual void font(const Font& font) { m_font = font; }

        /**
         * Set the alignment of the text.
         */
        virtual void text_align(alignmask align) { m_text_align = align; }

        virtual ~TextBox();

    protected:
        void start_cursor();
        void stop_cursor();

        std::string m_text;
        alignmask m_text_align {alignmask::CENTER | alignmask::LEFT};
        Font m_font;
        PeriodicTimer m_timer;

    private:
        void cursor_timeout();
        bool m_cursor_state{false};
    };

    /**
     * Multi-line text box.
     *
     * This supports more than one line of text.
     */
    class MultilineTextBox : public TextBox
    {
    public:
        MultilineTextBox(const std::string& text = std::string(),
                         const Rect& rect = Rect());

        virtual void draw(Painter& painter, const Rect& rect) override;

        /**
         * Get the last line of the text box.
         */
        std::string last_line() const;

        virtual ~MultilineTextBox();
    };

}

#endif
