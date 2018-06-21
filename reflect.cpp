/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "ui.h"
#include <math.h>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>

using namespace std;
using namespace mui;

class MyImage : public Image
{
public:
    MyImage(const string& filename, int x = 0, int y = 0)
	: Image(filename, x, y)
    {
	m_back = shared_cairo_surface_t(cairo_surface_create_similar(m_image.get(),
								     CAIRO_CONTENT_COLOR_ALPHA,
								     cairo_image_surface_get_width(m_image.get()),
								     cairo_image_surface_get_height(m_image.get())),
					cairo_surface_destroy);
    }

    void draw(const Rect& rect)
    {
	auto cr = screen()->context();

	cairo_save(cr.get());

	cairo_matrix_t matrix_perspective;
	cairo_matrix_init (&matrix_perspective,
			   0.5, 0.25,
			   0.0, 1.0,
			   0.0, 0.0);
	cairo_transform(cr.get(), &matrix_perspective);

	cairo_set_source_surface(cr.get(), m_image.get(), 0.0, 0.0);
	auto width = cairo_image_surface_get_width(m_image.get());
	auto height = cairo_image_surface_get_width(m_image.get());
	cairo_rectangle(cr.get(), 0.0, 0.0, width, height);
	cairo_fill(cr.get());

	cairo_matrix_t matrix_reflex;
	cairo_matrix_init (&matrix_reflex,
			   0.5, 0.25,
			   0.0, -1.0,
			   0.0, height * 2);
	cairo_transform(cr.get(), &matrix_reflex);

	cairo_pattern_t *pat = cairo_pattern_create_linear(width / 2,
							   height / 2,
							   width / 2,
							   height);
	cairo_pattern_add_color_stop_rgba(pat, 0,
					  Color::BLACK.redf(),
					  Color::BLACK.greenf(),
					  Color::BLACK.bluef(),
					  0);

	cairo_pattern_add_color_stop_rgba(pat, 1,
					  Color::BLACK.redf(),
					  Color::BLACK.greenf(),
					  Color::BLACK.bluef(),
					  0.5);

	cairo_set_source_surface(cr.get(), m_image.get(), 0.0, 0.0);
	cairo_mask(cr.get(), pat);
	//cairo_rectangle(cr.get(), 0.0, 0.0, width, height);
	//cairo_fill(cr.get());
		cairo_paint(cr.get());

	cairo_pattern_destroy(pat);

	cairo_restore(cr.get());
    }
protected:
    shared_cairo_surface_t m_back;
};

int main()
{
#ifdef HAVE_TSLIB
#ifdef HAVE_LIBPLANES
    KMSScreen kms;
    InputTslib input0("/dev/input/touchscreen0");
#else
    FrameBuffer fb("/dev/fb0");
#endif
#else
    X11Screen screen(Size(800,480));
#endif

    SimpleWindow win(Size(800,480));

    MyImage image("square.png", 100, 100);
    win.add(&image);

    EventLoop::run();

    return 0;
}
