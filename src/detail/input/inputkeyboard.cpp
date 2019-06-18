/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "detail/input/inputkeyboard.h"
#include <exception>
#include <spdlog/spdlog.h>

namespace egt
{
inline namespace v1
{
namespace detail
{

uint32_t BasicInputKeyboard::on_key(uint32_t key, eventid event)
{
    static const auto EVDEV_OFFSET = 8;
    auto code = detail::linux_to_ekey(key - EVDEV_OFFSET);
    auto utf32 = ekey_to_utf32(code,
                               m_key_states[EKEY_LSHIFT] || m_key_states[EKEY_RSHIFT],
                               m_key_states[EKEY_CAPSLOCK],
                               m_key_states[EKEY_NUMLOCK]);

    if (code == EKEY_CAPSLOCK)
    {
        if (event == eventid::keyboard_down)
            m_key_states[EKEY_CAPSLOCK] = !m_key_states[EKEY_CAPSLOCK];
    }
    else if (code == EKEY_NUMLOCK)
    {
        if (event == eventid::keyboard_down)
            m_key_states[EKEY_NUMLOCK] = !m_key_states[EKEY_NUMLOCK];
    }
    else
    {
        switch (event)
        {
        case eventid::keyboard_down:
            m_key_states[code] = true;
            break;
        case eventid::keyboard_up:
            m_key_states[code] = false;
            break;
        default:
            break;
        }
    }
    return utf32;
}

uint32_t BasicInputKeyboard::ekey_to_utf32(KeyboardCode code, bool shift, bool caps, bool numlock)
{
    switch (code)
    {
    case EKEY_TAB:
        return '\t';
    case EKEY_ENTER:
        return '\n';
    case EKEY_SPACE:
        return ' ';
    case EKEY_0:
        return shift ? ')' : '0';
    case EKEY_1:
        return shift ? '!' : '1';
    case EKEY_2:
        return shift ? '@' : '2';
    case EKEY_3:
        return shift ? '#' : '3';
    case EKEY_4:
        return shift ? '$' : '4';
    case EKEY_5:
        return shift ? '%' : '5';
    case EKEY_6:
        return shift ? '^' : '6';
    case EKEY_7:
        return shift ? '&' : '7';
    case EKEY_8:
        return shift ? '*' : '8';
    case EKEY_9:
        return shift ? '(' : '9';
    case EKEY_A:
    case EKEY_B:
    case EKEY_C:
    case EKEY_D:
    case EKEY_E:
    case EKEY_F:
    case EKEY_G:
    case EKEY_H:
    case EKEY_I:
    case EKEY_J:
    case EKEY_K:
    case EKEY_L:
    case EKEY_M:
    case EKEY_N:
    case EKEY_O:
    case EKEY_P:
    case EKEY_Q:
    case EKEY_R:
    case EKEY_S:
    case EKEY_T:
    case EKEY_U:
    case EKEY_V:
    case EKEY_W:
    case EKEY_X:
    case EKEY_Y:
    case EKEY_Z:
        return ((shift || caps) ? 'A' : 'a') + (code - EKEY_A);
    case EKEY_NUMPAD0:
    case EKEY_NUMPAD1:
    case EKEY_NUMPAD2:
    case EKEY_NUMPAD3:
    case EKEY_NUMPAD4:
    case EKEY_NUMPAD5:
    case EKEY_NUMPAD6:
    case EKEY_NUMPAD7:
    case EKEY_NUMPAD8:
    case EKEY_NUMPAD9:
        if (numlock)
            break;
        return '0' + (code - EKEY_NUMPAD0);
    case EKEY_MULTIPLY:
        return '*';
    case EKEY_ADD:
        return '+';
    case EKEY_SUBTRACT:
        return '-';
    case EKEY_DECIMAL:
        return '.';
    case EKEY_DIVIDE:
        return '/';
    case EKEY_PLUS:
        return '+';
    case EKEY_COMMA:
        return shift ? '<' : ',';
    case EKEY_MINUS:
        return shift ? '_' : '-';
    case EKEY_DOT:
        return shift ? '>' : '.';
    case EKEY_NUMDOT:
        return '.';
    case EKEY_SLASH:
        return shift ? '?' : '/';
    case EKEY_SEMICOLON:
        return shift ? ':' : ';';
    case EKEY_APOSTROPHE:
        return shift ? '"' : '\'';
    case EKEY_LEFTBRACE:
        return shift ? '{' : '[';
    case EKEY_RIGHTBRACE:
        return shift ? '}' : ']';
    case EKEY_EQUAL:
        return shift ? '+' : '=';
    case EKEY_GRAVE:
        return shift ? '~' : '`';
    case EKEY_BACKSLASH:
        return shift ? '|' : '\\';
    default:
        break;
    }

    return 0;
}

#ifdef HAVE_XKBCOMMON
XkbInputKeyboard::XkbInputKeyboard()
{
    m_ctx = unique_xkb_context(xkb_context_new(XKB_CONTEXT_NO_FLAGS));
    if (!m_ctx)
        throw std::runtime_error("could not create xkb context");

    struct xkb_rule_names names {};
    /*
    names.rules = nullptr;
    names.model = "pc105";
    names.layout = "us";
    names.variant = nullptr;
    names.options = nullptr;
    */

    m_keymap = unique_xkb_keymap(xkb_keymap_new_from_names(m_ctx.get(),
                                 &names,
                                 XKB_KEYMAP_COMPILE_NO_FLAGS));
    if (!m_keymap)
        throw std::runtime_error("could not create xkb keymap");

    m_state = unique_xkb_state(xkb_state_new(m_keymap.get()));
    if (!m_state)
        throw std::runtime_error("could not create xkb state");
}

uint32_t XkbInputKeyboard::on_key(uint32_t key, eventid event)
{
    uint32_t utf32 = 0;;
    xkb_keycode_t keycode = key;

#ifndef NDEBUG
    xkb_keysym_t keysym = xkb_state_key_get_one_sym(m_state.get(), keycode);
    char keysym_name[64];
    xkb_keysym_get_name(keysym, keysym_name, sizeof(keysym_name));
    SPDLOG_TRACE("keysym name {}", keysym_name);
#endif

    utf32 = xkb_state_key_get_utf32(m_state.get(), keycode);

    if (event == eventid::keyboard_repeat && !xkb_keymap_key_repeats(m_keymap.get(), keycode))
    {
    }
    else
    {
        if (event == eventid::keyboard_down)
            xkb_state_update_key(m_state.get(), keycode, XKB_KEY_DOWN);
        else if (event == eventid::keyboard_up)
            xkb_state_update_key(m_state.get(), keycode, XKB_KEY_UP);
    }

    return utf32;
}
#endif

}
}
}
