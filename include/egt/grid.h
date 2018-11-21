/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_GRID_H
#define EGT_GRID_H

/**
 * @file
 * @brief Working with grids.
 */

#include <egt/widget.h>
#include <egt/frame.h>
#include <egt/utils.h>

namespace egt
{

    /**
     * Static grid organization for widgets.
     *
     * A static grid that does not perform any drawing, but controls the
     * size and position of any widget added into a grid.
     */
    class StaticGrid : public Frame
    {
    public:
        StaticGrid(const Rect& rect = Rect(),
                   int columns = 1, int rows = 1, int spacing = 0);

        virtual void move(const Point& point) override
        {
            Frame::move(point);
            reposition();
        }

        virtual void resize(const Size& size) override
        {
            Frame::resize(size);
            reposition();
        }

        virtual void draw(Painter& painter, const Rect& rect) override;

        /**
         * Add a widget to the grid into a specific cell with an optional
         * alignment within the cell.
         *
         * This will automatically resize the grid to fit the widget as
         * necessary.
         */
        virtual Widget* add(Widget* widget, int column, int row,
                            alignmask align = alignmask::EXPAND);

        /**
         * Add a widget to the next empty cell.
         *
         * This will not automatically resize the grid to fit the widget as
         * necessary.
         */
        virtual Widget* add(Widget* widget,
                            alignmask align = alignmask::EXPAND);

        virtual Widget* get(const Point& point);

        virtual void remove(Widget* widget) override;

        /**
         * Reposition all child widgets.
         *
         * @note You should *not* have to manually call this under normal
         * circumstances.
         */
        virtual void reposition();

        int last_add_column() const
        {
            return m_last_add_column;
        }

        int last_add_row() const
        {
            return m_last_add_row;
        }

        virtual ~StaticGrid();

    protected:

        struct Cell
        {
            // cppcheck-suppress unusedStructMember
            Widget* widget{nullptr};
            alignmask align{alignmask::NONE};
        };

        using cell_array = std::vector<std::vector<Cell>>;

        cell_array m_cells;
        int m_spacing{0};

        int m_last_add_column{0};
        int m_last_add_row{0};
    };

    class HorizontalPositioner : public Frame
    {
    public:

        HorizontalPositioner(const Rect& rect,
                             int spacing = 0, alignmask align = alignmask::CENTER)
            : Frame(rect, widgetmask::NO_BACKGROUND),
              m_spacing(spacing),
              m_align(align)
        {}

        virtual void move(const Point& point) override
        {
            Frame::move(point);
            reposition();
        }

        virtual void resize(const Size& size) override
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
                width += child->w() + m_spacing;

            int offset = w() / 2 - width / 2;
            for (auto& child : m_children)
            {
                if (child)
                {
                    Point p;
                    if ((m_align & alignmask::CENTER) == alignmask::CENTER)
                    {
                        p.y = y() + (h() / 2) - (child->h() / 2);
                    }

                    if ((m_align & alignmask::TOP) == alignmask::TOP)
                        p.y = y();
                    else if ((m_align & alignmask::BOTTOM) == alignmask::BOTTOM)
                        p.y = y() + h() - child->h();

                    child->move(Point(x() + offset + m_spacing, p.y));
                    offset += (child->w() + m_spacing);
                }
            }

            damage();
        }

        virtual ~HorizontalPositioner()
        {}

    protected:
        int m_spacing{0};
        alignmask m_align{alignmask::NONE};
    };

    class SelectableGrid : public StaticGrid
    {
    public:
        SelectableGrid(const Rect& rect = Rect(),
                       int columns = 1, int rows = 1, int spacing = 0)
            : StaticGrid(rect, columns, rows, spacing)
        {
        }

        virtual void draw(Painter& painter, const Rect& rect) override;

        virtual Point selected() const
        {
            return Point(m_selected_column, m_selected_row);
        }

        virtual void select(int column, int row)
        {
            m_selected_column = column;
            m_selected_row = row;
            damage();
        }

        virtual ~SelectableGrid()
        {}

    protected:
        int m_selected_column{0};
        int m_selected_row{0};
    };

}

#endif
