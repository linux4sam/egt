/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_GAUGE_H
#define EGT_GAUGE_H

/**
 * @file
 * @brief Working with gauges.
 */

#include <egt/detail/math.h>
#include <egt/frame.h>
#include <egt/image.h>
#include <egt/widget.h>
#include <memory>
#include <vector>

namespace egt
{
inline namespace v1
{
namespace experimental
{
class Gauge;

/**
 * A layer of a GaugeWidget.
 */
class GaugeLayer : public Widget
{
public:

    /**
     * Construct a gauge layer with an image.
     *
     * @param[in] image The image to display.
     */
    explicit GaugeLayer(const Image& image = {}) noexcept;

    /**
     * Construct a gauge layer with an image and a parent gauge.
     *
     * @param gauge Parent gauge.
     * @param[in] image The image to display.
     */
    explicit GaugeLayer(Gauge& gauge, const Image& image = {}) noexcept;

    virtual void draw(Painter& painter, const Rect& rect) override;

    /**
     * Set the image of the gauge layer.
     *
     * @param[in] image The image to display.
     */
    inline void set_image(const Image& image)
    {
        m_image = image;
        resize(m_image.size());
        damage();
    }

    /**
     * Get the gauge of the layer.
     */
    inline Gauge* gauge() const { return m_gauge; }

    virtual ~GaugeLayer() = default;

protected:

    /**
     * Normally this does not need to be called directly.
     *
     * When a GaugeLayer is added to a Gauge, this will automatically be called.
     */
    virtual void set_gauge(Gauge* gauge)
    {
        if (!m_gauge || !gauge)
        {
            if (detail::change_if_diff<>(m_gauge, gauge))
                damage();
        }
    }

    /**
     * The Layer image.
     */
    Image m_image;

    /**
     * Parent gauge.
     */
    Gauge* m_gauge{nullptr};

    friend class Gauge;
};

/**
 * Special GaugeLayer that deals with a rotating needle.
 *
 * This works like an egt:: ValueRangeWidget. The needle is created with any
 * range you chose, and the value of the needle must fall in this range.  The
 * range corresponds to the configured display start and stop angle.
 */
class NeedleLayer : public GaugeLayer
{
public:

    using GaugeLayer::GaugeLayer;

    /**
     * @param[in] image The image for the layer.
     * @param[in] min The min value.
     * @param[in] max The max value.
     * @param[in] angle_start The starting angle of the needle.
     * @param[in] angle_stop The stop angle of the needle.
     * @param[in] clockwise Rotation is clockwise.
     */
    explicit NeedleLayer(const Image& image,
                         float min = 0.,
                         float max = 100,
                         float angle_start = 0,
                         float angle_stop = 360,
                         bool clockwise = true) noexcept;

    virtual void draw(Painter& painter, const Rect& rect) override;

    /**
     * Get the angle start value.
     */
    inline float angle_start() const { return m_angle_start; }

    /**
     * Set the start angle of the needle.
     *
     * @param[in] angle_start The starting angle of the needle.
     */
    inline float set_angle_start(float angle_start)
    {
        float orig = m_angle_start;
        if (detail::change_if_diff<float>(m_angle_start, angle_start))
            damage();
        return orig;
    }

    /**
     * Get the angle stop value.
     */
    inline float angle_stop() const { return m_angle_stop; }

    /**
     * Set the stop angle of the needle.
     *
     * @param[in] angle_stop The stop angle of the needle.
     */
    inline float set_angle_stop(float angle_stop)
    {
        float orig = m_angle_stop;
        if (detail::change_if_diff<float>(m_angle_stop, angle_stop))
            damage();
        return orig;
    }

    /**
     * Get the min value.
     */
    inline float min() const { return m_min; }

    /**
     * Set the min value.
     *
     * @param[in] min The min value.
     */
    inline float set_min(float min)
    {
        float orig = m_min;
        if (detail::change_if_diff<float>(m_min, min))
        {
            set_value(m_value);
            damage();
        }

        return orig;
    }

    /**
     * Get the max value.
     */
    inline float max() const { return m_max; }

    /**
     * Set the max value.
     *
     * @param[in] max The max value.
     */
    inline float set_max(float max)
    {
        float orig = m_max;
        if (detail::change_if_diff<float>(m_max, max))
        {
            set_value(m_value);
            damage();
        }

        return orig;
    }

    /**
     * Get the value of the needle.
     */
    inline float value() const { return m_value; }

    /**
     * Set the value.
     *
     * If this results in changing the value, it will damage() the widget.
     *
     * @return The old value.
     */
    virtual float set_value(float value)
    {
        auto orig = m_value;

        value = detail::clamp<float>(value, m_min, m_max);

        if (detail::change_if_diff<float>(m_value, value))
        {
            invoke_handlers(eventid::property_changed);
            damage();
        }

        return orig;
    }

    /**
     * Get the needle point.
     */
    inline PointF needle_point() const { return m_point; }

    /**
     * Set the needle point.
     *
     * This is the rotate point of the needle on the gauge surface.
     */
    inline void set_needle_point(const PointF& point)
    {
        if (detail::change_if_diff<PointF>(m_point, point))
        {
            auto dim = std::max(m_image.width(), m_image.height());
            auto circle = Circle(Point(m_point.x(), m_point.y()), dim * 2.);
            auto superrect = circle.rect();

            // real widget size is the big rect, center is a common point
            resize(superrect.size());
            move_to_center(superrect.center());
        }
    }

    /**
     * Get the needle center.
     */
    inline PointF needle_center() const { return m_center; }

    /**
     * Set the needle center.
     *
     * This is the rotate point of the needle on the needle itself.  For example,
     * if a symmetrical needle rotates on it's center, this would be a point on
     * the center of the needle layer itself.
     */
    inline void set_needle_center(const PointF& center)
    {
        if (detail::change_if_diff<PointF>(m_center, center))
            damage();
    }

    /**
     * Get the clockwise value.
     */
    inline bool clockwise() const { return m_clockwise; }

    /**
     * Set the clockwise value.
     *
     * @param[in] value The clockwise value.
     */
    inline void set_clockwise(bool value)
    {
        if (detail::change_if_diff<>(m_clockwise, value))
            damage();
    }

    virtual ~NeedleLayer() = default;

protected:

    virtual void set_gauge(Gauge* gauge) override;

    /**
     * Minimum value of the needle.
     */
    float m_min{0.0};

    /**
     * Maximum value of the needle.
     */
    float m_max{0.0};

    /**
     * The value of the needle.
     */
    float m_value{0.0};

    /**
     * Starting angle of the needle.
     */
    float m_angle_start{0.0};

    /*
     * Stopping/Ending angle of the needle.
     */
    float m_angle_stop{360.0};

    /**
     * Does the needle rotate clockwise or counterclockwise.
     */
    bool m_clockwise{true};

    /**
     * Center point of the needle.
     */
    PointF m_center;

    /**
     * Rotate point of the needle on the gauge.
     */
    PointF m_point;
};

/**
 * A Gauge Widget that is composed of GaugeLayer layers.
 *
 * In its simplest form, gauge layers are created using an Image. The images can
 * come from SVG files or other image file types, or even individual elements of
 * an SVG file using the SvgImage class.
 *
 * @image html gauge.png
 * @image latex gauge.png "gauge layers" width=5cm
 *
 * Creating a simple gauge with a base layer and a needle layer, from a single
 * SVG file is possible.
 *
 * @code{.cpp}
 * Gauge custom1;
 *
 * SvgImage custom1svg("gauge.svg", SizeF(200, 0));
 *
 * auto custom1_background = make_shared<GaugeLayer>(custom1svg.id("#base"));
 * custom1.add_layer(custom1_background);
 *
 * auto custom1_needle = make_shared<NeedleLayer>(custom1svg.id("#needle"),
 *                                                0, 100, 0, 180);
 * auto custom1_needle_point = custom1svg.id_box("#needlepoint").center();
 * custom1_needle->set_needle_point(custom1_needle_point);
 * custom1_needle->set_needle_center(custom1_needle_point);
 * custom1.add_layer(custom1_needle);
 * @endcode
 *
 * A GaugeLayer is actually a special Widget.  So, you can control its
 * properties just like any other widget and handle events from it as
 * well.
 *
 * Some [excellent tutorials](https://www.youtube.com/watch?v=VWFn1LOIScQ) can
 * be found online for creating your own gauges using
 * [Inkscape](https://inkscape.org/).  Of course, any tool that can create
 * standard SVG files works just as well.
 */
class Gauge : public Frame
{
public:

    using Frame::Frame;

    /**
     * Add a GaugeLayer to the Gauge.
     *
     * The Gauge will automatically be resized to the maximum size of any
     * GaugeLayer.
     *
     * @todo This does not account for adding the same layer multiple times.
     */
    virtual void add_layer(const std::shared_ptr<GaugeLayer>& layer);

    inline void add_layer(GaugeLayer& layer)
    {
        // Nasty, but it gets the job done.  If a widget is passed in as a
        // reference, we don't own it, so create a "pointless" shared_ptr that
        // will not delete it.
        add_layer(std::shared_ptr<GaugeLayer>(&layer, [](GaugeLayer*) {}));
    }

    virtual void remove_layer(const std::shared_ptr<GaugeLayer>& layer);

    virtual ~Gauge();

protected:

    /**
     * Get the size of the largest layer.
     */
    inline Size super_size() const
    {
        Rect result = size();
        for (const auto& layer : m_layers)
            result = Rect::merge(result, layer->box());

        return result.size();
    }

    using layer_array = std::vector<std::shared_ptr<GaugeLayer>>;

    /**
     * The layer's of the gauge.
     */
    layer_array m_layers;
};

}
}
}

#endif
