/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/layout.h"
#include "egt/frame.h"
#include "egt/sizer.h"
#include "egt/utils.h"
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

#define LAY_IMPLEMENTATION
#include "layout.h"

namespace egt
{
inline namespace v1
{
namespace detail
{

static void run_and_apply(lay_context& ctx, lay_id parent,
                          std::vector<LayoutRect>& children)
{
    for (const auto& child : children)
    {
        lay_id c = lay_item(&ctx);
        lay_set_size_xy(&ctx, c, child.rect.width(), child.rect.height());
        lay_set_margins_ltrb(&ctx, c, child.lmargin, child.tmargin, child.rmargin, child.bmargin);
        lay_set_behave(&ctx, c, child.behave);
        lay_insert(&ctx, parent, c);
    }

    lay_run_context(&ctx);

    auto re = children.begin();
    auto child = lay_first_child(&ctx, parent);
    while (child != LAY_INVALID_ID)
    {
        lay_vec4 r = lay_get_rect(&ctx, child);
        re->rect = Rect(r[0], r[1], r[2], r[3]);

        lay_item_t* pchild = lay_get_item(&ctx, child);
        child = pchild->next_sibling;
        re++;
    }
}

static uint32_t justify_to_contains(justification justify, orientation orient)
{
    uint32_t contains = 0;

    switch (justify)
    {
    case justification::start:
        contains |= LAY_START;
        break;
    case justification::middle:
        contains |= LAY_MIDDLE;
        break;
    case justification::end:
        contains |= LAY_END;
        break;
    case justification::justify:
        contains |= LAY_JUSTIFY;
        break;
    case justification::none:
        break;
    }

    if (orient == orientation::horizontal)
        contains |= LAY_ROW;
    else if (orient == orientation::vertical)
        contains |= LAY_COLUMN;
    else if (orient == orientation::none)
        contains |= (LAY_LAYOUT);
    else
        contains |= (LAY_FLEX | LAY_WRAP);

    return contains;
}

static uint32_t align_to_behave(alignmask align)
{
    uint32_t behave = 0;
    if ((align & alignmask::expand_horizontal) == alignmask::expand_horizontal)
        behave |= LAY_HFILL;
    else if ((align & alignmask::left) == alignmask::left)
        behave |= LAY_LEFT;
    else if ((align & alignmask::right) == alignmask::right)
        behave |= LAY_RIGHT;

    if ((align & alignmask::expand_vertical) == alignmask::expand_vertical)
        behave |= LAY_VFILL;
    else if ((align & alignmask::top) == alignmask::top)
        behave |= LAY_TOP;
    else if ((align & alignmask::bottom) == alignmask::bottom)
        behave |= LAY_BOTTOM;
    return behave;
}

void flex_layout(const Rect& parent,
                 std::vector<LayoutRect>& children,
                 justification justify,
                 orientation orient)
{
    lay_context ctx;
    lay_init_context(&ctx);

    scope_exit cleanup([&ctx]()
    {
        lay_destroy_context(&ctx);
    });

    lay_reserve_items_capacity(&ctx, children.size() + 1);

    lay_id outer_parent = lay_item(&ctx);
    lay_set_size_xy(&ctx, outer_parent, parent.width(), parent.height());
    uint32_t contains = justify_to_contains(justify, orient);
    lay_set_contain(&ctx, outer_parent, contains);

    run_and_apply(ctx, outer_parent, children);
}

void flex_layout(const Rect& parent,
                 std::vector<LayoutRect>& children,
                 justification justify,
                 orientation orient,
                 alignmask align)
{
    lay_context ctx;
    lay_init_context(&ctx);

    scope_exit cleanup([&ctx]()
    {
        lay_destroy_context(&ctx);
    });

    lay_reserve_items_capacity(&ctx, children.size() + 2);

    lay_id outer_parent = lay_item(&ctx);
    lay_id inner_parent = lay_item(&ctx);

    lay_set_size_xy(&ctx, outer_parent, parent.width(), parent.height());

    uint32_t contains = justify_to_contains(justify, orient);
    lay_set_contain(&ctx, inner_parent, contains);

    uint32_t dbehave = align_to_behave(align);
    lay_set_behave(&ctx, inner_parent, dbehave);
    lay_insert(&ctx, outer_parent, inner_parent);

    run_and_apply(ctx, inner_parent, children);
}

}
}
}
