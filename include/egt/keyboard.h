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

#include <egt/button.h>
#include <egt/input.h>
#include <egt/notebook.h>
#include <egt/popup.h>
#include <egt/sizer.h>
#include <memory>
#include <string>
#include <vector>

namespace egt
{
inline namespace v1
{

class Keyboard;
class Panel;

class Key : public Button
{
public:
    Key(std::string label, int link = -1, double length = 1.0);
    Key(std::string label, std::shared_ptr<Panel> multichoice, double length = 1.0);
    double length() const;
    int link() const;

protected:
    int m_link;
    double m_length;
    std::shared_ptr<Panel> m_multichoice{nullptr};
    friend class Keyboard;
};

class Panel : public VerticalBoxSizer
{
public:
    Panel(std::vector<std::vector<std::shared_ptr<Key>>> k,
          Size size = Size(0, 0));

protected:
    std::vector<std::vector<std::shared_ptr<Key>>> m_keys;
    friend class Keyboard;
};

class Keyboard : public Frame
{
public:
    Keyboard(std::vector<std::shared_ptr<Panel>> panels, Size size = Size());

private:
    struct MultichoicePopup : public Popup
    {
        MultichoicePopup()
        {
            m_notebook.set_align(alignmask::expand);
            add(m_notebook);
        }

        Notebook m_notebook;
        friend class Keyboard;
    };

    struct KeyboardInput : public Input
    {
        using Input::Input;
        friend class Keyboard;
    };
    KeyboardInput m_in;
    Notebook m_main_panel;
    MultichoicePopup m_multichoice_popup;
};

}
}

#endif
