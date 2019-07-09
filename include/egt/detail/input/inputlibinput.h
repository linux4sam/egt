/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef EGT_DETAIL_INPUT_INPUTLIBINPUT_H
#define EGT_DETAIL_INPUT_INPUTLIBINPUT_H

/**
 * @file
 * @brief Working with libinput devices.
 */

#include <egt/asio.hpp>
#include <linux/input.h>
#include <egt/input.h>
#include <memory>

struct libinput;
struct libinput_event;

namespace egt
{
inline namespace v1
{
class Application;

namespace detail
{
class InputKeyboard;

/**
 * Handles populating and reading input events from libinput.
 */
class InputLibInput : public Input
{
public:

    explicit InputLibInput(Application& app);

    virtual ~InputLibInput();

private:

    void handle_event_device_notify(struct libinput_event* ev);
    void handle_event_touch(struct libinput_event* ev);
    void handle_event_keyboard(struct libinput_event* ev);
    void handle_event_button(struct libinput_event* ev);

    void handle_read(const asio::error_code& error);

    /**
     * Application instance.
     */
    Application& m_app;

    /**
     * Input handler to read from the evdev fd.
     */
    asio::posix::stream_descriptor m_input;

    struct libinput* li;

    /**
     * The last point seen, used for reference internally.
     */
    DisplayPoint m_last_point;

    /**
     * Keyboard mapping instance.
     */
    std::unique_ptr<InputKeyboard> m_keyboard;
};

}
}
}

#endif
