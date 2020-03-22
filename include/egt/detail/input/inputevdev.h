/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_INPUT_INPUTEVDEV_H
#define EGT_DETAIL_INPUT_INPUTEVDEV_H

/**
 * @file
 * @brief Working with input devices.
 */

#include <egt/asio.hpp>
#include <egt/detail/meta.h>
#include <egt/input.h>
#include <string>
#include <vector>

namespace egt
{
inline namespace v1
{
class Application;

namespace detail
{
class InputKeyboard;

/**
 * Handles reading input events from evdev devices.
 */
class EGT_API InputEvDev : public Input
{
public:

    /**
     * @throws std::runtime_error if unable to open device.
     */
    explicit InputEvDev(Application& app, const std::string& path);

    ~InputEvDev() override;

private:
    void handle_read(const asio::error_code& error, std::size_t length);

    /**
     * Input handler to read from the evdev fd.
     */
    asio::posix::stream_descriptor m_input;

    /**
     * Input buffer of events.
     */
    std::vector<char> m_input_buf;

    /**
     * The last point seen, used for reference internally.
     */
    DisplayPoint m_last_point;

    /**
     * Keyboard mapping instance.
     */
    std::unique_ptr<InputKeyboard> m_keyboard;

    /**
     * Internal descriptor;
     */
    int m_fd{-1};
};

}
}
}

#endif
