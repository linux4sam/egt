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

#include <egt/frame.h>
#include <vector>

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

	Window(const Rect& rect,
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

	/**
	 * The buck stops on this call to Widget::screen() with a Window
	 * because the Window contains the screen.
	 */
        virtual IScreen* screen() override
        {
            assert(m_screen);
            return m_screen;
        }

	virtual ~Window()
        {}

    protected:

        IScreen* m_screen{nullptr};

    private:

        // unsupported
        virtual void resize(const Size& size) override
        {
            ignoreparam(size);
        }
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
