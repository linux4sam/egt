/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <egt/app.h>
#include <egt/detail/screen/kmsscreen.h>
#include <egt/detail/video/gstkmssinkimpl.h>
#include <egt/video.h>
#include <string>
#include <vector>

namespace egt
{
inline namespace v1
{
namespace detail
{
GstKmsSinkImpl::GstKmsSinkImpl(VideoWindow& interface, const Size& size, bool decodertype)
    : GstDecoderImpl(size),
      m_interface(interface),
      m_hwdecoder(decodertype)
{
}

#define HARDWARE_PIPE "uridecodebin uri=%s expose-all-streams=false name=srcVideo " \
					 " caps=video/x-raw srcVideo. ! video/x-raw,width=%d,height=%d,format=BGRx ! "\
					 " progressreport silent=true do-query=true update-freq=1 format=time " \
					 " name=progress ! g1kmssink gem-name=%d srcVideo. ! queue ! audioconvert ! " \
					 " volume name=volume ! alsasink async=false enable-last-sample=false sync=true "

#define SOFTWARE_PIPE  "uridecodebin uri=%s expose-all-streams=false name=video " \
					 " caps=video/x-raw video. ! queue ! videoscale  ! video/x-raw,width=%d,height=%d %s ! " \
					 " progressreport silent=true do-query=true update-freq=1 format=time name=progress ! " \
					 " g1kmssink gem-name=%d video. ! queue ! audioconvert ! volume name=volume ! " \
					 " alsasink async=false enable-last-sample=false sync=true"

bool GstKmsSinkImpl::set_media(const std::string& uri)
{
    GError* error = NULL;

    destroyPipeline();

    KMSOverlay* s = reinterpret_cast<KMSOverlay*>(m_interface.screen());
    assert(s);
    m_gem = s->gem();
    pixel_format format = detail::egt_format(s->get_plane_format());
    DBG("VideoWindow: egt_format = " << format);

    char buffer[2048];
    if (m_hwdecoder)
    {
        GstRegistry* reg = gst_registry_get();

        GstPluginFeature* g1h264_decode = gst_registry_lookup_feature(reg, "g1h264dec");
        if (g1h264_decode)
        {
            gst_plugin_feature_set_rank(g1h264_decode, GST_RANK_PRIMARY + 1);
            gst_object_unref(g1h264_decode);
        }

        GstPluginFeature* g1mp4_decode = gst_registry_lookup_feature(reg, "g1mp4dec");
        if (g1mp4_decode)
        {
            gst_plugin_feature_set_rank(g1mp4_decode, GST_RANK_PRIMARY + 1);
            gst_object_unref(g1mp4_decode);
        }

        GstPluginFeature* g1vp8_decode = gst_registry_lookup_feature(reg, "g1vp8dec");
        if (g1vp8_decode)
        {
            gst_plugin_feature_set_rank(g1vp8_decode, GST_RANK_PRIMARY + 1);
            gst_object_unref(g1vp8_decode);
        }

        if (uri.find("https://") != std::string::npos)
        {
            sprintf(buffer, HARDWARE_PIPE, uri.c_str(), m_width, m_height, m_gem);
        }
        else
        {
            sprintf(buffer, HARDWARE_PIPE, (std::string("file://") + uri).c_str(), m_width, m_height, m_gem);
        }
    }
    else
    {
        std::string vc = "! videoconvert ! video/x-raw,format=";
        if (format == pixel_format::yuv420)
            sprintf(buffer, SOFTWARE_PIPE, (std::string("file://") + uri).c_str(), m_width, m_height, "", m_gem);
        else if (format == pixel_format::yuyv)
            sprintf(buffer, SOFTWARE_PIPE, (std::string("file://") + uri).c_str(), m_width, m_height, (std::string(vc + "YUY2")).c_str(), m_gem);
        else
            sprintf(buffer, SOFTWARE_PIPE, (std::string("file://") + uri).c_str(), m_width, m_height, (std::string(vc + "BGRx")).c_str(), m_gem);
    }

    DBG("VideoWindow: " << std::string(buffer));

    m_pipeline = gst_parse_launch(buffer, &error);
    if (m_pipeline)
    {
        DBG("VideoWindow: Create pipeline success");

        m_volume = gst_bin_get_by_name(GST_BIN(m_pipeline), "volume");

        GstBus* bus;
        bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
        gst_bus_add_watch(bus, &bus_callback, this);
        gst_object_unref(bus);
        return true;
    }
    return false;
}

void GstKmsSinkImpl::top_draw()
{
    KMSOverlay* screen = reinterpret_cast<KMSOverlay*>(m_interface.screen());
    assert(screen);
    screen->apply();
}

void GstKmsSinkImpl::set_scale(float value)
{
    auto screen = reinterpret_cast<detail::KMSOverlay*>(m_interface.screen());
    assert(screen);
    screen->set_scale(value);
}

float GstKmsSinkImpl::scale()
{
    auto screen = reinterpret_cast<detail::KMSOverlay*>(m_interface.screen());
    assert(screen);
    return screen->scale();
}

void GstKmsSinkImpl::move(const Point& p)
{
    KMSOverlay* screen = reinterpret_cast<KMSOverlay*>(m_interface.screen());
    assert(screen);
    screen->set_position(DisplayPoint(p.x, p.y));
}

gboolean GstKmsSinkImpl::bus_callback(GstBus* bus, GstMessage* message, gpointer data)
{
    ignoreparam(bus);

    GError* error;
    gchar* debug;

    auto _this = reinterpret_cast<GstKmsSinkImpl*>(data);

    switch (GST_MESSAGE_TYPE(message))
    {
    case GST_MESSAGE_ERROR:
    {
        gst_message_parse_error(message, &error, &debug);
        _this->m_err_message = error->message;
        DBG("VideoWindow: GST_MESSAGE_ERROR from element " <<  message->src << "  " << error->message);
        DBG("VideoWindow: GST_MESSAGE_ERROR Debugging info: " << (debug ? debug : "none"));
        g_error_free(error);
        g_free(debug);
        asio::post(main_app().event().io(), std::bind(&VideoWindow::handle_gst_events,
                   &_this->m_interface, gsteventid::gst_error));
        break;
    }
    case GST_MESSAGE_WARNING:
        gst_message_parse_warning(message, &error, &debug);
        DBG("VideoWindow: GST_MESSAGE_WARNING from element " << message->src << "  " << error->message);
        DBG("VideoWindow: GST_MESSAGE_WARNING Debugging info: " << (debug ? debug : "none"));
        g_error_free(error);
        g_free(debug);
        break;
    case GST_MESSAGE_INFO:
    {
        gchar* name = gst_object_get_path_string(GST_MESSAGE_SRC(message));
        gst_message_parse_info(message, &error, &debug);
        DBG("VideoWindow: GST_MESSAGE_INFO: " << error->message);
        if (debug)
        {
            DBG("VideoWindow: GST_MESSAGE_INFO: \n" << debug << "\n");
        }
        g_clear_error(&error);
        g_free(debug);
        g_free(name);
        break;
    }
    break;
    case GST_MESSAGE_CLOCK_PROVIDE:
        DBG("VideoWindow: GST_MESSAGE_CLOCK_PROVIDE");
        break;
    case GST_MESSAGE_CLOCK_LOST:
        DBG("VideoWindow: GST_MESSAGE_CLOCK_LOST");
        break;
    case GST_MESSAGE_NEW_CLOCK:
        DBG("VideoWindow: GST_MESSAGE_NEW_CLOCK");
        break;
    case GST_MESSAGE_EOS:
    {
        DBG("VideoWindow: GST_MESSAGE_EOS");
        gst_element_seek(_this->m_pipeline, 1.0, GST_FORMAT_TIME,
                         GST_SEEK_FLAG_FLUSH,
                         GST_SEEK_TYPE_SET, 0,
                         GST_SEEK_TYPE_NONE, -1);

        gst_element_set_state(_this->m_pipeline, GST_STATE_PLAYING);

        break;
    }
    case GST_MESSAGE_PROGRESS:
        DBG("VideoWindow: GST_MESSAGE_PROGRESS");
        break;
    case GST_MESSAGE_DURATION_CHANGED:
        DBG("VideoWindow: GST_MESSAGE_DURATION_CHANGED");
        break;
    case GST_MESSAGE_ELEMENT:
    {
        const GstStructure* info = gst_message_get_structure(message);
        if (!std::string(gst_structure_get_name(info)).compare("progress"))
        {
            gint64 pos, len;
            if (gst_element_query_position(_this->m_pipeline, GST_FORMAT_TIME, &pos)
                && gst_element_query_duration(_this->m_pipeline, GST_FORMAT_TIME, &len))
            {
                _this->m_position = pos;
                _this->m_duration = len;
            }
            asio::post(main_app().event().io(), std::bind(&VideoWindow::handle_gst_events,
                       &_this->m_interface, gsteventid::gst_progress));
        }
        break;
    }
    default:
        break;
    }
    /* we want to be notified again the next time there is a message
     * on the bus, so returning TRUE (FALSE means we want to stop watching
     * for messages on the bus and our callback should not be called again)
     */
    return TRUE;
}

} // End of namespace detail

} // End of inline namespace v1

} //End of namespace egt
