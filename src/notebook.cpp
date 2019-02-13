/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "egt/notebook.h"
#include <algorithm>
#include <sstream>

using namespace std;

namespace egt
{
inline namespace v1
{

Notebook::Notebook(const Rect& rect)
    : Frame(rect, {widgetflag::NO_BACKGROUND})
{
    set_boxtype(Theme::boxtype::none);

    static auto notebook_id = 0;
    ostringstream ss;
    ss << "Notebook" << notebook_id++;
    set_name(ss.str());
}

NotebookTab* Notebook::add(NotebookTab* widget)
{
    if (!widget)
        return widget;

    Cell cell;
    cell.widget = widget;
    m_cells.push_back(cell);

    widget->resize(size());
    widget->set_align(alignmask::EXPAND);

    Frame::add(widget);

    if (m_current_index < 0)
    {
        m_current_index = 0;
        widget->show();
    }
    else
    {
        widget->hide();
    }

    return widget;
}

void Notebook::remove(Widget* widget)
{
    assert(widget);
    if (!widget)
        return;

    auto i = std::remove_if(m_cells.begin(), m_cells.end(),
                            [widget](const Notebook::Cell & cell)
    {
        return cell.widget == widget;
    });
    m_cells.erase(i, m_cells.end());

    Frame::remove(widget);

    if (m_current_index >= static_cast<int>(m_cells.size()))
    {
        if (m_cells.size())
            set_select(m_cells.size() - 1);
        else
            m_current_index = -1;
    }
}

void Notebook::set_select(uint32_t index)
{
    if (m_cells.empty())
        return;

    if (static_cast<int>(index) != m_current_index)
    {
        if (index < m_cells.size())
        {
            if (m_cells[m_current_index].widget->leave())
            {
                m_cells[m_current_index].widget->hide();
                m_current_index = index;
                m_cells[m_current_index].widget->enter();
                m_cells[m_current_index].widget->show();

                invoke_handlers(eventid::PROPERTY_CHANGED);
            }
        }
    }
}

Notebook::~Notebook()
{
}

}
}
