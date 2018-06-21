#include "ui.h"
#ifdef HAVE_LIBGD
#include <gd.h>
#endif
#ifdef HAVE_IMLIB2
#include <Imlib2.h>
#endif
#include <chrono>
#include <iostream>
#include <string.h>
#include <fstream>

using namespace mui;
using namespace std;

class CairoPainter
{
public:

    CairoPainter()
    {
	cairo_format_t format = CAIRO_FORMAT_ARGB32;

	surface = cairo_image_surface_create(format, 800, 480);
	assert(surface);

        cr = cairo_create(surface);
	assert(cr);
    }

    void write(const char* filename)
    {
	cairo_surface_write_to_png (surface,
				    filename);
    }

    void set_color(const Color& color)
    {
	cairo_set_source_rgba(cr, color.redf(), color.greenf(), color.bluef(), color.alphaf());
	//cairo_set_source_rgb(cr, color.redf(), color.greenf(), color.bluef());
    }

    void draw_rect(const Rect& rect)
    {
	cairo_rectangle(cr, rect.x, rect.y, rect.w, rect.h);
	cairo_fill(cr);
    }

    cairo_t* cr;
    cairo_surface_t* surface;

};

#ifdef HAVE_LIBGD
class GdPainter
{
public:

    GdPainter()
    {
	im = gdImageCreate(800, 480);
    }

    void set_color(const Color& color)
    {
	c = gdImageColorAllocateAlpha(im, color.red(), color.blue(), color.green(), color.alpha());
	//c = gdImageColorAllocate(im, color.red(), color.blue(), color.green());
    }

    void draw_rect(const Rect& rect)
    {
	gdImageFilledRectangle(im, rect.x, rect.y, rect.x + rect.w, rect.y + rect.h, c);
    }

    virtual ~GdPainter()
    {
	gdImageDestroy(im);
    }

    int c;
    gdImagePtr im;
};
#endif

#ifdef HAVE_IMLIB2
class Imlib2Painter
{
public:

    Imlib2Painter()
    {
	im = imlib_create_image(800, 480);
	imlib_context_set_image(im);
	imlib_context_set_blend(1);
    }

    void set_color(const Color& color)
    {
	imlib_context_set_color(color.red(), color.green(), color.blue(), color.alpha());
    }

    void draw_rect(const Rect& rect)
    {
	imlib_image_fill_rectangle(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h);
    }

    virtual ~Imlib2Painter()
    {

    }

    Imlib_Image im;
};
#endif

//#define BUF buffer[x][y]
#define BUF (buffer + (y*800) + x)

class CustomPainter
{
public:

    CustomPainter()
    {
	buffer = new uint32_t[800 * 480];

	/*
	for (int y = 0; y < 480;y++)
	    for (int x = 0; x < 800;x++)
		BUF = Color(Color::RED).pixel();
	*/

	//memset(buffer,0x0,800 * 480 * sizeof(uint32_t));
    }

    void write(const char* filename)
    {
	cairo_format_t format = CAIRO_FORMAT_ARGB32;
	cairo_surface_t* surface = cairo_image_surface_create_for_data((unsigned char*)buffer,
								       format,
								       800, 480,
								       cairo_format_stride_for_width(format, 800));
	cairo_surface_write_to_png (surface,
				    filename);
	cairo_surface_destroy(surface);
    }

    void set_color(const Color& color)
    {
	this->color = color;
    }

    // https://stackoverflow.com/questions/1102692/how-to-alpha-blend-rgba-unsigned-byte-color-fast

    inline unsigned int blendPreMulAlpha(unsigned int colora, unsigned int colorb, unsigned int alpha)
    {
	unsigned int rb = (colora & 0xFF00FF) + ( (alpha * (colorb & 0xFF00FF)) >> 8 );
	unsigned int g = (colora & 0x00FF00) + ( (alpha * (colorb & 0x00FF00)) >> 8 );
	return alpha << 24 | ((rb & 0xFF00FF) + (g & 0x00FF00));
    }

    inline unsigned int blendAlpha(unsigned int colora, unsigned int colorb, unsigned int alpha)
    {
	unsigned int rb1 = ((0x100 - alpha) * (colora & 0xFF00FF)) >> 8;
	unsigned int rb2 = (alpha * (colorb & 0xFF00FF)) >> 8;
	unsigned int g1  = ((0x100 - alpha) * (colora & 0x00FF00)) >> 8;
	unsigned int g2  = (alpha * (colorb & 0x00FF00)) >> 8;
	return (alpha << 24) + ((rb1 | rb2) & 0xFF00FF) + ((g1 | g2) & 0x00FF00);
    }

