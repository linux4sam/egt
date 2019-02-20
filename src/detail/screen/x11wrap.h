/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_X11_WRAP_H
#define EGT_X11_WRAP_H

extern "C" {
#include <X11/Xlib.h>
#include <X11/Xutil.h>

    // Define XK_xxx before the #include of <X11/keysym.h> so that <X11/keysym.h>
    // defines all KeySyms we need.
#define XK_3270  // For XK_3270_BackTab in particular.
#define XK_MISCELLANY
#define XK_LATIN1
#define XK_LATIN2
#define XK_LATIN3
#define XK_LATIN4
#define XK_LATIN8
#define XK_LATIN9
#define XK_KATAKANA
#define XK_ARABIC
#define XK_CYRILLIC
#define XK_GREEK
#define XK_TECHNICAL
#define XK_SPECIAL
#define XK_PUBLISHING
#define XK_APL
#define XK_HEBREW
#define XK_THAI
#define XK_KOREAN
#define XK_ARMENIAN
#define XK_GEORGIAN
#define XK_CAUCASUS
#define XK_VIETNAMESE
#define XK_CURRENCY
#define XK_MATHEMATICAL
#define XK_BRAILLE
#define XK_SINHALA
#define XK_XKB_KEYS
#ifndef XK_dead_greek
#define XK_dead_greek 0xfe8c
#endif
#include <X11/Sunkeysym.h>
#include <X11/XF86keysym.h>
#include <X11/keysym.h>
}

#endif
