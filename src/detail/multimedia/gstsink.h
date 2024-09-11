/*
 * Copyright (C) 2024 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SRC_DETAIL_MULTIMEDIA_GSTSINK_H
#define EGT_SRC_DETAIL_MULTIMEDIA_GSTSINK_H

#include <memory>
#include <string>

#include <gst/gst.h>

#include "egt/detail/meta.h"
#include "egt/geometry.h"
#include "egt/painter.h"
#include "egt/window.h"

namespace egt
{
inline namespace v1
{
namespace detail
{

class GstDecoderImpl;

class GstSink
{
public:

    GstSink(GstDecoderImpl& gst_decoder, const Size& size, PixelFormat format);

    EGT_NODISCARD virtual std::string description() = 0;

    virtual void draw(Painter& painter, const Rect& rect) = 0;

    virtual bool post_initialize();

    void size(const egt::Size& size);

protected:

    GstDecoderImpl& m_gst_decoder;
    GstElement* m_vcapsfilter{nullptr};
    egt::Size m_size;
    PixelFormat m_format;
};

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt

#endif
