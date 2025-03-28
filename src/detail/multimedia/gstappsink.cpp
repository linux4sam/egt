/*
 * Copyright (C) 2024 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "gstappsink.h"

#include "detail/egtlog.h"
#include "detail/multimedia/gstdecoderimpl.h"
#include "egt/detail/screen/kmsoverlay.h"
#include <egt/types.h>

namespace egt
{
inline namespace v1
{
namespace detail
{

GstAppSink::GstAppSink(GstDecoderImpl& gst_decoder, const Size& size, Window& window)
    : GstSink(gst_decoder, size, window.format()),
      m_videosample(nullptr, gst_sample_unref),
      m_window(window)
{
    EGTLOG_DEBUG("GstAppSink::GstAppSink: size={}, format={}",
                 size, detail::gstreamer_format(m_format));
}

std::string GstAppSink::description()
{
    auto size = m_size;
    /*
     * When we check if the window is a plane window, we assume that the
     * plane is a heo overlay. Unfortunately there is no way to check the type
     * of the overlay, there is not such information at the kernel level.
     */
    if (m_window.plane_window())
    {
        /*
        * In the case of an heo plane, if scaling occurred, the window had been
        * resized and is no longer corresponding to the original size. It means
        * that configuring the pipeline with this new size will cause two issues:
        * - Software scaling will happen as the video size is now different from
        * the window size.
        * - The heo plane is still configured to scale the video so we'll do a
        * hardware scaling in addition to the software scaling.
        */
        if (!m_window.user_requested_box().empty())
        {
            size = m_window.user_requested_box().size();
            const auto moat = m_window.moat();
            size -= Size(2. * moat, 2. * moat);
        }
        else
        {
            size = m_window.content_area().size();
        }
    }

    /*
     * If no size is provided when constructing a video or camera window, a
     * plane implementation has a size set to 32x32 while a basic window
     * implementation has a size set to 0x0. If we want to display of video,
     * we need a size higher than 0x0! So, resize the window to 32x32 for
     * consistency with the default size for plane window.
     */
    if (m_size.empty())
    {
        m_window.resize(Size(32, 32));
        m_size = Size(32, 32);
        const auto moat = m_window.moat();
        size = Size(32, 32) - Size(2. * moat, 2. * moat);
    }

    const auto video_caps_filter =
        fmt::format("caps=video/x-raw,width={},height={},format={}",
                    size.width(), size.height(), detail::gstreamer_format(m_format));

    return fmt::format("videoconvert ! videoscale ! capsfilter name=vcaps {} ! appsink name=appsink", video_caps_filter);
}

void GstAppSink::draw(Painter& painter, const Rect& rect)
{
    ignoreparam(rect);
    /*
     * its a Basic window copying buffer to Cairo surface.
     */
    if (m_videosample)
    {
        GstCaps* caps = gst_sample_get_caps(m_videosample.get());
        GstStructure* capsStruct = gst_caps_get_structure(caps, 0);
        int width = 0;
        int height = 0;
        gst_structure_get_int(capsStruct, "width", &width);
        gst_structure_get_int(capsStruct, "height", &height);

        GstBuffer* buffer = gst_sample_get_buffer(m_videosample.get());
        if (buffer)
        {
            GstMapInfo map;
            if (gst_buffer_map(buffer, &map, GST_MAP_READ))
            {
                auto box = m_window.box();
                const auto format = detail::cairo_format(m_format);
                auto surface = unique_cairo_surface_t(
                                   cairo_image_surface_create_for_data(map.data,
                                           format,
                                           width,
                                           height,
                                           cairo_format_stride_for_width(format, width)));

                if (cairo_surface_status(surface.get()) == CAIRO_STATUS_SUCCESS)
                {
                    auto cr = painter.context().get();
                    /*
                     * Cairo scaling should occur infrequently, but it is
                     * necessary when a video resize takes place. When the caps
                     * filter is updated with the new size, the pipeline does
                     * not update immediately. Consequently, a few frames with
                     * the previous size may be received while the window size
                     * has already been adjusted.
                     */
                    if (width != box.width() || height != box.height())
                    {
                        double scalex = static_cast<double>(box.width()) / width;
                        double scaley = static_cast<double>(box.height()) / height;
                        cairo_scale(cr, scalex, scaley);
                    }
                    cairo_set_source_surface(cr, surface.get(), box.x(), box.y());
                    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
                    cairo_paint(cr);
                }

                gst_buffer_unmap(buffer, &map);
            }
        }
    }
}

