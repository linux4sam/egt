/*
 * Copyright (C) 2023 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "detail/video/gstmeta.h"

#include <string>

#include <gst/gst.h>

#include "detail/egtlog.h"


namespace egt
{
inline namespace v1
{
namespace detail
{

std::string gstreamer_get_device_path(GstDevice* device)
{
    std::string devnode;
    GstStructureHandle props{gst_device_get_properties(device)};
    if (props)
    {
        EGTLOG_DEBUG("device properties: {}", gst_structure_to_string(props.get()));
        devnode = gst_structure_get_string(props.get(), "device.path");
    }

    return devnode;
}

}
}
}
