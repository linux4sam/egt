/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/math.h"
#include "egt/gauge.h"

namespace egt
{
inline namespace v1
{
namespace experimental
{

// NOLINTNEXTLINE(modernize-pass-by-value)
GaugeLayer::GaugeLayer(const Image& image) noexcept
    : m_image(image)
{
    name("GaugeLayer" + std::to_string(m_widgetid));

    if (!m_image.empty())
        m_box.size(m_image.size());
}

GaugeLayer::GaugeLayer(Gauge& gauge, const Image& image) noexcept
    : GaugeLayer(image)
{
    gauge.add(*this);
}

void GaugeLayer::draw(Painter& painter, const Rect&)
{
    painter.draw(point());
    painter.draw(m_image);
}

NeedleLayer::NeedleLayer(const Image& image,
                         float min,
                         float max,
                         float angle_start,
                         float angle_stop,
                         bool clockwise) noexcept
    : GaugeLayer(image),
      m_min(min),
      m_max(max),
      m_value(m_min),
      m_angle_start(angle_start),
      m_angle_stop(angle_stop),
      m_clockwise(clockwise)
{
    name("NeedleLayer" + std::to_string(m_widgetid));
    assert(m_max > m_min);
}

static void draw_image(Painter& painter,
                       const PointF& point,
                       const PointF& needle_center,
                       const Image& image,
                       float angle)
{
    Painter::AutoSaveRestore sr(painter);
    auto cr = painter.context().get();

    cairo_translate(cr, point.x(), point.y());
    cairo_rotate(cr, angle);

    /*
     * Some really odd artifact happens on the needle image edges *only* on
     * the ARM target. So, slightly clip those horizontal edges.  This is
     * probably an expensive operation, considering the rotate.
     */
    //cairo_rectangle(cr, -needle_center.x(), -needle_center.y() + 1,
    //                image.size().width(), image.size().height() - 2);
    //cairo_clip(cr);

    cairo_set_source_surface(cr, image.surface().get(),
                             -needle_center.x(), -needle_center.y());

    cairo_paint(cr);
}

void NeedleLayer::draw(Painter& painter, const Rect&)
{
    auto angle = detail::normalize_to_angle(m_value, m_min, m_max,
                                            m_angle_start, m_angle_stop,
                                            m_clockwise);

    draw_image(painter, m_point,
               m_center, m_image, detail::to_radians<double>(0, angle));
}

void NeedleLayer::gauge(Gauge* gauge)
{
    if (!m_gauge || !gauge)
    {
        if (detail::change_if_diff<>(m_gauge, gauge))
            damage();
    }
}

Gauge::Gauge(const Rect& rect, const Widget::Flags& flags) noexcept
    : Frame(rect, flags)
{
    name("Gauge" + std::to_string(m_widgetid));
}

Gauge::Gauge(Frame& parent, const Rect& rect, const Widget::Flags& flags) noexcept
    : Gauge(rect, flags)
{
    parent.add(*this);
}

void Gauge::add(std::shared_ptr<GaugeLayer>& layer)
{
    if (!layer)
        return;

    no_layout(true);

    auto i = std::find(m_layers.begin(), m_layers.end(), layer);
    if (i == m_layers.end())
    {
        m_layers.push_back(layer);
        layer->gauge(this);
        resize(super_size());
        damage();
    }

    Frame::add(layer);
}

void Gauge::remove(GaugeLayer* layer)
{
    if (!layer)
        return;

    auto i = std::find_if(m_layers.begin(), m_layers.end(), [layer](auto & ptr)
    {
        return layer == ptr.get();
    });

    if (i != m_layers.end())
    {
        layer->gauge(nullptr);
        m_layers.erase(i);
    }
}

Gauge::~Gauge() noexcept
{
    for (auto& layer : m_layers)
        layer->gauge(nullptr);
}

}
}
}
