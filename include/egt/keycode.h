/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_KEYCODE_H
#define EGT_KEYCODE_H

/**
 * @file
 * @brief Key codes.
 */

namespace egt
{
inline namespace v1
{

enum KeyboardCode
{
    EKEY_CANCEL = 0x03,
    EKEY_BACK = 0x08,
    EKEY_TAB = 0x09,
    EKEY_BACKTAB = 0x0A,
    EKEY_CLEAR = 0x0C,
    EKEY_RETURN = 0x0D,
    EKEY_SHIFT = 0x10,
    EKEY_CONTROL = 0x11,
    EKEY_MENU = 0x12,
    EKEY_PAUSE = 0x13,
    EKEY_CAPITAL = 0x14,
    EKEY_KANA = 0x15,
    EKEY_HANGUL = 0x15,
    EKEY_JUNJA = 0x17,
    EKEY_FINAL = 0x18,
    EKEY_HANJA = 0x19,
    EKEY_KANJI = 0x19,
    EKEY_ESCAPE = 0x1B,
    EKEY_CONVERT = 0x1C,
    EKEY_NONCONVERT = 0x1D,
    EKEY_ACCEPT = 0x1E,
    EKEY_MODECHANGE = 0x1F,
    EKEY_SPACE = 0x20,
    EKEY_PRIOR = 0x21,
    EKEY_NEXT = 0x22,
    EKEY_END = 0x23,
    EKEY_HOME = 0x24,
    EKEY_LEFT = 0x25,
    EKEY_UP = 0x26,
    EKEY_RIGHT = 0x27,
    EKEY_DOWN = 0x28,
    EKEY_SELECT = 0x29,
    EKEY_PRINT = 0x2A,
    EKEY_EXECUTE = 0x2B,
    EKEY_SNAPSHOT = 0x2C,  // Print Screen / SysRq
    EKEY_INSERT = 0x2D,
    EKEY_DELETE = 0x2E,
    EKEY_HELP = 0x2F,
    EKEY_0 = 0x30,
    EKEY_1 = 0x31,
    EKEY_2 = 0x32,
    EKEY_3 = 0x33,
    EKEY_4 = 0x34,
    EKEY_5 = 0x35,
    EKEY_6 = 0x36,
    EKEY_7 = 0x37,
    EKEY_8 = 0x38,
    EKEY_9 = 0x39,
    EKEY_A = 0x41,
    EKEY_B = 0x42,
    EKEY_C = 0x43,
    EKEY_D = 0x44,
    EKEY_E = 0x45,
    EKEY_F = 0x46,
    EKEY_G = 0x47,
    EKEY_H = 0x48,
    EKEY_I = 0x49,
    EKEY_J = 0x4A,
    EKEY_K = 0x4B,
    EKEY_L = 0x4C,
    EKEY_M = 0x4D,
    EKEY_N = 0x4E,
    EKEY_O = 0x4F,
    EKEY_P = 0x50,
    EKEY_Q = 0x51,
    EKEY_R = 0x52,
    EKEY_S = 0x53,
    EKEY_T = 0x54,
    EKEY_U = 0x55,
    EKEY_V = 0x56,
    EKEY_W = 0x57,
    EKEY_X = 0x58,
    EKEY_Y = 0x59,
    EKEY_Z = 0x5A,
    EKEY_LWIN = 0x5B,
    EKEY_COMMAND = EKEY_LWIN,  // Provide the Mac name for convenience.
    EKEY_RWIN = 0x5C,
    EKEY_APPS = 0x5D,
    EKEY_SLEEP = 0x5F,
    EKEY_NUMPAD0 = 0x60,
    EKEY_NUMPAD1 = 0x61,
    EKEY_NUMPAD2 = 0x62,
    EKEY_NUMPAD3 = 0x63,
    EKEY_NUMPAD4 = 0x64,
    EKEY_NUMPAD5 = 0x65,
    EKEY_NUMPAD6 = 0x66,
    EKEY_NUMPAD7 = 0x67,
    EKEY_NUMPAD8 = 0x68,
    EKEY_NUMPAD9 = 0x69,
    EKEY_MULTIPLY = 0x6A,
    EKEY_ADD = 0x6B,
    EKEY_SEPARATOR = 0x6C,
    EKEY_SUBTRACT = 0x6D,
    EKEY_DECIMAL = 0x6E,
    EKEY_DIVIDE = 0x6F,
    EKEY_F1 = 0x70,
    EKEY_F2 = 0x71,
    EKEY_F3 = 0x72,
    EKEY_F4 = 0x73,
    EKEY_F5 = 0x74,
    EKEY_F6 = 0x75,
    EKEY_F7 = 0x76,
    EKEY_F8 = 0x77,
    EKEY_F9 = 0x78,
    EKEY_F10 = 0x79,
    EKEY_F11 = 0x7A,
    EKEY_F12 = 0x7B,
    EKEY_F13 = 0x7C,
    EKEY_F14 = 0x7D,
    EKEY_F15 = 0x7E,
    EKEY_F16 = 0x7F,
    EKEY_F17 = 0x80,
    EKEY_F18 = 0x81,
    EKEY_F19 = 0x82,
    EKEY_F20 = 0x83,
    EKEY_F21 = 0x84,
    EKEY_F22 = 0x85,
    EKEY_F23 = 0x86,
    EKEY_F24 = 0x87,
    EKEY_NUMLOCK = 0x90,
    EKEY_SCROLL = 0x91,
    EKEY_LSHIFT = 0xA0,
    EKEY_RSHIFT = 0xA1,
    EKEY_LCONTROL = 0xA2,
    EKEY_RCONTROL = 0xA3,
    EKEY_LMENU = 0xA4,
    EKEY_RMENU = 0xA5,
    EKEY_BROWSER_BACK = 0xA6,
    EKEY_BROWSER_FORWARD = 0xA7,
    EKEY_BROWSER_REFRESH = 0xA8,
    EKEY_BROWSER_STOP = 0xA9,
    EKEY_BROWSER_SEARCH = 0xAA,
    EKEY_BROWSER_FAVORITES = 0xAB,
    EKEY_BROWSER_HOME = 0xAC,
    EKEY_VOLUME_MUTE = 0xAD,
    EKEY_VOLUME_DOWN = 0xAE,
    EKEY_VOLUME_UP = 0xAF,
    EKEY_MEDIA_NEXT_TRACK = 0xB0,
    EKEY_MEDIA_PREV_TRACK = 0xB1,
    EKEY_MEDIA_STOP = 0xB2,
    EKEY_MEDIA_PLAY_PAUSE = 0xB3,
    EKEY_MEDIA_LAUNCH_MAIL = 0xB4,
    EKEY_MEDIA_LAUNCH_MEDIA_SELECT = 0xB5,
    EKEY_MEDIA_LAUNCH_APP1 = 0xB6,
    EKEY_MEDIA_LAUNCH_APP2 = 0xB7,
    EKEY_OEM_1 = 0xBA,
    EKEY_OEM_PLUS = 0xBB,
    EKEY_OEM_COMMA = 0xBC,
    EKEY_OEM_MINUS = 0xBD,
    EKEY_OEM_PERIOD = 0xBE,
    EKEY_OEM_2 = 0xBF,
    EKEY_OEM_3 = 0xC0,
    EKEY_OEM_4 = 0xDB,
    EKEY_OEM_5 = 0xDC,
    EKEY_OEM_6 = 0xDD,
    EKEY_OEM_7 = 0xDE,
    EKEY_OEM_8 = 0xDF,
    EKEY_OEM_102 = 0xE2,
    EKEY_OEM_103 = 0xE3,  // GTV KEYCODE_MEDIA_REWIND
    EKEY_OEM_104 = 0xE4,  // GTV KEYCODE_MEDIA_FAST_FORWARD
    EKEY_PROCESSKEY = 0xE5,
    EKEY_PACKET = 0xE7,
    EKEY_OEM_ATTN = 0xF0,      // JIS DomKey::ALPHANUMERIC
    EKEY_OEM_FINISH = 0xF1,    // JIS DomKey::KATAKANA
    EKEY_OEM_COPY = 0xF2,      // JIS DomKey::HIRAGANA
    EKEY_DBE_SBCSCHAR = 0xF3,  // JIS DomKey::HANKAKU
    EKEY_DBE_DBCSCHAR = 0xF4,  // JIS DomKey::ZENKAKU
    EKEY_OEM_BACKTAB = 0xF5,   // JIS DomKey::ROMAJI
    EKEY_ATTN = 0xF6,          // DomKey::ATTN or JIS DomKey::KANA_MODE
    EKEY_CRSEL = 0xF7,
    EKEY_EXSEL = 0xF8,
    EKEY_EREOF = 0xF9,
    EKEY_PLAY = 0xFA,
    EKEY_ZOOM = 0xFB,
    EKEY_NONAME = 0xFC,
    EKEY_PA1 = 0xFD,
    EKEY_OEM_CLEAR = 0xFE,
    EKEY_UNKNOWN = 0,
    // POSIX specific EKEYs. Note that as of Windows SDK 7.1, 0x97-9F, 0xD8-DA,
    // and 0xE8 are unassigned.
    EKEY_WLAN = 0x97,
    EKEY_POWER = 0x98,
    EKEY_ASSISTANT = 0x99,
    EKEY_SETTINGS = 0x9A,
    EKEY_BRIGHTNESS_DOWN = 0xD8,
    EKEY_BRIGHTNESS_UP = 0xD9,
    EKEY_KBD_BRIGHTNESS_DOWN = 0xDA,
    EKEY_KBD_BRIGHTNESS_UP = 0xE8,
    // Windows does not have a specific key code for AltGr. We use the unused 0xE1
    // (VK_OEM_AX) code to represent AltGr, matching the behaviour of Firefox on
    // Linux.
    EKEY_ALTGR = 0xE1,
    // Windows does not have a specific key code for Compose. We use the unused
    // 0xE6 (VK_ICO_CLEAR) code to represent Compose.
    EKEY_COMPOSE = 0xE6,


};

}
}

#endif
