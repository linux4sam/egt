/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/utf8text.h"
#include "egt/detail/layout.h"
#include "egt/image.h"

namespace egt
{
inline namespace v1
{
namespace detail
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

static void draw_text_setup(std::vector<detail::LayoutRect>& rects,
                            const Painter& painter,
                            const std::string& text,
                            const TextBox::TextFlags& flags)
{
    const auto fe = painter.extents();

    // tokenize based on words or code points
    static const std::string delimiters = " \t\n\r";
    std::vector<std::string> tokens;
    if (flags.is_set(TextBox::TextFlag::multiline) &&
        flags.is_set(TextBox::TextFlag::word_wrap))
    {
        detail::tokenize_with_delimiters(text.cbegin(),
                                         text.cend(),
                                         delimiters.cbegin(),
                                         delimiters.cend(),
                                         tokens);
    }
    else
    {
        for (utf8_const_iterator ch(text.begin(), text.begin(), text.end());
             ch != utf8_const_iterator(text.end(), text.begin(), text.end()); ++ch)
        {
            tokens.emplace_back(utf8_char_to_string(ch.base(), text.cend()));
        }
    }

    rects.reserve(tokens.size());

    uint32_t default_behave = 0;
    uint32_t behave = default_behave;

    for (const auto& t : tokens)
    {
        if (t == "\n")
        {
            rects.emplace_back(behave, Rect(0, 0, 1, fe.height), t);
            behave |= LAY_BREAK;
        }
        else
        {
            const auto te = painter.extents(t);
            rects.emplace_back(behave, Rect(0, 0, te.x_advance, fe.height), t);
            behave = default_behave;
        }
    }
}

#define fl(f) static_cast<float>(f)

void draw_text(Painter& painter,
               const Rect& b,
               const std::string& text,
               const Font& font,
               const TextBox::TextFlags& flags,
               const AlignFlags& text_align,
               Justification justify,
               const Pattern& text_color,
               const std::function<void(const Point& offset, size_t height)>& draw_cursor,
               size_t cursor_pos,
               const Pattern& highlight_color,
               size_t select_start,
               size_t select_len)
{
    painter.set(font);
    const auto fe = painter.extents();

    std::vector<detail::LayoutRect> rects;

    draw_text_setup(rects,
                    painter,
                    text,
                    flags);

    detail::flex_layout(b, rects, justify, Orientation::flex, text_align);

    // draw the code points, cursor, and selected box
    size_t pos = 0;
    std::string last_char;
    bool workaround = false;
    for (const auto& r : rects)
    {
        float roff = 0.;
        for (utf8_const_iterator ch(r.str.begin(), r.str.begin(), r.str.end());
             ch != utf8_const_iterator(r.str.end(), r.str.begin(), r.str.end()); ++ch)
        {
            float char_width = 0;
            last_char = utf8_char_to_string(ch.base(), r.str.cend());

            if (*ch != '\n')
            {
                const auto te = painter.extents(last_char);;
                char_width = te.x_advance;

                auto p = PointF(fl(b.x()) + fl(r.rect.x()) + roff + fl(te.x_bearing),
                                fl(b.y()) + fl(r.rect.y()) + fl(te.y_bearing) - fl(fe.descent) + fl(fe.height));

                if (workaround)
                    p.y(p.y() + fl(fe.height));

                auto s = SizeF(char_width, r.rect.height());

                if (pos >= select_start && pos < select_start + select_len)
                {
                    auto p2 = PointF(fl(b.x()) + fl(r.rect.x()) + roff,
                                     fl(b.y()) + fl(r.rect.y()));

                    if (workaround)
                        p2.y(p2.y() + fl(fe.height));

                    auto rect = RectF(p2, s);
                    if (!rect.empty())
                    {
                        painter.draw(highlight_color, rect);
                    }
                }

                painter.set(text_color);
                painter.draw(p);
                painter.draw(last_char);

                roff += char_width;
            }
            else
            {
                if (!flags.is_set(TextBox::TextFlag::multiline))
                    break;

                // if first char is a "\n" layout doesn't respond right
                if (last_char.empty())
                    workaround = true;
            }

            // draw cursor if before current character
            if (pos == cursor_pos)
            {
                if (draw_cursor)
                {
                    auto p = Point(b.x() + r.rect.x() + roff - char_width, b.y() + r.rect.y());
                    draw_cursor(p, fe.height);
                }
            }

            pos++;
        }
    }

    // handle cursor after last character
    if (pos == cursor_pos)
    {
        if (draw_cursor)
        {
            if (!rects.empty())
            {
                auto p = b.point() + rects.back().rect.point() + Point(rects.back().rect.width(), 0);
                if (workaround)
                {
                    p.y(p.y() + fe.height);
                }

                if (last_char == "\n")
                {
                    p.x(b.x());
                    p.y(p.y() + fe.height);
                }

                draw_cursor(p, fe.height);
            }
            else
                draw_cursor(b.point(), fe.height);
        }
    }
}


void draw_text(Painter& painter,
               const Rect& b,
               const std::string& text,
               const Font& font,
               const TextBox::TextFlags& flags,
               const AlignFlags& text_align,
               Justification justify,
               const Pattern& text_color,
               const AlignFlags& image_align,
               const Image& image,
               const std::function<void(const Point& offset, size_t height)>& draw_cursor,
               size_t cursor_pos,
               const Pattern& highlight_color,
               size_t select_start,
               size_t select_len)
{
    painter.set(font);
    const auto fe = painter.extents();

    std::vector<detail::LayoutRect> rects;

    draw_text_setup(rects,
                    painter,
                    text,
                    flags);

    if (image_align.is_set(AlignFlag::top))
    {
        detail::LayoutRect r(LAY_BREAK, Rect(0, 0, 1, fe.height), "\n");
        rects.insert(rects.begin(), r);

        detail::LayoutRect r2(0, Rect(Point(), image.size()));
        rects.insert(rects.begin(), r2);
    }
    else if (image_align.is_set(AlignFlag::right))
    {
        rects.emplace_back(0, Rect(Point(), image.size()));
    }
    else if (image_align.is_set(AlignFlag::bottom))
    {
        rects.emplace_back(LAY_BREAK, Rect(0, 0, 1, fe.height), "\n");
        rects.emplace_back(0, Rect(Point(), image.size()));
    }
    else
    {
        detail::LayoutRect r(0, Rect(Point(), image.size()));
        rects.insert(rects.begin(), r);
    }

    detail::flex_layout(b, rects, justify, Orientation::flex, text_align);

    // draw the code points, cursor, and selected box
    size_t pos = 0;
    std::string last_char;
    bool workaround = false;
    for (const auto& r : rects)
    {
        if (r.str.empty())
        {
            auto p = Point(b.x() + r.rect.x(),
                           b.y() + r.rect.y());

            painter.draw(image, p);
            continue;
        }

        float roff = 0.;
        for (utf8_const_iterator ch(r.str.begin(), r.str.begin(), r.str.end());
             ch != utf8_const_iterator(r.str.end(), r.str.begin(), r.str.end()); ++ch)
        {
            float char_width = 0;
            last_char = utf8_char_to_string(ch.base(), r.str.cend());

            if (*ch != '\n')
            {
                const auto te = painter.extents(last_char);
                char_width = te.x_advance;

                auto p = PointF(fl(b.x()) + fl(r.rect.x()) + roff + fl(te.x_bearing),
                                fl(b.y()) + fl(r.rect.y()) + fl(te.y_bearing) - fl(fe.descent) + fl(fe.height));

                if (workaround)
                    p.y(p.y() + fl(fe.height));

                auto s = SizeF(char_width, r.rect.height());

                if (pos >= select_start && pos < select_start + select_len)
                {
                    auto p2 = PointF(fl(b.x()) + fl(r.rect.x()) + roff,
                                     fl(b.y()) + fl(r.rect.y()));

                    if (workaround)
                        p2.y(p2.y() + fl(fe.height));

                    auto rect = RectF(p2, s);
                    if (!rect.empty())
                    {
                        painter.draw(highlight_color, rect);
                    }
                }

                painter.set(text_color);
                painter.draw(p);
                painter.draw(last_char);

                roff += char_width;
            }
            else
            {
                if (!flags.is_set(TextBox::TextFlag::multiline))
                    break;

                // if first char is a "\n" layout doesn't respond right
                if (last_char.empty())
                    workaround = true;
            }

            // draw cursor if before current character
            if (pos == cursor_pos)
            {
                if (draw_cursor)
                {
                    auto p = Point(b.x() + r.rect.x() + roff - char_width, b.y() + r.rect.y());
                    draw_cursor(p, fe.height);
                }
            }

            pos++;
        }
    }

    // handle cursor after last character
    if (pos == cursor_pos)
    {
        if (draw_cursor)
        {
            if (!rects.empty())
            {
                auto p = b.point() + rects.back().rect.point() + Point(rects.back().rect.width(), 0);
                if (workaround)
                {
                    p.y(p.y() + fe.height);
                }

                if (last_char == "\n")
                {
                    p.x(b.x());
                    p.y(p.y() + fe.height);
                }

                draw_cursor(p, fe.height);
            }
            else
                draw_cursor(b.point(), fe.height);
        }
    }
}

}
}
}
