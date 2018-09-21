/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_GRID_H
#define MUI_GRID_H

/**
 * @file
 * @brief Working with grids.
 */

#include <mui/widget.h>

namespace mui
{

    /**
     * A static grid that does not perform any drawing, but controls the
     * size and position of any widget added.
     */
    class StaticGrid : public Widget
    {
    public:
        StaticGrid(int x, int y, int w, int h, int columns, int rows, int border = 0)
            : Widget(x, y, w, h),
              m_columns(columns),
              m_rows(rows),
              m_border(border)
        {}

        virtual void draw(const Rect& rect) {}
        virtual void damage() {}

        virtual void position(int x, int y)
        {
            Widget::position(x, y);
            reposition();
        }

        virtual void size(int w, int h)
        {
            Widget::size(w, h);
            reposition();
        }

        // TODO: this should really support per-cell/widget properties for alignment or expand
        virtual Widget* add(Widget* widget, int column, int row, uint32_t align = ALIGN_EXPAND)
        {
            if (column >= (int)m_widgets.size())
                m_widgets.resize(column + 1);

            if (row >= (int)m_widgets[column].size())
                m_widgets[column].resize(row + 1);

            Cell cell;
            cell.widget = widget;
            cell.align = align;
            m_widgets[column][row] = cell;

            return widget;
        }

        /**
         * Reposition all child widgets.
         */
        virtual void reposition()
        {
            for (int column = 0; column < (int)m_widgets.size(); column++)
            {
                for (int row = 0; row < (int)m_widgets[column].size(); row++)
                {
                    Cell& cell = m_widgets[column][row];
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
                        cell.widget->position(target.x, target.y);
                        cell.widget->resize(target.w, target.h);
                    }
                }
            }
        }

        virtual ~StaticGrid()
        {}

    protected:
        int m_columns;
        int m_rows;
        int m_border;

        struct Cell
        {
            Widget* widget;
            uint32_t align;
        };

        std::vector<std::vector<Cell>> m_widgets;
    };


}

#endif
