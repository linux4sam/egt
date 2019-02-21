/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_GRID_H
#define EGT_GRID_H

/**
 * @file
 * @brief Working with grids.
 */

#include <egt/frame.h>
#include <egt/utils.h>
#include <egt/widget.h>
#include <vector>

namespace egt
{
inline namespace v1
{

/**
 * Static grid organization for widgets.
 *
 * A static grid that does not perform any drawing, but controls the
 * size and position of any widget added into a grid.
 */
class StaticGrid : public Frame
{
public:
    StaticGrid(const Rect& rect,
               int columns = 1, int rows = 1, int spacing = 0);

    StaticGrid(int columns = 1, int rows = 1, int spacing = 0);

    virtual void move(const Point& point) override
    {
        Frame::move(point);
        reposition();
    }

    virtual void resize(const Size& size) override
    {
        Frame::resize(size);
        reposition();
    }

    virtual void draw(Painter& painter, const Rect& rect) override;

    /**
     * Add a widget to the grid into a specific cell with an optional
     * alignment within the cell.
     *
     * This will automatically resize the grid to fit the widget as
     * necessary.
     *
     * @param widget The widget to add, or nullptr.
     * @param column The column index.
     * @param row The row index.
     * @param align Align the widget in the cell by calling set_align() on
     *              the widget with this value.
     */
    virtual Widget* add(Widget* widget, int column, int row,
                        alignmask align = alignmask::expand);

    /**
     * Add a widget to the next empty cell.
     *
     * This will not automatically resize the grid to fit the widget as
     * necessary.
     *
     * @param widget The widget to add, or nullptr.
     * @param align Align the widget in the cell by calling set_align() on
     *              the widget with this value.
     */
    virtual Widget* add(Widget* widget,
                        alignmask align = alignmask::expand);

    /**
     * Get a widget at he specified row and column.
     *
     * @note Point here is used as a column and row.
     *
     * @param point The row and column to get.
     */
    virtual Widget* get(const Point& point);

    virtual void remove(Widget* widget) override;

    /**
     * Reposition all child widgets.
     *
     * @note You should *not* have to manually call this under normal
     * circumstances.
     */
    virtual void reposition();

    int last_add_column() const
    {
        return m_last_add_column;
    }

    int last_add_row() const
    {
        return m_last_add_row;
    }

    virtual ~StaticGrid();

protected:

    struct Cell
    {
        // cppcheck-suppress unusedStructMember
        Widget* widget{nullptr};
    };

    using cell_array = std::vector<std::vector<Cell>>;

    cell_array m_cells;
    int m_spacing{0};

    int m_last_add_column{0};
    int m_last_add_row{0};
};

/**
 * A StaticGrid where each item is visually selectable with a highlighted
 * border.
 */
class SelectableGrid : public StaticGrid
{
public:
    SelectableGrid(const Rect& rect = Rect(),
                   int columns = 1, int rows = 1, int spacing = 0)
        : StaticGrid(rect, columns, rows, spacing)
    {}

    virtual void draw(Painter& painter, const Rect& rect) override;

    virtual Point selected() const
    {
        return Point(m_selected_column, m_selected_row);
    }

    virtual void select(int column, int row)
    {
        m_selected_column = column;
        m_selected_row = row;
        damage();
    }

    virtual ~SelectableGrid()
    {}

protected:
    int m_selected_column{0};
    int m_selected_row{0};
};

}
}

#endif
