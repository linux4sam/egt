/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/utf8text.h"
#include "egt/canvas.h"
#include "egt/painter.h"
#include "egt/serialize.h"
#include "egt/textwidget.h"
#include <deque>

namespace egt
{
inline namespace v1
{

TextWidget::TextWidget(std::string text,
                       const Rect& rect,
                       // NOLINTNEXTLINE(modernize-pass-by-value)
                       const AlignFlags& text_align) noexcept
    : Widget(rect),
      m_text_align(text_align),
      m_text(std::move(text))
{}

TextWidget::TextWidget(Serializer::Properties& props, bool is_derived) noexcept
    : Widget(props, true)
{
    deserialize(props);

    if (!is_derived)
        deserialize_leaf(props);
}

void TextWidget::clear()
{
    if (!m_text.empty())
    {
        m_text.clear();
        on_text_changed.invoke();
        damage();
    }
}

void TextWidget::text(const std::string& str)
{
    if (detail::change_if_diff<>(m_text, str))
    {
        on_text_changed.invoke();
        damage();
        parent_layout();
    }
}

size_t TextWidget::len() const
{
    return detail::utf8len(m_text);
}

Font TextWidget::scale_font(const Size& target, const std::string& text, const Font& font)
{
    Canvas canvas(Size(10, 10));
    Painter painter(canvas.context());

    auto nfont = font;
    while (true)
    {
        painter.set(nfont);

        cairo_text_extents_t textext;
        cairo_text_extents(painter.context().get(), text.c_str(), &textext);

        if (textext.width - textext.x_bearing < target.width() &&
            textext.height - textext.y_bearing < target.height())
            return nfont;

        nfont.size(nfont.size() - 1);
        if (nfont.size() < 1)
            return font;
    }
    return nfont;
}

struct SizeCache
{
    struct SizeItem
    {
        std::string text;
        Font font;
    };

    std::deque<std::pair<SizeItem, Size>>::iterator find(const std::string& text, const Font& font)
    {
        for (auto i = cache.begin(); i != cache.end(); ++i)
        {
            if (i->first.text == text && i->first.font == font)
                return i;
        }

        return cache.end();
    }

    std::deque<std::pair<SizeItem, Size>>::iterator end()
    {
        return cache.end();
    }

    void add(const std::pair<SizeItem, Size>& item)
    {
        static constexpr auto MAX_CACHE_ITEM_SIZE = 1024;
        if (item.first.text.size() < MAX_CACHE_ITEM_SIZE)
        {
            cache.emplace_back(item);

            static constexpr auto MAX_CACHE_ITEMS = 128;
            while (cache.size() > MAX_CACHE_ITEMS)
                cache.pop_front();
        }
    }

private:
    std::deque<std::pair<SizeItem, Size>> cache;
};

static SizeCache size_cache;

Size TextWidget::text_size(const std::string& text) const
{
    auto i = size_cache.find(text, this->font());
    if (i != size_cache.end())
        return i->second;

    Canvas canvas(Size(100, 100));
    Painter painter(canvas.context());
    painter.set(this->font());

    auto size = painter.font_size(text);
    size_cache.add(std::make_pair(SizeCache::SizeItem{text, this->font()}, size));
    return size;
}

void TextWidget::serialize(Serializer& serializer) const
{
    Widget::serialize(serializer);
    if (!text().empty())
        serializer.add_property("text", text());
    if (!text_align().empty())
        serializer.add_property("text_align", text_align());
}

void TextWidget::deserialize(Serializer::Properties& props)
{
    props.erase(std::remove_if(props.begin(), props.end(), [&](auto & p)
    {
        if (std::get<0>(p) == "text")
            text(std::get<1>(p));
        else if (std::get<0>(p) == "text_align")
            text_align(AlignFlags(std::get<1>(p)));
        else
            return false;

        return true;
    }), props.end());

}

}
}
