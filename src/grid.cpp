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

StaticGrid::StaticGrid(const Rect& rect, int columns,
                       int rows, int spacing) noexcept
    : Frame(rect),
      m_spacing(spacing)
{
    set_name("StaticGrid" + std::to_string(m_widgetid));

    set_boxtype(Theme::boxtype::none);

    /*
     * The grid size is set here.  Every column should be the same size.  Don't
     * delete from the column vectors.  Only set empty cells to nullptr.
     */
    m_cells.resize(columns);
    for (auto& x : m_cells)
        x.resize(rows, nullptr);
}

StaticGrid::StaticGrid(int columns, int rows, int spacing) noexcept
    : StaticGrid(Rect(), columns, rows, spacing)
{
}

namespace detail
{
static inline default_dim_type round(default_dim_type x, default_dim_type y)
{
    //return (x + y - 1) / y;
    return x  / y;
}
}

/*
 * Calculates the rectangle for a cell. This calculates the rectangle right
 * down the center of any spacing if one exists.
 */
static Rect cell_rect(int columns, int rows, default_dim_type width, default_dim_type height,
                      int column, int row, int spacing)
{
    auto inner_width = detail::round((width - ((columns + 1) * spacing)), columns);
    auto inner_height = detail::round((height - ((rows + 1) * spacing)), rows);

    auto ix = (column * spacing) + (column * inner_width) + detail::round(spacing, 2);
    auto iy = (row * spacing) + (row * inner_height) + detail::round(spacing, 2);
    auto iw = inner_width + spacing;
    auto ih = inner_height + spacing;

    if (iw < 0)
        iw = 0;

    if (ih < 0)
        ih = 0;

    return Rect(ix, iy, iw, ih);
}

void StaticGrid::draw(Painter& painter, const Rect& rect)
{
    if (m_spacing > 0)
    {
        if (palette().color(Palette::ColorId::border) != Palette::transparent)
        {
            painter.set(palette().color(Palette::ColorId::border));
            painter.set_line_width(m_spacing);

            auto columns = m_cells.size();
            for (size_t column = 0; column < columns; column++)
            {
                auto rows = m_cells[column].size();
                for (size_t row = 0; row < rows; row++)
                {
                    Rect r = cell_rect(columns, rows, w(), h(), column, row, m_spacing);
                    r += point();
                    painter.draw(r);
                }
            }

            painter.stroke();
        }
    }

    Frame::draw(painter, rect);
}

void StaticGrid::add(const std::shared_ptr<Widget>& widget)
{
    if (!widget)
        return;

    int c = 0;
    for (auto& column : m_cells)
    {
        auto i = std::find_if(column.begin(), column.end(),
                              [](const Widget * widget)
        {
            return !widget;
        });

        if (i != column.end())
        {
            *i = widget.get();

            Frame::add(widget);

            reposition();

            m_last_add_column = c;
            m_last_add_row = std::distance(column.begin(), i);

            break;
        }

        c++;
    }
}

void StaticGrid::add(const std::shared_ptr<Widget>& widget, int column, int row)
{
    Frame::add(widget);

    if (column >= (int)m_cells.size())
        m_cells.resize(column + 1);

    if (row >= (int)m_cells[column].size())
    {
        for (auto& column : m_cells)
            column.resize(row + 1, nullptr);
    }

    m_cells[column][row] = widget.get();

    m_last_add_column = column;
    m_last_add_row = row;

    reposition();
}

Widget* StaticGrid::get(const Point& point)
{
    return m_cells[point.x][point.y];
}

void StaticGrid::remove(Widget* widget)
{
    assert(widget);
    if (!widget)
        return;

    auto predicate = [widget](const Widget * ptr)
    {
        return ptr == widget;
    };

    for (auto& x : m_cells)
    {
        for (auto i = std::find_if(x.begin(), x.end(), predicate); i != x.end();
             i = std::find_if(x.begin(), x.end(), predicate))
        {
            *i = nullptr;
        }
    }

    reposition();

    Frame::remove(widget);
}

void StaticGrid::reposition()
{
    if (size().empty())
        return;

    auto columns = m_cells.size();
    for (size_t column = 0; column < columns; column++)
    {
        auto rows = m_cells[column].size();
        for (size_t row = 0; row < rows; row++)
        {
            Widget* widget = m_cells[column][row];
            if (widget)
            {
                Rect bounding = cell_rect(columns, rows, w(), h(), column, row, m_spacing);

                bounding += Point(detail::round(m_spacing, 2), detail::round(m_spacing, 2));
                bounding -= Size(m_spacing, m_spacing);

                if (bounding.size().empty())
                    continue;

                // get the aligning rect
                Rect target = detail::align_algorithm(widget->box().size(),
                                                      bounding,
                                                      widget->align());

                // reposition/resize widget
                widget->move(target.point());
                widget->resize(target.size());
            }
        }
    }

    damage();
}

StaticGrid::~StaticGrid() noexcept
{
}

void SelectableGrid::draw(Painter& painter, const Rect& rect)
{
    if (m_spacing > 0)
    {
        painter.set(palette().color(Palette::ColorId::highlight));
        auto line_width = m_spacing / 2;
        if (line_width <= 0)
            line_width = m_spacing;
        painter.set_line_width(line_width);

        size_t column = m_selected_column;
        size_t row = m_selected_row;
        auto columns = m_cells.size();
        auto rows = m_cells[column].size();

        Rect rect = cell_rect(columns, rows, w(), h(), column, row, m_spacing);
        painter.draw(rect);
        painter.stroke();
    }

    Frame::draw(painter, rect);
}

}
}
