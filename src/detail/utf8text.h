/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_UTF8TEXT_H
#define EGT_DETAIL_UTF8TEXT_H

#include "egt/painter.h"
#include "egt/text.h"
#include <functional>
#include <string>
#include <utf8.h>
#include <vector>

namespace egt
{
inline namespace v1
{
namespace detail
{

using utf8_const_iterator = utf8::iterator<std::string::const_iterator>;
using utf8_iterator = utf8::iterator<std::string::iterator>;

/**
 * Returns the length of a utf-8 encoded string.
 */
inline size_t utf8len(const std::string& str)
{
    return utf8::distance(str.begin(), str.end());
}

/**
 * Convert a utf-8 iterator to a standalone std::string.
 */
template<class T1, class T2>
inline std::string utf8_char_to_string(T1 ch, T2 e)
{
    auto ch2 = ch;
    utf8::advance(ch2, 1, e);
    return std::string(ch.base(), ch2.base());
}

/**
 * Special utf-8 aware string tokenizer which keeps delimiters as tokens.
 *
 * @param[in] begin Input string begin iterator.
 * @param[in] end Input string end iterator.
 * @param[in] dbegin  Delimiter string begin iterator.
 * @param[in] dend Delimiter string end iterator.
 * @param[out] tokens The resulting string tokens.
 */
template<class T>
void tokenize_with_delimiters(T begin, T end,
                              T dbegin, T dend,
                              std::vector<std::string>& tokens)
{
    std::string token;

    for (auto pos = begin; pos != end;)
    {
        const auto ch = utf8::next(pos, end);
        bool found = false;
        for (auto d = dbegin; d != dend;)
        {
            auto del = utf8::next(d, dend);
            if (del == ch)
            {
                found = true;
                break;
            }
        }

        if (found)
        {
            if (!token.empty())
            {
                tokens.push_back(token);
                token.clear();
            }

            std::string tmp;
            utf8::append(ch, std::back_inserter(tmp));
            tokens.emplace_back(tmp);
        }
        else
        {
            utf8::append(ch, std::back_inserter(token));
        }
    }

    if (!token.empty())
        tokens.push_back(token);
}

void draw_text(Painter& painter,
               const Rect& b,
               const std::string& text,
               const Font& font,
               TextBox::flags_type flags,
               alignmask text_align,
               justification justify,
               const Color& text_color,
               std::function<void(const Point& offset, size_t height)> draw_cursor = nullptr,
               size_t cursor_pos = 0,
               const Color& highlight_color = Color(),
               size_t select_start = 0,
               size_t select_len = 0);

void draw_text(Painter& painter,
               const Rect& b,
               const std::string& text,
               const Font& font,
               TextBox::flags_type flags,
               alignmask text_align,
               justification justify,
               const Color& text_color,
               alignmask image_align,
               const Image& image,
               std::function<void(const Point& offset, size_t height)> draw_cursor = nullptr,
               size_t cursor_pos = 0,
               const Color& highlight_color = Color(),
               size_t select_start = 0,
               size_t select_len = 0);

}
}
}

#endif
