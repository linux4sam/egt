/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_SCREEN_X11SCREEN_H
#define EGT_DETAIL_SCREEN_X11SCREEN_H

#include <egt/asio.hpp>
#include <egt/screen.h>
#include <egt/input.h>
#include <memory>

namespace egt
{
inline namespace v1
{
namespace detail
{
struct X11Data;

/**
 * Screen in an X11 window.
 */
class X11Screen : public Screen
{
public:

    explicit X11Screen(const Size& size = Size(800, 480), bool borderless = false);

    virtual void schedule_flip() {}

    virtual void flip(const damage_array& damage) override;

    virtual ~X11Screen();

protected:

    void handle_read(const asio::error_code& error);

    std::shared_ptr<detail::X11Data> m_priv;
    asio::posix::stream_descriptor m_input;

    struct X11Input : public Input
    {
        using Input::Input;
        friend class X11Screen;
    };

    X11Input m_in;
};

}
}
}

#endif
