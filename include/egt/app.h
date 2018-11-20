/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_APP_H
#define EGT_APP_H

#include <string>
#include <egt/eventloop.h>
#include <egt/utils.h>

/**
 * @file
 * @brief Working with app.
 */

namespace egt
{

    /**
     * Defines an application.
     *
     * This is basically just a helper class that does standard setup for
     * inputs, outputs, the event loop, and more. It also acts as sort of a
     * global accessor to get to these things because of this.
     */
    class Application : public detail::noncopyable
    {
    public:
        /**
         * @param primary Is this the primary display application.
         * @param name Application name.  This is used for several things,
         *             including identifying the text domain used by gettext().
         */
        Application(int argc = 0, const char** argv = nullptr,
                    bool primary = true, const std::string& name = "egt");

        virtual int run();

        inline EventLoop& event() { return m_event; }

        virtual ~Application();

    protected:
        EventLoop m_event;
        int m_argc{0};
        const char** m_argv{nullptr};
    };

    /**
     * Reference to the main Application instance.
     */
    Application& main_app();

}

#endif
