/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "screen.h"
#include <sys/mman.h>
#include <cassert>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <unistd.h>
#include <iostream>

using namespace std;

namespace mui
{

    static IScreen* the_screen = 0;

    IScreen*& main_screen()
    {
        return the_screen;
    }

    IScreen::IScreen()
    {
    }

    void IScreen::blit(cairo_surface_t* surface, int srcx, int srcy, int srcw,
                       int srch, int dstx, int dsty, bool blend)
    {
        cairo_save(m_cr.get());
        cairo_set_source_surface(m_cr.get(), surface, dstx - srcx, dsty - srcy);
        cairo_rectangle(m_cr.get(), dstx, dsty, srcw, srch);
        if (blend)
            cairo_set_operator(m_cr.get(), CAIRO_OPERATOR_OVER);
        else
            cairo_set_operator(m_cr.get(), CAIRO_OPERATOR_SOURCE);
        cairo_fill(m_cr.get());
        cairo_restore(m_cr.get());

        assert(cairo_status(m_cr.get()) == CAIRO_STATUS_SUCCESS);
    }

    void IScreen::fill(const Color& color)
    {
        cairo_save(m_cr.get());
        cairo_set_source_rgba(m_cr.get(),
                              color.redf(),
                              color.greenf(),
                              color.bluef(),
                              color.alphaf());
        cairo_set_operator(m_cr.get(), CAIRO_OPERATOR_SOURCE);
        cairo_paint(m_cr.get());
        cairo_restore(m_cr.get());
    }

    void IScreen::rect(const Rect& rect, const Color& color)
    {
        cairo_save(m_cr.get());
        cairo_set_source_rgba(m_cr.get(),
                              color.redf(),
                              color.greenf(),
                              color.bluef(),
                              color.alphaf());
        cairo_set_operator(m_cr.get(), CAIRO_OPERATOR_SOURCE);
        cairo_rectangle(m_cr.get(), rect.x, rect.y, rect.w, rect.h);
        cairo_fill(m_cr.get());
        cairo_restore(m_cr.get());
    }

    void IScreen::flip(const damage_array& damage)
    {
        static int envset = -1;
        if (envset < 0)
            envset = !!getenv("MUI_GREENSCREEN");

        if (!damage.empty())
        {
            damage_array olddamage = m_buffers[index()].damage;

            // save the damage to all buffers
            for (auto& b : m_buffers)
                for (const auto& d : damage)
                    b.add_damage(d);

            DisplayBuffer& buffer = m_buffers[index()];

            if (envset)
                copy_to_buffer_greenscreen(buffer, olddamage);
            else
                copy_to_buffer(buffer);

            // delete all damage from current buffer
            buffer.damage.clear();
            schedule_flip();
        }
    }

    void IScreen::copy_to_buffer_greenscreen(DisplayBuffer& buffer,
            const damage_array& olddamage)
    {
        cairo_set_source_surface(buffer.cr.get(), m_surface.get(), 0, 0);
        cairo_set_operator(buffer.cr.get(), CAIRO_OPERATOR_SOURCE);

        for (const auto& d : buffer.damage)
            cairo_rectangle(buffer.cr.get(), d.x, d.y, d.w, d.h);

        cairo_fill(buffer.cr.get());

        Color color = Color::GREEN;
        cairo_set_source_rgb(buffer.cr.get(), color.redf(),
                             color.greenf(), color.bluef());
        cairo_set_line_width(buffer.cr.get(), 1.0);

        for (const auto& d : buffer.damage)
            if (find(olddamage.begin(), olddamage.end(), d) != olddamage.end())
                cairo_rectangle(buffer.cr.get(), d.x + 2, d.y + 2, d.w - 4, d.h - 4);

        cairo_stroke(buffer.cr.get());

        cairo_surface_flush(buffer.surface.get());
    }

    void IScreen::copy_to_buffer(DisplayBuffer& buffer)
    {
        cairo_set_source_surface(buffer.cr.get(), m_surface.get(), 0, 0);
        cairo_set_operator(buffer.cr.get(), CAIRO_OPERATOR_SOURCE);

        for (const auto& d : buffer.damage)
            cairo_rectangle(buffer.cr.get(), d.x, d.y, d.w, d.h);

        cairo_fill(buffer.cr.get());
        cairo_surface_flush(buffer.surface.get());
    }

    IScreen::~IScreen()
    {
    }

    void IScreen::init(void** ptr, uint32_t count, int w, int h, uint32_t f)
    {
        cout << "screen " << w << "," << h << endl;

        m_size = Size(w, h);

        cairo_format_t format = CAIRO_FORMAT_ARGB32;

        switch (f)
        {
        case DRM_FORMAT_RGB565:
            cout << "screen using RGB565 format" << endl;
            format = CAIRO_FORMAT_RGB16_565;
            break;
        case DRM_FORMAT_ARGB8888:
            cout << "screen using ARGB32 format" << endl;
            format = CAIRO_FORMAT_ARGB32;
            break;
        case DRM_FORMAT_XRGB8888:
            cout << "screen using RGB24 format" << endl;
            format = CAIRO_FORMAT_RGB24;
            break;
        default:
            //assert(0);
            break;
        }

        for (uint32_t x = 0; x < count; x++)
        {
            DisplayBuffer buffer;
            buffer.surface = shared_cairo_surface_t(
                                 cairo_image_surface_create_for_data((unsigned char*)ptr[x],
                                         format,
                                         w, h,
                                         cairo_format_stride_for_width(format, w)),
                                 cairo_surface_destroy);
            assert(buffer.surface);

            buffer.cr = shared_cairo_t(cairo_create(buffer.surface.get()), cairo_destroy);
            assert(buffer.cr);

            buffer.damage.push_back(Rect(0, 0, w, h));

            m_buffers.push_back(buffer);
        }

        m_surface = shared_cairo_surface_t(cairo_image_surface_create(format, w, h),
                                           cairo_surface_destroy);

        assert(m_surface.get());

        m_cr = shared_cairo_t(cairo_create(m_surface.get()), cairo_destroy);
        assert(m_cr);

        if (!the_screen)
            the_screen = this;
    }

    // https://github.com/toradex/cairo-fb-examples/blob/master/rectangles/rectangles.c

    FrameBuffer::FrameBuffer(const string& path)
        : m_fd(-1)
    {
        struct fb_fix_screeninfo fixinfo;
        struct fb_var_screeninfo varinfo;

        cout << path << endl;
        m_fd = open(path.c_str(), O_RDWR);
        assert(m_fd >= 0);

        if (ioctl(m_fd, FBIOGET_FSCREENINFO, &fixinfo) < 0)
            assert(0);

        if (ioctl(m_fd, FBIOGET_VSCREENINFO, &varinfo) < 0)
            assert(0);

        cout << "fb size " << fixinfo.smem_len << " " << varinfo.xres << "," << varinfo.yres << endl;

        m_fb = mmap(NULL, fixinfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0);
        assert(m_fb != (void*) -1);

        init(&m_fb, 1, varinfo.xres, varinfo.yres);
    }

    FrameBuffer::~FrameBuffer()
    {
        ::munmap(m_fb, 0);
        ::close(m_fd);
    }

}
