/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/gauge.h"
#include <egt/detail/math.h>
#include <egt/detail/floatingpoint.h>

using namespace std;

namespace egt
{
inline namespace v1
{
namespace experimental
{

static void draw_image(Painter& painter,
                       const PointF& center,
                       const PointF& needle_center,
                       const Image& image,
                       float angle)
{
    Painter::AutoSaveRestore sr(painter);
    auto cr = painter.context().get();

    cairo_translate(cr, center.x, center.y);
    cairo_rotate(cr, angle);

    /*
     * Some really odd artifacting happens on the needle image edges *only* on
     * the ARM target. So, slightly clip those horizontal edges.  This is
     * probably an expensive operation, considering the rotate.
     */
    cairo_rectangle(cr, -needle_center.x, -needle_center.y + 1,
                    image.size().width, image.size().height - 2);
    cairo_clip(cr);

    cairo_set_source_surface(cr, image.surface().get(), -needle_center.x, -needle_center.y);
    cairo_paint(cr);
}

void GaugeLayer::set_visible(bool visible)
{
    if (detail::change_if_diff<>(m_visible, visible))
        m_gauge->damage();
}

void NeedleLayer::set_value(float value)
{
    if (!detail::FloatingPoint<float>(value).
        AlmostEquals(detail::FloatingPoint<float>(m_value)))
    {
        m_value = value;
        m_gauge->damage();
    }
}

void NeedleLayer::draw(Painter& painter)
{
    auto angle = detail::normalize_to_angle(m_value, m_min, m_max, m_angle_start, m_angle_stop, m_clockwise);

    draw_image(painter, PointF(m_gauge->point().x, m_gauge->point().y) + m_point,
               m_center, m_image, detail::to_radians<double>(0, angle));
}

void GaugeLayer::draw(Painter& painter)
{
    painter.draw(m_gauge->point());
    painter.draw(m_image);
}

}
}
}
