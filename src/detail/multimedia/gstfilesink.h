/*
 * Copyright (C) 2024 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SRC_DETAIL_MULTIMEDIA_GSTFILESINK_H
#define EGT_SRC_DETAIL_MULTIMEDIA_GSTFILESINK_H

#include <memory>
#include <string>

#include <gst/gst.h>

#include "detail/multimedia/gstsink.h"
#include "detail/multimedia/gstdecoderimpl.h"
#include "egt/detail/meta.h"
#include "gstdecoderimpl.h"

namespace egt
{
inline namespace v1
{
namespace detail
{

class GstFileSink : public GstSink
{
public:

    GstFileSink(GstDecoderImpl& gst_decoder, const Size& size,
                PixelFormat format, const std::string& file);

    EGT_NODISCARD std::string description() override;

    void draw(Painter& painter, const Rect& rect) override;

    bool post_initialize() override;

private:

    GstSample* m_videosample{nullptr};

    std::string m_output;
};

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt

#endif

