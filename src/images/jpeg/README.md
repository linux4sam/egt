# cairo_jpg

## Introduction

This is an implementation of functions to import and export Cairo surfaces from
and to JPEG files. It uses the same function prototypes as Cairo's [PNG
support](http://www.cairographics.org/manual/cairo-PNG-Support.html).

## Description

The implementation is done on top of the Cairo API. It does not access
Cairo-internal functions.
For compression and decompression a JPEG library is used. It compiles against
[libjpeg-turbo](https://libjpeg-turbo.org/) or the original
[libjpeg](http://www.ijg.org/).

The following prototypes are implemented. Their functionality is equal to the
PNG functions of Cairo with the advance that there are memory-buffer-based
functions as well.

```C
cairo_status_t cairo_image_surface_write_to_jpeg_mem(cairo_surface_t *sfc, unsigned char **data, size_t *len, int quality);
cairo_status_t cairo_image_surface_write_to_jpeg_stream(cairo_surface_t *sfc, cairo_write_func_t write_func, void *closure, int quality);
cairo_status_t cairo_image_surface_write_to_jpeg(cairo_surface_t *sfc, const char *filename, int quality);
cairo_surface_t *cairo_image_surface_create_from_jpeg_mem(void *data, size_t len);
cairo_surface_t *cairo_image_surface_create_from_jpeg_stream(cairo_read_func_t read_func, void *closure);
cairo_surface_t *cairo_image_surface_create_from_jpeg(const char *filename);
```

To compile this code you need to have installed the packages libcairo2-dev
and libjpeg-dev. Compile with the following command to create an object file
linkable to your code:
```Shell
gcc -Wall -c `pkg-config cairo libjpeg --cflags --libs` cairo_jpg.c
```

Please have a look at the comments within the source files for further details.
Don't hesitate to contact me at [bf@abenteuerland.at](mailto:bf@abenteuerland.at).

## Testing

There is a ```main()``` function implemented which serves as demonstrational
purpose and for testing. To compile with the ```main()``` function run the
following statement:
```Shell
gcc -Wall -o cairo_jpg -DCAIRO_JPEG_MAIN `pkg-config cairo libjpeg --cflags --libs` cairo_jpg.c
```

