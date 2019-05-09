/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_GST_DECODER_IMPL_H
#define EGT_GST_DECODER_IMPL_H

#include <gst/gst.h>
#include <gst/pbutils/pbutils.h>
#include <gst/pbutils/gstdiscoverer.h>

#include <egt/geometry.h>
#include <egt/painter.h>

namespace egt
{
inline namespace v1
{
namespace detail
{

enum class gsteventid
{
    none,
    gst_progress,
    gst_stop,
    gst_error,
};

class GstDecoderImpl
{
public:

    GstDecoderImpl() = delete;

    GstDecoderImpl(const Size& size);

    virtual bool set_media(const std::string& filename) = 0;

    virtual void draw(Painter& painter, const Rect& rect) = 0;

    virtual bool play();

    virtual bool pause();

    virtual bool playing() const;

    virtual bool set_volume(double volume);

    virtual double get_volume();

    virtual std::string get_error_message();

    virtual uint64_t get_duration();

    virtual uint64_t get_position();

    virtual bool seek_to_time(gint64 time_nanoseconds);

    virtual void destroyPipeline();

    virtual void push_buffer(GstSample* sample)
    {
        ignoreparam(sample);
    }

protected:
    GstElement* m_pipeline{nullptr};
    Size m_size;
    uint64_t m_duration{0};
    uint64_t m_position{0};
    GstElement* m_volume{nullptr};
    std::string m_err_message;
};

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt

#endif
