/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SVGDESERIAL_H
#define EGT_SVGDESERIAL_H


#include "app.h"
#include "window.h"
#include "gauge.h"
#include "animation.h"
#include "text.h"
#include "shapes.h"

namespace egt
{
inline namespace v1
{

namespace experimental
{

class EGT_API SVGDeserial : public Gauge
{
public:

    /**
     * @param[in] parent The window to be added.
     */
    SVGDeserial(TopWindow& parent) noexcept;

    /**
     * @param[in] buf The buffer address of graphics data in eraw.bin.
     * @param[in] len The length of graphics data in eraw.bin.
     * @param[out] rect The box bundle of this graphics widget.
     */
    shared_cairo_surface_t DeSerialize(const unsigned char* buf, size_t len, std::shared_ptr<egt::Rect>& rect);

    /**
     * @param[in] buf The buffer address of graphics data in eraw.bin.
     * @param[in] len The length of graphics data in eraw.bin.
     * @param[in] show Show or hide the widget.
     */
    std::shared_ptr<GaugeLayer> AddWidgetByBuf(const unsigned char* buf, size_t len, bool show);

    /**
     * @param[in] buf The buffer address of graphics data in eraw.bin.
     * @param[in] len The length of graphics data in eraw.bin.
     * @param[in] min Start value of rotation.
     * @param[in] max End value of rotation.
     * @param[in] min_angle Start angle of rotation.
     * @param[in] max_angle End angle of rotation.
     * @param[in] clockwise Is the rotation clockwise or counterclockwise.
     * @param[in] center The center point of rotation.
     */
    std::shared_ptr<NeedleLayer> AddRotateWidgetByBuf(const unsigned char* buf, size_t len, int min, int max,
            int min_angle, int max_angle, bool clockwise, Point center);

    /**
     * @param[in] widget The widget need to be rotated.
     * @param[in] duration The rotation duration.
     * @param[in] easein In easing function.
     * @param[in] easeout Out easing function.
     */
    std::shared_ptr<AnimationSequence> RotateAnimation(std::shared_ptr<NeedleLayer> widget,
            std::chrono::milliseconds duration, const EasingFunc& easein, const EasingFunc& easeout);

    std::shared_ptr<egt::TextBox> find_text(const std::string& name) { return find_child<egt::TextBox>(name); }

    std::shared_ptr<egt::RectangleWidget> find_rec(const std::string& name) { return find_child<egt::RectangleWidget>(name); }

    std::shared_ptr<egt::experimental::GaugeLayer> find_layer(const std::string& name) { return find_child<egt::experimental::GaugeLayer>(name); }

    void add_text_widget(const std::string& id, const std::string& txt, const egt::Rect& rect, egt::Font::Size size);
    void add_text_widget(const std::string& id, const std::string& txt, const egt::Rect& rect, egt::Font::Size size, egt::Color color);

    bool is_point_in_line(egt::DefaultDim point, egt::DefaultDim start, egt::DefaultDim end);
    bool is_point_in_rect(egt::DisplayPoint& point, egt::Rect rect);
    ~SVGDeserial() noexcept {}

private:

};

}
}
}

#endif
