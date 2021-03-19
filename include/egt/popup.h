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
                       const Point& point = {}) noexcept
        : T(size)
    {
        this->move(point);
    }

    explicit PopupType(Serializer::Properties& props) noexcept
        : T(props)
    {}

    /**
     * Show the window centered.
     *
     * Position the window the center of its parent, or if it has no parent,
     * the screen.
     */
    virtual void show_centered()
    {
        this->zorder_top();

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
        if (!Application::instance().modal_window())
        {
            Application::instance().set_modal_window(this);

            this->zorder_top();

            if (center)
                this->show_centered();
            else
                this->show();
        }
    }

    /**
     * Show the Widget.
     *
     * Always move the widget to the top.
     */
    void show() override
    {
        this->zorder_top();

        T::show();
    }

    /**
     * Hide the Widget.
     *
     * A widget that is not visible will receive no draw() calls.
     *
     * This changes the visible() state of the Widget.
     */
    void hide() override
    {
        T::hide();

        if (Application::instance().modal_window() == this)
        {
            Application::instance().set_modal_window(nullptr);
        }
    }
};

/**
 * Helper type alias.
 */
using Popup = PopupType<Window>;

}
}

#endif
