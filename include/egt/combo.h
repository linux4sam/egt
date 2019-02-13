/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_COMBO_H
#define EGT_COMBO_H

/**
 * @file
 * @brief ComboBox definition.
 */

#include <egt/list.h>
#include <egt/widget.h>
#include <egt/popup.h>
#include <vector>
#include <string>

namespace egt
{
inline namespace v1
{
class ComboBox;

namespace detail
{

class ComboBoxPopup : public Popup
{
public:
    explicit ComboBoxPopup(ComboBox& parent);

    virtual int handle(eventid event) override;

    virtual void show(bool center = false) override;

    virtual ~ComboBoxPopup()
    {}

protected:

    void smart_pos();

    ListBox m_list;
    ComboBox& m_parent;

    friend class egt::ComboBox;
};

}


/**
 * Combo box widget.
 */
class ComboBox : public TextWidget
{
public:
    using item_array = std::vector<std::string>;

    ComboBox(const item_array& items = item_array(),
             const Rect& rect = Rect(),
             alignmask align = alignmask::CENTER | alignmask::LEFT,
             const Font& font = Font(),
             const widgetflags& flags = widgetflags()) noexcept;

    virtual int handle(eventid event) override;

    virtual void set_select(uint32_t index);

    virtual uint32_t selected() const { return m_selected; }

    virtual void resize(const Size& s) override;

    virtual void move(const Point& point) override;

    virtual void draw(Painter& painter, const Rect& rect) override;

    virtual void set_parent(Frame* parent) override;

    virtual ~ComboBox();

protected:
    item_array m_items;
    uint32_t m_selected{0};
    detail::ComboBoxPopup m_popup;

    friend class detail::ComboBoxPopup;
};

}
}

#endif
