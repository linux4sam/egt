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

StaticGrid::StaticGrid(const Rect& rect, const GridSize& size,
                       DefaultDim border)
    : Frame(rect)
{
    name("StaticGrid" + std::to_string(m_widgetid));
    this->border(border);

    reallocate(size);
}

void StaticGrid::reallocate(const GridSize& size)
{
    if (!size.width() || !size.height())
        throw std::invalid_argument("a static grid needs at least one cell i.e. one row and one col");

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

StaticGrid::StaticGrid(const GridSize& size, DefaultDim border)
    : StaticGrid(Rect(), size, border)
{
}

StaticGrid::StaticGrid(Frame& parent, const Rect& rect,
                       const GridSize& size, DefaultDim border)
    : StaticGrid(rect, size, border)
{
    parent.add(*this);
}

StaticGrid::StaticGrid(Frame& parent, const GridSize& size, DefaultDim border)
    : StaticGrid(size, border)
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
 * Calculates the rectangle for a cell. This calculates the rectangle right
 * down the center of any border_width if one exists.
 */
static constexpr Rect cell_rect(int columns, int rows, DefaultDim width, DefaultDim height,
                                int column, int row, int border_width = 0, int padding = 0)
{
    const auto inner_width = detail::round((width - ((columns + 1) * border_width)), columns);
    const auto inner_height = detail::round((height - ((rows + 1) * border_width)), rows);

    auto ix = (column * border_width) + (column * inner_width) + detail::round(border_width, 2);
    auto iy = (row * border_width) + (row * inner_height) + detail::round(border_width, 2);
    auto iw = inner_width + border_width;
    auto ih = inner_height + border_width;

    ix += padding;
    iy += padding;
    iw -= 2. * padding;
    ih -= 2. * padding;

    if (iw < 0)
        iw = 0;

    if (ih < 0)
        ih = 0;

    return {ix, iy, iw, ih};
}

void StaticGrid::draw(Painter& painter, const Rect& rect)
{
    if (grid_flags().is_set(GridFlag::show_border) && border() > 0)
    {
        const auto b = content_area();

        painter.set(color(Palette::ColorId::border));
        painter.line_width(border());

        const auto columns = m_cells.size();
        for (size_t column = 0; column < columns; column++)
        {
            const auto rows = m_cells[column].size();
            for (size_t row = 0; row < rows; row++)
            {
                auto r = cell_rect(columns, rows, b.width(), b.height(), column, row, border());
                r += b.point();
                painter.draw(r);
            }
        }

        painter.stroke();
    }

    Frame::draw(painter, rect);
}

void StaticGrid::add(std::shared_ptr<Widget> widget)
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
                                  [](const std::weak_ptr<Widget>& w)
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
                auto bounding = cell_rect(columns, rows, b.width(), b.height(), column, row, border(), padding());
                bounding += b.point() - point();

                if (border())
                {
                    bounding += Point(detail::round(border(), 2), detail::round(border(), 2));
                    bounding -= Size(border(), border());
                }

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

template<>
const std::pair<StaticGrid::GridFlag, char const*> detail::EnumStrings<StaticGrid::GridFlag>::data[] =
{
    {StaticGrid::GridFlag::show_border, "show_border"},
};

void StaticGrid::serialize(Serializer& serializer) const
{
    serializer.add_property("grid_flags", m_grid_flags.to_string());
    serializer.add_property("column_priority", m_column_priority);

    Frame::serialize(serializer);
}

void StaticGrid::deserialize(const std::string& name, const std::string& value,
                             const Serializer::Attributes& attrs)
{
    if (name == "grid_flags")
        m_grid_flags.from_string(value);
    else if (name == "column_priority")
        m_column_priority = detail::from_string(value);
    else
        Frame::deserialize(name, value, attrs);
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
                auto bounding = cell_rect(columns, rows, b.width(), b.height(), column, row, border(), padding());

                if (border())
                {
                    bounding += Point(detail::round(border(), 2), detail::round(border(), 2));
                    bounding -= Size(border(), border());
                }

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
    if (border() > 0)
    {
        painter.set(color(Palette::ColorId::border));
        auto line_width = border() / 2;
        if (line_width <= 0)
            line_width = border();
        painter.line_width(line_width);

        size_t column = m_selected_column;
        size_t row = m_selected_row;
        auto columns = m_cells.size();
        auto rows = m_cells[column].size();

        auto b = content_area();

        auto r = cell_rect(columns, rows, b.width(), b.height(), column, row, border(), padding());
        r += b.point();

        painter.draw(r);
        painter.stroke();
    }

    // NOLINTNEXTLINE(bugprone-parent-virtual-call)
    Frame::draw(painter, rect);
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
}

void SelectableGrid::deserialize(const std::string& name, const std::string& value,
                                 const Serializer::Attributes& attrs)
{
    if (name == "selected_column")
        m_selected_column = std::stoul(value);
    else if (name == "selected_row")
        m_selected_row = std::stoul(value);
    else
        StaticGrid::deserialize(name, value, attrs);
}

}
}
