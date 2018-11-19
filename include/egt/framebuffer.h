/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_FRAMEBUFFER_H
#define EGT_FRAMEBUFFER_H

/**
 * @file
 * @brief Working with framebuffers.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <egt/screen.h>

namespace egt
{
    /**
     * Screen on a fbdev framebuffer.
     *
     * The framebuffer is mmap()'ed and directly accessible.
     */
    class FrameBuffer : public IScreen
    {
    public:

        explicit FrameBuffer(const std::string& path = "/dev/fb0");
        virtual ~FrameBuffer();

    protected:
        int m_fd;
        void* m_fb;
    };

}

#endif
