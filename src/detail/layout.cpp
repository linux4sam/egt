/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
//#define DEBUG
#include "egt/detail/layout.h"

#define LAY_IMPLEMENTATION
#include "external/layout/layout.h"

using namespace std;

namespace egt
{
inline namespace v1
{
namespace detail
{

void flex_layout(const Rect& parent,
                 vector<LayoutRect>& children,
                 justification justify,
                 orientation orient)
{
    lay_context ctx;
    lay_init_context(&ctx);

    // The context will automatically resize its heap buffer to grow as needed
    // during use. But we can avoid multiple reallocations by reserving as much
    // space as we'll need up-front. Don't worry, lay_init_context doesn't do any
    // allocations, so this is our first and only alloc.
    lay_reserve_items_capacity(&ctx, 1024);

    lay_id p = lay_item(&ctx);

    lay_set_size_xy(&ctx, p, parent.w, parent.h);
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
    }

    if (orient == orientation::horizontal)
        contains |= LAY_ROW;
    else if (orient == orientation::vertical)
        contains |= LAY_COLUMN;
    else
        contains |= (LAY_FLEX | LAY_WRAP);

    lay_set_contain(&ctx, p, contains);

    for (auto& child : children)
    {
        lay_id c = lay_item(&ctx);
        auto item = lay_get_item(&ctx, c);
        item->data = &child;

        lay_set_size_xy(&ctx, c, child.rect.w, child.rect.h);

        uint32_t behave = child.behave;
        lay_set_behave(&ctx, c, behave);
        lay_set_margins_ltrb(&ctx, c, child.lmargin, child.tmargin, child.rmargin, child.bmargin);

        lay_insert(&ctx, p, c);
    }

    lay_run_context(&ctx);

    auto child = lay_first_child(&ctx, p);
    while (child != LAY_INVALID_ID)
    {
        lay_item_t* pchild = lay_get_item(&ctx, child);

        auto re = static_cast<LayoutRect*>(pchild->data);
        lay_vec4 r = lay_get_rect(&ctx, child);
        re->rect.x = r[0];
        re->rect.y = r[1];
        re->rect.w = r[2];
        re->rect.h = r[3];

        child = pchild->next_sibling;
    }

    lay_destroy_context(&ctx);
}

}
}
}