GstFlowReturn GstAppSink::on_new_buffer(GstElement* elt, gpointer data)
{
    auto impl = static_cast<GstAppSink*>(data);
    GstSample* sample;
    g_signal_emit_by_name(elt, "pull-sample", &sample);
    if (sample)
    {
#ifdef HAVE_LIBPLANES
        if (impl->m_window.plane_window())
        {
            GstCaps* caps = gst_sample_get_caps(sample);
            GstStructure* capsStruct = gst_caps_get_structure(caps, 0);
            int width = 0;
            int height = 0;
            gst_structure_get_int(capsStruct, "width", &width);
            gst_structure_get_int(capsStruct, "height", &height);
            auto vs = egt::Size(width, height);
            auto b = impl->m_window.content_area();
            /*
             * If scaling is requested, it's normal that the size of the
             * VideoWindow is different from the size of the video. Don't drop
             * the frame in this case.
             * Caution: checking only the scale may not enough to justifiy that
             * the window and video size are different and it won't lead to a
             * crash.
             */
            if (detail::float_equal(impl->m_window.hscale(), 1.0f)
                && detail::float_equal(impl->m_window.vscale(), 1.0f))
            {
                if (b.size() != vs)
                {
                    if (Application::check_instance())
                    {
                        asio::post(Application::instance().event().io(), [impl, vs, b]()
                        {
                            if (vs.width() < b.width() || vs.height() < b.height())
                                impl->m_window.resize(vs);
                            else
                                impl->m_window.resize(b.size());
                        });
                    }

                    // drop this frame and continue
                    gst_sample_unref(sample);
                    return GST_FLOW_OK;
                }
            }

            GstBuffer* buffer = gst_sample_get_buffer(sample);
            if (buffer)
            {
                GstMapInfo map;
                if (gst_buffer_map(buffer, &map, GST_MAP_READ))
                {
                    auto screen =
                        reinterpret_cast<detail::KMSOverlay*>(impl->m_window.screen());
                    assert(screen);
                    memcpy(screen->raw(), map.data, map.size);
                    screen->schedule_flip();
                    gst_buffer_unmap(buffer, &map);
                }
            }
        }
#endif
        if (Application::check_instance())
        {
            asio::post(Application::instance().event().io(), [impl, sample]()
            {
                impl->m_videosample.reset(sample);
                if (!impl->m_window.plane_window())
                    impl->m_window.damage();
            });
        }
        else
        {
            gst_sample_unref(sample);
        }
        return GST_FLOW_OK;
    }
    return GST_FLOW_ERROR;
}

bool GstAppSink::post_initialize()
{
    if (!GstSink::post_initialize())
        return false;

    EGTLOG_DEBUG("GstAppSink::post_initialize: retrieve appsink element");
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
    auto appsink = gst_bin_get_by_name(GST_BIN(m_gst_decoder.pipeline()), "appsink");
    if (!appsink)
    {
        detail::error("failed to get app sink element");
        m_gst_decoder.on_error.invoke("failed to get app sink element");
        return false;
    }

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
    g_object_set(G_OBJECT(appsink), "emit-signals", TRUE, "sync", TRUE, nullptr);
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
    g_signal_connect(appsink, "new-sample", G_CALLBACK(on_new_buffer), this);

    gst_object_unref(appsink);

    return true;
}

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt
