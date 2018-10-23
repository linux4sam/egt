/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_X11SCREEN_H
#define MUI_X11SCREEN_H

#ifdef HAVE_X11

#include "asio.hpp"
#include <mui/screen.h>
#include <memory>

namespace mui
{
    struct X11Data;

    /**
     * Screen in an X11 window.
     */
    class X11Screen : public IScreen
    {
    public:
        X11Screen(const Size& size = Size(1024, 1024), bool borderless = false);

        void flip(const damage_array& damage) override;

        virtual ~X11Screen();

    protected:

        void handle_read(const asio::error_code& error);

        std::shared_ptr<X11Data> m_priv;
        asio::posix::stream_descriptor m_input;
    };

}

#endif

#endif
