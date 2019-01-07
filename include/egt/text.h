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

#include <egt/font.h>
#include <egt/timer.h>
#include <egt/widget.h>
#include <string>

namespace egt
{
    inline namespace v1
    {
        /**
         * Input text box.
         *
         * This supports text on a single line.
         *
         * @image html widget_textbox.png
         * @image latex widget_textbox.png "widget_txtbox" height=5cm
         */
        class TextBox : public TextWidget
        {
        public:
            TextBox(const std::string& str,
                    const Rect& rect = Rect(),
                    alignmask align = alignmask::CENTER | alignmask::LEFT);

            TextBox(const Rect& rect = Rect(),
                    alignmask align = alignmask::CENTER | alignmask::LEFT);

            virtual int handle(eventid event) override;

            virtual void draw(Painter& painter, const Rect& rect) override;

            virtual void on_gain_focus() override;

            virtual void on_lost_focus() override;

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
            MultilineTextBox(const std::string& str,
                             const Rect& rect = Rect(),
                             alignmask align = alignmask::CENTER | alignmask::LEFT);

            MultilineTextBox(const Rect& rect = Rect(),
                             alignmask align = alignmask::CENTER | alignmask::LEFT);

            virtual void draw(Painter& painter, const Rect& rect) override;

            virtual void set_wrap(bool state)
            {
                m_wrap = state;
            }

            /**
             * Get the last line of the text box.
             */
            std::string last_line() const;

            virtual ~MultilineTextBox();

        private:

            /** @todo Not implemented. */
            bool m_wrap{false};
        };

    }
}

#endif
