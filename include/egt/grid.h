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
 * A static grid that does not perform any drawing, but controls the
 * size and position of any widget added into a grid.
 *
 * Every cell in a grid is the same size. The size of the grid and each cell is
 * established, and then the widgets are aligned within each cell according to
 * widget alignment and padding.
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
     * @param[in] border Border width. @see Widget::border().
     */
    explicit StaticGrid(const GridSize& size = GridSize(1, 1),
                        DefaultDim border = 0);

    /**
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] size Rows and columns.
     * @param[in] border Border width. @see Widget::border().
     */
    explicit StaticGrid(const Rect& rect,
                        const GridSize& size = GridSize(1, 1),
                        DefaultDim border = 0);

    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] size Rows and columns.
     * @param[in] border Border width. @see Widget::border().
     */
    StaticGrid(Frame& parent, const Rect& rect,
               const GridSize& size = GridSize(1, 1),
               DefaultDim border = 0);

    /**
     * @param[in] parent The parent Frame.
     * @param[in] size Rows and columns.
     * @param[in] border Border width. @see Widget::border().
     */
    explicit StaticGrid(Frame& parent,
                        const GridSize& size = GridSize(1, 1),
                        DefaultDim border = 0);

    EGT_OPS_NOCOPY_MOVE(StaticGrid);
    ~StaticGrid() noexcept override = default;

    void draw(Painter& painter, const Rect& rect) override;

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

    EGT_NODISCARD Rect content_area() const override
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
    EGT_NODISCARD const GridFlags& grid_flags() const { return m_grid_flags; }

    /**
     * Get a modifiable ref of the flags.
     */
    GridFlags& grid_flags() { return m_grid_flags; }

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

    void serialize(Serializer& serializer) const override;

    void deserialize(const std::string& name, const std::string& value,
                     const Serializer::Attributes& attrs) override;

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
    /// Grid flags.
    GridFlags m_grid_flags{};
    /// Grid size.
    GridSize m_grid_size{};
};

/**
 * StaticGrid with selectable cells.
 *
 * A StaticGrid where each item is visually selectable with a highlighted
 * border.
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

    void serialize(Serializer& serializer) const override;

    void deserialize(const std::string& name, const std::string& value,
                     const Serializer::Attributes& attrs) override;

protected:
    /// Currently selected column.
    size_t m_selected_column{0};
    /// Currently selected row.
    size_t m_selected_row{0};
};

}
}

#endif
