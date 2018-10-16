/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "mui/grid.h"
#include <algorithm>

using namespace std;

namespace mui
{
    StaticGrid::StaticGrid(const Point& point, const Size& size, int columns,
                           int rows, int border)
        : Frame(point, size, widgetmask::NO_BACKGROUND),
          m_columns(columns),
          m_rows(rows),
          m_border(border)
    {}

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

        return widget;
    }

    void StaticGrid::remove(Widget* widget)
    {
        assert(widget);
        if (!widget)
            return;

        for (auto& x : m_cells)
            std::remove_if(x.begin(), x.end(),
                           [widget](const StaticGrid::Cell & cell)
        {
            return cell.widget == widget;
        });

        Frame::remove(widget);
    }

    void StaticGrid::reposition()
    {
        for (int column = 0; column < (int)m_cells.size(); column++)
        {
            for (int row = 0; row < (int)m_cells[column].size(); row++)
            {
                Cell& cell = m_cells[column][row];
                if (cell.widget)
                {
                    // find the rect for the cell
                    int ix = x() + (column * (w() / m_columns)) + m_border;
                    int iy = y() + (row * (h() / m_rows)) + m_border;
                    int iw = (w() / m_columns) - (m_border * 2) - 1;
                    int ih = (h() / m_rows) - (m_border * 2) - 1;

                    // get the aligning rect
                    Rect target = align_algorithm(cell.widget->box().size(),
                                                  Rect(ix, iy, iw, ih),
                                                  cell.align);

                    // reposition/resize widget
                    cell.widget->move(target.point());
                    cell.widget->resize(target.size());
                }
            }
        }
    }

    StaticGrid::~StaticGrid()
    {}

}
