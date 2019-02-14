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

#include <egt/window.h>

namespace egt
{
inline namespace v1
{

/**
 * Popup window.
 */
template <class T>
class PopupType : public T
{
public:
    explicit PopupType(const Size& size = Size(),
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

    virtual void show_modal(bool center = false)
    {
        if (!modal_window())
        {
            modal_window() = this;
            this->show(center);
        }
    }

    virtual void hide() override
    {
        T::hide();

        if (modal_window() == this)
        {
            modal_window() = nullptr;
        }
    }

    virtual ~PopupType()
    {}

};

using Popup = PopupType<Window>;

}
}

#endif
