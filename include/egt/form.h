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
#include <egt/label.h>
#include <egt/sizer.h>
#include <memory>
#include <string>
#include <vector>

namespace egt
{
inline namespace v1
{

namespace experimental
{

/**
 * Utility class to help with standard form layout of widgets.
 */
class Form : public Frame
{
public:

    explicit Form(const std::string& title = {}) noexcept
        : m_vsizer(orientation::vertical)
    {
        set_name("Form" + std::to_string(m_widgetid));

        m_vsizer.set_align(alignmask::expand);
        add(m_vsizer);

        if (!title.empty())
        {
            auto label = std::make_shared<Label>(title, alignmask::center);
            label->set_font(Font(label->font().face(),
                                 label->font().size() + 4,
                                 Font::weightid::bold,
                                 label->font().slant()));
            label->set_align(alignmask::expand_horizontal);
            m_vsizer.add(label);
        }
    }

    explicit Form(Frame& parent, const std::string& title = {}) noexcept
        : Form(title)
    {
        parent.add(*this);
    }

    virtual void add_group(const std::string& caption)
    {
        auto label = std::make_shared<Label>(caption);
        label->set_font(Font(label->font().face(),
                             label->font().size(),
                             Font::weightid::bold,
                             label->font().slant()));
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
        grid->resize(Size(0, widget->min_size_hint().height()));
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
        grid->resize(Size(0, widget->min_size_hint().height()));
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

    /**
     * Internal BoxSizer used for layout.
     */
    BoxSizer m_vsizer;

    /**
     * Array of options.
     *
     * @todo This will keep pointers around to deleted child widgets.
     */
    std::vector<Widget*> m_options;
};

}
}
}

#endif
