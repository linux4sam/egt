/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "detail/spriteimpl.h"
#include "egt/detail/imagecache.h"
#include "egt/detail/screen/kmsoverlay.h"
#include "egt/image.h"
#include "egt/label.h"
#include "egt/painter.h"
#include "egt/sprite.h"
#ifdef HAVE_LIBPLANES
#include <planes/plane.h>
#endif

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
                   int frame_count, const Point& frame_point);

    virtual void show_frame(int index) override;

    virtual void draw(Painter& painter, const Rect& rect) override;

    virtual void paint(Painter& painter) override;

    virtual shared_cairo_surface_t surface() const override;

    virtual ~HardwareSprite() = default;
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
                   int frame_count, const Point& frame_point);

    virtual void show_frame(int index) override;

    virtual void draw(Painter& painter, const Rect& rect) override;

    virtual void paint(Painter& painter) override;

    virtual shared_cairo_surface_t surface() const override;

    virtual ~SoftwareSprite() = default;
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
                               rect.width(),
                               rect.height()),
                               cairo_surface_destroy);

    shared_cairo_t cr = shared_cairo_t(cairo_create(copy.get()), cairo_destroy);
    cairo_set_source_surface(cr.get(), image.surface().get(), -rect.x(), -rect.y());
    cairo_rectangle(cr.get(), 0, 0, rect.width(), rect.height());
    cairo_set_operator(cr.get(), CAIRO_OPERATOR_SOURCE);
    cairo_fill(cr.get());

    return copy;
}

#ifdef HAVE_LIBPLANES
HardwareSprite::HardwareSprite(Sprite& interface, const Image& image, const Size& frame_size,
                               int frame_count, const Point& frame_point)
    : SpriteImpl(image, frame_size, frame_count, frame_point),
      m_label(image),
      m_interface(interface)
{
    m_label.image_align().clear();
    interface.resize(m_image.size());

    interface.allocate_screen();

    KMSOverlay* s = reinterpret_cast<KMSOverlay*>(interface.screen());
    plane_set_pan_pos(s->s(), m_strips[m_strip].point.x(), m_strips[m_strip].point.y());
    plane_set_pan_size(s->s(), m_frame.width(), m_frame.height());

    // hack to change the size because the screen size and the box size are different
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
            plane_set_pan_pos(s->s(), origin.x(), origin.y());
            plane_set_pan_size(s->s(), m_frame.width(), m_frame.height());
            plane_apply(s->s());
        }
    }
}

shared_cairo_surface_t HardwareSprite::surface() const
{
    Point origin = get_frame_origin(m_index);
    return frame_surface(Rect(origin, Size(m_frame.width(), m_frame.height())), m_image);
}

void HardwareSprite::paint(Painter& painter)
{
    painter.draw(m_interface.point());
    painter.draw(Image(surface()));
}

#endif

SoftwareSprite::SoftwareSprite(Sprite& interface, const Image& image, const Size& frame_size,
                               int frame_count, const Point& frame_point)
    : SpriteImpl(image, frame_size, frame_count, frame_point),
      m_interface(interface)
{
    interface.m_box = Rect({}, frame_size);
}

void SoftwareSprite::draw(Painter& painter, const Rect& rect)
{
    ignoreparam(rect);

    Point origin = get_frame_origin(m_index);
    painter.draw(m_interface.box().point());
    painter.draw(Rect(origin.x(), origin.y(), m_frame.width(), m_frame.height()), m_image);
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
    return frame_surface(Rect(origin, Size(m_frame.width(), m_frame.height())), m_image);
}

void SoftwareSprite::paint(Painter& painter)
{
    auto cr = painter.context();
    cairo_move_to(cr.get(), m_interface.point().x(), m_interface.point().y());
    painter.draw(Image(surface()));
}

}

Sprite::Sprite()
    : Window(PixelFormat::argb8888)
{}

Sprite::Sprite(const Image& image, const Size& frame_size,
               int frame_count, const Point& frame_point)
    : Window(Rect({}, image.size()), PixelFormat::argb8888)
{
    name("Sprite" + std::to_string(m_widgetid));
    boxtype().clear();
    create_impl(image, frame_size, frame_count, frame_point);
}

void Sprite::init(const Image& image, const Size& frame_size,
                  int frame_count, const Point& frame_point)
{
    resize(image.size());
    create_impl(image, frame_size, frame_count, frame_point);
}

void Sprite::draw(Painter& painter, const Rect& rect)
{
    if (!m_simpl)
        return;

    Window::draw(painter, rect);
    m_simpl->draw(painter, rect);
}

void Sprite::show_frame(int index)
{
    if (!m_simpl)
        throw std::runtime_error("no sprite implementation initialized");
    m_simpl->show_frame(index);
}

void Sprite::paint(Painter& painter)
{
    if (!m_simpl)
        return;
    m_simpl->paint(painter);
}

shared_cairo_surface_t Sprite::surface() const
{
    if (!m_simpl)
        throw std::runtime_error("no sprite implementation initialized");
    return m_simpl->surface();
}

void Sprite::advance()
{
    if (!m_simpl)
        throw std::runtime_error("no sprite implementation initialized");
    m_simpl->advance();
}

bool Sprite::is_last_frame() const
{
    if (!m_simpl)
        throw std::runtime_error("no sprite implementation initialized");
    return m_simpl->is_last_frame();
}

uint32_t Sprite::frame_count() const
{
    if (!m_simpl)
        throw std::runtime_error("no sprite implementation initialized");
    return m_simpl->frame_count();
}

void Sprite::change_strip(uint32_t id)
{
    if (!m_simpl)
        throw std::runtime_error("no sprite implementation initialized");
    m_simpl->change_strip(id);
}

uint32_t Sprite::add_strip(int frame_count, const Point& frame_point)
{
    if (!m_simpl)
        throw std::runtime_error("no sprite implementation initialized");
    return m_simpl->add_strip(frame_count, frame_point);
}

void Sprite::create_impl(const Image& image, const Size& frame_size,
                         int frame_count, const Point& frame_point)
{
#ifdef HAVE_LIBPLANES
    if (flags().is_set(Widget::Flag::plane_window))
        m_simpl = detail::make_unique<detail::HardwareSprite>(*this, image, frame_size, frame_count, frame_point);
    else
#endif
        m_simpl = detail::make_unique<detail::SoftwareSprite>(*this, image, frame_size, frame_count, frame_point);
}

Sprite::~Sprite() = default;

}
}
