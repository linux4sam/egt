/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef EGT_VIEW_H
#define EGT_VIEW_H

/**
 * @file
 * @brief View definition.
 */

#include <egt/frame.h>

namespace egt
{

    class ScrolledView : public Frame
    {
    public:
        enum class Orientation
        {
            HORIZONTAL,
            VERTICAL
        };

        explicit ScrolledView(const Rect& rect = Rect());

        virtual int handle(eventid event) override;

        virtual void draw(Painter& painter, const Rect& rect) override;

        virtual ~ScrolledView();

    protected:

        void position(int offset)
        {
            if (offset <= 0 && -offset < box().w)
            {
                if (m_offset != offset)
                {
                    m_offset = offset;
                    damage();
                }
            }
        }

        int m_offset{0};
        Orientation m_orientation{Orientation::HORIZONTAL};
        bool m_moving{false};
        Point m_start_pos;
        int m_start_offset{0};
    };

}

#endif
