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
#include <vector>

namespace egt
{
inline namespace v1
{

/**
 * @brief A single layer of a Notebook.
 */
class NotebookTab : public Frame
{
public:

    NotebookTab()
    {
        set_name("NotebookTab" + std::to_string(m_widgetid));
        set_boxtype(Theme::boxtype::fillsolid);
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

    virtual ~NotebookTab()
    {}
};

/**
 * @brief Allows a collection of NotebookTab widgets to be shown one at a time.
 */
class Notebook : public Frame
{
public:
    explicit Notebook(const Rect& rect = Rect());

    virtual NotebookTab* add(NotebookTab* widget);

    virtual void remove(Widget* widget) override;

    virtual void set_select(uint32_t index);

    virtual ~Notebook();

protected:

    struct Cell
    {
        // cppcheck-suppress unusedStructMember
        NotebookTab* widget{nullptr};
        std::string name;
    };

    using cell_array = std::vector<Cell>;

    cell_array m_cells;

    int m_current_index{-1};

private:
    virtual Widget* add(Widget* widget) override
    {
        ignoreparam(widget);
        return nullptr;
    }

};

}
}

#endif
