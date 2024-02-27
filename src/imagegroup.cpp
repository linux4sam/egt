/*
 * Copyright (C) 2024 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/image.h"
#include "egt/imagegroup.h"

namespace egt
{
inline namespace v1
{

ImageGroup::ImageGroup(const std::string& suffix) noexcept
    : m_suffix(suffix)
{
}

std::unique_ptr<Image>& ImageGroup::select(Palette::GroupId group,
        bool allow_fallback)
{
    std::unique_ptr<Image>* img;

    switch (group)
    {
    default:
    case Palette::GroupId::normal:
        img = &m_normal;
        break;

    case Palette::GroupId::active:
        img = (m_active || !allow_fallback) ? &m_active : &m_normal;
        break;

    case Palette::GroupId::disabled:
        img = (m_disabled || !allow_fallback) ? &m_disabled : &m_normal;
        break;

    case Palette::GroupId::checked:
        img = (m_checked || !allow_fallback) ? &m_checked : &m_normal;
        break;
    }

    return *img;
}

Image* ImageGroup::get(Palette::GroupId group,
                       bool allow_fallback) const
{
    return select(group, allow_fallback).get();
}

void ImageGroup::set(Palette::GroupId group,
                     const Image& image)
{
    select(group) = std::make_unique<Image>(image);
}

bool ImageGroup::reset(Palette::GroupId group)
{
    auto& ptr = select(group);
    if (!ptr)
        return false;

    ptr.reset();
    return true;
}

void ImageGroup::serialize(Serializer& serializer) const
{
    if (m_normal)
        serializer.add_property(std::string("normal_") + m_suffix, m_normal->uri());
    if (m_active)
        serializer.add_property(std::string("active_") + m_suffix, m_active->uri());
    if (m_disabled)
        serializer.add_property(std::string("disabled_") + m_suffix, m_disabled->uri());
    if (m_checked)
        serializer.add_property(std::string("checked_") + m_suffix, m_checked->uri());
}

bool ImageGroup::deserialize(const std::string& name, const std::string& value)
{
    Palette::GroupId group;

    if (name == std::string("normal_") + m_suffix)
        group = Palette::GroupId::normal;
    else if (name == std::string("active_") + m_suffix)
        group = Palette::GroupId::active;
    else if (name == std::string("disabled_") + m_suffix)
        group = Palette::GroupId::disabled;
    else if (name == std::string("checked_") + m_suffix)
        group = Palette::GroupId::checked;
    else
        return false;

    set(group, Image(value));
    return true;
}

}
}
