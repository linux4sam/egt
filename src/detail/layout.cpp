/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/layout.h"
#include "egt/frame.h"
#include "egt/sizer.h"
#include "egt/utils.h"

#define LAY_IMPLEMENTATION
#include "layout.h"

namespace egt
{
inline namespace v1
{
namespace detail
{

/**
 * Serializes the widget tree to the layout framework.
 */
struct LayoutSerializer
{
    LayoutSerializer(lay_context& c, int max_level)
        : ctx(c),
          max_level(max_level)
    {}

    template<class T>
    lay_id widget_to_layout(T* widget, int level)
    {
        lay_id w = lay_item(&ctx);
        auto item = lay_get_item(&ctx, w);
        item->data = widget;

        auto min = widget->box();

        if (min.w < widget->min_size_hint().w)
            min.w = widget->min_size_hint().w;
        if (min.h < widget->min_size_hint().h)
            min.h = widget->min_size_hint().h;

        DBG("  current: " << min);
        lay_set_size_xy(&ctx, w, min.w, min.h);

        if (level != 0)
        {
            uint32_t behave = 0;

            if ((widget->align() & alignmask::center_horizontal) == alignmask::center_horizontal)
                behave |= LAY_HCENTER;
            else if ((widget->align() & alignmask::left) == alignmask::left)
                behave |= LAY_LEFT;
            else if ((widget->align() & alignmask::right) == alignmask::right)
                behave |= LAY_RIGHT;
            else if ((widget->align() & alignmask::expand_horizontal) == alignmask::expand_horizontal)
                behave |= LAY_HFILL;

            if ((widget->align() & alignmask::center_vertical) == alignmask::center_vertical)
                behave |= LAY_VCENTER;
            else if ((widget->align() & alignmask::top) == alignmask::top)
                behave |= LAY_TOP;
            else if ((widget->align() & alignmask::bottom) == alignmask::bottom)
                behave |= LAY_BOTTOM;
            else if ((widget->align() & alignmask::expand_vertical) == alignmask::expand_vertical)
                behave |= LAY_VFILL;

            if (behave)
            {
                DBG("  behave:" << std::hex << behave);
                lay_set_behave(&ctx, w, behave);
            }
        }

        if (widget->flags().is_set(Widget::flag::frame))
        {
            /*
             * The layout package has a single idea of margin.  So, we play a
             * trick here to make it work with our box model for containers
             * (frames).
             */
            auto moat = widget->margin() + widget->padding() + widget->border();
            lay_set_margins_ltrb(&ctx, w, moat, moat, moat, moat);

            lay_set_size_xy(&ctx, w, min.w - moat * 2, min.h - moat * 2);

            uint32_t contains = 0;

            try
            {
                auto sizer = dynamic_cast<BoxSizer*>(widget);
                if (sizer)
                {
                    switch (sizer->justify())
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

                    if (sizer->orient() == orientation::horizontal)
                        contains |= LAY_ROW;
                    else if (sizer->orient() == orientation::vertical)
                        contains |= LAY_COLUMN;
                    else
                        contains |= (LAY_FLEX | LAY_WRAP);
                }
            }
            catch (...)
            {}

            if (contains)
            {
                DBG("  contains:" << std::hex << contains);
                lay_set_contain(&ctx, w, contains);
            }
        }

        return w;
    }

    bool add(Widget* widget, int level)
    {
        /// @todo Ignore positioning widgets if they are not visible

        if (widget->flags().is_set(Widget::flag::no_layout))
            return false;

        if (level > max_level)
            return false;

        while (static_cast<int>(stack.size()) > level)
            stack.pop_back();

        DBG("level:" << level << " " << widget->name());

        auto child = widget_to_layout(widget, level);

        if (!stack.empty())
        {
            lay_insert(&ctx, stack.back(), child);
        }

        if (widget->flags().is_set(Widget::flag::frame))
            stack.push_back(child);

        return true;
    }

    void apply()
    {
        lay_run_context(&ctx);

        while (stack.size() > 1)
            stack.pop_back();

        assert(stack.size() == 1);
        apply_widget(stack.back(), true);
    }

protected:

    void apply_widget(lay_id id, bool top = false)
    {
        lay_item_t* item = lay_get_item(&ctx, id);

        auto widget = static_cast<Widget*>(item->data);

        if (!top)
        {
            lay_vec4 r = lay_get_rect(&ctx, id);
            //auto rect = Rect(widget->from_display(DisplayPoint(r[0], r[1])), Size(r[2], r[3]));
            auto rect = Rect(r[0], r[1], r[2], r[3]);

            if (widget->flags().is_set(Widget::flag::frame))
            {
                auto moat = widget->margin() + widget->padding() + widget->border();
                rect -= Point(moat, moat);
                rect += Size(2. * moat, 2. * moat);
            }

            DBG("apply " << widget->name() << " " << rect);
            DBG("apply real: " << Rect(r[0], r[1], r[2], r[3]));
            widget->set_box(rect);
        }

        auto child = lay_first_child(&ctx, id);
        while (child != LAY_INVALID_ID)
        {
            apply_widget(child);

            lay_item_t* pchild = lay_get_item(&ctx, child);
            child = pchild->next_sibling;
        }
    }

    lay_context& ctx;
    std::vector<lay_id> stack;
    int max_level{0};
};

void flex_layout(Frame* frame, int max_level)
{
    lay_context ctx;
    lay_init_context(&ctx);

    scope_exit cleanup([&ctx]()
    {
        lay_destroy_context(&ctx);
    });

    // The context will automatically resize its heap buffer to grow as needed
    // during use. But we can avoid multiple reallocations by reserving as much
    // space as we'll need up-front. Don't worry, lay_init_context doesn't do any
    // allocations, so this is our first and only alloc.
    lay_reserve_items_capacity(&ctx, 1024);

    LayoutSerializer s(ctx, max_level);
    frame->walk([&s](Widget * widget, int level)
    {
        return s.add(widget, level);
    });
    s.apply();
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


}

}
}
}
