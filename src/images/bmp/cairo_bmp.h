/*! This file contains all prototypes for the Cairo-BMP functions implemented
 * in cairo_bmp.c.
 */
#include <cairo.h>

#ifdef __cplusplus
extern "C" {
#endif
cairo_surface_t* cairo_image_surface_create_from_bmp_stream(cairo_read_func_t read_func, void* closure);
cairo_surface_t* cairo_image_surface_create_from_bmp(const char* filename);
#ifdef __cplusplus
}
#endif
