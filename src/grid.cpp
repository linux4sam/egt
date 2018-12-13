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
    StaticGrid::StaticGrid(const Rect& rect, int columns,
                           int rows, int spacing)
        : Frame(rect, widgetmask::NO_BACKGROUND),
          m_spacing(spacing)
    {
        set_boxtype(Theme::boxtype::none);
        palette().set(Palette::BORDER, Palette::GROUP_NORMAL, Color::TRANSPARENT);

        m_cells.resize(columns);
        for (auto& x : m_cells)
            x.resize(rows);
    }

    namespace detail
    {
        static inline int ceil(int x, int y)
        {
            return (x + y - 1) / y;
        }
    }

    /*
     * Calculates the rectangle for a cell. This calculates the rectangle right
     * down the center of any spacing if one exists.
     */
    static Rect cell_rect(int columns, int rows, int width, int height,
                          int column, int row, int spacing)
    {
        auto inner_width = detail::ceil((width - ((columns + 1) * spacing)), columns);
        auto inner_height = detail::ceil((height - ((rows + 1) * spacing)), rows);

        auto ix = (column * spacing) + (column * inner_width) + detail::ceil(spacing, 2);
        auto iy = (row * spacing) + (row * inner_height) + detail::ceil(spacing, 2);
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
        cell.align = align;
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
                i->align = align;

                Frame::add(widget);

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

                    bounding += Point(detail::ceil(m_spacing, 2), detail::ceil(m_spacing, 2));
                    bounding -= Size(m_spacing, m_spacing);

                    // get the aligning rect
                    Rect target = align_algorithm(cell.widget->box().size(),
                                                  bounding,
                                                  cell.align);

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
