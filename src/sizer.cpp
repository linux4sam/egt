/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/enum.h"
#include "egt/detail/layout.h"
#include "egt/serialize.h"
#include "egt/sizer.h"

namespace egt
{
inline namespace v1
{

enum
{
    LAY_LEFT = 0x020,
    // anchor to top item or top side of parent
    LAY_TOP = 0x040,
    // anchor to right item or right side of parent
    LAY_RIGHT = 0x080,
    // anchor to bottom item or bottom side of parent
    LAY_BOTTOM = 0x100,
    // anchor to both left and right item or parent borders
    LAY_HFILL = 0x0a0,
    // anchor to both top and bottom item or parent borders
    LAY_VFILL = 0x140,
    // center horizontally, with left margin as offset
    LAY_HCENTER = 0x000,
    // center vertically, with top margin as offset
    LAY_VCENTER = 0x000,
    // center in both directions, with left/top margin as offset
    LAY_CENTER = 0x000,
    // anchor to all four directions
    LAY_FILL = 0x1e0,
    // When in a wrapping container, put this element on a new line. Wrapping
    // layout code auto-inserts LAY_BREAK flags as needed. See GitHub issues for
    // TODO related to this.
    //
    // Drawing routines can read this via item pointers as needed after
    // performing layout calculations.
    LAY_BREAK = 0x200
};

BoxSizer::BoxSizer(Serializer::Properties& props)
	: Frame(props)
{
    deserialize(props);
}

void BoxSizer::layout()
{
    if (!visible())
        return;

    if (m_in_layout)
        return;

    if (m_children.empty())
        return;

    m_in_layout = true;
    auto reset = detail::on_scope_exit([this]() { m_in_layout = false; });

    auto rect = super_rect();

    // The purpose of the flex orientation is to automatically position
    // the widgets in two dimensions within a given box. Don't expand it now.
    // Let the flex algorithm position the widgets. If the box is too small then
    // it will be expanded. If the user didn't specify a size, let assume he
    // wishes the sizer grow as much as possible. If this case is not taken into
    // account, the sizer will be resized to the size of the first widget added
    // and next widgets will be shrunk to this size.
    if (orient() == Orientation::flex && !user_requested_box().empty())
    {
        if (rect.width() > width())
            rect.width(width());

        if (rect.height() > height())
            rect.height(height());
    }

    // Layout is performed several times before the sizer has its final size.
    // Never shrink the sizer to avoid corruption related to intermediate
    // steps.
    if (rect.width() < user_requested_box().width())
        rect.width(user_requested_box().width());
    if (rect.height() < user_requested_box().height())
        rect.height(user_requested_box().height());

    resize(rect);

    std::vector<detail::LayoutRect> rects;

    for (auto& child : m_children)
    {
        auto min = child->box();
        // Use the user requested box. The intermediate steps where the sizer
        // doesn't have its final size may have corrupt the child's box.
        if (!child->user_requested_box().empty())
            min = child->user_requested_box();

        if (child->autoresize())
        {
            if (min.width() < child->min_size_hint().width())
                min.width(child->min_size_hint().width());
            if (min.height() < child->min_size_hint().height())
                min.height(child->min_size_hint().height());
        }

        child->layout();

        uint32_t behave = 0;

        if (child->align().is_set(AlignFlag::expand_horizontal))
            behave |= LAY_HFILL;
        else if (child->align().is_set(AlignFlag::left))
            behave |= LAY_LEFT;
        else if (child->align().is_set(AlignFlag::right))
            behave |= LAY_RIGHT;

        if (child->align().is_set(AlignFlag::expand_vertical))
            behave |= LAY_VFILL;
        else if (child->align().is_set(AlignFlag::top))
            behave |= LAY_TOP;
        else if (child->align().is_set(AlignFlag::bottom))
            behave |= LAY_BOTTOM;

        rects.emplace_back(behave, min);
    }

    detail::flex_layout(content_area(), rects, justify(), orient());

    auto child = m_children.begin();
    for (const auto& r : rects)
    {
        (*child)->box(r.rect - point() + content_area().point());
        child++;
    }

    // Same as before, never shrink the sizer to avoid corruption related to
    // intermediate steps.
    rect = super_rect();
    if (rect.width() < user_requested_box().width())
        rect.width(user_requested_box().width());
    if (rect.height() < user_requested_box().height())
        rect.height(user_requested_box().height());

    resize(rect);
}

void BoxSizer::serialize(Serializer& serializer) const
{
    serializer.add_property("orient", std::string(detail::enum_to_string(m_orient)));
    serializer.add_property("justify", std::string(detail::enum_to_string(m_justify)));

    Frame::serialize(serializer);
}

void BoxSizer::deserialize(Serializer::Properties& props)
{
    props.erase(std::remove_if(props.begin(), props.end(), [&](auto & p)
    {
        if (std::get<0>(p) == "orient")
        {
            orient(detail::enum_from_string<Orientation>(std::get<1>(p)));
            return true;
        }
        else if (std::get<0>(p) == "justify")
        {
            justify(detail::enum_from_string<Justification>(std::get<1>(p)));
            return true;
        }
        return false;
    }), props.end());
}

}
}
