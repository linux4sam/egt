/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/screen/framebuffer.h"
#include "egt/utils.h"
#include <cassert>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

using namespace std;

namespace egt
{
inline namespace v1
{
namespace detail
{

FrameBuffer::FrameBuffer(const string& path)
{
    struct fb_fix_screeninfo fixinfo;
    struct fb_var_screeninfo varinfo;

    m_fd = ::open(path.c_str(), O_RDWR);
    if (m_fd < 0)
        throw std::runtime_error(("could not open device: " + path).c_str());

    if (::ioctl(m_fd, FBIOGET_FSCREENINFO, &fixinfo) < 0)
    {
        assert(0);
    }

    if (::ioctl(m_fd, FBIOGET_VSCREENINFO, &varinfo) < 0)
    {
        assert(0);
    }

    DBG("fb size " << fixinfo.smem_len << " " << varinfo.xres << "," << varinfo.yres);

    m_fb = ::mmap(NULL, fixinfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0);
    if (m_fb == MAP_FAILED)
        throw std::runtime_error(("could not map framebuffer device: " + path).c_str());

    init(&m_fb, 1, varinfo.xres, varinfo.yres);
}

FrameBuffer::~FrameBuffer()
{
    if (m_fb)
        ::munmap(m_fb, 0);
    if (m_fd >= 0)
        ::close(m_fd);
}

}
}
}
