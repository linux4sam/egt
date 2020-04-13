/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_KEYCODE_H
#define EGT_KEYCODE_H

#include <cstdint>
#include <egt/detail/meta.h>

/**
 * @file
 * @brief Key codes.
 */

namespace egt
{
inline namespace v1
{

/**
 * Key codes that represent a physical key.
 */
enum KeyboardCode
{
    EKEY_UNKNOWN = 0,
    EKEY_BACKSPACE = 0x08,
    EKEY_TAB = 0x09,
    EKEY_CLEAR = 0x0C,
    EKEY_ENTER = 0x0D,
    EKEY_MENU = 0x12,
    EKEY_PAUSE = 0x13,
    EKEY_CAPSLOCK = 0x14,
    EKEY_SNAPSHOT = 0x15,
    EKEY_ESCAPE = 0x1B,
    EKEY_NEXT = 0x1E,
    EKEY_SPACE = 0x20,
    EKEY_PAGEUP = 0x21,
    EKEY_PAGEDOWN = 0x22,
    EKEY_END = 0x23,
    EKEY_HOME = 0x24,
    EKEY_LEFT = 0x25,
    EKEY_UP = 0x26,
    EKEY_RIGHT = 0x27,
    EKEY_DOWN = 0x28,
    EKEY_SELECT = 0x29,
    EKEY_PRINT = 0x2A,
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
    EKEY_KPENTER = 0x6C,
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
    EKEY_LEFTALT = 0xA4,
    EKEY_RIGHTALT = 0xA5,
    EKEY_VOLUME_MUTE = 0xAD,
    EKEY_VOLUME_DOWN = 0xAE,
    EKEY_VOLUME_UP = 0xAF,
    EKEY_PLUS = 0xBB,
    EKEY_COMMA = 0xBC,
    EKEY_MINUS = 0xBD,
    EKEY_DOT = 0xBE,
    EKEY_NUMDOT = 0xBF,
    EKEY_SLASH = 0xC0,
    EKEY_SEMICOLON = 0xDB,
    EKEY_APOSTROPHE = 0xDC,
    EKEY_LEFTBRACE = 0xDD,
    EKEY_RIGHTBRACE = 0xDE,
    EKEY_EQUAL = 0xDF,
    EKEY_GRAVE = 0xE2,
    EKEY_BACKSLASH = 0xE3,
    EKEY_PLAY = 0xFA,
    EKEY_ZOOM = 0xFB,
    EKEY_WLAN = 0x97,
    EKEY_POWER = 0x98,
    EKEY_COMPOSE = 0xE6,
    EKEY_USER1 = 0x104,
};

namespace detail
{

/**
 * Map a Linux key code to an EKEY.
 */
EGT_API KeyboardCode linux_to_ekey(int code);

}

}
}

#endif
