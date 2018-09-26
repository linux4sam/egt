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
    class StaticGrid : public Frame
    {
    public:
        StaticGrid(const Point& point, const Size& size, int columns,
                   int rows, int border = 0)
            : Frame(point, size, FLAG_NO_BACKGROUND),
              m_columns(columns),
              m_rows(rows),
              m_border(border)
        {}

        virtual void position(int x, int y)
        {
            Frame::position(x, y);
            reposition();
        }

        virtual void size(int w, int h)
        {
            Frame::size(w, h);
            reposition();
        }

        virtual Widget* add(Widget* widget, int column, int row,
                            uint32_t align = ALIGN_EXPAND)
        {
            Frame::add(widget);

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

        virtual void remove(Widget* widget)
        {
            // TODO
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

        typedef std::vector<std::vector<Cell>> widget_array;

        widget_array m_widgets;

    private:

        // not allowed, use alternate add() method
        virtual void add(Widget* widget) {}
    };

    class HorizontalPositioner : public Frame
    {
    public:

        HorizontalPositioner(const Point& point, const Size& size,
                             int border = 0, int align = ALIGN_CENTER)
            : Frame(point, size, FLAG_NO_BACKGROUND),
              m_border(border),
              m_align(align)
        {}

        virtual void position(int x, int y)
        {
            Frame::position(x, y);
            reposition();
        }

        virtual void size(int w, int h)
        {
            Frame::size(w, h);
            reposition();
        }

        /**
         * Reposition all child widgets.
         */
        virtual void reposition()
        {
            // align everything in center
            int width = 0;
            for (auto& child : m_children)
                width += child->w() + m_border;

            int offset = w() / 2 - width / 2;
            for (auto& child : m_children)
            {
                if (child)
                {
                    Point p;
                    if (m_align & ALIGN_CENTER)
                    {
                        p.y = y() + (h() / 2) - (child->h() / 2);
                    }

                    if (m_align & ALIGN_TOP)
                        p.y = y();
                    if (m_align & ALIGN_BOTTOM)
                        p.y = y() + h() - child->h();

                    child->position(x() + offset + m_border, p.y);
                    offset += (child->w() + m_border);
                }
            }

            damage();
        }

        virtual ~HorizontalPositioner()
        {}

    protected:
        int m_border;
        int m_align;
    };

}

#endif
