/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <egt/app.h>
#include <egt/detail/video/gstdecoderimpl.h>
#include <string>

namespace egt
{
inline namespace v1
{
namespace detail
{

GstDecoderImpl::GstDecoderImpl(const Size& size)
    : m_size(size)
{}

bool GstDecoderImpl::playing() const
{
    GstState state = GST_STATE_VOID_PENDING;

    if (m_pipeline)
    {
        (void)gst_element_get_state(m_pipeline, &state, NULL,
                                    GST_CLOCK_TIME_NONE);
        return state == GST_STATE_PLAYING;
    }
    DBG("VideoWindow: Done " << __func__);
    return false;
}

bool GstDecoderImpl::play()
{
    if (m_pipeline)
    {
        auto ret = gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
        if (ret == GST_STATE_CHANGE_FAILURE)
        {
            DBG("VideoWindow: Unable to set the pipeline to the play state.\n");
            destroyPipeline();
            return false;
        }
    }
    DBG("VideoWindow: Done " << __func__);
    return true;
}

bool GstDecoderImpl::pause()
{
    if (m_pipeline)
    {
        auto ret = gst_element_set_state(m_pipeline, GST_STATE_PAUSED);
        if (ret == GST_STATE_CHANGE_FAILURE)
        {
            DBG("VideoWindow: Unable to set the pipeline to the Pause state.\n");
            destroyPipeline();
            return false;
        }
    }
    DBG("VideoWindow: Done " << __func__);
    return true;
}

double GstDecoderImpl::get_volume()
{
    if (!m_volume)
        return 0;

    gdouble volume = 0;
    g_object_get(m_volume, "volume", &volume, NULL);
    return volume * 10.0;
}

bool GstDecoderImpl::set_volume(double volume)
{
    if (!m_volume)
        return false;

    if (volume < 1)
        volume = 1;
    else if (volume > 10)
        volume = 10;
    else
        volume /= 10.0;

    g_object_set(m_volume, "volume", volume, NULL);
    return true;
}

bool GstDecoderImpl::seek_to_time(gint64 time_nanoseconds)
{
    if (!gst_element_seek_simple(m_pipeline, GST_FORMAT_TIME,
                                 (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT),
                                 time_nanoseconds))
    {
        DBG("VideoWindow: Seek failed");
        return false;
    }
    return true;
}

std::string GstDecoderImpl::get_error_message()
{
    return m_err_message;
}

uint64_t GstDecoderImpl::get_duration()
{
    return m_duration;
}

uint64_t GstDecoderImpl::get_position()
{
    return m_position;
}

void GstDecoderImpl::destroyPipeline()
{
    if (m_pipeline)
    {
        GstStateChangeReturn ret = gst_element_set_state(m_pipeline, GST_STATE_NULL);
        if (GST_STATE_CHANGE_FAILURE == ret)
        {
            ERR("VideoWindow: failed to set pipeline to GST_STATE_NULL");
        }
        g_object_unref(m_pipeline);
        m_pipeline = NULL;
    }
}

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt
