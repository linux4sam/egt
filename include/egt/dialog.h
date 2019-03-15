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

class Dialog : public Popup
{
public:

    enum class buttonid
    {
        button1,
        button2,
    };

    explicit Dialog(const Rect& rect);

    void set_title(const Image& icon, const std::string& title);

    void set_title(const std::string& title);

    void set_message(const std::string& message);

    void set_icon(const Image& icon);

    void set_button(buttonid button, const std::string& text);

    void set_widget(const std::shared_ptr<Widget>& widget);

    virtual void show(bool center = false) override;

    virtual ~Dialog() = default;

protected:

    std::shared_ptr<BoxSizer> m_vsizer;

    std::shared_ptr<StaticGrid> m_grid;

    std::shared_ptr<ImageLabel> m_title;

    std::shared_ptr<TextBox> m_message;

    std::shared_ptr<Button> m_button1;

    std::shared_ptr<Button> m_button2;

private:

    Dialog() = delete;

};

}

}

#endif
