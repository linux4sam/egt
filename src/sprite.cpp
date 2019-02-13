/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/image.h"
#include "egt/imagecache.h"
#include "egt/kmsscreen.h"
#include "egt/label.h"
#include "egt/painter.h"
#include "egt/sprite.h"
#include <sstream>
#include <vector>
#ifdef HAVE_LIBPLANES
#include <planes/plane.h>
#endif

using namespace std;

namespace egt
{
inline namespace v1
{

namespace detail
{

#ifdef HAVE_LIBPLANES
/**
 * Sprite implementation using hardware planes.
 */
class HardwareSprite : public SpriteImpl
{
public:
    HardwareSprite(Sprite& interface, const Image& image, const Size& frame_size,
                   int framecount, const Point& frame_point,
                   const Point& point = Point());

    virtual void show_frame(int index) override;

    virtual void draw(Painter& painter, const Rect& rect) override;

    virtual void paint(Painter& painter) override;

    virtual shared_cairo_surface_t surface() const override;

    virtual ~HardwareSprite();
protected:
    ImageLabel m_label;
    Sprite& m_interface;
};
#endif

/**
 * Sprite implementation using only software.
 */
class SoftwareSprite : public SpriteImpl
{
public:
    SoftwareSprite(Sprite& interface, const Image& image, const Size& frame_size,
                   int framecount, const Point& frame_point,
                   const Point& point = Point());

    virtual void show_frame(int index) override;

    virtual void draw(Painter& painter, const Rect& rect) override;

    virtual void paint(Painter& painter) override;

    virtual shared_cairo_surface_t surface() const override;

    virtual ~SoftwareSprite();
protected:
    Sprite& m_interface;
};

/**
 * This pulls out a frame into its own surface.
 */
static shared_cairo_surface_t frame_surface(const Rect& rect, const Image& image)
{
    // cairo_surface_create_for_rectangle() would work here with one
    // exception - the resulting image has no width and height

    shared_cairo_surface_t copy =
        shared_cairo_surface_t(cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                               rect.w,
                               rect.h),
                               cairo_surface_destroy);

    shared_cairo_t cr = shared_cairo_t(cairo_create(copy.get()), cairo_destroy);
    cairo_set_source_surface(cr.get(), image.surface().get(), -rect.x, -rect.y);
    cairo_rectangle(cr.get(), 0, 0, rect.w, rect.h);
    cairo_set_operator(cr.get(), CAIRO_OPERATOR_SOURCE);
    cairo_fill(cr.get());

    return copy;
}

#ifdef HAVE_LIBPLANES
HardwareSprite::HardwareSprite(Sprite& interface, const Image& image, const Size& frame_size,
                               int framecount, const Point& frame_point,
                               const Point& point)
    : SpriteImpl(image, frame_size, framecount, frame_point),
      m_label(image),
      m_interface(interface)
{
    interface.resize(m_image.size());

    interface.allocate_screen();

    KMSOverlay* s = reinterpret_cast<KMSOverlay*>(interface.screen());
    plane_set_pan_pos(s->s(), m_strips[m_strip].point.x, m_strips[m_strip].point.y);
    plane_set_pan_size(s->s(), m_frame.w, m_frame.h);

    // hack to change the size because the screen size and the box size are different
    interface.move(point);
    interface.m_box.size(frame_size);
}

void HardwareSprite::draw(Painter& painter, const Rect& rect)
{
    m_label.draw(painter, rect);
}

void HardwareSprite::show_frame(int index)
{
    if (index != m_index)
    {
        m_index = index;

        Point origin = get_frame_origin(m_index);

        if (m_interface.visible())
        {
            KMSOverlay* s = reinterpret_cast<KMSOverlay*>(m_interface.screen());
            plane_set_pan_pos(s->s(), origin.x, origin.y);
            plane_set_pan_size(s->s(), m_frame.w, m_frame.h);
            plane_apply(s->s());
        }
    }
}

shared_cairo_surface_t HardwareSprite::surface() const
{
    Point origin = get_frame_origin(m_index);
    return frame_surface(Rect(origin, Size(m_frame.w, m_frame.h)), m_image);
}

void HardwareSprite::paint(Painter& painter)
{
    painter.draw_image(m_interface.point(), Image(surface()));
}

HardwareSprite::~HardwareSprite()
{}

#endif

SoftwareSprite::SoftwareSprite(Sprite& interface, const Image& image, const Size& frame_size,
                               int framecount, const Point& frame_point,
                               const Point& point)
    : SpriteImpl(image, frame_size, framecount, frame_point),
      m_interface(interface)
{
    interface.m_box = Rect(point, frame_size);
}

void SoftwareSprite::draw(Painter& painter, const Rect& rect)
{
    ignoreparam(rect);

    Point origin = get_frame_origin(m_index);

    painter.draw_image(Rect(origin.x, origin.y, m_frame.w, m_frame.h),
                       m_interface.box().point(), m_image);
}

void SoftwareSprite::show_frame(int index)
{
    if (index != m_index)
    {
        m_index = index;
        m_interface.damage();
    }
}

shared_cairo_surface_t SoftwareSprite::surface() const
{
    Point origin = get_frame_origin(m_index);
    return frame_surface(Rect(origin, Size(m_frame.w, m_frame.h)), m_image);
}

void SoftwareSprite::paint(Painter& painter)
{
    painter.draw_image(m_interface.point(), Image(surface()));
}

SoftwareSprite::~SoftwareSprite()
{
}

}

Sprite::Sprite(const Image& image, const Size& frame_size,
               int framecount, const Point& frame_point,
               const Point& point)
    : Window(Rect(point, image.size() /*frame_size*/),
{widgetflag::NO_BACKGROUND},
pixel_format::argb8888)
{
    static auto window_id = 0;
    ostringstream ss;
    ss << "Sprite" << window_id++;
    set_name(ss.str());

    create_impl(image, frame_size, framecount, frame_point);
}

void Sprite::draw(Painter& painter, const Rect& rect)
{
    Window::draw(painter, rect);
    m_simpl->draw(painter, rect);
}

void Sprite::show_frame(int index)
{
    m_simpl->show_frame(index);
}

void Sprite::paint(Painter& painter)
{
    m_simpl->paint(painter);
}

shared_cairo_surface_t Sprite::surface() const
{
    return m_simpl->surface();
}

void Sprite::advance()
{
    m_simpl->advance();
}

bool Sprite::is_last_frame() const
{
    return m_simpl->is_last_frame();
}

uint32_t Sprite::frame_count() const
{
    return m_simpl->frame_count();
}

void Sprite::set_strip(uint32_t id)
{
    m_simpl->set_strip(id);
}

uint32_t Sprite::add_strip(int framecount, const Point& point)
{
    return m_simpl->add_strip(framecount, point);
}

void Sprite::create_impl(const Image& image, const Size& frame_size,
                         int framecount, const Point& frame_point)
{
#ifdef HAVE_LIBPLANES
    if (is_flag_set(widgetflag::PLANE_WINDOW))
        m_simpl.reset(new detail::HardwareSprite(*this, image, frame_size, framecount, frame_point));
    else
#endif
        m_simpl.reset(new detail::SoftwareSprite(*this, image, frame_size, framecount, frame_point));
}

}
}
