/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/alignment.h"
#include "egt/grid.h"
#include "egt/painter.h"
#include <algorithm>

using namespace std;

namespace egt
{
inline namespace v1
{

StaticGrid::StaticGrid(const Rect& rect, const Tuple& size,
                       default_dim_type border) noexcept
    : Frame(rect)
{
    set_name("StaticGrid" + std::to_string(m_widgetid));
    set_border(border);

    /*
     * The grid size is set here.  Every column should be the same size.  Don't
     * delete from the column vectors.  Only set empty cells to nullptr.
     */
    m_cells.resize(size.width());
    for (auto& x : m_cells)
        x.resize(size.height(), {});
}

void StaticGrid::reallocate(const Tuple& size)
{
    m_cells.resize(size.width());
    for (auto& x : m_cells)
        x.resize(size.height(), {});
}

StaticGrid::StaticGrid(const Tuple& size, default_dim_type border) noexcept
    : StaticGrid(Rect(), size, border)
{
}

StaticGrid::StaticGrid(Frame& parent, const Rect& rect,
                       const Tuple& size, default_dim_type border) noexcept
    : StaticGrid(rect, size, border)
{
    parent.add(*this);
}

StaticGrid::StaticGrid(Frame& parent, const Tuple& size, default_dim_type border) noexcept
    : StaticGrid(size, border)
{
    parent.add(*this);
}

namespace detail
{
static inline default_dim_type round(default_dim_type x, default_dim_type y)
{
    return x  / y;
}
}

/*
 * Calculates the rectangle for a cell. This calculates the rectangle right
 * down the center of any border_width if one exists.
 */
static Rect cell_rect(int columns, int rows, default_dim_type width, default_dim_type height,
                      int column, int row, int border_width = 0, int padding = 0)
{
    auto inner_width = detail::round((width - ((columns + 1) * border_width)), columns);
    auto inner_height = detail::round((height - ((rows + 1) * border_width)), rows);

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

    return Rect(ix, iy, iw, ih);
}

void StaticGrid::draw(Painter& painter, const Rect& rect)
{
    if (grid_flags().is_set(flag::show_border) && border() > 0)
    {
        auto b = content_area();

        painter.set(color(Palette::ColorId::border).color());
        painter.set_line_width(border());

        auto columns = m_cells.size();
        for (size_t column = 0; column < columns; column++)
        {
            auto rows = m_cells[column].size();
            for (size_t row = 0; row < rows; row++)
            {
                Rect r = cell_rect(columns, rows, b.width(), b.height(), column, row, border());
                r += b.point();
                painter.draw(r);
            }
        }

        painter.stroke();
    }

    Frame::draw(painter, rect);
}

void StaticGrid::add(const std::shared_ptr<Widget>& widget)
{
    if (!widget)
        return;

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

void StaticGrid::add(const std::shared_ptr<Widget>& widget, int column, int row)
{
    Frame::add(widget);

    if (column >= static_cast<int>(m_cells.size()))
        m_cells.resize(column + 1);

    if (row >= static_cast<int>(m_cells[column].size()))
    {
        for (auto& c : m_cells)
            c.resize(row + 1, {});
    }

    m_cells[column][row] = widget;

    m_last_add_column = column;
    m_last_add_row = row;

    reposition();
}

Widget* StaticGrid::get(const Point& point)
{
    auto widget = m_cells[point.x()][point.y()].lock();
    if (widget)
        return widget.get();
    return nullptr;
}

void StaticGrid::remove(Widget* widget)
{
    assert(widget);
    if (!widget)
        return;

    auto predicate = [widget](const std::weak_ptr<Widget>& ptr)
    {
        auto w = ptr.lock();
        return w.get() == widget;
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
                Rect bounding = cell_rect(columns, rows, b.width(), b.height(), column, row, border(), padding());
                bounding += b.point() - point();

                if (border())
                {
                    bounding += Point(detail::round(border(), 2), detail::round(border(), 2));
                    bounding -= Size(border(), border());
                }

                if (bounding.size().empty())
                    continue;

                // get the aligning rect
                Rect target = detail::align_algorithm(widget->box().size(),
                                                      bounding,
                                                      widget->align());

                // re-position/resize widget
                widget->move(target.point());
                widget->resize(target.size());

                if (widget->flags().is_set(Widget::flag::frame))
                {
                    auto frame = dynamic_cast<Frame*>(widget.get());
                    frame->layout();
                }
            }
        }
    }

    damage();
}

void SelectableGrid::handle(Event& event)
{
    StaticGrid::handle(event);

    if (event.id() == eventid::raw_pointer_down)
    {
        auto b = content_area().size();
        Point pos = display_to_local(event.pointer().point);

        auto columns = m_cells.size();
        for (size_t column = 0; column < columns; column++)
        {
            auto rows = m_cells[column].size();
            for (size_t row = 0; row < rows; row++)
            {
                Rect bounding = cell_rect(columns, rows, b.width(), b.height(), column, row, border(), padding());

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
                    set_selected(column, row);
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
        painter.set(color(Palette::ColorId::border).color());
        auto line_width = border() / 2;
        if (line_width <= 0)
            line_width = border();
        painter.set_line_width(line_width);

        size_t column = m_selected_column;
        size_t row = m_selected_row;
        auto columns = m_cells.size();
        auto rows = m_cells[column].size();

        auto b = content_area();

        Rect r = cell_rect(columns, rows, b.width(), b.height(), column, row, border(), padding());
        r += b.point();

        painter.draw(r);
        painter.stroke();
    }

    Frame::draw(painter, rect);
}

void SelectableGrid::set_selected(int column, int row)
{
    const auto columns = m_cells.size();
    if (column >= static_cast<ssize_t>(columns))
        return;

    if (!columns || row >= static_cast<ssize_t>(m_cells[column].size()))
        return;

    auto c = detail::change_if_diff<>(m_selected_column, column);
    auto r = detail::change_if_diff<>(m_selected_row, row);
    if (c || r)
        damage();
}

}
}
