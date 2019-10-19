/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_SCREEN_X11SCREEN_H
#define EGT_DETAIL_SCREEN_X11SCREEN_H

#include <egt/detail/meta.h>
#include <egt/asio.hpp>
#include <egt/screen.h>
#include <egt/input.h>
#include <memory>

namespace egt
{
inline namespace v1
{
class Application;

namespace detail
{
struct X11Data;
class InputKeyboard;

/**
 * Screen in an X11 window.
 */
class EGT_API X11Screen : public Screen
{
public:

    explicit X11Screen(Application& app, const Size& size = Size(800, 480), bool borderless = false);

    virtual void schedule_flip() override {}

    virtual void flip(const damage_array& damage) override;

    virtual ~X11Screen();

protected:

    void handle_read(const asio::error_code& error);

    virtual void copy_to_buffer(ScreenBuffer& buffer) override;

    Application& m_app;
    std::unique_ptr<detail::X11Data> m_priv;
    asio::posix::stream_descriptor m_input;

    /**
     * X11 input dispatcher.
     *
     * Because a X11Screen also handles input from an X11Server, this needs to
     * have it's own Input device.
     */
    struct X11Input : public Input
    {
        using Input::Input;
        friend class X11Screen;
    };

    X11Input m_in;
    std::unique_ptr<InputKeyboard> m_keyboard;
};

}
}
}

#endif
