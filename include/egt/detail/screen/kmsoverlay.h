/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_SCREEN_KMSOVERLAY_H
#define EGT_DETAIL_SCREEN_KMSOVERLAY_H

/**
 * @file
 * @brief Working with KMS screens.
 */

#include <egt/detail/screen/kmstype.h>
#include <egt/input.h>
#include <egt/screen.h>
#include <egt/widgetflags.h>
#include <memory>

struct plane_data;

namespace egt
{
inline namespace v1
{
namespace detail
{
class FlipThread;

/**
 * A screen backed by an overlay plane.
 */
class KMSOverlay : public Screen
{
public:

    KMSOverlay() = delete;

    KMSOverlay(const Size& size, pixel_format format, windowhint hint);

    virtual void resize(const Size& size);
    virtual void set_position(const DisplayPoint& point);
    virtual void set_scale(float scalex, float scaley);
    virtual void set_pan_size(const Size& size);
    virtual void set_pan_pos(const Point& point);
    virtual float scale_x() const;
    virtual float scale_y() const;

    virtual uint32_t get_plane_format();

    virtual void hide();

    virtual void show();

    virtual int gem();

    virtual void apply();

    void* raw();

    plane_data* s() const
    {
        return m_plane.get();
    }

    void schedule_flip() override;

    uint32_t index() override;

protected:
    unique_plane_t m_plane;
    uint32_t m_index{0};
    std::unique_ptr<FlipThread> m_pool;
};

}
}
}

#endif
