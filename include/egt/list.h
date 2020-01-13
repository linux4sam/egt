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

#include <egt/detail/meta.h>
#include <egt/frame.h>
#include <egt/label.h>
#include <egt/sizer.h>
#include <egt/view.h>
#include <memory>
#include <string>

namespace egt
{
inline namespace v1
{

/**
 * ListBox string helper.
 *
 * Light wrapper around a list item to make each item in the ListBox a little
 * bit bigger.
 */
struct EGT_API StringItem : public ImageLabel
{
    StringItem(const std::string& text = {},
               const Image& image = {},
               const AlignFlags& align = AlignFlag::center) noexcept
        : ImageLabel(image, text, align)
    {
        boxtype(Theme::BoxFlag::fill);
    }

    StringItem(const std::string& text,
               const Image& image,
               const Rect& rect,
               const AlignFlags& align = AlignFlag::center) noexcept
        : ImageLabel(image, text, rect, align)
    {
        boxtype(Theme::BoxFlag::fill);
    }

    StringItem(const std::string& text,
               const Rect& rect,
               const AlignFlags& align = AlignFlag::center) noexcept
        : ImageLabel(Image(), text, rect, align)
    {
        boxtype(Theme::BoxFlag::fill);
    }

    explicit StringItem(Frame& parent,
                        const std::string& text = {},
                        const AlignFlags& align = AlignFlag::center) noexcept
        : ImageLabel(parent, Image(), text, align)
    {
        boxtype(Theme::BoxFlag::fill);
    }

    StringItem(Frame& parent,
               const std::string& text,
               const Rect& rect,
               const AlignFlags& align = AlignFlag::center) noexcept
        : ImageLabel(parent, Image(), text, rect, align)
    {
        boxtype(Theme::BoxFlag::fill);
    }

    virtual Size min_size_hint() const override
    {
        return {100, 40};
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
 *
 * @note This interface only supports a vertical Orientation.
 */
class EGT_API ListBox : public Frame
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
     * @param[in] rect Rectangle for the widget.
     */
    explicit ListBox(Frame& parent, const item_array& items = {}, const Rect& rect = {}) noexcept;

    virtual void handle(Event& event) override;

    virtual void resize(const Size& s) override
    {
        if (s != size())
        {
            Frame::resize(s);
            if (m_view)
            {
                auto carea = content_area();
                if (!carea.empty())
                {
                    m_view->box(to_child(carea));
                    m_sizer->resize(carea.size());
                }
            }
        }
    }

    /**
     * Select an item by index.
     */
    virtual void selected(size_t index);

    /**
     * Get the currently selected index.
     */
    inline ssize_t selected() const { return m_selected; }

    /**
     * Return the number of items in the list.
     */
    virtual size_t item_count() const { return m_sizer->count_children(); }

    /**
     * Add a new item to the end of the list.
     */
    virtual void add_item(const std::shared_ptr<Widget>& widget);

    /**
     * Get the currently selected index item from list.
     */
    virtual std::shared_ptr<Widget> item_at(size_t index) const;

    /**
     * Remove an item from the list.
     */
    virtual void remove_item(Widget* widget);

    /**
     * Remove all items from the list.
     */
    virtual void clear();

    virtual ~ListBox() = default;

protected:

    /**
     * Currently selected index.
     */
    ssize_t m_selected{-1};

    /**
     * View used to contain the possible large sizer.
     */
    std::shared_ptr<ScrolledView> m_view;

    /**
     * Internal sizer used to layout items.
     */
    std::shared_ptr<BoxSizer> m_sizer;

private:

    void add_item_private(const std::shared_ptr<Widget>& widget);
};

}
}

#endif
