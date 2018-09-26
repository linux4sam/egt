/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_APP_H
#define MUI_APP_H

#include <string>

/**
 * @file
 * @brief Working with app.
 */

namespace mui
{

    /**
     * Defines an application.
     *
     * This is basically just a helper class that does standard setup for
     * inputs, outputs, the event loop, and more. It also acts as sort of a
     * global accessor to get to some things because of this.
     */
    class Application
    {
    public:
        /**
        * @param name Application name.  This is used for several things,
         *             including identifying the text domain used by gettext().
               */
        Application(bool primary = true, const std::string& name = "mui");

        virtual int run();

        virtual ~Application();
    };

}

#endif
