/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_NOTEBOOK_H
#define EGT_NOTEBOOK_H

/**
 * @file
 * @brief Working with notebooks.
 */

#include <egt/detail/meta.h>
#include <egt/detail/signal.h>
#include <egt/frame.h>
#include <memory>
#include <string>
#include <vector>

namespace egt
{
inline namespace v1
{

/**
 * A single layer of a Notebook.
 */
class EGT_API NotebookTab : public Frame
{
public:

    NotebookTab()
    {
        name("NotebookTab" + std::to_string(m_widgetid));

        // tabs are not transparent by default
        boxtype({Theme::BoxFlag::fill, Theme::BoxFlag::solid});
    }

    /**
     * @return true if allowed to leave, false otherwise.
     */
    virtual bool leave()
    {
        return true;
    }

    /**
     * Called when this tab is entered and before show() is invoked.
     */
    virtual void enter()
    {
    }

    /**
     * Select this tab.
     */
    void select();
};

/**
 * Allows a collection of NotebookTab widgets to be shown one at a time.
 *
 * @ingroup controls
 */
class EGT_API Notebook : public Frame
{
public:

    detail::Signal<> on_selected_changed;

    /*
     * @param[in] rect Initial rectangle of the Frame.
     */
    explicit Notebook(const Rect& rect = {}) noexcept;

    /*
     * @param[in] parent The parent Frame.
     * @param[in] rect Initial rectangle of the Frame.
     */
    explicit Notebook(Frame& parent, const Rect& rect = {}) noexcept;

    using Frame::add;

    virtual void add(std::shared_ptr<Widget> widget) override;

    virtual void remove(Widget* widget) override;

    /**
     * Set the selected widget by index.
     */
    virtual void selected(size_t index);

    /**
     * Set the selected widget by widget.
     */
    virtual void selected(Widget* widget);

    /**
     * Get the currently selected index.
     */
    inline ssize_t selected() const { return m_selected; }

    /**
     * Get a widget at he specified index.
     *
     * @param index The index of the widget.
     */
    virtual NotebookTab* get(size_t index) const;

    virtual ~Notebook() = default;

protected:

    using cell_array = std::vector<std::weak_ptr<NotebookTab>>;

    cell_array m_cells;

    /**
     * Currently selected index.
     */
    ssize_t m_selected{-1};
};

}
}

#endif
