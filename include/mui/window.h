/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_WINDOW_H
#define MUI_WINDOW_H

/**
 * @file
 * @brief Working with windows.
 */

#include <config.h>
#include <mui/frame.h>
#include <vector>
#include <list>
#ifdef HAVE_LIBPLANES
#include <drm_fourcc.h>
#define DEFAULT_FORMAT DRM_FORMAT_ARGB8888
#else
#define DEFAULT_FORMAT 0
#endif

namespace mui
{

    /**
     * A window is a Frame Widget that is backed by a screen.
     *
     * Any widget to be displayed, must be a child of a window.
     */
    class Window : public Frame
    {
    public:
        Window(const Size& size = Size(),
               widgetmask flags = widgetmask::WINDOW_DEFAULT);

        virtual void enter()
        {
            show();
            damage();
        }

        virtual void exit()
        {
            hide();
        }

        // why can't i see this from Widget::size()?
        Size size() const
        {
            return m_box.size();
        }

#if 0
        Rect to_screen(const Rect& rect)
        {
            return Rect(x() + rect.x, y() + rect.y, rect.w, rect.h);
        }

        Rect to_child(const Rect& rect)
        {
            return Rect(rect.x - x(), rect.y - y(), rect.w, rect.h);
        }
#endif

    protected:

        // unsupported
        virtual void resize(const Size& size) { ignoreparam(size); /* Not supported yet. */ }

        virtual IScreen* screen()
        {
            assert(m_screen);
            return m_screen;
        }

        IScreen* m_screen;
    };

    class KMSOverlayScreen;

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

        explicit PlaneWindow(const Size& size = Size(),
                             widgetmask flags = widgetmask::WINDOW_DEFAULT,
                             uint32_t format = DEFAULT_FORMAT,
                             bool heo = false);

        virtual void damage()
        {
            damage(m_box);
        }

        virtual void damage(const Rect& rect);

        virtual void resize(const Size& size);

        virtual void move(const Point& point);

        virtual void draw();

        virtual void show();

        virtual void hide();

        virtual ~PlaneWindow();

    protected:
        uint32_t m_format {DEFAULT_FORMAT};
        bool m_dirty {true};
        bool m_heo {false};
    };

    /**
     * Popup window.
     */
    template <class T>
    class Popup : public T
    {
    public:
        explicit Popup(const Size& size = Size(),
                       const Point& point = Point())
            : T(size)
        {
            this->move(point);
        }

        /**
         * Show the window.
         *
         * @param[in] center Move the window to the center of the screen first.
         */
        virtual void show(bool center = false)
        {
            if (center)
            {
                this->move_to_center(main_screen()->box().center());
            }

            T::show();
        }
    };

    Window*& main_window();
    std::vector<Window*>& windows();
}

#endif
