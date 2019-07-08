/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_GAUGE_H
#define EGT_GAUGE_H

#include <egt/detail/math.h>
#include <egt/flags.h>
#include <egt/image.h>
#include <egt/valuewidget.h>
#include <vector>
#include <memory>

namespace egt
{
inline namespace v1
{
namespace experimental
{
class Gauge;

/**
 * A layer of a gauge display.
 */
class GaugeLayer
{
public:
    explicit GaugeLayer(const Image& image)
        : m_image(image)
    {}

    inline Size size() const { return m_image.size(); }

    virtual void draw(Painter& painter);

    inline bool visible() const { return m_visible; }

    inline void visible_toggle() { set_visible(!m_visible); }

    void set_visible(bool visible);

    inline void set_gauge(Gauge* gauge)
    {
        m_gauge = gauge;
    }

    virtual ~GaugeLayer() = default;

protected:
    Image m_image;
    Gauge* m_gauge{nullptr};
    bool m_visible{true};
};

/**
 * Special GaugeLayer that deals with rotating and displaying a needle.
 */
class NeedleLayer : public GaugeLayer
{
public:

    NeedleLayer(const Image& image, float min, float max, float angle_start,
                float angle_stop, bool clockwise = true)
        :  GaugeLayer(image),
           m_min(min),
           m_max(max),
           m_angle_start(angle_start),
           m_angle_stop(angle_stop),
           m_clockwise(clockwise)
    {
    }

    virtual void draw(Painter& painter) override;

    virtual void set_value(float value);

    inline void set_needle_point(const PointF& point)
    {
        m_point = point;
    }

    inline void set_needle_center(const PointF& center)
    {
        m_center = center;
    }

    virtual ~NeedleLayer() = default;

protected:

    float m_value{0.0};
    float m_min{0.0};
    float m_max{0.0};
    float m_angle_start{0.0};
    float m_angle_stop{360.0};
    bool m_clockwise{true};

    PointF m_center;
    PointF m_point;
};

/**
 * A Gauge Widget that is composed of GaugeLayer layers.
 */
class Gauge : public Widget
{
public:

    using Widget::Widget;

    void add_layer(const std::shared_ptr<GaugeLayer>& layer)
    {
        m_layers.push_back(layer);
        resize(super_size());
    }

    virtual void draw(Painter& painter, const Rect&) override
    {
        for (auto& layer : m_layers)
        {
            if (layer->visible())
                layer->draw(painter);
        }
    }

protected:

    inline Size super_size() const
    {
        Size result = size();
        for (auto& layer : m_layers)
        {
            if (layer->size().width > result.width ||
                layer->size().height > result.height)
                result = layer->size();
        }

        return result;
    }

    std::vector<std::shared_ptr<GaugeLayer>> m_layers;
};

}
}
}

#endif
