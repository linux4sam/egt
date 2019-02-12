/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/grid.h"
#include "egt/painter.h"
#include <algorithm>

using namespace std;

namespace egt
{
inline namespace v1
{
static auto staticgrid_id = 0;

StaticGrid::StaticGrid(const Rect& rect, int columns,
                       int rows, int spacing)
    : Frame(rect, widgetmask::NO_BACKGROUND),
      m_spacing(spacing)
{
    set_boxtype(Theme::boxtype::none);

    m_cells.resize(columns);
    for (auto& x : m_cells)
        x.resize(rows);

    ostringstream ss;
    ss << "StaticGrid" << staticgrid_id++;
    set_name(ss.str());
}

StaticGrid::StaticGrid(int columns, int rows, int spacing)
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
        if (palette().color(Palette::BORDER) != Color::TRANSPARENT)
        {
            painter.set_color(palette().color(Palette::BORDER));
            painter.set_line_width(m_spacing);

            auto columns = m_cells.size();
            for (size_t column = 0; column < columns; column++)
            {
                auto rows = m_cells[column].size();
                for (size_t row = 0; row < rows; row++)
                {
                    Rect r = cell_rect(columns, rows, w(), h(), column, row, m_spacing);
                    r += point();
                    painter.rectangle(r);
                }
            }

            painter.stroke();
        }
    }

    Frame::draw(painter, rect);
}

Widget* StaticGrid::add(Widget* widget, int column, int row, alignmask align)
{
    Frame::add(widget);

    if (column >= (int)m_cells.size())
        m_cells.resize(column + 1);

    if (row >= (int)m_cells[column].size())
        m_cells[column].resize(row + 1);

    Cell cell;
    cell.widget = widget;
    if (widget)
        widget->set_align(align);
    m_cells[column][row] = cell;

    m_last_add_column = column;
    m_last_add_row = row;

    reposition();

    return widget;
}

Widget* StaticGrid::add(Widget* widget, alignmask align)
{
    int c = 0;
    for (auto& column : m_cells)
    {
        auto i = std::find_if(column.begin(), column.end(),
                              [](const Cell & cell)
        {
            return !cell.widget;
        });

        if (i != column.end())
        {
            i->widget = widget;

            Frame::add(widget);

            if (widget)
                widget->set_align(align);

            reposition();

            m_last_add_column = c;
            m_last_add_row = std::distance(column.begin(), i);

            return widget;
        }

        c++;
    }

    return nullptr;
}

Widget* StaticGrid::get(const Point& point)
{
    return m_cells[point.x][point.y].widget;
}

void StaticGrid::remove(Widget* widget)
{
    assert(widget);
    if (!widget)
        return;

    for (auto& x : m_cells)
    {
        auto i = std::remove_if(x.begin(), x.end(),
                                [widget](const StaticGrid::Cell & cell)
        {
            return cell.widget == widget;
        });
        x.erase(i, x.end());
    }

    reposition();

    Frame::remove(widget);
}

void StaticGrid::reposition()
{
    if (size().empty())
        return;

    int columns = m_cells.size();
    for (int column = 0; column < columns; column++)
    {
        int rows = m_cells[column].size();
        for (int row = 0; row < rows; row++)
        {
            Cell& cell = m_cells[column][row];
            if (cell.widget)
            {
                Rect bounding = cell_rect(columns, rows, w(), h(), column, row, m_spacing);

                bounding += Point(detail::round(m_spacing, 2), detail::round(m_spacing, 2));
                bounding -= Size(m_spacing, m_spacing);

                if (bounding.size().empty())
                    continue;

                // get the aligning rect
                Rect target = detail::align_algorithm(cell.widget->box().size(),
                                                      bounding,
                                                      cell.widget->align());

                // reposition/resize widget
                cell.widget->move(target.point());
                cell.widget->resize(target.size());
            }
        }
    }

    damage();
}

StaticGrid::~StaticGrid()
{}

void SelectableGrid::draw(Painter& painter, const Rect& rect)
{
    if (m_spacing > 0)
    {
        painter.set_color(palette().color(Palette::HIGHLIGHT));
        auto line_width = m_spacing / 2;
        if (line_width <= 0)
            line_width = m_spacing;
        painter.set_line_width(line_width);

        size_t column = m_selected_column;
        size_t row = m_selected_row;
        auto columns = m_cells.size();
        auto rows = m_cells[column].size();

        Rect rect = cell_rect(columns, rows, w(), h(), column, row, m_spacing);
        painter.rectangle(rect);
        painter.stroke();
    }

    Frame::draw(painter, rect);
}

}
}
