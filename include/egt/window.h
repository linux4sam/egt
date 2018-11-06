/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef EGT_WINDOW_H
#define EGT_WINDOW_H

/**
 * @file
 * @brief Working with windows.
 */

#include <config.h>
#include <egt/frame.h>
#include <vector>
#include <list>
#ifdef HAVE_LIBPLANES
#include <drm_fourcc.h>
#define DEFAULT_FORMAT DRM_FORMAT_ARGB8888
#else
#define DEFAULT_FORMAT 0
#endif

namespace egt
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

        virtual void show() override;

        // why can't i see this from Widget::size()?
        virtual Size size() const override
        {
            return m_box.size();
        }

        virtual ~Window()
        {}

    protected:

        virtual IScreen* screen() override
        {
            assert(m_screen);
            return m_screen;
        }

        IScreen* m_screen;

    private:

        // unsupported
        virtual void resize(const Size& size) override
        {
            ignoreparam(size);
        }
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

        virtual void damage() override
        {
            damage(m_box);
        }

        virtual void damage(const Rect& rect) override;

        virtual void resize(const Size& size) override;

        virtual void move(const Point& point) override;

        virtual void draw() override;

        virtual void show() override;

        virtual void hide() override;

        virtual ~PlaneWindow();

    protected:
        uint32_t m_format{DEFAULT_FORMAT};
        bool m_dirty{true};
        bool m_heo{false};

    private:
        void do_resize(const Size& size);
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
                if (T::parent())
                    this->move_to_center(T::parent()->box().center());
                else
                    this->move_to_center(main_screen()->box().center());
            }

            T::show();
        }
    };

    Window*& main_window();
    std::vector<Window*>& windows();
}

#endif
