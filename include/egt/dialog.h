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
 * A dialog does not fill the screen and is normally used for modal
 * events that require users to take an action before they can proceed
 * further.
 *
 * A Basic dialog window contains three regions:
 *
 * Title: used for showing title info of dialog.
 * Content area: The display message or other widget in dialog.
 * buttons: two buttons to make a decision.
 *
 */
class Dialog : public Popup
{
public:
    enum class buttonid
    {
        button1,
        button2,
    };

    /**
     * Create a dialog window with two buttons to handle decision.
     *
     * @param[in] rect is a size of dialog window.
     */
    explicit Dialog(const Rect& rect = {});

    /**
     * Set the title info for this dialog window.
     *
     * @param[in] icon set icon in a title area of a dialog window.
     * @param[in] title set title message in a title area of a dialog window.
     */
    virtual void set_title(const Image& icon, const std::string& title);

    /**
     * Set the title text for this dialog window.
     */
    virtual void set_title(const std::string& title);

    /**
     * Set the icon in a title area of a dialog window.
     */
    virtual void set_icon(const Image& icon);

    /**
     * Set the message text for this dialog window.
     *
     * @param[in] message to display in content area of a dialog window.
     */
    virtual void set_message(const std::string& message);

    /**
     * Set Button details for this dialog window.
     *
     * @param[in] button is buttonid
     * @param[in] text is a text to display on button.
     */
    virtual void set_button(buttonid button, const std::string& text);

    /**
     * Set a custom widget for this dialog window.
     *
     * @param[in] widget to add in content area of a dialog window
     */
    virtual void set_widget(const std::shared_ptr<Widget>& widget);

    virtual void show() override;

    virtual void show_centered() override;

    virtual ~Dialog() = default;

protected:
    std::shared_ptr<BoxSizer> m_vsizer;
    std::shared_ptr<StaticGrid> m_grid;
    std::shared_ptr<ImageLabel> m_title;
    std::shared_ptr<TextBox> m_message;
    std::shared_ptr<Button> m_button1;
    std::shared_ptr<Button> m_button2;

};

}

}

#endif
