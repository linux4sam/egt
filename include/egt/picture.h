/*
 * Copyright (C) 2025 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_PICTURE_H
#define EGT_PICTURE_H

/**
 * @file
 * @brief Working with pictures.
 */

#include <egt/image.h>
#include <egt/widget.h>
#include <string>

namespace egt
{
inline namespace v1
{

class Frame;

/**
 * A widget with only an image and related properties.
 */
class EGT_API Picture final : public Widget
{
public:

    /**
     * @param[in] uri Resource path. @see @ref resources
     * @param[in] flags Align flags used during the layout computation.
     * @param[in] rect Initial rectangle of the Picture.
     */
    explicit Picture(const std::string& uri = {},
                     const AlignFlags& flags = {},
                     const Rect& rect = {}) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] uri Resource path. @see @ref resources
     * @param[in] flags Align flags used during the layout computation.
     * @param[in] rect Initial rectangle of the Picture.
     */
    explicit Picture(Frame& parent,
                     const std::string& uri = {},
                     const AlignFlags& flags = {},
                     const Rect& rect = {}) noexcept;

    /**
     * @param[in] props list of widget argument and its properties.
     */
    explicit Picture(Serializer::Properties& props) noexcept;
};

}
}

#endif
