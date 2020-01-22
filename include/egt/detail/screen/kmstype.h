/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_SCREEN_KMSTYPE_H
#define EGT_DETAIL_SCREEN_KMSTYPE_H

#include <memory>

struct plane_data;

namespace egt
{
inline namespace v1
{
namespace detail
{

/// @private
struct plane_t_deleter
{
    void operator()(plane_data* plane);
};

/**
 * Unique pointer for a cairo context.
 */
using unique_plane_t =
    std::unique_ptr<plane_data, detail::plane_t_deleter>;

}
}
}

#endif
