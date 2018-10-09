/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */

#ifndef MUI_INPUT_H
#define MUI_INPUT_H

/**
 * @file
 * @brief Working with input devices.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <linux/input.h>
#include <mui/geometry.h>
#include <string>

namespace mui
{

    /**
     * Event identifiers.
     */
    enum
    {
        EVT_NONE = 0,

        EVT_MOUSE_DOWN = 1,
        EVT_MOUSE_UP,
        EVT_MOUSE_MOVE,
        EVT_MOUSE_DBLCLICK,

        /**
         * Sent when a widget gets focus.
         */
        EVT_ENTER,

        /**
         * Sent when a widget loses focus.
         */
        EVT_LEAVE,
        EVT_KEY_DOWN,
        EVT_KEY_UP,
        EVT_KEY_REPEAT,
    };

    /**
     * Global mouse position.
     *
     * Call this to retrieve the last mouse position, usually in response to a
     * mouse event.
     */
    Point& mouse_position();

    /**
     * Global key value.
     *
     * Call this to retrieve the last key value, usually in response to a key
     * event.
     */
    int& key_value();

    /**
     * Base input class.
     */
    class IInput
    {
    protected:
        static void dispatch(int event);
    };

    class InputEvDev : public IInput
    {
    public:

        InputEvDev(const std::string& path);

        virtual ~InputEvDev();

    private:
        static void process(int fd, uint32_t mask, void* data);

        int m_fd;
    };

#ifdef HAVE_TSLIB
    class InputTslib : public IInput
    {
    public:

        InputTslib(const std::string& path);

        virtual ~InputTslib();

    private:

        static void process(int fd, uint32_t mask, void* data);
        static void timer_callback(int fd, void* data);

        int m_fd;
        bool m_active;
    };
#endif

}

#endif
