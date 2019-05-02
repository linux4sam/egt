/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*! This file contains two functions for reading BMP files to Cairo image surfaces.
 *
 * All prototypes are defined in cairo_bmp.h All functions and their parameters
 * and return values are described below directly at the functions. You may
 * also have a look at the preprocessor macros defined below.
 *
 * To compile this code you need to have installed the packages libcairo2-dev
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cairo.h>

#include "cairo_bmp.h"
#define BMP_HEADER_OFFSET 138
#define BMP_WIDTH_OFFSET 18
#define BMP_HEIGHT_OFFSET 22
#define BMP_BITS_OFFSET 28

/*! This function reads an BMP image from a stream and creates a Cairo image
 * surface.
 * @param read_func Pointer to function which reads data.
 * @param closure Pointer which is passed to read_func().
 * @return Returns a pointer to a cairo_surface_t structure. It should be
 * checked with cairo_surface_status() for errors.
 * @note If the surface returned is invalid you can use errno(3) to determine
 * further reasons. Errno is set according to realloc(3). If you
 * intend to check errno you shall set it to 0 before calling this function
 * because it modifies errno only in case of an error.
 */
cairo_surface_t* cairo_image_surface_create_from_bmp_stream(cairo_read_func_t read_func, void* closure)
{
    int i, j, z;
    cairo_status_t status;
    cairo_surface_t* sfc;
    cairo_format_t format = CAIRO_FORMAT_RGB24;
    unsigned char* data, *bitmapdata;
    unsigned char info[BMP_HEADER_OFFSET];
    status = read_func(closure, info, BMP_HEADER_OFFSET);
    if (status == CAIRO_STATUS_READ_ERROR)
        return cairo_image_surface_create(CAIRO_FORMAT_INVALID, 0, 0);

    // extract image height, width and bitsperpixel from header
    int width = (info[BMP_WIDTH_OFFSET]
                 | (int)info[BMP_WIDTH_OFFSET + 1] << 8
                 | (int)info[BMP_WIDTH_OFFSET + 2] << 16
                 | (int)info[BMP_WIDTH_OFFSET + 3] << 24);

    int height = (info[BMP_HEIGHT_OFFSET]
                  | (int)info[BMP_HEIGHT_OFFSET + 1] << 8
                  | (int)info[BMP_HEIGHT_OFFSET + 2] << 16
                  | (int)info[BMP_HEIGHT_OFFSET + 3] << 24);

    short bits = (info[BMP_BITS_OFFSET]
                  | (int)info[BMP_BITS_OFFSET + 1] << 8);

    // calculate stride, size
    int stride = cairo_format_stride_for_width(format, width);
    int size = 4 * width * height;
    int dest = size - 1;

    // create buffer of size stride and check for error
    if ((data = (unsigned char*)(malloc(stride))) == NULL)
    {
        free(data);
        return cairo_image_surface_create(CAIRO_FORMAT_INVALID, 0, 0);
    }

    // create cairo image surface and check for error
    sfc = cairo_image_surface_create(format, width, height);
    if (cairo_surface_status(sfc) != CAIRO_STATUS_SUCCESS)
        goto abort;

    bitmapdata = cairo_image_surface_get_data(sfc);
    if (size % stride == 0)
    {
        // read all data into memory buffer in blocks
        for (z = 0; z < size; z += stride)
        {
            if (bits == 32)
            {
                if ((status = read_func(closure, data, stride)) == CAIRO_STATUS_READ_ERROR)
                    goto abort;

                for (i = 0, j = stride - 1; i < stride; i += 4, j -= 4)
                {
                    bitmapdata[dest - i] = 0;           //A(not used)
                    bitmapdata[dest - i - 1] = data[j - 1]; //R
                    bitmapdata[dest - i - 2] = data[j - 2]; //G
                    bitmapdata[dest - i - 3] = data[j - 3]; //B
                }
            }
            else if (bits == 24)
            {
                int block = 3 * width;
                if ((status = read_func(closure, data, block)) == CAIRO_STATUS_READ_ERROR)
                    goto abort;

                for (i = 0, j = block - 1; i < stride; i += 4, j -= 3)
                {
                    bitmapdata[dest - i] = 0;           //A(not used)
                    bitmapdata[dest - i - 1] = data[j]; //R
                    bitmapdata[dest - i - 2] = data[j - 1]; //G
                    bitmapdata[dest - i - 3] = data[j - 2]; //B
                }
            }
            dest -= stride;
        }
    }
    else
    {
        goto abort;
    }

    free(data);
    return sfc;

abort:
    free(data);
    cairo_surface_destroy(sfc);
    return cairo_image_surface_create(CAIRO_FORMAT_INVALID, 0, 0);
}

/*! This is the read function which is called by
 * cairo_image_surface_create_from_bmp_stream().
 */
static cairo_status_t cj_read(void* closure, unsigned char* data, unsigned int length)
{
    return read((intptr_t) closure, data, length) < (int)length ? CAIRO_STATUS_READ_ERROR : CAIRO_STATUS_SUCCESS;
}

/*! This function reads an BMP image from a file and creates a Cairo image
 * surface. Internally the function calls
 * cairo_image_surface_create_from_bmp_stream() to actually read the data.
 * @param filename Pointer to filename of BMP file.
 * @return Returns a pointer to a cairo_surface_t structure. It should be
 * checked with cairo_surface_status() for errors.
 * @note If the returned surface is invalid you can use errno to determine
 * further reasons. Errno is set according to fopen(3) and malloc(3). If you
 * intend to check errno you shall set it to 0 before calling this function
 * because it does not modify errno itself.
 */
cairo_surface_t* cairo_image_surface_create_from_bmp(const char* filename)
{
    cairo_surface_t* sfc;
    int infile;

    // open input file
    if ((infile = open(filename, O_RDONLY)) == -1)
        return cairo_image_surface_create(CAIRO_FORMAT_INVALID, 0, 0);

    // call stream loading function
    sfc = cairo_image_surface_create_from_bmp_stream(cj_read, (void*)(intptr_t) infile);
    close(infile);

    return sfc;

}
