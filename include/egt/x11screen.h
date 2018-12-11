/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_X11SCREEN_H
#define EGT_X11SCREEN_H

#include <asio.hpp>
#include <egt/screen.h>
#include <memory>

namespace egt
{
    namespace detail
    {
        struct X11Data;
    }

    /**
     * Screen in an X11 window.
     */
    class X11Screen : public IScreen
    {
    public:

        X11Screen(const Size& size = Size(800, 480), bool borderless = false);

        virtual void flip(const damage_array& damage) override;

        virtual ~X11Screen();

    protected:

        void handle_read(const asio::error_code& error);

        std::shared_ptr<detail::X11Data> m_priv;
        asio::posix::stream_descriptor m_input;
    };

}

#endif
