/*
 * Copyright (C) 2025 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/frame.h"
#include "egt/picture.h"

namespace egt
{
inline namespace v1
{

Picture::Picture(const std::string& uri,
                 const AlignFlags& flags,
                 const Rect& rect) noexcept
    : Widget(rect)
{
    name("Picture" + std::to_string(m_widgetid));

    align(flags);
    fill_flags(Theme::FillFlag::blend);
    if (!uri.empty())
    {
        Image image(uri);

        if (!image.empty())
        {
            if (rect.empty())
            {
                auto size = image.size();
                auto m = moat();
                size += Size(m, m);
                resize(size);
            }
            background(image);
        }
    }
}

Picture::Picture(Frame& parent,
                 const std::string& uri,
                 const AlignFlags& flags,
                 const Rect& rect) noexcept
    : Picture(uri, flags, rect)
{
    parent.add(*this);
}

Picture::Picture(Serializer::Properties& props) noexcept
    : Widget(props, true)
{
    deserialize_leaf(props);
}

}
}
