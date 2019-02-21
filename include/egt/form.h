/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_FORM_H
#define EGT_FORM_H

/**
 * @file
 * @brief Working with forms.
 */

#include <egt/frame.h>
#include <egt/grid.h>
#include <egt/sizer.h>

namespace egt
{
inline namespace v1
{

namespace experimental
{

/**
 * Help with the creation is standard layout forms.
 *
 */
class Form : public Frame
{
public:

    Form(const std::string& title)
        : Frame(),
          m_vsizer(orientation::vertical)
    {
        add(&vsizer);

        if (!title.empty())
        {
            auto label = make_shared<Label>(caption);
            m_store.push_back(label);

            label->set_align(alignmask::expand_horizontal);
            m_vsizer.add(label);
        }
    }

    void add_group(const std::string& caption)
    {
        auto label = make_shared<Label>(caption);
        m_store.push_back(label);

        label->font().size(label->font().size() + 5);
        label->font().weight(weightid::BOLD);
        label->set_align(alignmask::expand);
        m_vsizer.add(label);
    }

    void add_option(const std::string& name, Widget& widget)
    {
        widget->set_align(alignmask::expand);

        auto label = make_shared<Label>(name);
        m_store.push_back(label);

        auto grid = new StaticGrid(Rect(Point(), Size(0, 50), 1, 2);
                                   grid->set_align(alignmask::expand_horizontal);
                                   grid->add(label);
                                   grid->add(widget);
                                   m_vsizer.add(grid);
    }

    void add_option(Widget& widget)
    {
        widget->set_align(alignmask::expand);
        m_vsizer.add(widget);
    }

protected:

    BoxSizer m_vsizer;
    std::vector<std::shared_ptr<Widget>> m_store;
};

}
}
}

#endif