    inline uint32_t blendPreMulAlpha2(uint32_t p1, uint32_t p2)
    {
	static const int AMASK = 0xFF000000;
	static const int RBMASK = 0x00FF00FF;
	static const int GMASK = 0x0000FF00;
	static const int AGMASK = AMASK | GMASK;
	static const int ONEALPHA = 0x01000000;
	unsigned int a = (p2 & AMASK) >> 24;
	unsigned int na = 255 - a;
	unsigned int rb = ((na * (p1 & RBMASK)) + (a * (p2 & RBMASK))) >> 8;
	unsigned int ag = (na * ((p1 & AGMASK) >> 8)) + (a * (ONEALPHA | ((p2 & GMASK) >> 8)));
	return ((rb & RBMASK) | (ag & AGMASK));
    }

    inline uint32_t blendPreMulAlpha3(uint32_t p1, uint32_t p2)
    {
	uint32_t dr = (p1 >> 16) & 0xff;
	uint32_t dg = (p1 >> 8) & 0xff;
	uint32_t db = (p1) & 0xff;
	uint32_t ia = 255 - ((p2 >> 24) & 0xff);

	// normal: result = (color.RGB * source.A) + (dest.RGB * (1 - source.A));
	// pre-multiplied: result = source.RGB + (dest.RGB * (1 - source.A))
	uint8_t r = ((p2 >> 16) & 0xff) + dr * ia;
	uint8_t g = ((p2 >> 8) & 0xff) + dg * ia;
	uint8_t b = (p2 & 0xff) + db * ia;

	return color.alpha() << 24 | r << 16 | g << 8 | b;
    }

    void draw_rect(const Rect& rect)
    {
	if (color.alpha() == 0)
	    return;

	uint32_t pixel = color.pixel();

	int w = rect.x + rect.w;
	int h = rect.y + rect.h;
	for (int y = rect.y; y < h;y++)
	{
	    for (int x = rect.x; x < w;x++)
	    {
		if (color.alpha() == 255)
		{
		    *BUF = pixel;
		}
		else
		{
		    uint32_t* b = BUF;

		    //*b = blendPreMulAlpha(*b, pixel, color.alpha());
		    //*b = blendAlpha(*b, pixel, color.alpha());
		    //*b = blendPreMulAlpha2(*b, pixel);
		    *b = blendPreMulAlpha3(*b, pixel);
		}
	    }
	}
    }

    virtual ~CustomPainter()
    {

    }

    uint32_t* buffer;
    Color color;

};

int main()
{
    Color color(Color::RED);
    color.alpha(0x55);

    CairoPainter c;
    c.set_color(color);
#ifdef HAVE_LIBGD
    GdPainter g;
    g.set_color(color);
#endif
#ifdef HAVE_IMLIB2
    Imlib2Painter i;
    i.set_color(color);
#endif
    CustomPainter d;
    d.set_color(color);

    Rect rect(10,10,200,200);

    int LOOP = 1000;
    int RUNS = 3;

    cout << "cairo: " << flush;
    c.draw_rect(rect);
    c.write("cairo.png");
    for (int y = 0; y < RUNS;y++)
    {
	auto started = std::chrono::high_resolution_clock::now();
	for (int x = 0; x < LOOP;x++)
	{
	    c.draw_rect(rect);
	}
	auto done = std::chrono::high_resolution_clock::now();

	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(done-started).count() << " " << flush;
    }
    cout << endl;

#ifdef HAVE_LIBGD
    cout << "gd: " << flush;
    for (int y = 0; y < RUNS;y++)
    {
	auto started = std::chrono::high_resolution_clock::now();
	for (int x = 0; x < LOOP;x++)
	{
	    g.draw_rect(rect);
	}
	auto done = std::chrono::high_resolution_clock::now();

	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(done-started).count() << " " << flush;
    }
    cout << endl;
#endif

#ifdef HAVE_IMLIB2
    cout << "imlib2: " << flush;
    for (int y = 0; y < RUNS;y++)
    {
	auto started = std::chrono::high_resolution_clock::now();
	for (int x = 0; x < LOOP;x++)
	{
	    i.draw_rect(rect);
	}
	auto done = std::chrono::high_resolution_clock::now();

	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(done-started).count() << " " << flush;
    }
    cout << endl;
#endif

    cout << "custom: " << flush;
    d.draw_rect(rect);
    d.write("custom.png");
    for (int y = 0; y < RUNS;y++)
    {
	auto started = std::chrono::high_resolution_clock::now();
	for (int x = 0; x < LOOP;x++)
	{
	    d.draw_rect(rect);
	}
	auto done = std::chrono::high_resolution_clock::now();

	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(done-started).count() << " " << flush;
    }
    cout << endl;

    return 0;
}
