/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/layout.h"
#include "egt/sizer.h"

namespace egt
{
inline namespace v1
{

void BoxSizer::layout()
{
    if (!visible())
        return;

    if (m_in_layout)
        return;

    m_in_layout = true;
    detail::scope_exit reset([this]() { m_in_layout = false; });

    resize(super_rect());

    detail::flex_layout(this, 1);

    for (auto& child : m_children)
    {
        if (child->flags().is_set(Widget::flag::frame))
        {
            auto frame = dynamic_cast<Frame*>(child.get());
            frame->layout();
        }
    }

    resize(super_rect());
}

}
}
