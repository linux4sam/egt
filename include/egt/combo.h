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
#include <egt/popup.h>
#include <egt/widget.h>
#include <memory>
#include <string>
#include <vector>

namespace egt
{
inline namespace v1
{
class ComboBox;

namespace detail
{

/**
 * @brief Popup class used by ComboBox.
 */
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

    std::shared_ptr<ListBox> m_list;
    ComboBox& m_parent;

    friend class egt::ComboBox;
};

}

/**
 * @brief Combo box widget.
 *
 * This manages a list of selectable items, but otherwise just shows only what
 * is selected.
 *
 * @ingroup controls
 */
class ComboBox : public TextWidget
{
public:
    using item_array = std::vector<std::string>;

    /**
     * @param[in] items Array of items to insert into the list.
     */
    explicit ComboBox(const item_array& items = item_array()) noexcept;

    /**
     * @param[in] rect Rectangle for the widget.
     */
    explicit ComboBox(const Rect& rect) noexcept;

    /**
     * @param[in] items Array of items to insert into the list.
     * @param[in] rect Rectangle for the widget.
     */
    ComboBox(const item_array& items, const Rect& rect) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] items Array of items to insert into the list.
     */
    explicit ComboBox(Frame& parent, const item_array& items = item_array()) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] items Array of items to insert into the list.
     * @param[in] rect Rectangle for the widget.
     */
    ComboBox(Frame& parent, const item_array& items, const Rect& rect) noexcept;

    virtual int handle(eventid event) override;

    virtual std::unique_ptr<Widget> clone() override
    {
        return std::unique_ptr<Widget>(make_unique<ComboBox>(*this).release());
    }

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
    std::shared_ptr<detail::ComboBoxPopup> m_popup;

    friend class detail::ComboBoxPopup;
};

}
}

#endif
