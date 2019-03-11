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
#include <memory>

namespace egt
{
inline namespace v1
{

namespace experimental
{

/**
 * Help with the creation is standard for layouts.
 *
 */
class Form : public Frame
{
public:

    explicit Form(const std::string& title = std::string()) noexcept
        : Frame(),
          m_vsizer(orientation::vertical)
    {
        set_name("Form" + std::to_string(m_widgetid));

        m_vsizer.set_align(alignmask::expand);
        add(m_vsizer);

        if (!title.empty())
        {
            auto label = std::make_shared<Label>(title,
                                                 alignmask::center,
                                                 Font(Font::weightid::BOLD));
            label->font().size(label->font().size() + 5);
            label->set_align(alignmask::expand_horizontal);
            m_vsizer.add(label);
        }
    }

    explicit Form(Frame& parent, const std::string& title = std::string()) noexcept
        : Form(title)
    {
        parent.add(*this);
    }

    virtual void add_group(const std::string& caption)
    {
        auto label = std::make_shared<Label>(caption);
        label->font().weight(Font::weightid::BOLD);
        label->set_align(alignmask::expand_horizontal);
        label->set_text_align(alignmask::bottom | alignmask::left);
        m_vsizer.add(label);
        m_options.push_back(label.get());
    }

    template<class T>
    void add_option(const std::string& name, const std::shared_ptr<T>& widget)
    {
        auto p = std::dynamic_pointer_cast<Widget>(widget);
        add_option(name, p);
    }

    virtual void add_option(const std::string& name, const std::shared_ptr<Widget>& widget)
    {
        widget->set_align(alignmask::expand);
        auto label = std::make_shared<Label>(name);
        label->set_align(alignmask::expand);
        label->set_text_align(alignmask::center | alignmask::left);
        auto grid = std::make_shared<StaticGrid>(Tuple(2, 1));
        grid->set_margin(2); /// @todo Not working
        grid->resize(Size(0, 20));
        grid->set_align(alignmask::expand_horizontal);
        grid->add(label);
        grid->add(widget);
        m_vsizer.add(grid);
        m_options.push_back(grid.get());
    }

    virtual void add_option(const std::shared_ptr<Widget>& widget)
    {
        widget->set_align(alignmask::expand);
        auto grid = std::make_shared<StaticGrid>(Tuple(1, 1));
        grid->set_margin(2); /// @todo Not working
        grid->resize(Size(0, 20));
        grid->set_align(alignmask::expand_horizontal);
        grid->add(widget);
        m_vsizer.add(grid);
        m_options.push_back(grid.get());
    }

    virtual void remove_options()
    {
        for (auto option : m_options)
            m_vsizer.remove(option);
    }

protected:

    BoxSizer m_vsizer;
    std::vector<Widget*> m_options;
};

}
}
}

#endif
