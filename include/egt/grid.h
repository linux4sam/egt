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
#include <memory>
#include <vector>
#include <tuple>

namespace egt
{
inline namespace v1
{

/**
 * Static grid organization for widgets.
 *
 * A static grid that does not perform any drawing, but controls the
 * size and position of any widget added into a grid.
 *
 * Every cell in a grid is the same size. The size of the grid and each cell is
 * established, and then the widgets are aligned within each cell according to
 * widget alignment and padding.
 *
 * @ingroup sizers
 */
class StaticGrid : public Frame
{
public:

    enum class flag
    {
        /**
         * When set, draw a border using Palette::ColorId::border.
         */
        show_border,
    };

    using flags_type = Flags<flag>;

    /**
     * @param[in] size Rows and columns.
     * @param[in] border Border width. @see Widget::set_border().
     */
    explicit StaticGrid(const std::tuple<int, int>& size = std::make_tuple(1, 1),
                        default_dim_type border = 0) noexcept;

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] size Rows and columns.
     * @param[in] border Border width. @see Widget::set_border().
     */
    explicit StaticGrid(const Rect& rect, const std::tuple<int, int>& size = std::make_tuple(1, 1),
                        default_dim_type border = 0) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Rectangle for the widget.
     * @param[in] size Rows and columns.
     * @param[in] border Border width. @see Widget::set_border().
     */
    StaticGrid(Frame& parent, const Rect& rect,
               const std::tuple<int, int>& size = std::make_tuple(1, 1),
               default_dim_type border = 0) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] size Rows and columns.
     * @param[in] border Border width. @see Widget::set_border().
     */
    explicit StaticGrid(Frame& parent, const std::tuple<int, int>& size = std::make_tuple(1, 1),
                        default_dim_type border = 0) noexcept;

    virtual void draw(Painter& painter, const Rect& rect) override;

    using Frame::add;

    /**
     * Add a widget to the next empty cell.
     *
     * This will not automatically resize the grid to fit the widget as
     * necessary.
     *
     * @param widget The widget to add, or nullptr.
     */
    virtual void add(const std::shared_ptr<Widget>& widget) override;

    /**
     * Add a widget to the grid into a specific cell.
     *
     * This will automatically resize the grid to fit the widget as
     * necessary.
     *
     * @param widget The widget to add, or nullptr.
     * @param column The column index.
     * @param row The row index.
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

    virtual void layout() override;

    /**
     * Returns the last column used for an add() call.
     *
     * @note This will return -1 if nothing has been added.
     */
    inline int last_add_column() const
    {
        return m_last_add_column;
    }

    /**
     * Returns the last row used for an add() call.
     *
     * @note This will return -1 if nothing has been added.
     */
    inline int last_add_row() const
    {
        return m_last_add_row;
    }

    virtual Rect content_area() const override
    {
        // we don't include border, grid handles that itself
        auto moat = margin() + padding();
        auto b = box();
        b += Point(moat, moat);
        b -= Size(2. * moat, 2. * moat);
        return b;
    }

    /**
     * Get a const ref of the flags.
     */
    inline const flags_type& grid_flags() const { return m_grid_flags; }

    /**
     * Get a modifiable ref of the flags.
     */
    inline flags_type& grid_flags() { return m_grid_flags; }

    virtual void reallocate(const std::tuple<int, int>& size);

    virtual ~StaticGrid() noexcept = default;

protected:

    /**
     * Re-position all child widgets.
     *
     * @note You should *not* have to manually call this under normal
     * circumstances.
     */
    virtual void reposition();

    using cell_array = std::vector<std::vector<std::weak_ptr<Widget>>>;

    cell_array m_cells;
    int m_last_add_column{-1};
    int m_last_add_row{-1};
    bool m_column_priority{false};

    /**
     * Grid flags.
     */
    flags_type m_grid_flags;
};

/**
 * StaticGrid with select-able cells.
 *
 * A StaticGrid where each item is visually select-able with a highlighted
 * border.
 *
 * @ingroup sizers
 */
class SelectableGrid : public StaticGrid
{
public:

    using StaticGrid::StaticGrid;

    virtual void draw(Painter& painter, const Rect& rect) override;

    virtual Point selected() const
    {
        return {m_selected_column, m_selected_row};
    }

    virtual void handle(Event& event) override;

    /**
     * Set the selected cell.
     *
     * @param[in] column The column to select.
     * @param[in] row The row to select.
     */
    virtual void set_selected(int column, int row);

    virtual ~SelectableGrid() noexcept = default;

protected:
    int m_selected_column{0};
    int m_selected_row{0};
};

}
}

#endif
