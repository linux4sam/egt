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
class Frame;

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

    /**
     * @param[in] text The text to display.
     * @param[in] rect Rectangle for the widget.
     */
    explicit CheckBox(const std::string& text = {},
                      const Rect& rect = {}) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] text The text to display.
     * @param[in] rect Rectangle for the widget.
     */
    explicit CheckBox(Frame& parent,
                      const std::string& text = {},
                      const Rect& rect = {}) noexcept;

    virtual void handle(Event& event) override;

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
 *
 * @ingroup controls
 */
class ToggleBox : public CheckBox
{
public:

    /**
     * @param[in] rect Rectangle for the widget.
     */
    explicit ToggleBox(const Rect& rect = {}) noexcept;

    /**
    * @param[in] parent The parent Frame.
    * @param[in] rect Rectangle for the widget.
    */
    explicit ToggleBox(Frame& parent, const Rect& rect = {}) noexcept;

    virtual void draw(Painter& painter, const Rect& rect) override;

    /**
     * Default draw method for the CheckBox.
     */
    static void default_draw(ToggleBox& widget, Painter& painter, const Rect& rect);

    virtual Size min_size_hint() const override;

    /**
     * Set the optional "on" and "off" text.
     *
     * @param[in] off_text The "off" text to display.
     * @param[in] on_text The "on" text to display.
     */
    virtual void set_toggle_text(const std::string& off_text,
                                 const std::string& on_text)
    {
        bool off = detail::change_if_diff<>(m_off_text, off_text);
        bool on = detail::change_if_diff<>(m_on_text, on_text);

        if (on || off)
            damage();
    }

    /**
     * Get the "on" text.
     */
    inline const std::string& on_text() const { return m_on_text; }

    /**
     * Get the "off" text.
     */
    inline const std::string& off_text() const { return m_off_text; }

    virtual ~ToggleBox() = default;

protected:

    virtual void set_text(const std::string& /*str*/) override {}

    /**
     * Optional "off" text.
     */
    std::string m_off_text;

    /**
     * Optional "on" text.
     */
    std::string m_on_text;
};

}
}

#endif
