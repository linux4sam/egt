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

#include <egt/screen.h>
#include <egt/input.h>
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

    explicit KMSOverlay(struct plane_data* plane);

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

    struct plane_data* s() const
    {
        return m_plane;
    }

    void schedule_flip() override;

    uint32_t index() override;

    virtual ~KMSOverlay();

protected:
    struct plane_data* m_plane {nullptr};
    uint32_t m_index{};
    std::unique_ptr<FlipThread> m_pool;
};

}
}
}

#endif
