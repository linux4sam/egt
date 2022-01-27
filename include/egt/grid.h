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

#include <egt/detail/meta.h>
#include <egt/frame.h>
#include <egt/widget.h>
#include <memory>
#include <vector>

namespace egt
{
inline namespace v1
{

/**
 * Static grid organization for widgets.
 *
 * A static grid provides cells of the same size. Cells of the last column or
 * row may have a different size to counterbalance rounding errors.
 *
 * Widgets added to the grid will be positionned within the cell according to
 * their alignment.
 *
 * @image html staticgrid.png
 * @image latex staticgrid.png "StaticGrid" width=8cm
 *
 * If extra horizontal or vertical space is needed, the margin, border and
 * padding properties of the widget within the cell can still be used.
 *
 * @ingroup sizers
 */
class EGT_API StaticGrid : public Frame
{
public:

    /// Grid flags.
    enum class GridFlag
    {
        /**
         * When set, draw a border using Palette::ColorId::border.
         */
        show_border = detail::bit(0),
    };

    /// Grid flags.
    using GridFlags = egt::Flags<GridFlag>;

    /// Rows and columns size of the grid.
    using GridSize = SizeType<size_t, detail::Compatible::grid>;

    /// Rows and column of the grid.
    using GridPoint = PointType<size_t, detail::Compatible::grid>;

    /**
     * @param[in] size Rows and columns.
     */
    explicit StaticGrid(const GridSize& size = GridSize(1, 1));

    /**
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] size Rows and columns.
     */
    explicit StaticGrid(const Rect& rect,
                        const GridSize& size = GridSize(1, 1));

    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] size Rows and columns.
     */
    StaticGrid(Frame& parent, const Rect& rect,
               const GridSize& size = GridSize(1, 1));

    /**
     * @param[in] parent The parent Frame.
     * @param[in] size Rows and columns.
     */
    explicit StaticGrid(Frame& parent,
                        const GridSize& size = GridSize(1, 1));

    /**
     * @param[in] props list of widget argument and its properties.
     */
    explicit StaticGrid(Serializer::Properties& props)
        : StaticGrid(props, false)
    {
    }

protected:

    explicit StaticGrid(Serializer::Properties& props, bool is_derived);

public:

    using Frame::add;

    /**
     * Add a widget to the next empty cell.
     *
     * This will not automatically extend the grid to fit the widget as
     * necessary.
     *
     * @param widget The widget to add, or nullptr.
     */
    void add(const std::shared_ptr<Widget>& widget) override;

    /**
     * Add a widget to the grid into a specific cell.
     *
     * This will automatically extend the grid to fit the widget as
     * necessary.
     *
     * @param widget The widget to add, or nullptr.
     * @param column The column index.
     * @param row The row index.
     */
    virtual void add(const std::shared_ptr<Widget>& widget, size_t column, size_t row);

    /**
     * Add a widget to the grid into a specific cell.
     *
     * @param widget The widget to add, or nullptr.
     * @param column The column index.
     * @param row The row index.
     */
    virtual void add(Widget& widget, size_t column, size_t row)
    {
        // Nasty, but it gets the job done.  If a widget is passed in as a
        // reference, we don't own it, so create a "pointless" shared_ptr that
        // will not delete it.
        auto w = std::shared_ptr<Widget>(&widget, [](Widget*) {});
        add(w, column, row);
    }

    /**
     * Add a widget to the grid into a specific cell.
     *
     * This will automatically extend the grid to fit the widget as
     * necessary.
     *
     * @param widget The widget to add, or nullptr.
     * @param point The row and column to add the widget.
     */
    virtual void add(const std::shared_ptr<Widget>& widget, const GridPoint& point)
    {
        add(widget, point.x(), point.y());
    }

    /**
     * Add a widget to the grid into a specific cell.
     *
     * @param widget The widget to add, or nullptr.
     * @param point The row and column to add the widget.
     */
    virtual void add(Widget& widget, const GridPoint& point)
    {
        // Nasty, but it gets the job done.  If a widget is passed in as a
        // reference, we don't own it, so create a "pointless" shared_ptr that
        // will not delete it.
        auto w = std::shared_ptr<Widget>(&widget, [](Widget*) {});
        add(w, point);
    }

    /**
     * Get a widget at the specified row and column.
     *
     * @note Point here is used as a column and row.
     *
     * @param point The row and column to get.
     */
    Widget* get(const GridPoint& point);

    void remove(Widget* widget) override;

    void layout() override;

    /**
     * Returns the last column used for an add() call.
     *
     * @note This will return -1 if nothing has been added.
     */
    EGT_NODISCARD int last_add_column() const
    {
        return m_last_add_column;
    }

    /**
     * Returns the last row used for an add() call.
     *
     * @note This will return -1 if nothing has been added.
     */
    EGT_NODISCARD int last_add_row() const
    {
        return m_last_add_row;
    }

    /**
     * Get the column priority status.
     */
    EGT_NODISCARD bool column_priority() const { return m_column_priority; }

    /**
     * Set the column priority status.
     *
     * @note Set column priority when expanding the grid automatically. By
     * default, value is false.
     */
    void column_priority(bool value) { m_column_priority = value; }

    /**
     * Get the number of columns.
     */
    EGT_NODISCARD size_t n_col() const
    {
        return m_grid_size.width();
    }

    /**
     * Get the number of rows.
     */
    EGT_NODISCARD size_t n_row() const
    {
        return m_grid_size.height();
    }

    /**
     * Get the GridSize.
     */
    EGT_NODISCARD GridSize grid_size() const
    {
        return m_grid_size;
    }

    /**
     * Set the GridSize.
     */
    void grid_size(const GridSize size)
    {
        if (detail::change_if_diff<>(m_grid_size, size))
        {
            reallocate(size);
        }
    }

    /**
     * Set the horizontal space i.e. the space between rows.
     */
    void horizontal_space(DefaultDim space)
    {
        if (detail::change_if_diff<>(m_horizontal_space, space))
        {
            layout();
            damage();
        }
    }

    /**
     * Get the horizontal space.
     */
    EGT_NODISCARD DefaultDim horizontal_space() const
    {
        return m_horizontal_space;
    }

    /**
     * Set the vertical space i.e. the space between columns.
     */
    void vertical_space(DefaultDim space)
    {
        if (detail::change_if_diff<>(m_vertical_space, space))
        {
            layout();
            damage();
        }
    }

    /**
     * Get the vertical space.
     */
    EGT_NODISCARD DefaultDim vertical_space() const
    {
        return m_vertical_space;
    }

    void serialize(Serializer& serializer) const override;

protected:

    /// Reallocate the size of the grid keeping any existing cells intact.
    void reallocate(const GridSize& size);

    /**
     * Re-position all child widgets.
     *
     * @note You should *not* have to manually call this under normal
     * circumstances.
     */
    void reposition();

    /// Type for cell array.
    using CellArray = std::vector<std::vector<std::weak_ptr<Widget>>>;

    /// Cell array of the grid.
    CellArray m_cells;

    /// Last added column.
    int m_last_add_column{-1};
    /// Last added row.
    int m_last_add_row{-1};
    /// Column priority when expanding the grid automatically.
    bool m_column_priority{false};
    /// Grid size.
    GridSize m_grid_size{};
    /// Space between rows.
    DefaultDim m_horizontal_space{};
    /// Space betwwen columns.
    DefaultDim m_vertical_space{};

private:

    void deserialize(Serializer::Properties& props);
};

/**
 * StaticGrid with selectable cells.
 *
 * A StaticGrid where each item is visually selectable with a highlighted
 * border.
 *
 * @note The highlighted border for the item selected is drawn within the
 * cell. It means the border will be drawn on top of the content area of the
 * widgets if they have their margin, border and padding set to 0;
 *
 * @ingroup sizers
 */
// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
class EGT_API SelectableGrid : public StaticGrid
{
public:

