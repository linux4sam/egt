/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DIALOG_H
#define EGT_DIALOG_H

#include <egt/button.h>
#include <egt/detail/meta.h>
#include <egt/grid.h>
#include <egt/label.h>
#include <egt/popup.h>
#include <egt/signal.h>
#include <egt/sizer.h>
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
class EGT_API Dialog : public Popup
{
public:

    /**
     * Used to identify the button on the dialog.
     */
    enum class ButtonId
    {
        button1,
        button2,
    };

    /**
     * Event signal.
     * @{
     */
    /// Signal when button1 is clicked.
    Signal<> on_button1_click;
    /// Signal when button2 is clicked.
    Signal<> on_button2_click;
    /** @} */

    /**
     * Create a dialog with two buttons to handle decision.
     *
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit Dialog(const Rect& rect = {}) noexcept;

    /**
     * @param[in] props list of widget argument and its properties.
     */
    explicit Dialog(Serializer::Properties& props) noexcept
        : Dialog(props, false)
    {
    }

protected:

    explicit Dialog(Serializer::Properties& props, bool is_derived) noexcept;

public:

    /**
     * Set the title info for this dialog.
     *
     * @param[in] icon set the icon in a title.
     * @param[in] title set the title text.
     */
    void title(const Image& icon, const std::string& title);

    /**
     * Set a title text for this dialog.
     */
    void title(const std::string& title);

    /**
     * Set a icon in a title area of a dialog.
     */
    void icon(const Image& icon);

    /**
     * Set Button details for this dialog.
     *
     * @param[in] button is buttonid
     * @param[in] text is a text to display on button.
     */
    void button(ButtonId button, const std::string& text);

    /**
     * Set a widget for this dialog.
     *
     * @param[in] widget
     */
    void widget(const std::shared_ptr<Widget>& widget);

    /**
     * Get a minimum size hint of the Dialog Widget.
     */
    Size min_size_hint() const override;

    /**
     * Perform layout on dialog Widget.
     */
    void layout() override;

    /**
     * Resize the Dialog Widget.
     *
     * Changes the width and height of the Widget.
     *
     * @param[in] size The new size of the Widget.
     */
    void resize(const Size& size) override;

protected:

    /// Layout sizer.
    VerticalBoxSizer m_layout;

    /// Content sizer.
    VerticalBoxSizer m_content;

    /// Dialog title.
    ImageLabel m_title;

    /// Grid for organizing okay & cancel Buttons.
    std::shared_ptr<StaticGrid> m_grid;

    /// Button for okay or cancel.
    Button m_button1;

    /// Button for okay or cancel.
    Button m_button2;

    void initialize();
};

}

}

#endif
