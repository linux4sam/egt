/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/enum.h"
#include "egt/detail/serialize.h"
#include "egt/detail/string.h"
#include "egt/palette.h"
#include <iostream>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

namespace egt
{
inline namespace v1
{

const Pattern& Palette::color(ColorId id, GroupId group) const
{
    const auto g = m_colors.find(group);
    if (g != m_colors.end())
    {
        const auto c = g->second.find(id);
        if (c != g->second.end())
            return c->second;
    }

    throw std::runtime_error(fmt::format("color not found in palette:{}/{}",
                                         group, id));
}

Palette& Palette::set(ColorId id, GroupId group, const Pattern& color)
{
    m_colors[group][id] = color;
    return *this;
}

Palette& Palette::set(ColorId id, const Pattern& color, GroupId group)
{
    return set(id, group, color);
}

void Palette::clear(ColorId id, GroupId group)
{
    const auto g = m_colors.find(group);
    if (g != m_colors.end())
        g->second.erase(id);
}

void Palette::clear()
{
    m_colors.clear();
}

bool Palette::exists(ColorId id, GroupId group) const
{
    auto g = m_colors.find(group);
    if (g != m_colors.end())
        return g->second.find(id) != g->second.end();

    return false;
}

constexpr Color Palette::transparent;
constexpr Color Palette::aliceblue;
constexpr Color Palette::antiquewhite;
constexpr Color Palette::aqua;
constexpr Color Palette::aquamarine;
constexpr Color Palette::azure;
constexpr Color Palette::beige;
constexpr Color Palette::bisque;
constexpr Color Palette::black;
constexpr Color Palette::blanchedalmond;
constexpr Color Palette::blue;
constexpr Color Palette::blueviolet;
constexpr Color Palette::brown;
constexpr Color Palette::burlywood;
constexpr Color Palette::cadetblue;
constexpr Color Palette::chartreuse;
constexpr Color Palette::chocolate;
constexpr Color Palette::coral;
constexpr Color Palette::cornflowerblue;
constexpr Color Palette::cornsilk;
constexpr Color Palette::crimson;
constexpr Color Palette::cyan;
constexpr Color Palette::darkblue;
constexpr Color Palette::darkcyan;
constexpr Color Palette::darkgoldenrod;
constexpr Color Palette::darkgray;
constexpr Color Palette::darkgreen;
constexpr Color Palette::darkgrey;
constexpr Color Palette::darkkhaki;
constexpr Color Palette::darkmagenta;
constexpr Color Palette::darkolivegreen;
constexpr Color Palette::darkorange;
constexpr Color Palette::darkorchid;
constexpr Color Palette::darkred;
constexpr Color Palette::darksalmon;
constexpr Color Palette::darkseagreen;
constexpr Color Palette::darkslateblue;
constexpr Color Palette::darkslategray;
constexpr Color Palette::darkslategrey;
constexpr Color Palette::darkturquoise;
constexpr Color Palette::darkviolet;
constexpr Color Palette::deeppink;
constexpr Color Palette::deepskyblue;
constexpr Color Palette::dimgray;
constexpr Color Palette::dimgrey;
constexpr Color Palette::dodgerblue;
constexpr Color Palette::firebrick;
constexpr Color Palette::floralwhite;
constexpr Color Palette::forestgreen;
constexpr Color Palette::fuchsia;
constexpr Color Palette::gainsboro;
constexpr Color Palette::ghostwhite;
constexpr Color Palette::gold;
constexpr Color Palette::goldenrod;
constexpr Color Palette::gray;
constexpr Color Palette::green;
constexpr Color Palette::greenyellow;
constexpr Color Palette::grey;
constexpr Color Palette::honeydew;
constexpr Color Palette::hotpink;
constexpr Color Palette::indianred;
constexpr Color Palette::indigo;
constexpr Color Palette::ivory;
constexpr Color Palette::khaki;
constexpr Color Palette::lavender;
constexpr Color Palette::lavenderblush;
constexpr Color Palette::lawngreen;
constexpr Color Palette::lemonchiffon;
constexpr Color Palette::lightblue;
constexpr Color Palette::lightcoral;
constexpr Color Palette::lightcyan;
constexpr Color Palette::lightgoldenrodyellow;
constexpr Color Palette::lightgray;
constexpr Color Palette::lightgreen;
constexpr Color Palette::lightgrey;
constexpr Color Palette::lightpink;
constexpr Color Palette::lightsalmon;
constexpr Color Palette::lightseagreen;
constexpr Color Palette::lightskyblue;
constexpr Color Palette::lightslategray;
constexpr Color Palette::lightslategrey;
constexpr Color Palette::lightsteelblue;
constexpr Color Palette::lightyellow;
constexpr Color Palette::lime;
constexpr Color Palette::limegreen;
constexpr Color Palette::linen;
constexpr Color Palette::magenta;
constexpr Color Palette::maroon;
constexpr Color Palette::mediumaquamarine;
constexpr Color Palette::mediumblue;
constexpr Color Palette::mediumorchid;
constexpr Color Palette::mediumpurple;
constexpr Color Palette::mediumseagreen;
constexpr Color Palette::mediumslateblue;
constexpr Color Palette::mediumspringgreen;
constexpr Color Palette::mediumturquoise;
constexpr Color Palette::mediumvioletred;
constexpr Color Palette::midnightblue;
constexpr Color Palette::mintcream;
constexpr Color Palette::mistyrose;
constexpr Color Palette::moccasin;
constexpr Color Palette::navajowhite;
constexpr Color Palette::navy;
constexpr Color Palette::oldlace;
constexpr Color Palette::olive;
constexpr Color Palette::olivedrab;
constexpr Color Palette::orange;
constexpr Color Palette::orangered;
constexpr Color Palette::orchid;
constexpr Color Palette::palegoldenrod;
constexpr Color Palette::palegreen;
constexpr Color Palette::paleturquoise;
constexpr Color Palette::palevioletred;
constexpr Color Palette::papayawhip;
constexpr Color Palette::peachpuff;
constexpr Color Palette::peru;
constexpr Color Palette::pink;
constexpr Color Palette::plum;
constexpr Color Palette::powderblue;
constexpr Color Palette::purple;
constexpr Color Palette::red;
constexpr Color Palette::rosybrown;
constexpr Color Palette::royalblue;
constexpr Color Palette::saddlebrown;
constexpr Color Palette::salmon;
constexpr Color Palette::sandybrown;
constexpr Color Palette::seagreen;
constexpr Color Palette::seashell;
constexpr Color Palette::sienna;
constexpr Color Palette::silver;
constexpr Color Palette::skyblue;
constexpr Color Palette::slateblue;
constexpr Color Palette::slategray;
constexpr Color Palette::slategrey;
constexpr Color Palette::snow;
constexpr Color Palette::springgreen;
constexpr Color Palette::steelblue;
constexpr Color Palette::tan;
constexpr Color Palette::teal;
constexpr Color Palette::thistle;
constexpr Color Palette::tomato;
constexpr Color Palette::turquoise;
constexpr Color Palette::violet;
constexpr Color Palette::wheat;
constexpr Color Palette::white;
constexpr Color Palette::whitesmoke;
constexpr Color Palette::yellow;
constexpr Color Palette::yellowgreen;

template<>
const std::pair<Palette::ColorId, char const*> detail::EnumStrings<Palette::ColorId>::data[] =
{
    {Palette::ColorId::bg, "bg"},
    {Palette::ColorId::text, "text"},
    {Palette::ColorId::text_highlight, "text_highlight"},
    {Palette::ColorId::cursor, "cursor"},
    {Palette::ColorId::border, "border"},
    {Palette::ColorId::button_bg, "button_bg"},
    {Palette::ColorId::button_fg, "button_fg"},
    {Palette::ColorId::button_text, "button_text"},
    {Palette::ColorId::label_bg, "label_bg"},
    {Palette::ColorId::label_text, "label_text"},
};

std::ostream& operator<<(std::ostream& os, const Palette::ColorId& color)
{
    return os << detail::enum_to_string(color);
}

template<>
const std::pair<Palette::GroupId, char const*> detail::EnumStrings<Palette::GroupId>::data[] =
{
    {Palette::GroupId::normal, "normal"},
    {Palette::GroupId::active, "active"},
    {Palette::GroupId::disabled, "disabled"},
    {Palette::GroupId::checked, "checked"},
};

std::ostream& operator<<(std::ostream& os, const Palette::GroupId& group)
{
    return os << detail::enum_to_string(group);
}

void Palette::serialize(const std::string& name, detail::Serializer& serializer) const
{
    for (const auto& x : m_colors)
    {
        for (const auto& y : x.second)
        {
            const std::map<std::string, std::string> attrs =
            {
                {"id", detail::enum_to_string(y.first)},
                {"group", detail::enum_to_string(x.first)},
            };

            serializer.add_property(name,
                                    y.second.color().hex(),
                                    attrs);
        }
    }
}

void Palette::deserialize(const std::string& name, const std::string& value,
                          const std::map<std::string, std::string>& attrs)
{
    const auto i = attrs.find("id");
    const auto g = attrs.find("group");
    if (i != attrs.end() && g != attrs.end())
    {
        const auto id = detail::enum_from_string<Palette::ColorId>(i->second);
        Palette::GroupId group = Palette::GroupId::normal;
        if (g != attrs.end())
            group = detail::enum_from_string<Palette::GroupId>(g->second);

        set(id, Color(std::stoul(value, nullptr, 16)), group);
    }
}

}
}
