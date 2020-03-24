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
#include <tuple>
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
    using GridFlags = detail::Flags<GridFlag>;

    /**
     * @param[in] size Rows and columns.
     * @param[in] border Border width. @see Widget::border().
     */
    explicit StaticGrid(const std::tuple<int, int>& size = std::make_tuple(1, 1),
                        DefaultDim border = 0) noexcept;

    /**
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] size Rows and columns.
     * @param[in] border Border width. @see Widget::border().
     */
    explicit StaticGrid(const Rect& rect, const std::tuple<int, int>& size = std::make_tuple(1, 1),
                        DefaultDim border = 0) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] size Rows and columns.
     * @param[in] border Border width. @see Widget::border().
     */
    StaticGrid(Frame& parent, const Rect& rect,
               const std::tuple<int, int>& size = std::make_tuple(1, 1),
               DefaultDim border = 0) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] size Rows and columns.
     * @param[in] border Border width. @see Widget::border().
     */
    explicit StaticGrid(Frame& parent, const std::tuple<int, int>& size = std::make_tuple(1, 1),
                        DefaultDim border = 0) noexcept;

    EGT_OPS_NOCOPY_MOVE(StaticGrid);
    ~StaticGrid() noexcept override = default;

    void draw(Painter& painter, const Rect& rect) override;

    using Frame::add;

    /**
     * Add a widget to the next empty cell.
     *
     * This will not automatically resize the grid to fit the widget as
     * necessary.
     *
     * @param widget The widget to add, or nullptr.
     */
    void add(std::shared_ptr<Widget> widget) override;

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

    /**
     * Add a widget to the grid into a specific cell.
     *
     * @param widget The widget to add, or nullptr.
     * @param column The column index.
     * @param row The row index.
     */
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

    void remove(Widget* widget) override;

    void layout() override;

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

    Rect content_area() const override
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
    inline const GridFlags& grid_flags() const { return m_grid_flags; }

    /**
     * Get a modifiable ref of the flags.
     */
    inline GridFlags& grid_flags() { return m_grid_flags; }

    void serialize(Serializer& serializer) const override;

    void deserialize(const std::string& name, const std::string& value,
                     const Serializer::Attributes& attrs) override;

protected:

    /// Reallocate the size of the grid keeping any existing cells intact.
    void reallocate(const std::tuple<int, int>& size);

    /**
     * Re-position all child widgets.
     *
     * @note You should *not* have to manually call this under normal
     * circumstances.
     */
    virtual void reposition();

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
};

/**
 * StaticGrid with select-able cells.
 *
 * A StaticGrid where each item is visually select-able with a highlighted
 * border.
 *
 * @ingroup sizers
 */
class EGT_API SelectableGrid : public StaticGrid
{
public:

    /**
     * Event signal.
     * @{
     */
    /// Invoked when the selection changes.
    detail::Signal<> on_selected_changed;
    /** @} */

    using StaticGrid::StaticGrid;

    EGT_OPS_NOCOPY_MOVE(SelectableGrid);
    ~SelectableGrid() noexcept override = default;

    void draw(Painter& painter, const Rect& rect) override;

    /// Get the selected cell.
    virtual Point selected() const
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
    virtual void selected(int column, int row);

    void serialize(Serializer& serializer) const override;

    void deserialize(const std::string& name, const std::string& value,
                     const Serializer::Attributes& attrs) override;

protected:
    /// Currently selected column.
    int m_selected_column{0};
    /// Currently selected row.
    int m_selected_row{0};
};

}
}

#endif
