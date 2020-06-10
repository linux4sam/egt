/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_INPUTKEYBOARD_H
#define EGT_DETAIL_INPUTKEYBOARD_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/event.h"
#include "egt/keycode.h"
#include <array>
#include <cassert>
#include <spdlog/spdlog.h>

#ifdef HAVE_XKBCOMMON
#include <xkbcommon/xkbcommon.h>
#endif

namespace egt
{
inline namespace v1
{
namespace detail
{

class InputKeyboardImpl
{
public:

    /**
     * Handle a key from the input device.
     *
     * @return A UTF32 representation of the key, or zero.
     */
    virtual uint32_t on_key(uint32_t key, EventId event) = 0;

    virtual ~InputKeyboardImpl() = default;
};

/**
 * Standard, plain QWERTY keyboard mapping with basic modifier key support.
 */
class BasicInputKeyboard : public InputKeyboardImpl
{
public:
    uint32_t on_key(uint32_t key, EventId event) override;
protected:

    static uint32_t ekey_to_utf32(KeyboardCode code, bool shift, bool caps, bool numlock);

    std::array<bool, 512> m_key_states{};
};

#ifdef HAVE_XKBCOMMON
struct xkb_context_deleter
{
    void operator()(xkb_context* ctx) { xkb_context_unref(ctx); }
};

using unique_xkb_context =
    std::unique_ptr<xkb_context, xkb_context_deleter>;

struct xkb_keymap_deleter
{
    void operator()(xkb_keymap* keymap) { xkb_keymap_unref(keymap); }
};

using unique_xkb_keymap =
    std::unique_ptr<xkb_keymap, xkb_keymap_deleter>;

struct xkb_state_deleter
{
    void operator()(xkb_state* state) { xkb_state_unref(state); }
};

using unique_xkb_state =
    std::unique_ptr<xkb_state, xkb_state_deleter>;

/**
 * https://xkbcommon.org/doc/current/md_doc_quick-guide.html
 */
class XkbInputKeyboard : public InputKeyboardImpl
{
public:

    XkbInputKeyboard();

    uint32_t on_key(uint32_t key, EventId event) override;

    virtual ~XkbInputKeyboard() = default;

protected:

    unique_xkb_context m_ctx;
    unique_xkb_keymap m_keymap;
    unique_xkb_state m_state;
};
#endif

/**
 * Manages key state and unicode conversion.
 */
class InputKeyboard
{
public:

    uint32_t on_key(uint32_t key, EventId event)
    {
        // delayed allocation of implementation on first use
        allocate();
        return m_impl->on_key(key, event);
    }

    void allocate()
    {
        if (!m_impl)
        {
#ifdef HAVE_XKBCOMMON
            try
            {
                m_impl = std::make_unique<XkbInputKeyboard>();
                SPDLOG_DEBUG("using xkb input keyboard mapping");
                return;
            }
            catch (...)
            {
                spdlog::error("failed to load xkb input keyboard mapping");
            }
#endif

            m_impl = std::make_unique<BasicInputKeyboard>();
            SPDLOG_DEBUG("using basic input keyboard mapping");
        }
    }

protected:

    std::unique_ptr<InputKeyboardImpl> m_impl;
};

}
}
}

#endif
