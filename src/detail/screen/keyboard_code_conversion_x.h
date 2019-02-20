// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef UI_EVENTS_KEYCODES_KEYBOARD_CODE_CONVERSION_X_H_
#define UI_EVENTS_KEYCODES_KEYBOARD_CODE_CONVERSION_X_H_
#include <stdint.h>
#include <egt/keycode.h>
typedef union _XEvent XEvent;
typedef struct _XDisplay XDisplay;
namespace egt
{
inline namespace v1
{
namespace detail
{
KeyboardCode KeyboardCodeFromXKeyEvent(const XEvent* xev);
KeyboardCode KeyboardCodeFromXKeysym(unsigned int keysym);
// Returns a character on a standard US PC keyboard from an XEvent.
uint16_t GetCharacterFromXEvent(const XEvent* xev);
// Converts a KeyboardCode into an X KeySym.
int XKeysymForWindowsKeyCode(KeyboardCode keycode,
                             bool shift);
// Returns a XKeyEvent keycode (scancode) for a KeyboardCode. Keyboard layouts
// are usually not injective, so inverse mapping should be avoided when
// practical. A round-trip keycode -> KeyboardCode -> keycode will not
// necessarily return the original keycode.
unsigned int XKeyCodeForWindowsKeyCode(KeyboardCode key_code,
                                       int flags,
                                       XDisplay* display);
// Converts an X keycode into ui::KeyboardCode.
KeyboardCode
DefaultKeyboardCodeFromHardwareKeycode(unsigned int hardware_code);
}
}
}
#endif  // UI_EVENTS_KEYCODES_KEYBOARD_CODE_CONVERSION_X_H_
