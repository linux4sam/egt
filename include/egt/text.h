/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef EGT_TEXT_H
#define EGT_TEXT_H

/**
 * @file
 * @brief Working with text input.
 */

#include <egt/font.h>
#include <egt/timer.h>
#include <egt/widget.h>
#include <string>

namespace egt
{

    /**
     * Input text box.
     *
     * This supports text on a single line.
     *
     * @image html widget_textbox.png
     * @image latex widget_textbox.png "widget_txtbox" height=10cm
     */
    class TextBox : public TextWidget
    {
    public:
        TextBox(const std::string& str = std::string(),
                const Rect& rect = Rect());

        virtual int handle(eventid event) override;

        virtual void draw(Painter& painter, const Rect& rect) override;

        virtual void focus(bool value) override;

        /**
         * Append text to the existing contents.
         */
        virtual void append(const std::string& str);

        virtual ~TextBox();

    protected:
        void start_cursor();
        void stop_cursor();

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
        MultilineTextBox(const std::string& str = std::string(),
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
