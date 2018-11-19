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
                           int rows, int border)
        : Frame(rect, widgetmask::NO_BACKGROUND | widgetmask::NO_BORDER),
          m_border(border)
    {
        m_cells.resize(columns);
        for (auto& x : m_cells)
            x.resize(rows);
    }

    namespace detail
    {
        static inline int ceil(int x, int y)
        {
            //return x / y;
            return (x + y - 1) / y;
        }
    }

    void StaticGrid::draw(Painter& painter, const Rect& rect)
    {
        if (m_border > 0)
        {
            if (!is_flag_set(widgetmask::NO_BORDER))
            {
                painter.set_color(palette().color(Palette::BORDER));
                painter.set_line_width(m_border);

                auto columns = m_cells.size();
                for (size_t column = 0; column < columns; column++)
                {
                    auto rows = m_cells[column].size();
                    for (size_t row = 0; row < rows; row++)
                    {
                        // find the rect for the cell
                        int iw = detail::ceil(w(), columns);
                        int ih = detail::ceil(h(), rows);

                        if (iw < 0)
                            iw = 0;

                        if (ih < 0)
                            ih = 0;

                        int ix = x() + (column * iw);
                        int iy = y() + (row * ih);

                        painter.rectangle(Rect(ix, iy, iw, ih));
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

        reposition();

        return widget;
    }

    Widget* StaticGrid::add(Widget* widget, alignmask align)
    {
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

                return widget;
            }
        }

        return nullptr;
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
                    // find the rect for the cell

                    int ix = x() + (column * (w() / columns)) + detail::ceil(m_border, 2);
                    int iy = y() + (row * (h() / rows)) + detail::ceil(m_border, 2);
                    int iw = (w() / columns) - (m_border);
                    int ih = (h() / rows) - (m_border);
                    /*
                                        int iw = detail::ceil(w(), columns) - (m_border * 2);
                                        int ih = detail::ceil(h(), rows) - (m_border * 2);
                    */
                    if (iw < 0)
                        iw = 0;

                    if (ih < 0)
                        ih = 0;
                    /*
                                        int ix = x() + (column * iw) + m_border;
                                        int iy = y() + (row * ih) + m_border;
                    */

                    Rect bounding(ix, iy, iw, ih);

                    //cout << bounding << endl;

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

}
