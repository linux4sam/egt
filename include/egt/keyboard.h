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
#include <memory>
#include <vector>

namespace egt
{
inline namespace v1
{

/**
 * @brief On-screen keyboard.
 */
template <class T>
class Keyboard : public T
{
public:
    Keyboard()
        : T(Size(800, 200)),
          m_grid(Tuple(10, 4), 5)
    {
        this->add(expand(m_grid));
        this->m_grid.palette().set(Palette::ColorId::border, Palette::GroupId::normal, Palette::transparent);

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

                std::shared_ptr<Button> b;

                if (label.find(".png") != std::string::npos)
                {
                    auto c = std::make_shared<ImageButton>(Image(label));
                    c->set_image_align(alignmask::center);
                    b = c;
                }
                else
                    b = std::make_shared<Button>(label);

                b->on_event([this, b](eventid event)
                {
                    if (!b->text().empty())
                    {
                        if (event == eventid::raw_pointer_down)
                        {
                            // hack: ascii characters translate directly
                            m_in.m_keys.key = b->text()[0];
                            m_in.m_keys.code = 0;
                            m_in.dispatch(eventid::keyboard_down);
                        }
                        else if (event == eventid::raw_pointer_up)
                        {
                            // hack: ascii characters translate directly
                            m_in.m_keys.key = b->text()[0];
                            m_in.m_keys.code = 0;
                            m_in.dispatch(eventid::keyboard_up);
                        }
                    }

                    return 0;
                });

                this->m_grid.add(expand(b), c, r);
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
