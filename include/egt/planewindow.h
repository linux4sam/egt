/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_PLANEWINDOW_H
#define EGT_PLANEWINDOW_H

/**
 * @file
 * @brief Working with plane windows.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <egt/window.h>

namespace egt
{
    /**
     * A PlaneWindow uses a hardware overlay as a screen.
     *
     * PlaneWindow seperates "changing attributes" and "applying attributes".
     * This maps to the libplanes plane_apply() function. Which, is the same way
     * event handle() vs. draw() works in this toolkit.
     */
    class PlaneWindow : public Window
    {
    public:

        constexpr static const auto DEFAULT_FORMAT = pixel_format::argb8888;

        explicit PlaneWindow(const Size& size = Size(),
                             widgetmask flags = widgetmask::WINDOW_DEFAULT,
                             pixel_format format = DEFAULT_FORMAT,
                             bool heo = false);

        explicit PlaneWindow(const Rect& rect,
                             widgetmask flags = widgetmask::WINDOW_DEFAULT,
                             pixel_format format = DEFAULT_FORMAT,
                             bool heo = false);

        virtual void damage(const Rect& rect) override;

        virtual void move(const Point& point) override;

        virtual void top_draw() override;

        virtual void show() override;

        virtual void hide() override;

        virtual void paint(Painter& painter) override;

        virtual ~PlaneWindow();

    protected:

        void allocate_screen();
        void do_resize(const Size& size);

        pixel_format m_format{DEFAULT_FORMAT};
        bool m_dirty{true};
        bool m_heo{false};
    };

}

#endif
