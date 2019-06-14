/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_LIST_H
#define EGT_LIST_H

/**
 * @file
 * @brief ListBox definition.
 */

#include <egt/frame.h>
#include <egt/label.h>
#include <egt/sizer.h>
#include <egt/view.h>
#include <memory>
#include <string>
#include <vector>

namespace egt
{
inline namespace v1
{

/**
 * ListBox string helper.
 *
 * Light wrapper around a Label to make each item in the ListBox a little bit
 * bigger.
 */
struct StringItem : public ImageLabel
{
    StringItem(const std::string& text = {},
               const Image& image = {},
               alignmask align = alignmask::center) noexcept
        : ImageLabel(image, text, align)
    {
        set_boxtype(Theme::boxtype::blank);
    }

    StringItem(const std::string& text,
               const Image& image,
               const Rect& rect,
               alignmask align = alignmask::center) noexcept
        : ImageLabel(image, text, rect, align)
    {
        set_boxtype(Theme::boxtype::blank);
    }

    StringItem(const std::string& text,
               const Rect& rect,
               alignmask align = alignmask::center) noexcept
        : ImageLabel(Image(), text, rect, align)
    {
        set_boxtype(Theme::boxtype::blank);
    }

    explicit StringItem(Frame& parent,
                        const std::string& text = {},
                        alignmask align = alignmask::center) noexcept
        : ImageLabel(parent, Image(), text, align)
    {
        set_boxtype(Theme::boxtype::blank);
    }

    StringItem(Frame& parent,
               const std::string& text,
               const Rect& rect,
               alignmask align = alignmask::center) noexcept
        : ImageLabel(parent, Image(), text, rect, align)
    {
        set_boxtype(Theme::boxtype::blank);
    }

    virtual Size min_size_hint() const override
    {
        return Size(100, 40);
    }

};

/**
 * ListBox that manages a select-able list of widgets.
 *
 * Only one item may be selected at a time. The items are based on Widget, so
 * any Widget can be used.
 *
 * @image html widget_listbox.png
 * @image latex widget_listbox.png "widget_listbox" width=5cm
 *
 * @ingroup controls
 */
class ListBox : public Frame
{
public:

    using item_array = std::vector<std::shared_ptr<Widget>>;

    /**
     * @param[in] items Array of items to insert into the list.
     */
    explicit ListBox(const item_array& items = item_array()) noexcept;

    /**
     * @param[in] rect Rectangle for the widget.
     */
    explicit ListBox(const Rect& rect) noexcept;

    /**
     * @param[in] items Array of items to insert into the list.
     * @param[in] rect Rectangle for the widget.
     */
    ListBox(const item_array& items, const Rect& rect) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] items Array of items to insert into the list.
     */
    explicit ListBox(Frame& parent, const item_array& items = item_array()) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] items Array of items to insert into the list.
     * @param[in] rect Rectangle for the widget.
     */
    ListBox(Frame& parent, const item_array& items, const Rect& rect) noexcept;

    virtual void handle(Event& event) override;

    /**
     * Select an item by index.
     */
    virtual void set_select(uint32_t index);

    /**
     * Return the number of items in the list.
     */
    virtual size_t item_count() const { return m_sizer->count_children(); }

    /**
     * Get the currently selected index.
     */
    virtual uint32_t selected() const { return m_selected; }

    /**
     * Add a new item to the end of the list.
     */
    virtual void add_item(const std::shared_ptr<Widget>& widget);

    /**
     * Get the currently selected index item from list.
     */
    virtual Widget* get_item(uint32_t index);

    /**
     * Remove an item from the list.
     */
    virtual void remove_item(Widget* widget);

    /**
     * Remove all items from the list.
     */
    virtual void clear();

    virtual void layout() override
    {
        if (m_sizer)
            m_sizer->layout();
    }

    virtual void resize(const Size& s) override
    {
        if (s != size())
        {
            Frame::resize(s);
            if (m_sizer)
            {
                auto carea = content_area();
                if (!carea.empty())
                    m_sizer->set_box(to_child(carea));
            }
        }
    }

    virtual ~ListBox() = default;

protected:

    uint32_t m_selected{0};
    std::shared_ptr<BoxSizer> m_sizer;

private:

    void add_item_private(const std::shared_ptr<Widget>& widget);
};

}
}

#endif
