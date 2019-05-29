/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_PALETTE_H
#define EGT_PALETTE_H

/**
 * @file
 * @brief Color Palette definition.
 */

#include <egt/color.h>
#include <map>
#include <memory>

namespace egt
{
inline namespace v1
{

/**
 * Color palette.
 *
 * The Color Palette defines a bank of colors used by the Theme, and ultimately
 * widgets.
 *
 * The palette can be extended by simply inserting more groups and color
 * ids in the event the palette needs to be expanded to support more colors.
 */
class Palette
{
public:

    using pattern_type = Pattern;

    /**
     * @{
     * Pre-defined color.
     * See https://www.w3.org/TR/css-color-3/
     */
    constexpr static Color transparent = Color(0x00000000);
    constexpr static Color aliceblue = Color::rgb(0xf0f8ff);
    constexpr static Color antiquewhite = Color::rgb(0xfaebd7);
    constexpr static Color aqua = Color::rgb(0x00ffff);
    constexpr static Color aquamarine = Color::rgb(0x7fffd4);
    constexpr static Color azure = Color::rgb(0xf0ffff);
    constexpr static Color beige = Color::rgb(0xf5f5dc);
    constexpr static Color bisque = Color::rgb(0xffe4c4);
    constexpr static Color black = Color::rgb(0x000000);
    constexpr static Color blanchedalmond = Color::rgb(0xffebcd);
    constexpr static Color blue = Color::rgb(0x0000ff);
    constexpr static Color blueviolet = Color::rgb(0x8a2be2);
    constexpr static Color brown = Color::rgb(0xa52a2a);
    constexpr static Color burlywood = Color::rgb(0xdeb887);
    constexpr static Color cadetblue = Color::rgb(0x5f9ea0);
    constexpr static Color chartreuse = Color::rgb(0x7fff00);
    constexpr static Color chocolate = Color::rgb(0xd2691e);
    constexpr static Color coral = Color::rgb(0xff7f50);
    constexpr static Color cornflowerblue = Color::rgb(0x6495ed);
    constexpr static Color cornsilk = Color::rgb(0xfff8dc);
    constexpr static Color crimson = Color::rgb(0xdc143c);
    constexpr static Color cyan = Color::rgb(0x00ffff);
    constexpr static Color darkblue = Color::rgb(0x00008b);
    constexpr static Color darkcyan = Color::rgb(0x008b8b);
    constexpr static Color darkgoldenrod = Color::rgb(0xb8860b);
    constexpr static Color darkgray = Color::rgb(0xa9a9a9);
    constexpr static Color darkgreen = Color::rgb(0x006400);
    constexpr static Color darkgrey = Color::rgb(0xa9a9a9);
    constexpr static Color darkkhaki = Color::rgb(0xbdb76b);
    constexpr static Color darkmagenta = Color::rgb(0x8b008b);
    constexpr static Color darkolivegreen = Color::rgb(0x556b2f);
    constexpr static Color darkorange = Color::rgb(0xff8c00);
    constexpr static Color darkorchid = Color::rgb(0x9932cc);
    constexpr static Color darkred = Color::rgb(0x8b0000);
    constexpr static Color darksalmon = Color::rgb(0xe9967a);
    constexpr static Color darkseagreen = Color::rgb(0x8fbc8f);
    constexpr static Color darkslateblue = Color::rgb(0x483d8b);
    constexpr static Color darkslategray = Color::rgb(0x2f4f4f);
    constexpr static Color darkslategrey = Color::rgb(0x2f4f4f);
    constexpr static Color darkturquoise = Color::rgb(0x00ced1);
    constexpr static Color darkviolet = Color::rgb(0x9400d3);
    constexpr static Color deeppink = Color::rgb(0xff1493);
    constexpr static Color deepskyblue = Color::rgb(0x00bfff);
    constexpr static Color dimgray = Color::rgb(0x696969);
    constexpr static Color dimgrey = Color::rgb(0x696969);
    constexpr static Color dodgerblue = Color::rgb(0x1e90ff);
    constexpr static Color firebrick = Color::rgb(0xb22222);
    constexpr static Color floralwhite = Color::rgb(0xfffaf0);
    constexpr static Color forestgreen = Color::rgb(0x228b22);
    constexpr static Color fuchsia = Color::rgb(0xff00ff);
    constexpr static Color gainsboro = Color::rgb(0xdcdcdc);
    constexpr static Color ghostwhite = Color::rgb(0xf8f8ff);
    constexpr static Color gold = Color::rgb(0xffd700);
    constexpr static Color goldenrod = Color::rgb(0xdaa520);
    constexpr static Color gray = Color::rgb(0x808080);
    constexpr static Color green = Color::rgb(0x008000);
    constexpr static Color greenyellow = Color::rgb(0xadff2f);
    constexpr static Color grey = Color::rgb(0x808080);
    constexpr static Color honeydew = Color::rgb(0xf0fff0);
    constexpr static Color hotpink = Color::rgb(0xff69b4);
    constexpr static Color indianred = Color::rgb(0xcd5c5c);
    constexpr static Color indigo = Color::rgb(0x4b0082);
    constexpr static Color ivory = Color::rgb(0xfffff0);
    constexpr static Color khaki = Color::rgb(0xf0e68c);
    constexpr static Color lavender = Color::rgb(0xe6e6fa);
    constexpr static Color lavenderblush = Color::rgb(0xfff0f5);
    constexpr static Color lawngreen = Color::rgb(0x7cfc00);
    constexpr static Color lemonchiffon = Color::rgb(0xfffacd);
    constexpr static Color lightblue = Color::rgb(0xadd8e6);
    constexpr static Color lightcoral = Color::rgb(0xf08080);
    constexpr static Color lightcyan = Color::rgb(0xe0ffff);
    constexpr static Color lightgoldenrodyellow = Color::rgb(0xfafad2);
    constexpr static Color lightgray = Color::rgb(0xd3d3d3);
    constexpr static Color lightgreen = Color::rgb(0x90ee90);
    constexpr static Color lightgrey = Color::rgb(0xd3d3d3);
    constexpr static Color lightpink = Color::rgb(0xffb6c1);
    constexpr static Color lightsalmon = Color::rgb(0xffa07a);
    constexpr static Color lightseagreen = Color::rgb(0x20b2aa);
    constexpr static Color lightskyblue = Color::rgb(0x87cefa);
    constexpr static Color lightslategray = Color::rgb(0x778899);
    constexpr static Color lightslategrey = Color::rgb(0x778899);
    constexpr static Color lightsteelblue = Color::rgb(0xb0c4de);
    constexpr static Color lightyellow  = Color::rgb(0xffffe0);
    constexpr static Color lime = Color::rgb(0x00ff00);
    constexpr static Color limegreen = Color::rgb(0x32cd32);
    constexpr static Color linen = Color::rgb(0xfaf0e6);
    constexpr static Color magenta = Color::rgb(0xff00ff);
    constexpr static Color maroon = Color::rgb(0x800000);
    constexpr static Color mediumaquamarine = Color::rgb(0x66cdaa);
    constexpr static Color mediumblue = Color::rgb(0x0000cd);
    constexpr static Color mediumorchid = Color::rgb(0xba55d3);
    constexpr static Color mediumpurple = Color::rgb(0x9370db);
    constexpr static Color mediumseagreen = Color::rgb(0x3cb371);
    constexpr static Color mediumslateblue = Color::rgb(0x7b68ee);
    constexpr static Color mediumspringgreen = Color::rgb(0x00fa9a);
    constexpr static Color mediumturquoise = Color::rgb(0x48d1cc);
    constexpr static Color mediumvioletred = Color::rgb(0xc71585);
    constexpr static Color midnightblue = Color::rgb(0x191970);
    constexpr static Color mintcream = Color::rgb(0xf5fffa);
    constexpr static Color mistyrose = Color::rgb(0xffe4e1);
    constexpr static Color moccasin = Color::rgb(0xffe4b5);
    constexpr static Color navajowhite = Color::rgb(0xffdead);
    constexpr static Color navy = Color::rgb(0x000080);
    constexpr static Color oldlace = Color::rgb(0xfdf5e6);
    constexpr static Color olive = Color::rgb(0x808000);
    constexpr static Color olivedrab = Color::rgb(0x6b8e23);
    constexpr static Color orange = Color::rgb(0xffa500);
    constexpr static Color orangered = Color::rgb(0xff4500);
    constexpr static Color orchid = Color::rgb(0xda70d6);
    constexpr static Color palegoldenrod = Color::rgb(0xeee8aa);
    constexpr static Color palegreen = Color::rgb(0x98fb98);
    constexpr static Color paleturquoise = Color::rgb(0xafeeee);
    constexpr static Color palevioletred = Color::rgb(0xdb7093);
    constexpr static Color papayawhip = Color::rgb(0xffefd5);
    constexpr static Color peachpuff = Color::rgb(0xffdab9);
    constexpr static Color peru = Color::rgb(0xcd853f);
    constexpr static Color pink = Color::rgb(0xffc0cb);
    constexpr static Color plum = Color::rgb(0xdda0dd);
    constexpr static Color powderblue = Color::rgb(0xb0e0e6);
    constexpr static Color purple = Color::rgb(0x800080);
    constexpr static Color red  = Color::rgb(0xff0000);
    constexpr static Color rosybrown = Color::rgb(0xbc8f8f);
    constexpr static Color royalblue = Color::rgb(0x4169e1);
    constexpr static Color saddlebrown = Color::rgb(0x8b4513);
    constexpr static Color salmon = Color::rgb(0xfa8072);
    constexpr static Color sandybrown = Color::rgb(0xf4a460);
    constexpr static Color seagreen = Color::rgb(0x2e8b57);
    constexpr static Color seashell = Color::rgb(0xfff5ee);
    constexpr static Color sienna = Color::rgb(0xa0522d);
    constexpr static Color silver = Color::rgb(0xc0c0c0);
    constexpr static Color skyblue = Color::rgb(0x87ceeb);
    constexpr static Color slateblue = Color::rgb(0x6a5acd);
    constexpr static Color slategray = Color::rgb(0x708090);
    constexpr static Color slategrey = Color::rgb(0x708090);
    constexpr static Color snow = Color::rgb(0xfffafa);
    constexpr static Color springgreen = Color::rgb(0x00ff7f);
    constexpr static Color steelblue = Color::rgb(0x4682b4);
    constexpr static Color tan = Color::rgb(0xd2b48c);
    constexpr static Color teal = Color::rgb(0x008080);
    constexpr static Color thistle = Color::rgb(0xd8bfd8);
    constexpr static Color tomato = Color::rgb(0xff6347);
    constexpr static Color turquoise = Color::rgb(0x40e0d0);
    constexpr static Color violet = Color::rgb(0xee82ee);
    constexpr static Color wheat = Color::rgb(0xf5deb3);
    constexpr static Color white = Color::rgb(0xffffff);
    constexpr static Color whitesmoke = Color::rgb(0xf5f5f5);
    constexpr static Color yellow = Color::rgb(0xffff00);
    constexpr static Color yellowgreen = Color::rgb(0x9acd32);
    //@}

    enum class GroupId
    {
        /**
         * Default color group associated with a normal Widget state.
         */
        normal = 1,

        /**
         * Color group usually associated with the Widget::flag::active flag.
         */
        active = 2,

        /**
         * Color group usually associated with the Widget::flag::disabled flag.
         */
        disabled = 3
    };

    enum class ColorId
    {
        /**
         * General (Window, Frame) background color.
         */
        bg = 1,

        /**
         * Control (TextBox, ComboBox, RadioBox) text color.
         */
        text,

        /**
         * TextBox highlight color.
         */
        text_highlight,

        /**
         * TextBox cursor color.
         */
        cursor,

        /**
         * Border color.
         */
        border,

        /**
         * Button background color.
         */
        button_bg,

        /**
         * Button foreground color.
         */
        button_fg,

        /**
         * Button text color.
         */
        button_text,

        /**
         * Label background color.
         */
        label_bg,

        /**
         * Label text color.
         */
        label_text,
    };

    /**
     * Get a color.
     *
     * @param id Color id.
     * @param group Color group.
     * @return The color or pattern.
     */
    const pattern_type& color(ColorId id, GroupId group = GroupId::normal) const;

    /**
     * Set a color in the Palette.
     *
     * @param id Color id.
     * @param group Color group.
     * @param color The color or pattern.
     * @return Reference to the Palette instance.
     */
    Palette& set(ColorId id, GroupId group, const pattern_type& color);

    /**
     * Set a color in a Palette.
     *
     * @param id Color id.
     * @param color The color or pattern.
     * @param group Color group.
     * @return Reference to the Palette instance.
     */
    Palette& set(ColorId id, const pattern_type& color, GroupId group = GroupId::normal);

    /**
     * Remove a color from the Palette.
     *
     * @param id Color id.
     * @param group Color group.
     */
    void clear(ColorId id, GroupId group = GroupId::normal);

    /**
     * Check if a color exists in the palette.
     *
     * @param id Color id.
     * @param group Color group.
     * @return True if exists.
     */
    bool exists(ColorId id, GroupId group = GroupId::normal) const;

protected:

    std::map<GroupId, std::map<ColorId, pattern_type>> m_colors;
};

}
}

#endif
