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
#include <egt/frame.h>
#include <egt/signal.h>
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
        fill_flags(Theme::FillFlag::solid);
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

    /**
     * Event signal.
     * @{
     */
    /**
     * Invoked when a notebook tab changes.
     */
    Signal<> on_selected_changed;
    /** @} */

    /**
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit Notebook(const Rect& rect = {}) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit Notebook(Frame& parent, const Rect& rect = {}) noexcept;

    EGT_OPS_NOCOPY_MOVE(Notebook);
    ~Notebook() noexcept override = default;

    using Frame::add;

    void add(std::shared_ptr<Widget> widget) override;

    void remove(Widget* widget) override;

    /**
     * Set the selected widget by index.
     */
    void selected(size_t index);

    /**
     * Set the selected widget by widget.
     */
    void selected(Widget* widget);

    /**
     * Get the currently selected index.
     */
    ssize_t selected() const { return m_selected; }

    /**
     * Get a widget at the specified index.
     *
     * @param index The index of the widget.
     */
    NotebookTab* get(size_t index) const;

protected:

    /// Type of array of notebook tabs.
    using CellArray = std::vector<std::weak_ptr<NotebookTab>>;

    /// Array of notebook tabs.
    CellArray m_cells;

    /// Currently selected index.
    ssize_t m_selected{-1};
};

}
}

#endif
