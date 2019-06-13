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
class NotebookTab : public Frame
{
public:

    NotebookTab()
    {
        set_name("NotebookTab" + std::to_string(m_widgetid));

        // tabs are not transparent by default
        set_boxtype(Theme::boxtype::blank | Theme::boxtype::solid);
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

    virtual ~NotebookTab() = default;
};

/**
 * Allows a collection of NotebookTab widgets to be shown one at a time.
 *
 * @ingroup controls
 */
class Notebook : public Frame
{
public:
    explicit Notebook(const Rect& rect = {});

    /**
     * @todo This should explicitly only allow NotebookTab widgets.
     */
    virtual void add(const std::shared_ptr<Widget>& widget) override;

    virtual void remove(Widget* widget) override;

    virtual void set_select(uint32_t index);

    virtual ~Notebook() = default;

protected:

    /// @private
    struct Cell
    {
        // cppcheck-suppress unusedStructMember
        std::shared_ptr<NotebookTab> widget;
        std::string name;
    };

    using cell_array = std::vector<Cell>;

    cell_array m_cells;

    int m_current_index{-1};
};

}
}

#endif
