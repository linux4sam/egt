/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/alignment.h"
#include "egt/detail/enum.h"
#include "egt/grid.h"
#include "egt/painter.h"
#include "egt/serialize.h"
#include <algorithm>
#include <cassert>

namespace egt
{
inline namespace v1
{

StaticGrid::StaticGrid(const Rect& rect, const GridSize& size)
    : Frame(rect)
{
    name("StaticGrid" + std::to_string(m_widgetid));

    reallocate(size);
}

StaticGrid::StaticGrid(Serializer::Properties& props, bool is_derived)
    : Frame(props, true)
{
    name("StaticGrid" + std::to_string(m_widgetid));

    reallocate(GridSize(1, 1));

    deserialize(props);

    if (!is_derived)
        deserialize_leaf(props);
}

void StaticGrid::reallocate(const GridSize& size)
{
    if (!size.width() || !size.height())
        throw std::invalid_argument("a static grid needs at least one cell i.e. one row and one col");

    m_grid_size = size;

    /// If columns or rows are removed, remove widgets in these cells
    auto current_columns = m_cells.size();
    const auto new_columns = size.width();
    if (new_columns < current_columns)
    {
        for (size_t x = new_columns; x < current_columns; x++)
        {
            for (size_t y = 0; y < m_cells[0].size(); y++)
            {
                auto w = get(GridPoint(x, y));
                if (w)
                    w->detach();
            }
        }
    }
    m_cells.resize(size.width());
    current_columns = m_cells.size();

    const auto current_rows = m_cells[0].size();
    const auto new_rows = size.height();
    if (new_rows < current_rows)
    {
        for (size_t x = 0; x < current_columns; x++)
        {
            for (size_t y = new_rows; y < current_rows; y++)
            {
                auto w = get(GridPoint(x, y));
                if (w)
                    w->detach();
            }
        }
    }

    /*
     * The grid size is set here.  Every column should be the same size.  Don't
     * delete from the column vectors.  Only set empty cells to nullptr.
     */
    for (auto& x : m_cells)
        x.resize(size.height(), {});
}

StaticGrid::StaticGrid(const GridSize& size)
    : StaticGrid(Rect(), size)
{
}

StaticGrid::StaticGrid(Frame& parent, const Rect& rect, const GridSize& size)
    : StaticGrid(rect, size)
{
    parent.add(*this);
}

StaticGrid::StaticGrid(Frame& parent, const GridSize& size)
    : StaticGrid(size)
{
    parent.add(*this);
}

namespace detail
{
static constexpr DefaultDim round(DefaultDim x, DefaultDim y)
{
    return x  / y;
}
}

/*
 * Calculates the rectangle for a cell taking into account grid spaces.
 */
static constexpr Rect cell_rect(int columns, int rows,
                                DefaultDim width, DefaultDim height,
                                int column, int row,
                                DefaultDim h_space, DefaultDim v_space)
{
    auto cell_width = detail::round(width - (columns - 1) * v_space, columns);
    auto cell_height = detail::round(height - (rows - 1) * h_space, rows);
    auto cell_x = column * (cell_width + v_space);
    auto cell_y = row * (cell_height + h_space);

    /*
     * If there are errors due to rounding, use the latest column and row to
     * counterbalance them.
     */
    if (column == (columns - 1))
    {
        auto error = width - cell_x - cell_width;
        if (error)
            cell_width += error;
    }

    if (row == (rows - 1))
    {
        auto error = height - cell_y - cell_height;
        if (error)
            cell_height += error;
    }

    return {cell_x, cell_y, cell_width, cell_height};
}

void StaticGrid::add(const std::shared_ptr<Widget>& widget)
{
    if (!widget)
        return;

    if (widget.get() == this)
        throw std::runtime_error("cannot add a widget to itself");

    assert(!widget->parent() && "widget already has parent!");

    if (m_column_priority)
    {
        int c = 0;
        for (auto& column : m_cells)
        {
            auto i = std::find_if(column.begin(), column.end(),
                                  [](const auto & w)
            {
                return w.expired();
            });

            if (i != column.end())
            {
                *i = widget;

                Frame::add(widget);

                reposition();

                m_last_add_column = c;
                m_last_add_row = std::distance(column.begin(), i);

                break;
            }

            c++;
        }
    }
    else
    {
        for (size_t r = 0; r < m_cells[0].size(); r++)
        {
            int c = 0;
            for (auto& column : m_cells)
            {
                auto w = column[r].lock();
                if (!w)
                {
                    column[r] = widget;

                    Frame::add(widget);

                    reposition();

                    m_last_add_column = c;
                    m_last_add_row = r;

                    return;
                }

                c++;
            }
        }
    }
}

void StaticGrid::add(const std::shared_ptr<Widget>& widget, size_t column, size_t row)
{
    Frame::add(widget);

    if (column >= m_cells.size())
        m_cells.resize(column + 1);

    if (row >= m_cells[column].size())
    {
        for (auto& c : m_cells)
            c.resize(row + 1, {});
    }

    m_cells[column][row] = widget;

    m_last_add_column = column;
    m_last_add_row = row;

    reposition();
}

Widget* StaticGrid::get(const GridPoint& point)
{
    if (point.x() < m_cells.size() &&
        point.y() < m_cells[point.x()].size())
    {
        auto widget = m_cells[point.x()][point.y()].lock();
        if (widget)
            return widget.get();
    }
    return nullptr;
}

void StaticGrid::remove(Widget* widget)
{
    if (!widget)
        return;

    auto predicate = [widget](const std::weak_ptr<Widget>& ptr)
    {
        auto w = ptr.lock();
        if (w)
            return w.get() == widget;
        return false;
    };

    for (auto& x : m_cells)
    {
        for (auto i = std::find_if(x.begin(), x.end(), predicate); i != x.end();
             i = std::find_if(x.begin(), x.end(), predicate))
        {
            i->reset();
        }
    }

    reposition();

    Frame::remove(widget);
}

void StaticGrid::reposition()
{
    auto b = content_area();

    if (b.empty())
        return;

    auto columns = m_cells.size();
    for (size_t column = 0; column < columns; column++)
    {
        auto rows = m_cells[column].size();
        for (size_t row = 0; row < rows; row++)
        {
            auto widget = m_cells[column][row].lock();
            if (widget)
            {
                auto bounding = cell_rect(columns, rows, b.width(), b.height(), column, row,
                                          horizontal_space(), vertical_space());
                bounding += b.point() - point();

                if (bounding.size().empty())
                    continue;

                // the widget has to be inside the cell
                if (widget->x() < bounding.x())
                    widget->x(bounding.x());

                if (widget->y() < bounding.y())
                    widget->y(bounding.y());

                // get the aligning rect
                const auto target = detail::align_algorithm(widget->box(),
                                    bounding,
                                    widget->align());

                // re-position/resize widget
                widget->box(target);

                if (widget->frame())
                {
                    auto frame = dynamic_cast<Frame*>(widget.get());
                    frame->layout();
                }
            }
        }
    }

    damage();
}

void StaticGrid::layout()
{
    if (!visible())
        return;

    // we cannot layout with no space
    if (size().empty())
        return;

    if (m_in_layout)
        return;

    if (m_children.empty())
        return;

    m_in_layout = true;
    auto reset = detail::on_scope_exit([this]() { m_in_layout = false; });

    reposition();
}

void StaticGrid::serialize(Serializer& serializer) const
{
    serializer.add_property("column_priority", m_column_priority);
    serializer.add_property("n_col", static_cast<unsigned int>(n_col()));
    serializer.add_property("n_row", static_cast<unsigned int>(n_row()));
    serializer.add_property("horizontal_space", horizontal_space());
    serializer.add_property("vertical_space", vertical_space());

    Frame::serialize(serializer);
}

void StaticGrid::deserialize(Serializer::Properties& props)
{
    props.erase(std::remove_if(props.begin(), props.end(), [&](auto & p)
    {
        auto name = std::get<0>(p);
        if (name == "column_priority")
            m_column_priority = detail::from_string(std::get<1>(p));
        else if (name == "n_col")
            reallocate(GridSize(std::stoi(std::get<1>(p)), n_row()));
        else if (name == "n_row")
            reallocate(GridSize(n_col(), std::stoi(std::get<1>(p))));
        else if (name == "horizontal_space")
            horizontal_space(std::stoi(std::get<1>(p)));
        else if (name == "vertical_space")
            vertical_space(std::stoi(std::get<1>(p)));
        else
            return false;
        return true;
    }), props.end());

}

SelectableGrid::SelectableGrid(Serializer::Properties& props, bool is_derived)
    : StaticGrid(props, true)
{
    deserialize(props);

    if (!is_derived)
        deserialize_leaf(props);
}

void SelectableGrid::handle(Event& event)
{
    StaticGrid::handle(event);

    if (event.id() == EventId::raw_pointer_down)
    {
        auto b = content_area().size();
        Point pos = display_to_local(event.pointer().point);

        auto columns = m_cells.size();
        for (size_t column = 0; column < columns; column++)
        {
            auto rows = m_cells[column].size();
            for (size_t row = 0; row < rows; row++)
            {
                // Include the padding region
                auto bounding = cell_rect(columns, rows, b.width(), b.height(), column, row,
                                          horizontal_space(), vertical_space());
                bounding += content_area().point();

                assert(!bounding.size().empty());
                if (bounding.size().empty())
                    continue;

                if (bounding.intersect(pos))
                {
                    selected(column, row);
                    break;
                }
            }
        }
    }
}

void SelectableGrid::draw(Painter& painter, const Rect& rect)
{
    StaticGrid::draw(painter, rect);

    // Draw the selection highlight
    if (selection_highlight() > 0)
    {
        painter.set(color(Palette::ColorId::border));
        painter.line_width(selection_highlight());

        size_t column = m_selected_column;
        size_t row = m_selected_row;
        auto columns = m_cells.size();
        auto rows = m_cells[column].size();

        auto b = content_area();

        auto cell = cell_rect(columns, rows, b.width(), b.height(), column, row,
                              horizontal_space(), vertical_space());

        auto hx = b.x() + cell.x() + selection_highlight() / 2;
        auto hy = b.y() + cell.y() + selection_highlight() / 2;
        auto hwidth = cell.width() - selection_highlight();
        auto hheight = cell.height() - selection_highlight();

        painter.draw(Rect(Point(hx, hy), Size(hwidth, hheight)));
        painter.stroke();
    }
}

void SelectableGrid::selected(size_t column, size_t row)
{
    const auto columns = m_cells.size();
    if (column >= columns)
        return;

    if (!columns || row >= m_cells[column].size())
        return;

    auto c = detail::change_if_diff<>(m_selected_column, column);
    auto r = detail::change_if_diff<>(m_selected_row, row);
    if (c || r)
    {
        damage();
        on_selected_changed.invoke();
    }
}

void SelectableGrid::serialize(Serializer& serializer) const
{
    StaticGrid::serialize(serializer);

    serializer.add_property("selected_column", static_cast<int>(m_selected_column));
    serializer.add_property("selected_row", static_cast<int>(m_selected_row));
    serializer.add_property("selection_highlight", selection_highlight());
}

void SelectableGrid::deserialize(Serializer::Properties& props)
{
    props.erase(std::remove_if(props.begin(), props.end(), [&](auto & p)
    {
        auto name = std::get<0>(p);
        if (name == "selected_column")
            m_selected_column = std::stoul(std::get<1>(p));
        else if (name == "selected_row")
            m_selected_row = std::stoul(std::get<1>(p));
        else if (name == "selection_highlight")
            selection_highlight(std::stoi(std::get<1>(p)));
        else
            return false;
        return true;
    }), props.end());
}

}
}
