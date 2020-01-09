/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/notebook.h"
#include <algorithm>
#include <string>

namespace egt
{
inline namespace v1
{

void NotebookTab::select()
{
    if (parent())
    {
        auto notebook = dynamic_cast<Notebook*>(parent());
        if (notebook)
            notebook->selected(this);
    }
}

Notebook::Notebook(const Rect& rect) noexcept
    : Frame(rect)
{
    name("Notebook" + std::to_string(m_widgetid));
}

Notebook::Notebook(Frame& parent, const Rect& rect) noexcept
    : Notebook(rect)
{
    parent.add(*this);
}

void Notebook::add(std::shared_ptr<Widget> widget)
{
    if (!widget)
        return;

    if (widget.get() == this)
        throw std::runtime_error("cannot add a widget to itself");

    assert(!widget->parent() && "widget already has parent!");

    auto cell = std::dynamic_pointer_cast<NotebookTab>(widget);
    if (!cell)
        throw std::invalid_argument("only NotebookTab can be added to a Notebook");

    m_cells.push_back(cell);

    widget->align(AlignFlag::expand);

    Frame::add(widget);

    if (m_selected < 0)
    {
        m_selected = 0;
        widget->show();
    }
    else
    {
        widget->hide();
    }

    layout();
}

void Notebook::remove(Widget* widget)
{
    assert(widget);
    if (!widget)
        return;

    auto i = std::remove_if(m_cells.begin(), m_cells.end(),
                            [widget](const std::weak_ptr<NotebookTab>& cell)
    {
        auto w = cell.lock();
        if (w)
            return w.get() == widget;
        return false;
    });
    m_cells.erase(i, m_cells.end());

    Frame::remove(widget);

    if (m_selected >= static_cast<int>(m_cells.size()))
    {
        if (!m_cells.empty())
            selected(m_cells.size() - 1);
        else
            m_selected = -1;
    }
}

void Notebook::selected(size_t index)
{
    if (m_cells.empty())
        return;

    if (index >= m_cells.size())
        return;

    if (static_cast<int>(index) != m_selected)
    {
        if (m_selected >= 0 &&
            m_selected < static_cast<int>(m_cells.size()))
        {
            auto from = m_cells[m_selected].lock();
            if (from)
            {
                if (!from->leave())
                    return;
                from->hide();
            }
        }

        m_selected = index;
        auto to = m_cells[m_selected].lock();
        if (to)
        {
            to->enter();
            to->show();
        }

        on_selected_changed.invoke();
    }
}

void Notebook::selected(Widget* widget)
{
    auto predicate = [widget](const std::weak_ptr<NotebookTab>& ptr)
    {
        auto w = ptr.lock();
        if (w)
            return w.get() == widget;
        return false;
    };

    auto i = std::find_if(m_cells.begin(), m_cells.end(), predicate);
    if (i != m_cells.end())
        selected(std::distance(m_cells.begin(), i));
}

NotebookTab* Notebook::get(size_t index) const
{
    if (index < m_cells.size())
    {
        auto w = m_cells[index].lock();
        if (w)
            return w.get();
    }

    return nullptr;
}

}
}
