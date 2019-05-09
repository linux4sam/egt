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
 * Popup class used by ComboBox.
 */
class ComboBoxPopup : public Popup
{
public:
    explicit ComboBoxPopup(ComboBox& parent);

    virtual int handle(eventid event) override;

    virtual void show(bool center = false) override;

    virtual ~ComboBoxPopup() = default;

protected:

    void smart_pos();

    std::shared_ptr<ListBox> m_list;
    ComboBox& m_parent;

    friend class egt::ComboBox;
};

}

/**
 * Combo box widget.
 *
 * This manages a list of select-able items, but otherwise just shows only what
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

    virtual void set_select(size_t index);

    virtual size_t selected() const { return m_selected; }

    virtual std::string item(size_t index) const { return m_items[index]; }

    virtual void resize(const Size& s) override;

    virtual void move(const Point& point) override;

    virtual void draw(Painter& painter, const Rect& rect) override;

    /**
     * Default draw method for the ComboBox.
     */
    static void default_draw(ComboBox& widget, Painter& painter, const Rect& rect);

    inline size_t item_count() const
    {
        return m_items.size();
    }

    virtual void set_parent(Frame* parent) override;

    virtual Size min_size_hint() const override;

    virtual ~ComboBox() = default;

protected:
    item_array m_items;
    size_t m_selected{0};
    std::shared_ptr<detail::ComboBoxPopup> m_popup;

    friend class detail::ComboBoxPopup;
};

}
}

#endif
