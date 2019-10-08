/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_POPUP_H
#define EGT_POPUP_H

/**
 * @file
 * @brief Working with popup windows.
 */

#include <egt/app.h>
#include <egt/window.h>

namespace egt
{
inline namespace v1
{

/**
 * Popup window.
 *
 * This is a utility class that manages a popup window with extra features like
 * automatic centering and modal mode.
 */
template <class T>
class PopupType : public T
{
public:

    /**
     * @param size The size of the Window.
     * @param point The origin point of the window.
     */
    explicit PopupType(const Size& size = {},
                       const Point& point = {})
        : T(size)
    {
        this->move(point);
    }

    /**
     * Show the window centered.
     *
     * Position the window the center of its parent, or if it has no parent,
     * the screen.
     */
    virtual void show_centered()
    {
        if (T::parent())
            this->move_to_center(T::parent()->box().center());
        else
            this->move_to_center(Application::instance().screen()->box().center());

        T::show();
    }

    /**
     * Show the window in modal mode.
     *
     * This means it will explicitly steal all input events as long as it is
     * visible.
     */
    virtual void show_modal(bool center = false)
    {
        if (!modal_window())
        {
            detail::set_modal_window(this);

            if (center)
                this->show_centered();
            else
                this->show();
        }
    }

    virtual void hide() override
    {
        T::hide();

        if (modal_window() == this)
        {
            detail::set_modal_window(nullptr);
        }
    }

    virtual ~PopupType() = default;

};

/**
 * Helper type for a default Popup.
 */
using Popup = PopupType<Window>;

}
}

#endif
