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

#define UTF8CPP_CHECKED
#ifdef UTF8CPP_CHECKED
namespace utf8ns = utf8;
#else
namespace utf8ns = utf8::unchecked;
#endif

using utf8_const_iterator = utf8ns::iterator<std::string::const_iterator>;
using utf8_iterator = utf8ns::iterator<std::string::iterator>;

/**
 * Returns the length of a utf-8 encoded string.
 */
inline size_t utf8len(const std::string& str)
{
    return utf8ns::distance(str.begin(), str.end());
}

/**
 * Convert a utf-8 iterator to a standalone std::string.
 */
#ifdef UTF8CPP_CHECKED
template<class T1, class T2>
inline std::string utf8_char_to_string(T1 ch, T2 e)
#else
template<class T>
inline std::string utf8_char_to_string(T ch)
#endif
{
    auto ch2 = ch;
#ifdef UTF8CPP_CHECKED
    utf8ns::advance(ch2, 1, e);
#else
    utf8ns::advance(ch2, 1);
#endif
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
#ifdef UTF8CPP_CHECKED
        auto ch = utf8ns::next(pos, end);
#else
        auto ch = utf8ns::next(pos);
#endif

        bool found = false;
        for (auto d = dbegin; d != dend;)
        {
#ifdef UTF8CPP_CHECKED
            auto del = utf8ns::next(d, dend);
#else
            auto del = utf8ns::next(d);
#endif
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
