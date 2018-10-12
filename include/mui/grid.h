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
#include <mui/frame.h>

namespace mui
{

    /**
     * Grid widget positioning helper.
     *
     * A static grid that does not perform any drawing, but controls the
     * size and position of any widget added into a grid.
     */
    class StaticGrid : public Frame
    {
    public:
        StaticGrid(const Point& point, const Size& size, int columns,
                   int rows, int border = 0);

        virtual void move(const Point& point)
        {
            Frame::move(point);
            reposition();
        }

        virtual void resize(const Size& size)
        {
            Frame::resize(size);
            reposition();
        }

        /**
         * Add a widget to the grid into a specific cell with an optional
         * alignment within the cell.
         */
        virtual Widget* add(Widget* widget, int column, int row,
                            uint32_t align = ALIGN_EXPAND);

        virtual void remove(Widget* widget);

        /**
         * Reposition all child widgets.
         */
        virtual void reposition();

        virtual ~StaticGrid();

    protected:
        int m_columns {0};
        int m_rows {0};
        int m_border {0};

        struct Cell
        {
            Widget* widget {nullptr};
            uint32_t align {Widget::ALIGN_NONE};
        };

        using cell_array = std::vector<std::vector<Cell>>;

        cell_array m_cells;

    private:

        // not allowed, use alternate add() method
        virtual Widget* add(Widget* widget) { return widget; }
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

        virtual void move(const Point& point)
        {
            Frame::move(point);
            reposition();
        }

        virtual void resize(const Size& size)
        {
            Frame::resize(size);
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

                    child->move(Point(x() + offset + m_border, p.y));
                    offset += (child->w() + m_border);
                }
            }

            damage();
        }

        virtual ~HorizontalPositioner()
        {}

    protected:
        int m_border {0};
        int m_align {Widget::ALIGN_NONE};
    };

}

#endif
