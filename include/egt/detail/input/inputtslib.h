/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_INPUT_INPUTTSLIB_H
#define EGT_DETAIL_INPUT_INPUTTSLIB_H

/**
 * @file
 * @brief Working with input devices.
 */

#include <egt/input.h>
#include <memory>
#include <string>

namespace egt
{
inline namespace v1
{
namespace detail
{
struct tslibimpl;

/**
 * Handles reading input from a tslib supported device.
 */
class InputTslib : public Input
{
public:

    explicit InputTslib(Application& app, const std::string& path);

    virtual ~InputTslib();

private:

    void handle_read(const asio::error_code& error);

    Application& m_app;
    asio::posix::stream_descriptor m_input;
    std::unique_ptr<tslibimpl> m_impl;
    bool m_active{false};
    DisplayPoint m_last_point;
};

}
}
}

#endif
