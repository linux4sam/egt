/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/form.h"
#include "egt/grid.h"
#include "egt/label.h"

using namespace std;

namespace egt
{
inline namespace v1
{
namespace experimental
{

Form::Form(const std::string& title) noexcept
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

Form::Form(Frame& parent, const std::string& title) noexcept
    : Form(title)
{
    parent.add(*this);
}

void Form::add_group(const std::string& caption)
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

void Form::add_option(const std::string& name, const std::shared_ptr<Widget>& widget)
{
    if (!widget)
        return;

    assert(!widget->parent() && "widget already has parent!");

    widget->set_align(alignmask::expand);
    auto label = std::make_shared<Label>(name);
    label->set_align(alignmask::expand);
    label->set_text_align(alignmask::center | alignmask::left);
    auto grid = std::make_shared<StaticGrid>(Tuple(2, 1));
    grid->set_margin(2); /// @todo Not working
    auto b = widget->size();
    if (b.height() < widget->min_size_hint().height())
        b.set_height(widget->min_size_hint().height());
    if (b.height() < min_option_height())
        b.set_height(min_option_height());
    grid->resize(Size(0, b.height()));
    grid->set_align(alignmask::expand_horizontal);
    grid->add(label);
    grid->add(widget);
    m_vsizer.add(grid);
    m_options.push_back(grid.get());
}

void Form::add_option(const std::shared_ptr<Widget>& widget)
{
    widget->set_align(alignmask::expand);
    auto grid = std::make_shared<StaticGrid>(Tuple(1, 1));
    grid->set_margin(2); /// @todo Not working
    auto b = widget->size();
    if (b.height() < widget->min_size_hint().height())
        b.set_height(widget->min_size_hint().height());
    if (b.height() < min_option_height())
        b.set_height(min_option_height());
    grid->resize(Size(0, b.height()));
    grid->set_align(alignmask::expand_horizontal);
    grid->add(widget);
    m_vsizer.add(grid);
    m_options.push_back(grid.get());
}

void Form::remove_options()
{
    for (auto option : m_options)
        m_vsizer.remove(option);
}

}
}
}
