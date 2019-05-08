/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_CHECKBOX_H
#define EGT_CHECKBOX_H

/**
 * @file
 * @brief Working with checkboxes.
 */
#include <egt/button.h>
#include <string>

namespace egt
{
inline namespace v1
{

class Painter;

/**
 * Boolean checkbox.
 *
 * @image html widget_checkbox1.png
 * @image latex widget_checkbox1.png "widget_checkbox1" width=5cm
 * @image html widget_checkbox2.png
 * @image latex widget_checkbox2.png "widget_checkbox2" width=5cm
 *
 * @todo This should be a ValueWidget<bool>.
 *
 * @ingroup controls
 */
class CheckBox : public Button
{
public:
    CheckBox(const std::string& text = std::string(),
             const Rect& rect = Rect());

    virtual int handle(eventid event) override;

    virtual void draw(Painter& painter, const Rect& rect) override;

    /**
     * Default draw method for the CheckBox.
     */
    static void default_draw(CheckBox& widget, Painter& painter, const Rect& rect);

    virtual Size min_size_hint() const override;

    virtual ~CheckBox() = default;
};

/**
 * CheckBox with a boolean slider style interface.
 *
 * @image html widget_slidingcheckbox.png
 * @image latex widget_slidingcheckbox.png "widget_slidingcheckbox" width=5cm
 */
class ToggleBox : public CheckBox
{
public:
    explicit ToggleBox(const Rect& rect = Rect());

    virtual void draw(Painter& painter, const Rect& rect) override;

    /**
     * Default draw method for the CheckBox.
     */
    static void default_draw(ToggleBox& widget, Painter& painter, const Rect& rect);

    virtual Size min_size_hint() const override;

    void set_text(const std::string& on_text,
                  const std::string& off_text)
    {
        bool on = detail::change_if_diff<>(m_on_text, on_text);
        bool off = detail::change_if_diff<>(m_off_text, off_text);

        if (on || off)
            damage();
    }

    inline const std::string& on_text() const { return m_on_text; }
    inline const std::string& off_text() const { return m_off_text; }

    virtual ~ToggleBox() = default;

protected:

    std::string m_off_text;
    std::string m_on_text;
};

}
}

#endif
