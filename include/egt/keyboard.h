/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_KEYBOARD_H
#define EGT_KEYBOARD_H

/**
 * @file
 * @brief On-screen Keyboard.
 */

#include <egt/grid.h>
#include <vector>

namespace egt
{
inline namespace v1
{
/**
 * On-screen keyboard.
 */
template <class T>
class Keyboard : public T
{
public:
    Keyboard()
        : T(Size(800, 200)),
          m_grid(Rect(), 10, 4, 5)
    {
        this->add(&m_grid);
        this->m_grid.set_align(alignmask::EXPAND);
        this->m_grid.palette().set(Palette::BORDER, Palette::GROUP_NORMAL, Color::TRANSPARENT);

        std::vector<std::vector<std::string>> buttons =
        {
            {"q", "w", "e", "r", "t", "y", "u", "i", "o", "p" },
            {"", "a", "s", "d", "f", "g", "h", "j", "k", "l" },
            {"@arrow_up.png", "z", "x", "c", "v", "b", "n", "m", "", "@arrow_left.png"},
            {"123", ",", "", "", "     ", "", "", "", "", "."}
        };

        for (size_t r = 0; r < buttons.size(); r++)
        {
            for (size_t c = 0; c < buttons[r].size(); c++)
            {
                std::string label = buttons[r][c];
                if (label.empty())
                    continue;

                Button* b;

                if (label.find(".png") != std::string::npos)
                {
                    auto c = new ImageButton(Image(label));
                    c->set_image_align(alignmask::CENTER);
                    b = c;
                }
                else
                    b = new Button(label);

                b->on_event([this, b](eventid event)
                {
                    if (!b->text().empty())
                    {
                        if (event == eventid::RAW_POINTER_DOWN)
                        {
                            // hack: ascii characters translate directly
                            m_in.m_keys.key = b->text()[0];
                            m_in.m_keys.code = 0;
                            m_in.dispatch(eventid::KEYBOARD_DOWN);
                        }
                        else if (event == eventid::RAW_POINTER_UP)
                        {
                            // hack: ascii characters translate directly
                            m_in.m_keys.key = b->text()[0];
                            m_in.m_keys.code = 0;
                            m_in.dispatch(eventid::KEYBOARD_UP);
                        }
                    }

                    return 0;
                });

                this->m_grid.add(b, c, r);
            }
        }

        this->m_grid.reposition();
    }

protected:

    struct KeyboardInput : public Input
    {
        using Input::Input;
        friend class Keyboard;
    };

    KeyboardInput m_in;

    StaticGrid m_grid;
};

}
}

#endif
