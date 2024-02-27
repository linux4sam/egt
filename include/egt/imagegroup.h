/*
 * Copyright (C) 2024 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_IMAGE_GROUP_H
#define EGT_IMAGE_GROUP_H

/**
 * @file
 * @brief Gather images, one per Palette::GroupId.
 */

#include <egt/detail/meta.h>
#include <egt/palette.h>

namespace egt
{
inline namespace v1
{

class Image;

class EGT_API ImageGroup
{
public:
    ImageGroup(const std::string& suffix) noexcept;

    EGT_NODISCARD Image* get(Palette::GroupId group, bool allow_fallback = false) const;
    void set(Palette::GroupId group, const Image& image);
    bool reset(Palette::GroupId group);

    void serialize(Serializer& serializer) const;
    bool deserialize(const std::string& name, const std::string& value);

private:

    std::unique_ptr<Image>& select(Palette::GroupId group, bool allow_fallback = false);
    const std::unique_ptr<Image>& select(Palette::GroupId group, bool allow_fallback) const
    {
        return const_cast<ImageGroup*>(this)->select(group, allow_fallback);
    }

    std::string m_suffix;
    std::unique_ptr<Image> m_normal;
    std::unique_ptr<Image> m_active;
    std::unique_ptr<Image> m_disabled;
    std::unique_ptr<Image> m_checked;
};

}
}

#endif
