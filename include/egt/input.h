/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef EGT_INPUT_H
#define EGT_INPUT_H

/**
 * @file
 * @brief Working with input devices.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <asio.hpp>
#include <egt/geometry.h>
#include <egt/object.h>
#include <linux/input.h>
#include <memory>
#include <string>

namespace egt
{

    /**
     * Global mouse position.
     *
     * Call this to retrieve the last mouse position, usually in response to a
     * mouse event.
     */
    Point& event_mouse();

    /**
     * Global button value.
     *
     * BTN_LEFT, BTN_RIGHT, BTN_MIDDLE
     */
    int& event_button();

    /**
     * Global key value.
     *
     * Call this to retrieve the last key value, usually in response to a key
     * event.
     */
    int& event_key();

    /**
     * Global key code.
     */
    int& event_code();

    namespace detail
    {
        /**
         * Abstract input class.
         */
        class IInput
        {
        public:
            /**
             * Dispatch the event globally.
             */
            static void dispatch(eventid event);

            static detail::Object& global_input()
            {
                return m_global_input;
            }

        protected:
            static detail::Object m_global_input;
        };

    }

    /**
     * Handles reading input events from evdev devices.
     */
    class InputEvDev : public detail::IInput
    {
    public:

        explicit InputEvDev(const std::string& path);

        virtual ~InputEvDev();

    private:
        void handle_read(const asio::error_code& error, std::size_t length);

        asio::posix::stream_descriptor m_input;
        std::vector<char> m_input_buf;
    };

#ifdef HAVE_TSLIB
    namespace detail
    {
        struct tslibimpl;
    }

    /**
     * Handles reading input from a tslib supported device.
     */
    class InputTslib : public detail::IInput
    {
    public:

        explicit InputTslib(const std::string& path);

        virtual ~InputTslib();

    private:

        void handle_read(const asio::error_code& error);

        asio::posix::stream_descriptor m_input;
        std::unique_ptr<detail::tslibimpl> m_impl;
        bool m_active{false};
    };
#endif

}

#endif
