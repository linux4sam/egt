/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "detail/utf8text.h"
#include "egt/canvas.h"
#include "egt/detail/alignment.h"
#include "egt/detail/enum.h"
#include "egt/detail/layout.h"
#include "egt/detail/string.h"
#include "egt/frame.h"
#include "egt/input.h"
#include "egt/painter.h"
#include "egt/serialize.h"
#include "egt/text.h"
#include "layout.h"

#ifdef ENABLE_VIRTUALKEYBOARD
#include "egt/virtualkeyboard.h"
#endif

namespace egt
{
inline namespace v1
{

size_t TextRect::length(void) const noexcept
{
    return detail::utf8len(m_text);
}

TextRect& TextRect::consolidate(const TextRect& r, cairo_t* cr) noexcept
{
    m_text += r.m_text;
    m_rect.width(m_rect.width() + r.m_rect.width());
    cairo_text_extents(cr, m_text.c_str(), &m_te);
    return *this;
}

TextRect TextRect::split(size_t pos, cairo_t* cr) noexcept
{
    auto it = m_text.begin();
    utf8::advance(it, pos, m_text.end());

    std::string head_text(m_text.begin(), it);
    std::string tail_text(it, m_text.end());

    cairo_text_extents_t head_te;
    cairo_text_extents(cr, head_text.c_str(), &head_te);

    cairo_text_extents_t tail_te;
    cairo_text_extents(cr, tail_text.c_str(), &tail_te);

    /*
     * Fix a former rounding issue when computing:
     * tail_rect.width() - head_te.x_advance
     * to update tail_rect.width().
     *
     * Now converting, here and once for all, the double 'head_te.x_advance'
     * (see its definition in cairo.h) into the DefaultDim (int) 'head_width'.
     */
    DefaultDim head_width = head_te.x_advance;
    Rect tail_rect(m_rect);
    tail_rect.x(tail_rect.x() + head_width);
    tail_rect.width(tail_rect.width() - head_width);
    TextRect tail(m_behave, tail_rect, tail_text, tail_te, m_text_rect_flags);

    m_text = head_text;
    m_rect.width(head_width);
    m_te = head_te;

    return tail;
}

static void tokenize(TextRects& rects,
                     cairo_t* cr,
                     const std::string& text,
                     const TextBox::TextFlags& flags)
{
    cairo_font_extents_t fe;
    cairo_font_extents(cr, &fe);

    // tokenize based on words or code points
    static const std::string delimiters = " \t\n\r";
    std::vector<std::string> tokens;

    tokens.reserve(text.length());

    bool multiline = flags.is_set(TextBox::TextFlag::multiline);
    if (multiline && flags.is_set(TextBox::TextFlag::word_wrap))
    {
        detail::tokenize_with_delimiters(text.cbegin(),
                                         text.cend(),
                                         delimiters.cbegin(),
                                         delimiters.cend(),
                                         tokens);
    }
    else
    {
        for (detail::utf8_const_iterator ch(text.begin(), text.begin(), text.end());
             ch != detail::utf8_const_iterator(text.end(), text.begin(), text.end()); ++ch)
        {
            auto t = detail::utf8_char_to_string(ch.base(), text.cend());

            if (!multiline && t == "\n")
                break;

            tokens.emplace_back(t);
        }
    }

    uint32_t default_behave = 0;
    uint32_t behave = default_behave;

    bool empty_line = true;
    for (const auto& t : tokens)
    {
        cairo_text_extents_t te;

        if (t == "\n")
        {
            TextRect::TextRectFlags flags = {};
            if (!empty_line)
                flags.set(TextRect::TextRectFlag::eonel);

            memset(&te, 0, sizeof(te));
            te.width = 1;
            te.height = fe.height;
            te.x_advance = 1;
            rects.emplace_back(behave, Rect(0, 0, te.x_advance, fe.height), t, te, flags);
            behave |= LAY_BREAK;
            empty_line = true;
        }
        else
        {
            cairo_text_extents(cr, t.c_str(), &te);
            rects.emplace_back(behave, Rect(0, 0, te.x_advance, fe.height), t, te);
            behave = default_behave;
            empty_line = false;
        }
    }
}

static void compute_layout(const Rect& boundaries,
                           TextRects& rects,
                           Justification justify,
                           Orientation orient,
                           const AlignFlags& align)
{
    lay_context ctx;
    lay_init_context(&ctx);

    auto cleanup = detail::on_scope_exit([&ctx]()
    {
        lay_destroy_context(&ctx);
    });

    lay_reserve_items_capacity(&ctx, rects.size() + 2);

    lay_id outer_parent = lay_item(&ctx);
    lay_id inner_parent = lay_item(&ctx);

    lay_set_size_xy(&ctx, outer_parent, boundaries.width(), boundaries.height());

    uint32_t contains = detail::justify_to_contains(justify, orient);
    lay_set_contain(&ctx, inner_parent, contains);

    uint32_t dbehave = detail::align_to_behave(align);
    lay_set_behave(&ctx, inner_parent, dbehave);
    lay_insert(&ctx, outer_parent, inner_parent);

    for (const auto& r : rects)
    {
        if (r.end_of_non_empty_line())
            continue;

        lay_id c = lay_item(&ctx);
        lay_set_size_xy(&ctx, c, r.rect().width(), r.rect().height());
        lay_set_behave(&ctx, c, r.behave());
        lay_insert(&ctx, inner_parent, c);
    }

    lay_run_context(&ctx);

    auto it = rects.begin();
    auto child = lay_first_child(&ctx, inner_parent);
    while (child != LAY_INVALID_ID)
    {
        lay_vec4 r = lay_get_rect(&ctx, child);
        auto x = boundaries.x() + r[0];
        auto y = boundaries.y() + r[1];
        it->point(Point(x, y));
        x += it->rect().width();

        lay_item_t* pchild = lay_get_item(&ctx, child);
        child = pchild->next_sibling;
        ++it;

        if (it != rects.end() && it->end_of_non_empty_line())
        {
            it->point(Point(x, y));
            ++it;
        }
    }
}

static void consolidate(TextRects& rects,
                        cairo_t* cr)
{
    for (auto it = rects.begin(); it != rects.end();)
    {
        auto nx = std::next(it, 1);
        if (nx == rects.end())
            break;

        if (it->can_consolidate(*nx))
        {
            it->consolidate(*nx, cr);
            rects.erase(nx);
        }
        else
        {
            ++it;
        }
    }
}

static void clear_selection(TextRects& rects)
{
    for (auto& r : rects)
        r.deselect();
}

static void set_selection(TextRects& rects,
                          cairo_t* cr,
                          size_t select_start,
                          size_t select_len)
{
    if (!select_len)
        return;

    size_t pos = 0;
    size_t select_end = select_start + select_len;
    for (auto it = rects.begin(); it != rects.end(); ++it)
    {
        size_t len = it->length();
        size_t start = pos;
        size_t end = start + len;

        if (select_start <= start)
        {
            if (select_end <= start)
            {
                // No more selected rect: stop here
                return;
            }
            else if (select_end < end)
            {
                // Split it and select the head
                TextRect tail(std::move(it->split(select_end - start, cr)));
                it->select();
                it = rects.insert(std::next(it), std::move(tail));
            }
            else
            {
                // Select the whole rect
                it->select();
            }
        }
        else if (select_start < end)
        {
            if (select_end < end)
            {
                // Split it twice and select the middle rect
                size_t head_len = select_start - start;
                size_t mid_len = select_len;

                TextRect mid(std::move(it->split(head_len, cr)));
                TextRect tail(std::move(mid.split(mid_len, cr)));
                mid.select();
                it = rects.insert(std::next(it), std::move(mid));
                it = rects.insert(std::next(it), std::move(tail));
            }
            else
            {
                // Split it and select the tail
                TextRect tail(std::move(it->split(select_start - start, cr)));
                tail.select();
                it = rects.insert(std::next(it), std::move(tail));
            }
        }
        else
        {
            // Selection has not started yet
        }

        pos += len;
    }
}

static void get_line(const TextRects& rects,
                     TextRects::iterator& pos,
                     std::string& line,
                     Rect& rect)
{
    line.clear();
    rect.clear();

    if (pos == rects.end())
        return;

    auto y = pos->rect().y();
    rect = pos->rect();
    rect.width(0);
    while (pos != rects.end())
    {
        const Rect& r = pos->rect();

        if (r.y() != y)
            break;

        line += pos->text();
        rect.width(rect.width() + r.width());
        ++pos;
    }
}

static std::string longest_prefix(const std::string& s1,
                                  const std::string& s2)
{
    detail::utf8_const_iterator it1(s1.cbegin(), s1.cbegin(), s1.cend());
    detail::utf8_const_iterator end1(s1.cend(), s1.cbegin(), s1.cend());
    detail::utf8_const_iterator it2(s2.cbegin(), s2.cbegin(), s2.cend());
    detail::utf8_const_iterator end2(s2.cend(), s2.cbegin(), s2.cend());

    while (it1 != end1 && it2 != end2 && *it1 == *it2)
    {
        ++it1;
        ++it2;
    }

    return std::string(s1.cbegin(), it1.base());
}

static std::string longest_suffix(const std::string& s1,
                                  const std::string& s2)
{
    utf8::iterator<std::string::const_reverse_iterator> it1(s1.crbegin(), s1.crbegin(), s1.crend());
    utf8::iterator<std::string::const_reverse_iterator> end1(s1.crend(), s1.crbegin(), s1.crend());
    utf8::iterator<std::string::const_reverse_iterator> it2(s2.crbegin(), s2.crbegin(), s2.crend());
    utf8::iterator<std::string::const_reverse_iterator> end2(s2.crend(), s2.crbegin(), s2.crend());

    while (it1 != end1 && it2 != end2 && *it1 == *it2)
    {
        ++it1;
        ++it2;
    }

    return std::string(it1.base().base(), s1.cend());
}

static void tag_default_aligned_line(TextRects& prev,
                                     TextRects::iterator& prev_pos,
                                     TextRects& next,
                                     TextRects::iterator& next_pos,
                                     Widget& parent)
{
    std::string prev_line;
    Rect prev_rect;
    get_line(prev, prev_pos, prev_line, prev_rect);

    std::string next_line;
    Rect next_rect;
    get_line(next, next_pos, next_line, next_rect);

    if (prev_line != next_line)
    {
        auto x_min = std::min(prev_rect.x(), next_rect.x());
        auto x_max = std::max(prev_rect.x() + prev_rect.width(), next_rect.x() + next_rect.width());
        auto y_min = std::min(prev_rect.y(), next_rect.y());
        auto y_max = std::max(prev_rect.y() + prev_rect.height(), next_rect.y() + next_rect.height());
        Rect r(x_min, y_min, x_max - x_min, y_max - y_min);
        if (!r.empty())
            parent.damage(r);
    }
}

static void tag_left_aligned_line(TextRects& prev,
                                  TextRects::iterator& prev_pos,
                                  TextRects& next,
                                  TextRects::iterator& next_pos,
                                  Widget& parent,
                                  cairo_t* cr)
{
    std::string prev_line;
    Rect prev_rect;
    get_line(prev, prev_pos, prev_line, prev_rect);

    auto next_it = next_pos;
    std::string next_line;
    Rect next_rect;
    get_line(next, next_pos, next_line, next_rect);

    std::string prefix = longest_prefix(prev_line, next_line);

    if (prefix == next_line)
    {
        if (next_rect.width() < prev_rect.width())
        {
            auto x = prev_rect.x() + next_rect.width();
            auto y = prev_rect.y();
            auto w = prev_rect.width() - next_rect.width();
            auto h = prev_rect.height();
            parent.damage(Rect(x, y, w, h));
        }
        return;
    }

    Rect prefix_rect(next_it->rect());
    prefix_rect.width(0);

    size_t len = detail::utf8len(prefix);
    size_t l;
    while ((l = next_it->length()) <= len)
    {
        prefix_rect.width(prefix_rect.width() + next_it->rect().width());
        len -= l;
        ++next_it;
    }

    if (len && len < next_it->length())
    {
        TextRect tail(std::move(next_it->split(len, cr)));
        prefix_rect.width(prefix_rect.width() + next_it->rect().width());
        next.insert(std::next(next_it), std::move(tail));
    }

    Rect r(prefix_rect);
    r.x(prefix_rect.x() + prefix_rect.width());
    r.width(std::max(next_rect.width(), prev_rect.width()) - prefix_rect.width());
    if (!r.empty())
        parent.damage(r);
}

static void tag_right_aligned_line(TextRects& prev,
                                   TextRects::iterator& prev_pos,
                                   TextRects& next,
                                   TextRects::iterator& next_pos,
                                   Widget& parent,
                                   cairo_t* cr)
{
    std::string prev_line;
    Rect prev_rect;
    get_line(prev, prev_pos, prev_line, prev_rect);

    auto next_it = next_pos;
    std::string next_line;
    Rect next_rect;
    get_line(next, next_pos, next_line, next_rect);

    std::string suffix = longest_suffix(prev_line, next_line);

    if (suffix == next_line)
    {
        if (next_rect.width() < prev_rect.width())
        {
            auto x = prev_rect.x();
            auto y = prev_rect.y();
            auto w = prev_rect.width() - next_rect.width();
            auto h = prev_rect.height();
            parent.damage(Rect(x, y, w, h));
        }
        return;
    }

    Rect prefix_rect(next_it->rect());
    prefix_rect.width(0);

    size_t len = detail::utf8len(next_line) - detail::utf8len(suffix);
    size_t l;
    while (len && (l = next_it->length()) <= len)
    {
        prefix_rect.width(prefix_rect.width() + next_it->rect().width());
        len -= l;
        ++next_it;
    }

    if (len && len < next_it->length())
    {
        TextRect tail(next_it->split(len, cr));
        prefix_rect.width(prefix_rect.width() + next_it->rect().width());
        next.insert(std::next(next_it), std::move(tail));
    }

    Rect r(prefix_rect);
    r.x(std::min(next_rect.x(), prev_rect.x()));
    r.width(prefix_rect.x() + prefix_rect.width() - r.x());
    if (!r.empty())
        parent.damage(r);
}

static void tag_line(const AlignFlags& text_align,
                     TextRects& prev,
                     TextRects::iterator& prev_pos,
                     TextRects& next,
                     TextRects::iterator& next_pos,
                     Widget& parent,
                     cairo_t* cr)
{
    /* Currently AlignFlag::expand_horizontal is left-aligned. */
    if (text_align.is_set(AlignFlag::left) ||
        text_align.is_set(AlignFlag::expand_horizontal))
        tag_left_aligned_line(prev, prev_pos, next, next_pos, parent, cr);
    else if (text_align.is_set(AlignFlag::right))
        tag_right_aligned_line(prev, prev_pos, next, next_pos, parent, cr);
    else
        tag_default_aligned_line(prev, prev_pos, next, next_pos, parent);
}

static void tag_text(const AlignFlags& text_align,
                     TextRects& prev,
                     TextRects& next,
                     Widget& parent,
                     cairo_t* cr)
{
    auto prev_pos = prev.begin();
    auto next_pos = next.begin();
    while (prev_pos != prev.end() && next_pos != next.end())
        tag_line(text_align, prev, prev_pos, next, next_pos, parent, cr);

    std::string line;
    Rect rect;
    while (next_pos != next.end())
    {
        get_line(next, next_pos, line, rect);
        parent.damage(rect);
    }

    while (prev_pos != prev.end())
    {
        get_line(prev, prev_pos, line, rect);
        parent.damage(rect);
    }
}

static void get_line_selection(const TextRects& rects,
                               TextRects::const_iterator& pos,
                               Rect& rect)
{
    rect.clear();

    if (pos == rects.cend())
        return;

    bool found = false;
    auto y = pos->rect().y();
    rect = pos->rect();
    rect.width(0);
    while (pos != rects.cend())
    {
        const Rect& r = pos->rect();

        if (r.y() != y)
            break;

        if (pos->is_selected())
        {
            if (!found)
            {
                rect.x(r.x());
                found = true;
            }

            rect.width(rect.width() + r.width());
        }

        ++pos;
    }
}

static void tag_line_selection(const TextRects& prev,
                               TextRects::const_iterator& prev_pos,
                               const TextRects& next,
                               TextRects::const_iterator& next_pos,
                               Widget& parent)
{
    Rect pr;
    get_line_selection(prev, prev_pos, pr);

    Rect nr;
    get_line_selection(next, next_pos, nr);

    if (!pr.width() && !nr.width())
        return;

    if (pr.width() && !nr.width())
    {
        parent.damage(pr);
        return;
    }

    if (nr.width() && !pr.width())
    {
        parent.damage(nr);
        return;
    }

    auto min_lx = std::min(pr.x(), nr.x());
    auto max_lx = std::max(pr.x(), nr.x());
    if (min_lx != max_lx)
    {
        Rect left(min_lx, pr.y(), max_lx - min_lx, pr.height());
        parent.damage(left);
    }

    auto min_rx = std::min(pr.x() + pr.width(), nr.x() + nr.width());
    auto max_rx = std::max(pr.x() + pr.width(), nr.x() + nr.width());
    if (min_rx != max_rx)
    {
        Rect right(min_rx, pr.y(), max_rx - min_rx, pr.height());
        parent.damage(right);
    }
}

static void tag_text_selection(const TextRects& prev,
                               const TextRects& next,
                               Widget& parent)
{
    auto prev_pos = prev.cbegin();
    auto next_pos = next.cbegin();
    while (prev_pos != prev.cend() && next_pos != next.cend())
        tag_line_selection(prev, prev_pos, next, next_pos, parent);
}

#define fl(f) static_cast<float>(f)

static void draw_text(Painter& painter,
                      const Rect& rect,
                      const TextRects& rects,
                      const Font& font,
                      const Pattern& text_color,
                      const Pattern& highlight_color)
{
    auto cr = painter.context().get();

    painter.set(font);
    cairo_font_extents_t fe;
    cairo_font_extents(cr, &fe);

    for (const auto& r : rects)
    {
        if (r.text() != "\n")
        {
            if (!r.rect().intersect(rect))
                continue;

            const cairo_text_extents_t& te = r.text_extents();

            auto p = PointF(fl(r.rect().x()) + fl(te.x_bearing),
                            fl(r.rect().y()) + fl(te.y_bearing) - fl(fe.descent) + fl(fe.height));

            auto s = SizeF(te.x_advance, r.rect().height());

            if (r.is_selected())
            {
                auto p2 = PointF(fl(r.rect().x()), fl(r.rect().y()));

                auto r2 = RectF(p2, s);
                if (!r2.empty())
                {
                    painter.set(highlight_color);
                    painter.draw(r2);
                    painter.fill();
                }
            }

            painter.set(text_color);
            painter.draw(p);
            painter.draw(r.text());
        }
    }
}

static void prepare_text(TextRects& rects,
                         cairo_t* cr,
                         const Rect& b,
                         const std::string& text,
                         const Font& font,
                         const TextBox::TextFlags& flags,
                         const AlignFlags& text_align,
                         size_t select_start,
                         size_t select_len)
{
    cairo_set_scaled_font(cr, font.scaled_font());

    rects.clear();

    tokenize(rects, cr, text, flags);
    compute_layout(b, rects, Justification::start, Orientation::flex, text_align);
    set_selection(rects, cr, select_start, select_len);
}

constexpr static auto CURSOR_WIDTH = 2;
constexpr static auto CURSOR_X_MARGIN = 1;
constexpr static auto CURSOR_RECT_WIDTH = CURSOR_WIDTH + 2 * CURSOR_X_MARGIN;

static void get_cursor_rect(const TextRects& rects,
                            cairo_t* cr,
                            const Rect& boundaries,
                            const Font& font,
                            size_t cursor_pos,
                            Rect& rect)
{
    cairo_set_scaled_font(cr, font.scaled_font());
    cairo_font_extents_t fe;
    cairo_font_extents(cr, &fe);

    Point p(boundaries.point() + Point(-CURSOR_X_MARGIN, 0));
    Size s(CURSOR_RECT_WIDTH, fe.height);

    size_t pos = 0;
    for (const auto& r : rects)
    {
        size_t l = r.length();
        if (pos + l < cursor_pos)
        {
            pos += l;
            continue;
        }

        if (pos == cursor_pos)
        {
            p = r.rect().point();
            p.x(p.x() - CURSOR_X_MARGIN);
            break;
        }

        if (r.text() == "\n")
        {
            p.x(boundaries.x() - CURSOR_X_MARGIN);
            p.y(r.rect().y() + fe.height);
            break;
        }

        auto it = r.text().begin();
        utf8::advance(it, cursor_pos - pos, r.text().end());
        std::string str(r.text().begin(), it);

        cairo_text_extents_t te;
        cairo_text_extents(cr, str.c_str(), &te);

        p = r.rect().point();
        p.x(p.x() + te.x_advance - CURSOR_X_MARGIN);
        break;
    }

    rect.point(p);
    rect.size(s);
}

static AlignFlags default_text_align_value{AlignFlag::expand};

AlignFlags TextBox::default_text_align()
{
    return default_text_align_value;
}

void TextBox::default_text_align(const AlignFlags& align)
{
    default_text_align_value = align;
}

TextBox::TextBox(const std::string& text,
                 const AlignFlags& text_align,
                 const TextFlags& flags) noexcept
    : TextBox(text, {}, text_align, flags)
{}

TextBox::TextBox(const std::string& text,
                 const TextFlags& flags,
                 const AlignFlags& text_align) noexcept
    : TextBox(text, {}, text_align, flags)
{}

TextBox::TextBox(const std::string& text,
                 const Rect& rect,
                 const AlignFlags& text_align,
                 // NOLINTNEXTLINE(modernize-pass-by-value)
                 const TextFlags& flags) noexcept
    : TextWidget( {}, rect, text_align),
m_timer(std::chrono::seconds(1)),
m_text_flags(flags)
{
    name("TextBox" + std::to_string(m_widgetid));
    initialize();

    insert(text);
}

TextBox::TextBox(Frame& parent,
                 const std::string& text,
                 const AlignFlags& text_align,
                 const TextFlags& flags) noexcept
    : TextBox(text, Rect(), text_align, flags)
{
    parent.add(*this);
}

TextBox::TextBox(Frame& parent,
                 const std::string& text,
                 const Rect& rect,
                 const AlignFlags& text_align,
                 const TextFlags& flags) noexcept
    : TextBox(text, rect, text_align, flags)
{
    parent.add(*this);
}

TextBox::TextBox(Serializer::Properties& props, bool is_derived) noexcept
    : TextWidget(props, true),
      m_timer(std::chrono::seconds(1))
{
    initialize(false);

    deserialize(props);

    if (!is_derived)
        deserialize_leaf(props);
}

void TextBox::initialize(bool init_inherited_properties)
{
    if (init_inherited_properties)
    {
        border(theme().default_border());
        fill_flags(Theme::FillFlag::blend);
        border_radius(4.0);
        padding(5);
    }

    m_timer.on_timeout([this]() { cursor_timeout(); });

    m_gain_focus_reg = on_gain_focus([this]()
    {
        show_cursor();
#ifdef ENABLE_VIRTUALKEYBOARD
        if (!text_flags().is_set(TextFlag::no_virt_keyboard))
            if (popup_virtual_keyboard())
                popup_virtual_keyboard()->show();
#endif
    });

    m_lost_focus_reg = on_lost_focus([this]()
    {
        hide_cursor();
#ifdef ENABLE_VIRTUALKEYBOARD
        if (!text_flags().is_set(TextFlag::no_virt_keyboard))
            if (popup_virtual_keyboard())
                popup_virtual_keyboard()->hide();
#endif
    });
}

void TextBox::handle(Event& event)
{
    Widget::handle(event);

    switch (event.id())
    {
    case EventId::pointer_click:
        detail::keyboard_focus(this);
        break;
    case EventId::keyboard_down:
        handle_key(event.key());
        event.stop();
    default:
        break;
    }
}

void TextBox::handle_key(const Key& key)
{
    switch (key.keycode)
    {
    case EKEY_ESCAPE:
        // do nothing
        break;
    case EKEY_BACKSPACE:
    {
        if (cursor())
        {
            if (!m_select_len)
                selection(cursor() - 1, 1);
            selection_delete();
        }
        break;
    }
    case EKEY_DELETE:
    {
        if (!m_select_len)
            selection(cursor(), 1);
        selection_delete();
        break;
    }
    case EKEY_ENTER:
    {
        if (text_flags().is_set(TextFlag::multiline))
            insert("\n");
        break;
    }
    case EKEY_LEFT:
    {
        if (key.state.is_set(Key::KeyMod::shift))
        {
            selection_backward();
        }
        else if (m_select_len)
        {
            // Move the cursor to the beginning of the selection and deselect the text
            cursor_set(m_select_start);
            m_select_len = 0;
            selection_damage();
        }
        else
        {
            cursor_backward();
        }
        break;
    }
    case EKEY_RIGHT:
    {
        if (key.state.is_set(Key::KeyMod::shift))
        {
            selection_forward();
        }
        else if (m_select_len)
        {
            // Move the cursor to the end of the selection and deselect the text
            cursor_set(m_select_start + m_select_len);
            m_select_len = 0;
            selection_damage();
        }
        else
        {
            cursor_forward();
        }
        break;
    }
    /// NOLINTNEXTLINE(bugprone-branch-clone)
    case EKEY_UP:
    {
        if (text_flags().is_set(TextFlag::multiline))
        {
            // TODO
        }
        break;
    }
    case EKEY_DOWN:
    {
        if (text_flags().is_set(TextFlag::multiline))
        {
            // TODO
        }
        break;
    }
    /// NOLINTNEXTLINE(bugprone-branch-clone)
    case EKEY_END:
    {
        // TODO
        break;
    }
    case EKEY_HOME:
    {
        // TODO
        break;
    }
    default:
    {
        if (key.unicode)
        {
            std::string tmp;
            utf8::append(key.unicode, std::back_inserter(tmp));
            insert(tmp);
        }
        break;
    }
    }
}

constexpr static auto CURSOR_Y_OFFSET = 2.;

void TextBox::draw(Painter& painter, const Rect& rect)
{
    State state(content_area(), &font(), flags(), text_align(), text_flags());

    if ((m_cr.get() != painter.context().get()) ||
        m_state != state)
    {
        m_cr = painter.context();

        m_state = std::move(state);

        prepare_text(m_rects,
                     m_cr.get(),
                     content_area(),
                     m_text,
                     font(),
                     text_flags(),
                     text_align(),
                     m_select_start,
                     m_select_len);

        get_cursor_rect(m_rects,
                        m_cr.get(),
                        content_area(),
                        font(),
                        m_cursor_pos,
                        m_cursor_rect);
    }

    // box
    if (box().right() <= rect.right() ||
        box().bottom() <= rect.bottom() ||
        box().left() >= rect.left() ||
        box().top() >= rect.top())
    {
        draw_box(painter, Palette::ColorId::bg, Palette::ColorId::border);
    }
    else
    {
        painter.set(color(Palette::ColorId::bg));
        painter.draw(rect);
        painter.fill();
    }

    draw_text(painter,
              rect,
              m_rects,
              font(),
              color(Palette::ColorId::text),
              color(Palette::ColorId::text_highlight));


    if (m_cursor_rect.intersect(rect))
    {
        if (focus() && m_cursor_state)
        {
            Point offset(m_cursor_rect.point() + Point(CURSOR_X_MARGIN, 0));
            auto height = m_cursor_rect.height();

            painter.set(color(Palette::ColorId::cursor));
            painter.line_width(CURSOR_WIDTH);

            painter.draw(offset, offset + Point(0, height));
            painter.stroke();
        }
    }
}

void TextBox::text(const std::string& str)
{
    selection_clear();
    clear();
    insert(str);
}

void TextBox::clear()
{
    m_rects.clear();
    selection_clear();
    cursor_begin();
    TextWidget::clear();
}

void TextBox::max_length(size_t len)
{
    if (detail::change_if_diff<>(m_max_len, len))
    {
        if (m_max_len)
        {
            const auto length = detail::utf8len(m_text);
            if (length > m_max_len)
            {
                auto i = m_text.begin();
                utf8::advance(i, m_max_len, m_text.end());
                m_text.erase(i, m_text.end());
                on_text_changed.invoke();
            }
        }

        damage();
    }
}

size_t TextBox::append(const std::string& str)
{
    cursor_end();
    return insert(str);
}

size_t TextBox::width_to_len(const std::string& str) const
{
    const auto b = content_area();

    Canvas c(Size(1, 1));
    auto cr = c.context();
    Painter painter(cr);
    painter.set(font());

    size_t len = 0;
    float total = 0;
    for (detail::utf8_const_iterator ch(str.begin(), str.begin(), str.end());
         ch != detail::utf8_const_iterator(str.end(), str.begin(), str.end()); ++ch)
    {
        const auto txt = detail::utf8_char_to_string(ch.base(), str.cend());
        cairo_text_extents_t te;
        cairo_text_extents(cr.get(), txt.c_str(), &te);
        if (total + static_cast<float>(te.x_advance) > b.width())
            return len;
        total += static_cast<float>(te.x_advance);
        len++;
    }

    return len;
}

size_t TextBox::insert(const std::string& str)
{
    if (str.empty())
        return 0;

    const auto current_len = detail::utf8len(m_text);
    auto len = detail::utf8len(str);

    if (m_max_len)
    {
        if (current_len + len > m_max_len)
            len = m_max_len - current_len;
    }

    if (!text_flags().is_set(TextFlag::multiline) &&
        text_flags().is_set(TextFlag::fit_to_width) &&
        !content_area().empty())
    {
        /*
         * Have to go through the motions and build the expected string at this
         * point to see what will fit.
         */

        // insert at cursor position
        auto text = m_text;
        auto i = text.begin();
        utf8::advance(i, m_cursor_pos, text.end());
        auto end = str.begin();
        utf8::advance(end, len, str.end());
        text.insert(i, str.begin(), end);

        const auto maxlen = width_to_len(text);
        if (current_len + len > maxlen)
            len = maxlen - current_len;
    }

    if (m_validate_input)
        if (!validate_input(str))
            return 0;

    if (len > 0)
    {
        // insert at cursor position
        auto i = m_text.begin();
        utf8::advance(i, m_cursor_pos, m_text.end());
        auto end = str.begin();
        utf8::advance(end, len, str.end());
        m_text.insert(i, str.begin(), end);
        selection_clear();

        on_text_changed.invoke();

        if (m_cr)
        {
            TextRects rects;
            prepare_text(rects,
                         m_cr.get(),
                         content_area(),
                         m_text,
                         font(),
                         text_flags(),
                         text_align(),
                         m_select_start,
                         m_select_len);
            tag_text(text_align(), m_rects, rects, *this, m_cr.get());
            m_rects = std::move(rects);
        }
        else
        {
            damage();
        }

        cursor_forward(len);
        continue_show_cursor();
    }

    return len;
}

size_t TextBox::cursor() const
{
    return m_cursor_pos;
}

void TextBox::cursor_begin()
{
    cursor_set(0);
}

void TextBox::cursor_end()
{
    // one past end
    cursor_set(detail::utf8len(m_text));
}

void TextBox::cursor_forward(size_t count)
{
    cursor_set(m_cursor_pos + count);
}

void TextBox::cursor_backward(size_t count)
{
    if (m_cursor_pos > count)
        cursor_set(m_cursor_pos - count);
    else
        cursor_set(0);
}

void TextBox::cursor_set(size_t pos)
{
    const auto len = detail::utf8len(m_text);
    if (pos > len)
        pos = len;

    if (m_cursor_pos != pos)
    {
        m_cursor_pos = pos;

        if (m_cr)
        {
            damage(m_cursor_rect);
            get_cursor_rect(m_rects,
                            m_cr.get(),
                            content_area(),
                            font(),
                            m_cursor_pos,
                            m_cursor_rect);
        }
    }

    show_cursor();
}

void TextBox::selection_all()
{
    selection(0, detail::utf8len(m_text));
}

void TextBox::selection_damage()
{
    if (!m_cr)
    {
        damage();
        return;
    }

    consolidate(m_rects, m_cr.get());
    TextRects rects(m_rects);
    clear_selection(rects);
    consolidate(rects, m_cr.get());
    set_selection(rects, m_cr.get(), m_select_start, m_select_len);
    tag_text_selection(m_rects, rects, *this);
    m_rects = std::move(rects);
}

void TextBox::selection(size_t pos, size_t length)
{
    const auto len = detail::utf8len(m_text);
    if (pos > len)
        pos = len;

    auto i = m_text.begin();
    utf8::advance(i, pos, m_text.end());
    size_t d = utf8::distance(i, m_text.end());
    if (length > d)
        length = d;

    if (pos != m_select_start || length != m_select_len)
    {
        m_select_start = pos;
        m_select_len = length;
        selection_damage();
    }
}

void TextBox::selection_forward(size_t count)
{
    if (!m_select_len)
        m_select_start = m_cursor_pos;

    size_t select_end = m_select_start + m_select_len;
    if (select_end == m_cursor_pos && m_select_len)
    {
        count = std::min(count, m_select_len);

        // Reduce the selection from the left
        m_select_start += count;
        m_select_len -= count;
        selection_damage();
    }
    else if (select_end < detail::utf8len(m_text))
    {
        count = std::min(count, detail::utf8len(m_text) - select_end);

        // Extend the selection to the right
        if (m_cursor_state)
            hide_cursor();
        m_select_len += count;
        selection_damage();
    }
}

void TextBox::selection_backward(size_t count)
{
    if (!m_select_len)
        m_select_start = m_cursor_pos;

    if (m_select_start == m_cursor_pos && m_select_len)
    {
        count = std::min(count, m_select_len);

        // Reduce the selection from the right
        m_select_len -= count;
        selection_damage();
    }
    else if (m_select_start > 0)
    {
        count = std::min(count, m_select_start);

        // Extend the selection to the left
        if (m_cursor_state)
            hide_cursor();
        m_select_start -= count;
        m_select_len += count;
        selection_damage();
    }
}

void TextBox::selection_clear()
{
    if (m_select_len)
    {
        m_select_len = 0;
        selection_damage();
    }
}

std::string TextBox::selected_text() const
{
    if (m_select_len)
    {
        auto i = m_text.begin();
        utf8::advance(i, m_select_start, m_text.end());
        auto l = i;
        utf8::advance(l, m_select_len, m_text.end());
        return m_text.substr(std::distance(m_text.begin(), i),
                             std::distance(i, l));
    }

    return std::string();
}

void TextBox::selection_delete()
{
    if (m_select_len)
    {
        auto i = m_text.begin();
        utf8::advance(i, m_select_start, m_text.end());
        auto l = i;
        utf8::advance(l, m_select_len, m_text.end());
        size_t p = utf8::distance(m_text.begin(), i);

        m_text.erase(i, l);
        cursor_set(p);
        selection_clear();
        on_text_changed.invoke();

        if (m_cr)
        {
            TextRects rects;
            prepare_text(rects,
                         m_cr.get(),
                         content_area(),
                         m_text,
                         font(),
                         text_flags(),
                         text_align(),
                         m_select_start,
                         m_select_len);
            tag_text(text_align(), m_rects, rects, *this, m_cr.get());
            m_rects = std::move(rects);
        }
        else
        {
            damage();
        }
    }
}

void TextBox::input_validation_enabled(bool enabled)
{
    m_validate_input = enabled;
}

void TextBox::add_validator_function(ValidatorCallback callback)
{
    m_validator_callbacks.emplace_back(std::move(callback));
}

bool TextBox::validate_input(const std::string& str)
{
    for (auto& callback : m_validator_callbacks)
    {
        const auto valid = callback(str);
        if (!valid)
            return false;
    }

    return true;
}

void TextBox::cursor_timeout()
{
    m_cursor_state = !m_cursor_state;
    damage(m_cursor_rect);
}

void TextBox::show_cursor()
{
    m_cursor_state = true;
    m_timer.start();
    damage(m_cursor_rect);
}

void TextBox::continue_show_cursor()
{
    if (m_cursor_state)
    {
        m_timer.start();
        damage(m_cursor_rect);
    }
}

void TextBox::hide_cursor()
{
    m_timer.cancel();
    m_cursor_state = false;
    damage(m_cursor_rect);
}

Size TextBox::min_size_hint() const
{
    if (!m_min_size.empty())
        return m_min_size;

    auto text = m_text;
    if (text.empty())
        text = "Hello World";

    auto s = text_size(text);
    s += Widget::min_size_hint() + Size(0, CURSOR_Y_OFFSET * 2.);
    return s;
}

TextBox::~TextBox() noexcept
{
    on_gain_focus.remove(m_gain_focus_reg);
    on_lost_focus.remove(m_lost_focus_reg);
}

template<>
const std::pair<TextBox::TextFlag, char const*> detail::EnumStrings<TextBox::TextFlag>::data[] =
{
    {TextBox::TextFlag::fit_to_width, "fit_to_width"},
    {TextBox::TextFlag::multiline, "multiline"},
    {TextBox::TextFlag::word_wrap, "word_wrap"},
    {TextBox::TextFlag::no_virt_keyboard, "no_virt_keyboard"},
};

void TextBox::serialize(Serializer& serializer) const
{
    TextWidget::serialize(serializer);

    if (max_length())
        serializer.add_property("maxlength", static_cast<unsigned int>(max_length()));
    if (!m_text_flags.empty())
        serializer.add_property("text_flags", m_text_flags.to_string());
    if (cursor())
        serializer.add_property("cursor", static_cast<unsigned int>(cursor()));
    if (selection_start())
        serializer.add_property("selection_start", static_cast<unsigned int>(selection_start()));
    if (selection_length())
        serializer.add_property("selection_length", static_cast<unsigned int>(selection_length()));
}

void TextBox::deserialize(Serializer::Properties& props)
{
    props.erase(std::remove_if(props.begin(), props.end(), [&](auto & p)
    {
        if (std::get<0>(p) == "maxlength")
            max_length(std::stoul(std::get<1>(p)));
        else if (std::get<0>(p) == "text_flags")
            m_text_flags.from_string(std::get<1>(p));
        else if (std::get<0>(p) == "cursor")
            cursor_set(std::stoul(std::get<1>(p)));
        else if (std::get<0>(p) == "selection_start")
            selection(std::stoul(std::get<1>(p)), selection_length());
        else if (std::get<0>(p) == "selection_length")
            selection(selection_start(), std::stoul(std::get<1>(p)));
        else
            return false;
        return true;
    }), props.end());
}

}
}
