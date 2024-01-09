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

#include <array>
#include <egt/asio.hpp>
#include <egt/detail/meta.h>
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
class EGT_API InputLibInput : public Input
{
public:

    /**
     * @throws std::runtime_error if unable to open device.
     */
    explicit InputLibInput(Application& app);

    ~InputLibInput() noexcept override;

private:

    void handle_event_device_notify(struct libinput_event* ev);
    void handle_event_touch(struct libinput_event* ev);
    void handle_event_keyboard(struct libinput_event* ev);
    void handle_event_button(struct libinput_event* ev);
    void handle_event_pointer_motion(struct libinput_event* ev);
    void handle_event_pointer_motion_absolute(struct libinput_event* ev);

    void handle_read(const asio::error_code& error);

    /// Application instance.
    Application& m_app;

    /// Input handler to read from the evdev fd.
    asio::posix::stream_descriptor m_input;

    struct LibInputImpl;
    std::unique_ptr<LibInputImpl> m_impl;

    struct libinput* m_libinput_handle;

    /// The last point seen, indexed by slot, used for reference internally.
    std::array<DisplayPoint, 2> m_last_point{};
};

}
}
}

#endif
