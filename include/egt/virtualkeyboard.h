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
#include <egt/keycode.h>
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

class VirtualKeyboard : public Frame
{
public:
    class Panel;

    /**
     * @brief Key widget for the Keyboard widget.
     *
     * This widget allows to describe a key that will be used by the Keyboard
     * widget.
     */
    class Key
    {
    public:

        /**
         * @param[in] unicode UTF32 code point for the key. It's also the label
         * of the key.
         * @param[in] length Length of the key. It multiplies the width of the
         * default key size which is configured when creating a Panel.
         */
        Key(uint32_t unicode, double length = 1.0);

        /**
         * @param[in] label Label of the key.
         * @param[in] keycode Code of the key.
         * @param[in] length Length of the key. It multiplies the width of the
         * default key size which is configured when creating a Panel.
         */
        Key(const std::string& label, KeyboardCode keycode, double length = 1.0);

        /**
         * @param[in] label Label of the key.
         * @param[in] link Id of the main panel to display when clicking on this
         * key.
         * The id depends on the order of registration of the main panels. It
         * starts from 0.
         * @param[in] length Length of the key. It multiplies the width of the
         * default key size which is configured when creating a Panel.
         */
        Key(const std::string& label, int link, double length = 1.0);

        /**
         * @param[in] unicode UTF32 code point for the key.
         * @param[in] multichoice Multichoice panel that has to be displayed
         * after a long touch event.
         * @param[in] length Length of the key. It multiplies the width of the
         * default key size which is configured when creating a Panel.
         * @param[in] keycode Code of the key.
         */
        Key(uint32_t unicode, std::shared_ptr<Panel> multichoice,
            double length = 1.0, KeyboardCode keycode = EKEY_UNKNOWN);

        /**
         * Return the length of a key.
         */
        inline double length() const { return m_length; }

        /**
         * Return the MainPanel link of a key.
         */
        inline int link() const { return m_link; }

    protected:
        /**
         * Button associated with the key.
         */
        std::shared_ptr<Button> m_button{nullptr};

        /**
         * Id of the MainPanel to display when clicking on this button.
         */
        int m_link{-1};

        /**
         * Code of the key.
         */
        KeyboardCode m_keycode{EKEY_UNKNOWN};

        /**
         * UTF32 code point for the key.
         */
        uint32_t m_unicode{0};

        /**
         * Length multiplicator of the panel default key size.
         */
        double m_length;

        /**
         * Link to a multichoice panel to display when long touch happens.
         */
        std::shared_ptr<Panel> m_multichoice{nullptr};
        friend class VirtualKeyboard;
    };

    class Panel : public NotebookTab
    {
    public:
        Panel(std::vector<std::vector<std::shared_ptr<Key>>> keys,
              Size key_size = {}, int spacing = 0);

    protected:
        std::shared_ptr<VerticalBoxSizer> m_vsizer;
        std::vector<std::vector<std::shared_ptr<Key>>> m_keys;
        friend class VirtualKeyboard;
    };

    /**
     * A default keyboard with a qwerty layout.
     *
     * @param[in] panels Main panels of the keyboard.
     * @param[in] rect Rect of the Keyboard.
     */
    VirtualKeyboard(const Rect& rect = {});

    /**
     * A custom keyboard with your own panels.
     *
     * @param[in] panels Main panels of the keyboard.
     * @param[in] rect Rect of the Keyboard.
     */
    VirtualKeyboard(std::vector<std::shared_ptr<Panel>> panels, const Rect& rect = {});

private:
    struct MultichoicePopup : public Popup
    {
        MultichoicePopup()
        {
            m_notebook.set_align(alignmask::expand);
            add(m_notebook);
        }

        Notebook m_notebook;
        friend class VirtualKeyboard;
    };

    struct KeyboardInput : public Input
    {
        using Input::Input;
        friend class VirtualKeyboard;
    };

    void set_key_link(const std::shared_ptr<Key>& k);
    void set_key_input_value(const std::shared_ptr<Key>& k);
    void set_key_multichoice(const std::shared_ptr<Key>& k, unsigned id);

    KeyboardInput m_in;
    Notebook m_main_panel;
    MultichoicePopup m_multichoice_popup;
};

class PopupVirtualKeyboard : public Popup
{
public:
    PopupVirtualKeyboard(std::shared_ptr<VirtualKeyboard> keyboard);
private:
    VerticalBoxSizer m_vsizer;
    HorizontalBoxSizer m_hsizer;
    ImageButton m_close_button {Image("@cancel.png")};
    ImageButton m_top_bottom_button {Image("@arrow_up.png")};
};

/**
 * Get a pointer to the popup virtual keyboard.
 */
PopupVirtualKeyboard*& popup_virtual_keyboard();

}
}

#endif
