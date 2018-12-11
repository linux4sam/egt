/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_KMSOVERLAY_H
#define EGT_KMSOVERLAY_H

/**
 * @file
 * @brief Working with KMS screens.
 */

#include <egt/screen.h>

struct plane_data;

namespace egt
{

    /**
     * A screen backed by an overlay plane.
     */
    class KMSOverlay : public IScreen
    {
    public:

        constexpr static const uint32_t NUM_OVERLAY_BUFFERS = 3;

        explicit KMSOverlay(struct plane_data* plane);

        virtual void position(const Point& point);
        virtual void scale(float scale);
        virtual void set_pan_size(const Size& size);
        virtual void set_pan_pos(const Point& point);
        virtual float scale() const;

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
        struct plane_data* m_plane;
        uint32_t m_index;
    };

}

#endif
