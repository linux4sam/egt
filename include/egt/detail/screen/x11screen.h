/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_SCREEN_X11SCREEN_H
#define EGT_DETAIL_SCREEN_X11SCREEN_H

/**
 * @file
 * @brief X11 screen support.
 */

#include <egt/asio.hpp>
#include <egt/detail/meta.h>
#include <egt/input.h>
#include <egt/screen.h>
#include <memory>

namespace egt
{
inline namespace v1
{
class Application;

namespace detail
{
struct X11Data;

/**
 * Screen in an X11 window.
 */
class EGT_API X11Screen : public Screen
{
public:

    X11Screen() = delete;

    /**
     * @param app Application instance this screen is associated with.
     * @param size Size of the screen.
     * @param borderless Create the X11 window without a border.
     */
    explicit X11Screen(Application& app, const Size& size = Size(800, 480),
                       bool borderless = false);

    EGT_OPS_NOCOPY_NOMOVE(X11Screen);
    ~X11Screen() noexcept override;

    void schedule_flip() override
    {}

    void flip(const DamageArray& damage) override;

    /// Disable window decorations
    void disable_window_decorations();

protected:

    /// Callback for X11 server data.
    void handle_read(const asio::error_code& error);

    void copy_to_buffer(ScreenBuffer& buffer) override;

    /// Application reference.
    Application& m_app;

    /// @private
    std::unique_ptr<detail::X11Data> m_priv;

    /// Input stream for handling X11 server events
    asio::posix::stream_descriptor m_input;

    /**
     * X11 input dispatcher.
     *
     * Because a X11Screen also handles input from an X11Server, this needs to
     * have its own Input device.
     * @private
     */
    struct X11Input : public Input
    {
        using Input::Input;
        friend class X11Screen;
    };

    /// Custom input for dispatching events.
    X11Input m_in;
};

}
}
}

#endif
