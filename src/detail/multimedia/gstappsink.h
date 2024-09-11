/*
 * Copyright (C) 2024 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SRC_DETAIL_MULTIMEDIA_GSTAPPSINK_H
#define EGT_SRC_DETAIL_MULTIMEDIA_GSTAPPSINK_H

#include <memory>
#include <string>

#include <gst/gst.h>

#include "detail/multimedia/gstsink.h"
#include "egt/detail/meta.h"

namespace egt
{
inline namespace v1
{
namespace detail
{

class GstDecoderImpl;

class GstAppSink : public GstSink
{
public:

    GstAppSink(GstDecoderImpl& gst_decoder, const Size& size, Window& interface);

    EGT_NODISCARD std::string description() override;

    void draw(Painter& painter, const Rect& rect) override;

    bool post_initialize() override;

private:

    static GstFlowReturn on_new_buffer(GstElement* elt, gpointer data);

    GstSample* m_videosample{nullptr};

    Window& m_interface;
};

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt

#endif
