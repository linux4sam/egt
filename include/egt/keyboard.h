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

class Keyboard : public Frame
{
public:
    class MultichoicePanel;

    /**
     * @brief Key widget for the Keyboard widget.
     *
     * This widget allows to describe a key that will be used by the Keyboard
     * widget.
     */
    class Key : public Button
    {
    public:

        /**
         * @param[in] label Label of the key.
         * @param[in] length Length of the key. It multiplies the width of the
         * default key size which is configured when creating a Panel.
         */
        Key(std::string label, double length = 1.0);

        /**
         * @param[in] label Label of the key.
         * @param[in] link Id of the main panel to display when clicking on this
         * key.
         * The id depends on the order of registration of the main panels. It
         * starts from 0.
         * @param[in] length Length of the key. It multiplies the width of the
         * default key size which is configured when creating a Panel.
         */
        Key(std::string label, int link, double length = 1.0);

        /**
         * @param[in] label Label of the key.
         * @param[in] multichoice Multichoice panel that has to be displayed
         * after a long touch event.
         * @param[in] length Length of the key. It multiplies the width of the
         * default key size which is configured when creating a Panel.
         */
        Key(std::string label, std::shared_ptr<MultichoicePanel> multichoice, double length = 1.0);

        double length() const;
        int link() const;

    protected:
        /**
         * Id of the main panel to display when clicking on this button.
         */
        int m_link{-1};

        /**
         * Length multiplicator of the panel default key size.
         */
        double m_length;

        /**
         * Link to a multichoice panel to display when long touch happens.
         */
        std::shared_ptr<MultichoicePanel> m_multichoice{nullptr};
        friend class Keyboard;
    };

    class Panel : public VerticalBoxSizer
    {
    public:
        Panel(std::vector<std::vector<std::shared_ptr<Key>>>  p,
              Size key_size = Size(0, 0));

    protected:
        std::vector<std::vector<std::shared_ptr<Key>>> m_keys;
        friend class Keyboard;
    };

    /**
     * @brief MainPanel widget for the Keyboard widget.
     *
     * This widget is used to define the main panels that will be displayed by
     * the keyboard.
     */
    class MainPanel : public NotebookTab
    {
    public:
        MainPanel(std::vector<std::vector<std::shared_ptr<Key>>> keys,
                  Size key_size = Size(0, 0));
    protected:
        std::shared_ptr<Panel> m_panel;
        friend class Keyboard;
    };

    /**
     * @brief MultichoicePanel widget for the Keyboard widget.
     *
     * This widget is used to define the multichoice panels that are displayed
     * when a long touch happens on a key.
     */
    class MultichoicePanel : public NotebookTab
    {
    public:
        MultichoicePanel(std::vector<std::vector<std::shared_ptr<Key>>> keys,
                         Size key_size = Size(0, 0));
    protected:
        std::shared_ptr<Panel> m_panel;
        friend class Keyboard;
    };

    /**
     * @param[in] panels Main panels of the keyboard.
     * @param[in] size Size of the Keyboard.
     */
    Keyboard(std::vector<std::shared_ptr<MainPanel>> panels, Size size = Size());

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

    void set_key_link(const std::shared_ptr<Key>& k);
    void set_key_input_value(const std::shared_ptr<Key>& k);
    void set_key_multichoice(const std::shared_ptr<Key>& k, unsigned id);

    KeyboardInput m_in;
    Notebook m_main_panel;
    MultichoicePopup m_multichoice_popup;
};

}
}

#endif
