/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */

#ifndef EGT_LIBINPUT_H
#define EGT_LIBINPUT_H

/**
 * @file
 * @brief Working with libinput devices.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "asio.hpp"
#include <linux/input.h>
#include <egt/input.h>
#include <memory>

struct libinput;
struct libinput_event;

namespace egt
{

    /**
     * Handles populating and reading input events from libinput.
     */
    class LibInput : public IInput
    {
    public:

        LibInput();

        virtual ~LibInput();

    private:

        void handle_event_device_notify(struct libinput_event* ev);
        void handle_event_motion(struct libinput_event* ev);
        void handle_event_absmotion(struct libinput_event* ev);
        bool handle_event_touch(struct libinput_event* ev);
        void handle_event_axis(struct libinput_event* ev);
        void handle_event_keyboard(struct libinput_event* ev);
        void handle_event_button(struct libinput_event* ev);
        void handle_event_swipe(struct libinput_event* ev);
        void handle_event_pinch(struct libinput_event* ev);
        void handle_event_tablet(struct libinput_event* ev);

        void handle_read(const asio::error_code& error);

        asio::posix::stream_descriptor m_input;

        struct libinput* li;
    };

}

#endif
