/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DIALOG_H
#define EGT_DIALOG_H

#include <egt/button.h>
#include <egt/grid.h>
#include <egt/input.h>
#include <egt/label.h>
#include <egt/popup.h>
#include <egt/sizer.h>
#include <egt/text.h>
#include <functional>
#include <memory>
#include <string>

namespace egt
{
inline namespace v1
{

/**
 * A dialog is a widget that allows user to make a decision.
 *
 * A dialog does not fill the screen and is normally used for modal
 * events that require users to take an action before they can proceed
 * further.
 *
 * A basic dialog contains three regions:
 *
 * Title: Title of a dialog.
 * Content: a message text or an widget.
 * Buttons: two buttons to make a decision.
 */
class Dialog : public Popup
{
public:
    /**
     * buttonid used for dialog button properties.
     */
    enum class buttonid
    {
        button1,
        button2,
    };

    /**
     * Create a dialog with two buttons to handle decision.
     *
     * @param[in] rect is a size of dialog.
     */
    explicit Dialog(const Rect& rect = {}) noexcept;

    /**
     * Set the title info for this dialog.
     *
     * @param[in] icon set the icon in a title.
     * @param[in] title set the title text.
     */
    virtual void set_title(const Image& icon, const std::string& title);

    /**
     * Set a title text for this dialog.
     */
    virtual void set_title(const std::string& title);

    /**
     * Set a icon in a title area of a dialog.
     */
    virtual void set_icon(const Image& icon);

    /**
     * Set a text message for this dialog.
     *
     * @param[in] message is a dialog text message.
     */
    virtual void set_message(const std::string& message);

    /**
     * Set Button details for this dialog.
     *
     * @param[in] button is buttonid
     * @param[in] text is a text to display on button.
     */
    virtual void set_button(buttonid button, const std::string& text);

    /**
     * Set a widget for this dialog.
     *
     * @param[in] widget
     */
    virtual void set_widget(const std::shared_ptr<Widget>& widget);

    virtual void show() override;

    virtual void show_centered() override;

    virtual ~Dialog() = default;

protected:
    /**
     * Vertical BoxSizer
     */
    std::shared_ptr<BoxSizer> m_vsizer;

    /**
     * grid for organizing okay & cancel Buttons.
     */
    std::shared_ptr<StaticGrid> m_grid;

    /**
     * m_title is for dialog title.
     */
    std::shared_ptr<ImageLabel> m_title;

    /**
     * m_message is for dialog message.
     */
    std::shared_ptr<TextBox> m_message;

    /**
     * m_button1 Button for okay or cancel.
     */
    std::shared_ptr<Button> m_button1;

    /**
     * m_button2 Button for okay or cancel.
     */
    std::shared_ptr<Button> m_button2;
};

}

}

#endif