    /**
     * Event signal.
     * @{
     */
    /// Invoked when the selection changes.
    Signal<> on_selected_changed;
    /** @} */

    /**
     * @param[in] props list of widget argument and its properties.
     */
    explicit SelectableGrid(Serializer::Properties& props)
        : SelectableGrid(props, false)
    {
    }

protected:

    explicit SelectableGrid(Serializer::Properties& props, bool is_derived);

public:

    using StaticGrid::StaticGrid;

    void draw(Painter& painter, const Rect& rect) override;

    /// Get the selected cell.
    EGT_NODISCARD GridPoint selected() const
    {
        return {m_selected_column, m_selected_row};
    }

    void handle(Event& event) override;

    /**
     * Set the selected cell.
     *
     * @param[in] column The column to select.
     * @param[in] row The row to select.
     */
    void selected(size_t column, size_t row);

    /**
     * Set the dimension of the selection highlight.
     */
    void selection_highlight(DefaultDim highlight)
    {
        if (detail::change_if_diff<>(m_selection_highlight, highlight))
        {
            layout();
            damage();
        }
    }

    /**
     * Get the dimension of the selection highlight.
     */
    EGT_NODISCARD DefaultDim selection_highlight() const
    {
        return m_selection_highlight;
    }

    void serialize(Serializer& serializer) const override;

protected:
    /// Currently selected column.
    size_t m_selected_column{0};
    /// Currently selected row.
    size_t m_selected_row{0};
    /// Dimension of the highlight border.
    DefaultDim m_selection_highlight{5};

private:

    void deserialize(Serializer::Properties& props);
};

}
}

#endif
