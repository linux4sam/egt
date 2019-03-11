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
 * @brief Static grid organization for widgets.
 *
 * A static grid that does not perform any drawing, but controls the
 * size and position of any widget added into a grid.
 */
class StaticGrid : public Frame
{
public:

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] size Rows and columns.
     * @param[in] spacing Amount of spacing to put between cells.
     */
    StaticGrid(const Rect& rect, const Tuple& size = Tuple(1, 1),
               int spacing = 0) noexcept;

    /**
     * @param[in] size Rows and columns.
     * @param[in] spacing Amount of spacing to put between cells.
     */
    StaticGrid(const Tuple& size = Tuple(1, 1), int spacing = 0) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Rectangle for the widget.
     * @param[in] size Rows and columns.
     * @param[in] spacing Amount of spacing to put between cells.
     */
    StaticGrid(Frame& parent, const Rect& rect, const Tuple& size = Tuple(1, 1),
               int spacing = 0) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] size Rows and columns.
     * @param[in] spacing Amount of spacing to put between cells.
     */
    StaticGrid(Frame& parent, const Tuple& size = Tuple(1, 1),
               int spacing = 0) noexcept;

    StaticGrid(const StaticGrid& rhs) noexcept
        : Frame(rhs),
          m_cells(rhs.m_cells),
          m_spacing(rhs.m_spacing),
          m_last_add_column(rhs.m_last_add_column),
          m_last_add_row(rhs.m_last_add_row)
    {
        // fix our own pointers after deep copy of children
        for (size_t x = 0; x < m_children.size(); ++x)
        {
            for (auto& column : m_cells)
            {
                std::replace(column.begin(), column.end(),
                             rhs.m_children[x].get(), m_children[x].get());
            }
        }
    }

    StaticGrid(StaticGrid&& rhs) = delete;

    StaticGrid& operator=(const StaticGrid& rhs) noexcept
    {
        Frame::operator=(rhs);

        m_cells = rhs.m_cells;
        m_spacing = rhs.m_spacing;
        m_last_add_column = rhs.m_last_add_column;
        m_last_add_row = rhs.m_last_add_row;

        // fix our own pointers after deep copy of children
        for (size_t x = 0; x < m_children.size(); ++x)
        {
            for (auto& column : m_cells)
            {
                std::replace(column.begin(), column.end(),
                             rhs.m_children[x].get(), m_children[x].get());
            }
        }

        return *this;
    }

    StaticGrid& operator=(StaticGrid&& rhs) noexcept = delete;

    virtual std::unique_ptr<Widget> clone() override
    {
        return std::unique_ptr<Widget>(make_unique<StaticGrid>(*this).release());
    }

    virtual void draw(Painter& painter, const Rect& rect) override;

    /**
     * Add a widget to the next empty cell.
     *
     * This will not automatically resize the grid to fit the widget as
     * necessary.
     *
     * @param widget The widget to add, or nullptr.
     */
    virtual void add(const std::shared_ptr<Widget>& widget) override;

    /** @todo Why is this not inherited from Frame::add()? */
    virtual void add(Widget& widget) override
    {
        // Nasty, but it gets the job done.  If a widget is passed in as a
        // reference, we don't own it, so create a "pointless" shared_ptr that
        // will not delete it.
        add(std::shared_ptr<Widget>(&widget, [](Widget*) {}));
    }

    /**
     * Add a widget to the grid into a specific cell.
     *
     * This will automatically resize the grid to fit the widget as
     * necessary.
     *
     * @param widget The widget to add, or nullptr.
     * @param column The column index.
     * @param row The row index.
     *
     * @todo remove align off both of the add() functions here. This is an
     * API inconsistency and align needs to be controlled on the widget itself
     * and not always overwritten or set again here. Which begs the question:
     * what should the align default be or do we default to size of widget?
     */
    virtual void add(const std::shared_ptr<Widget>& widget, int column, int row);

    virtual void add(Widget& widget, int column, int row)
    {
        // Nasty, but it gets the job done.  If a widget is passed in as a
        // reference, we don't own it, so create a "pointless" shared_ptr that
        // will not delete it.
        add(std::shared_ptr<Widget>(&widget, [](Widget*) {}), column, row);
    }

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

    virtual void layout() override
    {
        //Frame::layout();
        reposition();
    }

    int last_add_column() const
    {
        return m_last_add_column;
    }

    int last_add_row() const
    {
        return m_last_add_row;
    }

    virtual ~StaticGrid() noexcept = default;

protected:

    using cell_array = std::vector<std::vector<Widget*>>;

    cell_array m_cells;
    int m_spacing{0};

    int m_last_add_column{0};
    int m_last_add_row{0};
};

/**
 * @brief StaticGrid with selectable cells.
 *
 * A StaticGrid where each item is visually selectable with a highlighted
 * border.
 */
class SelectableGrid : public StaticGrid
{
public:

    using StaticGrid::StaticGrid;

    virtual std::unique_ptr<Widget> clone() override
    {
        return std::unique_ptr<Widget>(make_unique<SelectableGrid>(*this).release());
    }

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

    virtual ~SelectableGrid() noexcept = default;

protected:
    int m_selected_column{0};
    int m_selected_row{0};
};

}
}

#endif
