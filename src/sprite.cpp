/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "detail/spriteimpl.h"
#include "egt/image.h"
#include "egt/label.h"
#include "egt/painter.h"
#include "egt/sprite.h"

#ifdef HAVE_LIBPLANES
#include "egt/detail/screen/kmsoverlay.h"
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
    HardwareSprite(Sprite& iface, const Image& image, const Size& frame_size,
                   int frame_count, const Point& frame_point);

    virtual void show_frame(int index) override;

    virtual void draw(Painter& painter, const Rect& rect) override;

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
    SoftwareSprite(Sprite& iface, const Image& image, const Size& frame_size,
                   int frame_count, const Point& frame_point);

    void show_frame(int index) override;

    void draw(Painter& painter, const Rect& rect) override;

protected:
    Sprite& m_interface;
};

#ifdef HAVE_LIBPLANES
HardwareSprite::HardwareSprite(Sprite& iface, const Image& image, const Size& frame_size,
                               int frame_count, const Point& frame_point)
    : SpriteImpl(image, frame_size, frame_count, frame_point),
      m_label(image),
      m_interface(iface)
{
    m_label.image_align().clear();
    iface.resize(m_image.size());

    iface.allocate_screen();

    KMSOverlay* s = reinterpret_cast<KMSOverlay*>(iface.screen());
    plane_set_pan_pos(s->s(), m_strips[m_strip].point.x(), m_strips[m_strip].point.y());
    plane_set_pan_size(s->s(), m_frame.width(), m_frame.height());

    // hack to change the size because the screen size and the box size are different
    iface.m_box.size(frame_size);
}

void HardwareSprite::draw(Painter& painter, const Rect& rect)
{
    /**
     * 'm_label' can be seen as a component of the 'm_interface' Sprite widget.
     * Therefore, insert a translation of 'm_interface.point()', as
     * Widget::draw() would have done before drawing the subordinate/component.
     */
    Painter::AutoSaveRestore sr(painter);

    const auto& origin = m_interface.point();
    painter.translate(origin);
    m_label.draw(painter, rect - origin);
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

#endif

SoftwareSprite::SoftwareSprite(Sprite& iface, const Image& image, const Size& frame_size,
                               int frame_count, const Point& frame_point)
    : SpriteImpl(image, frame_size, frame_count, frame_point),
      m_interface(iface)
{
    iface.m_box = Rect({}, frame_size);
}

void SoftwareSprite::draw(Painter& painter, const Rect& rect)
{
    ignoreparam(rect);

    Point origin = get_frame_origin(m_index);
    const auto& point = m_interface.box().point();
    painter.draw(m_image, point - origin, Rect(point, m_frame));
}

void SoftwareSprite::show_frame(int index)
{
    if (index != m_index)
    {
        m_index = index;
        m_interface.damage();
    }
}

}

Sprite::Sprite(WindowHint hint)
    : Window(PixelFormat::argb8888, hint)
{
    name("Sprite" + std::to_string(m_widgetid));
    fill_flags().clear();
}

Sprite::Sprite(const Image& image, const Size& frame_size,
               int frame_count, const Point& frame_point,
               WindowHint hint)
    : Window(Rect({}, image.size()), PixelFormat::argb8888, hint)
{
    name("Sprite" + std::to_string(m_widgetid));
    fill_flags().clear();
    create_impl(image, frame_size, frame_count, frame_point);
}

Sprite::Sprite(Serializer::Properties& props, bool is_derived) noexcept
    : Window(props, true)
{
    if (!is_derived)
        deserialize_leaf(props);
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
    if (image.empty())
        throw std::runtime_error("sprite image cannot be empty");

#ifdef HAVE_LIBPLANES
    if (plane_window())
    {
        m_simpl = std::make_unique<detail::HardwareSprite>(*this, image, frame_size, frame_count, frame_point);
    }
    else
#endif
    {
        m_simpl = std::make_unique<detail::SoftwareSprite>(*this, image, frame_size, frame_count, frame_point);
    }
}

Sprite::Sprite(Sprite&&) noexcept = default;
Sprite& Sprite::operator=(Sprite&&) noexcept = default;

Sprite::~Sprite() noexcept = default;

}
}
