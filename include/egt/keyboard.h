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
#include <egt/sizer.h>
#include <memory>
#include <string>
#include <vector>

namespace egt
{
inline namespace v1
{

class Keyboard;

class Key : public Button
{
public:
    Key(std::string label, double length = 1.0);
    double length() const;

protected:
    double m_length;
};

class Panel : public NotebookTab
{
public:
    Panel(std::vector<std::vector<std::shared_ptr<Key>>> k,
          Size size = Size(0, 0));

    void set_keyboard(Keyboard* keyboard);

protected:
    VerticalBoxSizer m_sizer;
    Keyboard* m_keyboard;
};

class Keyboard : public Notebook
{
public:
    Keyboard(std::vector<std::shared_ptr<Panel>> panels, Size size = Size());

private:
    struct KeyboardInput : public Input
    {
        using Input::Input;
        friend class Panel;
    };
    KeyboardInput m_in;
    friend class Panel;
};

}
}

#endif
