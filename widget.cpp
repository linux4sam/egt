/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "widget.h"
#include "geometry.h"
#include "input.h"
#include "screen.h"
#include "utils.h"
#include <vector>
#include <string>
#include <cassert>
#include <cairo.h>
#include <iostream>
#include "window.h"

using namespace std;

namespace mui
{

    Widget::Widget(int x, int y, int w, int h)
	: m_box(x, y, w, h),
	  m_visible(true),
	  m_focus(false),
	  m_active(false)
    {
    }

    int Widget::handle(int event)
    {
	return 0;
    }

    void Widget::position(int x, int y)
    {
	if (x != box().x || y != box().y)
	{
	    damage();
	    m_box.x = x;
	    m_box.y = y;
	}
    }

    void Widget::size(int w, int h)
    {
	if (w != box().w || h != box().h)
	{
	    damage();
	    m_box.w = w;
	    m_box.h = h;
	}
    }

    void Widget::resize(int w, int h)
    {
	if (w != box().w || h != box().h)
	{
	    damage();
	    size(w, h);
	    damage();
	}
    }

    void Widget::move(int x, int y)
    {
	if (x != box().x || y != box().y)
	{
	    damage();
	    position(x, y);
	    damage();
#if 0
	    Rect damaged = box();
	    position(x, y);
	    main_window()->damage(Rect::merge(damaged, box()));
#endif
	}
    }

    void Widget::damage()
    {
	main_window()->damage(box());
    }

    void Widget::draw_text(const std::string& text, const Color& color, int align, int standoff)
    {
	auto cr = screen()->context();

	cairo_text_extents_t textext;
	cairo_select_font_face(cr.get(), "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr.get(), 16);
	cairo_text_extents(cr.get(),text.c_str(), &textext);

	Point p;
	if (align & ALIGN_CENTER)
	{
	    p.x = x() + (w()/2) - (textext.width/2) + textext.x_bearing;
	    p.y = y() + (h()/2) + (textext.height/2);
	}

	if (align & ALIGN_LEFT)
	    p.x = x() + standoff;
	if (align & ALIGN_RIGHT)
	    p.x = x() + w() - textext.width - standoff;
	if (align & ALIGN_TOP)
	    p.y = y() + textext.height + standoff;
	if (align & ALIGN_BOTTOM)
	    p.y = y() + h() - standoff;


	cairo_set_source_rgba(cr.get(), color.redf(), color.greenf(), color.bluef(), color.alphaf());
	cairo_move_to(cr.get(), p.x, p.y);
	cairo_show_text(cr.get(), text.c_str());
    }


    void Widget::draw_image(shared_cairo_surface_t image, int align, int standoff)
    {
	auto width = cairo_image_surface_get_width(image.get());
	auto height = cairo_image_surface_get_height(image.get());

	Point p;
	if (align & ALIGN_CENTER)
	{
	    p.x = x() + (w()/2) - (width/2);
	    p.y = y() + (h()/2) - (height/2);
	}

	if (align & ALIGN_LEFT)
	    p.x = x() + standoff;
	if (align & ALIGN_RIGHT)
	    p.x = x() + w() - width - standoff;
	if (align & ALIGN_TOP)
	    p.y = y() + height + standoff;
	if (align & ALIGN_BOTTOM)
	    p.y = y() + h() - standoff;

	screen()->blit(image.get(), p.x, p.y, width, height, p.x, p.y);
    }

    Widget::~Widget()
    {}

    Image::Image(const string& filename, int x, int y)
	: m_filename(filename),
	  m_scale(1.0)
    {
	m_image = shared_cairo_surface_t(cairo_image_surface_create_from_png(filename.c_str()), cairo_surface_destroy);
	assert(m_image.get());

	m_cache.insert(std::make_pair(1.0 * 100, m_image));

	position(x, y);
	size(cairo_image_surface_get_width(m_image.get()),
	     cairo_image_surface_get_height(m_image.get()));

	m_back = shared_cairo_surface_t(cairo_surface_create_similar(m_image.get(),
								     CAIRO_CONTENT_COLOR_ALPHA,
								     cairo_image_surface_get_width(m_image.get()),
								     cairo_image_surface_get_height(m_image.get())),
					cairo_surface_destroy);
	assert(m_back.get());
	auto cr = shared_cairo_t(cairo_create(m_back.get()), cairo_destroy);
	cairo_set_source_surface(cr.get(), m_image.get(), 0, 0);
	cairo_set_operator(cr.get(), CAIRO_OPERATOR_SOURCE);
	cairo_paint(cr.get());

	damage();
    }

    static shared_cairo_surface_t
    scale_surface(shared_cairo_surface_t old_surface,
		  int old_width, int old_height,
		  int new_width, int new_height)
    {
	auto new_surface = shared_cairo_surface_t(cairo_surface_create_similar(old_surface.get(),
									       CAIRO_CONTENT_COLOR_ALPHA,
									       new_width,
									       new_height),
						  cairo_surface_destroy);
	auto cr = shared_cairo_t(cairo_create(new_surface.get()), cairo_destroy);

	/* Scale *before* setting the source surface (1) */
	cairo_scale(cr.get(),
		    (double)new_width / old_width,
		    (double)new_height / old_height);
	cairo_set_source_surface(cr.get(), old_surface.get(), 0, 0);

	/* To avoid getting the edge pixels blended with 0 alpha, which would
	 * occur with the default EXTEND_NONE. Use EXTEND_PAD for 1.2 or newer (2)
	 */
	cairo_pattern_set_extend(cairo_get_source(cr.get()), CAIRO_EXTEND_REFLECT);

	/* Replace the destination with the source instead of overlaying */
	cairo_set_operator(cr.get(), CAIRO_OPERATOR_SOURCE);

	/* Do the actual drawing */
	cairo_paint(cr.get());

	return new_surface;
    }

    static float round(float v, float fraction)
    {
	return floor(v) + floor( (v-floor(v))/fraction) * fraction;
    }

    void Image::scale(double scale)
    {
	damage();

	double neww = cairo_image_surface_get_width(m_back.get());
	double newh = cairo_image_surface_get_height(m_back.get());

	scale = round(scale, 0.01);

	auto i = m_cache.find(scale * 100);
	if (i != m_cache.end())
	{
	    m_image = i->second;
	}
	else
	{
	    cout << "cache miss " << scale * 100 << endl;

	    m_image = scale_surface(m_back,
				    neww, newh,
				    neww * scale, newh * scale);

	    m_cache.insert(std::make_pair(scale * 100, m_image));
	}

	size(neww * scale, newh * scale);
	m_scale = scale;
	damage();
    }

    void Image::draw(const Rect& rect)
    {
	screen()->blit(m_image.get(), rect.x, rect.y, rect.w, rect.h, box().x, box().y);
    }

    Image::~Image()
    {

    }

    Button::Button(const string& label, const Point& point, const Size& size)
	: Widget(point.x, point.y, size.w, size.h),
	  m_label(label)
    {

    }

    int Button::handle(int event)
    {
	switch (event)
	{
	case EVT_MOUSE_DOWN:
	    cout << "button: mouse down" << endl;
	    if (!active())
	    {
		damage();
		active(true);
		return 1;
	    }
	    break;
	case EVT_MOUSE_UP:
	    cout << "button: mouse up" << endl;
	    if (active())
	    {
		damage();
		active(false);
		return 1;
	    }
	    break;
	case EVT_MOUSE_MOVE:
	    cout << "button: mouse move" << endl;
	    break;
	case EVT_KEYDOWN:
	    cout << "button: key: " << key_position() << endl;
	    break;
	}

	return Widget::handle(event);
    }

    void Button::draw(const Rect& rect)
    {
	Color white(Color::WHITE);

	auto cr = screen()->context();

	cairo_save(cr.get());

	double rx         = x(),
	    ry         = y(),
	    width         = w(),
	    height        = h(),
	    aspect        = 1.0,
	    corner_radius = height / 10.0;

	double radius = corner_radius / aspect;
	double degrees = M_PI / 180.0;

	cairo_new_sub_path (cr.get());
	cairo_arc (cr.get(), rx + width - radius, ry + radius, radius, -90 * degrees, 0 * degrees);
	cairo_arc (cr.get(), rx + width - radius, ry + height - radius, radius, 0 * degrees, 90 * degrees);
	cairo_arc (cr.get(), rx + radius, ry + height - radius, radius, 90 * degrees, 180 * degrees);
	cairo_arc (cr.get(), rx + radius, ry + radius, radius, 180 * degrees, 270 * degrees);
	cairo_close_path (cr.get());

	cairo_pattern_t *pat3;
	pat3 = cairo_pattern_create_linear(x()+w()/2, y(), x()+w()/2, y()+h());

	if (!active())
	{
	    Color step = white;
	    cairo_pattern_add_color_stop_rgb(pat3, 0, step.redf(), step.greenf(), step.bluef());
	    step = white.tint(.9);
	    cairo_pattern_add_color_stop_rgb(pat3, 0.43, step.redf(), step.greenf(), step.bluef());
	    step = white.tint(.82);
	    cairo_pattern_add_color_stop_rgb(pat3, 0.5, step.redf(), step.greenf(), step.bluef());
	    step = white.tint(.95);
	    cairo_pattern_add_color_stop_rgb(pat3, 1.0, step.redf(), step.greenf(), step.bluef());

	    cairo_set_line_width (cr.get(), 1.0);
	}
	else
	{
	    Color step = white;
	    cairo_pattern_add_color_stop_rgb(pat3, 1, step.redf(), step.greenf(), step.bluef());
	    step = white.tint(.9);
	    cairo_pattern_add_color_stop_rgb(pat3, 0.5, step.redf(), step.greenf(), step.bluef());
	    step = white.tint(.82);
	    cairo_pattern_add_color_stop_rgb(pat3, 0.43, step.redf(), step.greenf(), step.bluef());
	    step = white.tint(.95);
	    cairo_pattern_add_color_stop_rgb(pat3, 0, step.redf(), step.greenf(), step.bluef());

	    cairo_set_line_width (cr.get(), 2.0);
	}

	cairo_set_source(cr.get(), pat3);
	cairo_fill_preserve (cr.get());

	cairo_set_source_rgba(cr.get(),
			      BORDER_COLOR.redf(),
			      BORDER_COLOR.greenf(),
			      BORDER_COLOR.bluef(),
			      BORDER_COLOR.alphaf());

	cairo_stroke (cr.get());

	cairo_set_source_rgb (cr.get(),
			      TEXT_COLOR.redf(),
			      TEXT_COLOR.greenf(),
			      TEXT_COLOR.bluef());

	draw_text(m_label);

	cairo_pattern_destroy(pat3);

	cairo_restore(cr.get());
    }

    Button::~Button()
    {
    }

    Combo::Combo(const string& label, const Point& point, const Size& size)
	: Widget(point.x, point.y, size.w, size.h),
	  m_label(label)
    {

    }

    int Combo::handle(int event)
    {
	return Widget::handle(event);
    }

    void Combo::draw(const Rect& rect)
    {
	Color white(Color::WHITE);

	auto cr = screen()->context();

	cairo_save(cr.get());

	// path
	double rx = x(),
	    ry = y(),
	    width = w(),
	    height = h(),
	    aspect = 1.0,
	    corner_radius = height / 10.0;

	double radius = corner_radius / aspect;
	double degrees = M_PI / 180.0;

	cairo_new_sub_path (cr.get());
	cairo_arc (cr.get(), rx + width - radius, ry + radius, radius, -90 * degrees, 0 * degrees);
	cairo_arc (cr.get(), rx + width - radius, ry + height - radius, radius, 0 * degrees, 90 * degrees);
	cairo_arc (cr.get(), rx + radius, ry + height - radius, radius, 90 * degrees, 180 * degrees);
	cairo_arc (cr.get(), rx + radius, ry + radius, radius, 180 * degrees, 270 * degrees);
	cairo_close_path (cr.get());

	// fill
	cairo_pattern_t *pat3;
	pat3 = cairo_pattern_create_linear(x()+w()/2, y(), x()+w()/2, y()+h());

	Color step = white;
	cairo_pattern_add_color_stop_rgb(pat3, 0, step.redf(), step.greenf(), step.bluef());
	step = white.tint(.9);
	cairo_pattern_add_color_stop_rgb(pat3, 0.43, step.redf(), step.greenf(), step.bluef());
	step = white.tint(.82);
	cairo_pattern_add_color_stop_rgb(pat3, 0.5, step.redf(), step.greenf(), step.bluef());
	step = white.tint(.95);
	cairo_pattern_add_color_stop_rgb(pat3, 1.0, step.redf(), step.greenf(), step.bluef());

	cairo_set_source(cr.get(), pat3);
	cairo_fill_preserve(cr.get());
	cairo_pattern_destroy(pat3);

	// border
	cairo_set_source_rgba(cr.get(),
			      BORDER_COLOR.redf(),
			      BORDER_COLOR.greenf(),
			      BORDER_COLOR.bluef(),
			      BORDER_COLOR.alphaf());
	cairo_set_line_width (cr.get(), 1.0);
	cairo_stroke(cr.get());

	// text
	draw_text(m_label, TEXT_COLOR, ALIGN_LEFT|ALIGN_CENTER);

#if 0
	// triangle
	cairo_set_source_rgb (cr.get(), border.redf(), border.greenf(), border.bluef());
	cairo_move_to(cr.get(), 240, 40);
	cairo_line_to(cr.get(), 240, 160);
	cairo_line_to(cr.get(), 350, 160);
	cairo_close_path(cr.get());
	cairo_stroke_preserve(cr.get());
	cairo_fill(cr.get());
#endif

	cairo_restore(cr.get());

	assert(CAIRO_HAS_PNG_FUNCTIONS == 1);

	// images
	auto up = shared_cairo_surface_t(cairo_image_surface_create_from_png("icons/bullet_arrow_up.png"), cairo_surface_destroy);
	assert(cairo_surface_status(up.get()) == CAIRO_STATUS_SUCCESS);

	auto down = shared_cairo_surface_t(cairo_image_surface_create_from_png("icons/bullet_arrow_down.png"), cairo_surface_destroy);
	assert(cairo_surface_status(down.get()) == CAIRO_STATUS_SUCCESS);

	auto upwidth = cairo_image_surface_get_width(up.get());
	auto upheight = cairo_image_surface_get_height(up.get());
	screen()->blit(up.get(),
		       x() + w() - upwidth - 5,
		       y() + 5,
		       upwidth,
		       upheight,
		       x() + w() - upwidth - 5,
		       y() + 5,
		       true);

	auto downwidth = cairo_image_surface_get_width(down.get());
	auto downheight = cairo_image_surface_get_height(down.get());
	screen()->blit(down.get(),
		       x() + w() - downwidth - 5,
		       y() + h() - downheight - 5,
		       downwidth,
		       downheight,
		       x() + w() - downwidth - 5,
		       y() + h() - downheight - 5,
		       true);
    }

    Combo::~Combo()
    {
    }

    Slider::Slider(int min, int max, const Point& point, const Size& size)
	: Widget(point.x, point.y, size.w, size.h),
	  m_min(min),
	  m_max(max),
	  m_pos(min),
	  m_moving_x(0)
    {

    }

    int Slider::handle(int event)
    {
	switch (event)
	{
	case EVT_MOUSE_DOWN:
	    m_moving_x = mouse_position().x;
	    m_start_pos = position();
	    active(true);
	    return 1;
	    break;
	case EVT_MOUSE_UP:
	    active(false);
	    return 1;
	    break;
	case EVT_MOUSE_MOVE:
	    if (active())
	    {
		int diff = mouse_position().x - m_moving_x;
		position(m_start_pos + denormalize(diff));
	    }
	    break;
	case EVT_KEYDOWN:
	    cout << "button: key: " << key_position() << endl;
	    break;
	}

	return Widget::handle(event);
    }

    void Slider::draw(const Rect& rect)
    {
	auto cr = screen()->context();

	cairo_save(cr.get());

	// line
	cairo_set_source_rgba(cr.get(),
			      BORDER_COLOR.redf(),
			      BORDER_COLOR.greenf(),
			      BORDER_COLOR.bluef(),
			      BORDER_COLOR.alphaf());
	cairo_set_line_width(cr.get(), 4.0);
	cairo_move_to(cr.get(), x() + RADIUS, y()+h()/2);
	cairo_line_to(cr.get(), x() + w() - RADIUS, y()+h()/2);
	cairo_stroke(cr.get());

	// path
	double rx         = x() + normalize(m_pos) + 1,
	    ry         = y() + 1,
	    width         = RADIUS * 4-2,
	    height        = h()-2,
	    aspect        = 1.0,
	    corner_radius = height / 10.0;

	double radius = corner_radius / aspect;
	double degrees = M_PI / 180.0;

	cairo_new_sub_path (cr.get());
	cairo_arc (cr.get(), rx + width - radius, ry + radius, radius, -90 * degrees, 0 * degrees);
	cairo_arc (cr.get(), rx + width - radius, ry + height - radius, radius, 0 * degrees, 90 * degrees);
	cairo_arc (cr.get(), rx + radius, ry + height - radius, radius, 90 * degrees, 180 * degrees);
	cairo_arc (cr.get(), rx + radius, ry + radius, radius, 180 * degrees, 270 * degrees);
	cairo_close_path (cr.get());

	// fill
	cairo_pattern_t *pat3;
	pat3 = cairo_pattern_create_linear(x()+normalize(m_pos)+RADIUS/2, y(), x()+normalize(m_pos)+RADIUS/2, y()+RADIUS*2);


	Color step = GLOW_COLOR;
	cairo_pattern_add_color_stop_rgb(pat3, 0, step.redf(), step.greenf(), step.bluef());
	step = GLOW_COLOR.tint(.9);
	cairo_pattern_add_color_stop_rgb(pat3, 0.43, step.redf(), step.greenf(), step.bluef());
	step = GLOW_COLOR.tint(.82);
	cairo_pattern_add_color_stop_rgb(pat3, 0.5, step.redf(), step.greenf(), step.bluef());
	step = GLOW_COLOR.tint(.95);
	cairo_pattern_add_color_stop_rgb(pat3, 1.0, step.redf(), step.greenf(), step.bluef());

	cairo_set_line_width (cr.get(), 1.0);

	cairo_set_source(cr.get(), pat3);
	cairo_fill_preserve (cr.get());

	cairo_set_source_rgba(cr.get(),
			      BORDER_COLOR.redf(),
			      BORDER_COLOR.greenf(),
			      BORDER_COLOR.bluef(),
			      BORDER_COLOR.alphaf());
	cairo_stroke(cr.get());


#if 0
	cairo_pattern_t *pat;
	pat = cairo_pattern_create_radial(normalize(m_pos) + x() + RADIUS, y()+h()/2, 1,
					  normalize(m_pos) + x() + RADIUS, y()+h()/2, RADIUS);
	Color step = color;
	cairo_pattern_add_color_stop_rgba(pat, 0, step.redf(), step.greenf(), step.bluef(), step.alphaf());
	step = color.tint(0.9);
	cairo_pattern_add_color_stop_rgba(pat, 1, step.redf(), step.greenf(), step.bluef(), step.alphaf());


	cairo_set_source(cr.get(), pat);
	cairo_arc(cr.get(), normalize(m_pos) + x() + RADIUS, y()+h()/2, RADIUS, 0, 2 * M_PI);
	cairo_fill(cr.get());

	cairo_set_line_width(cr.get(), 1.0);
	cairo_set_source_rgba(cr.get(),
			      BORDER_COLOR.redf(),
			      BORDER_COLOR.greenf(),
			      BORDER_COLOR.bluef(),
			      BORDER_COLOR.alphaf());
	cairo_arc(cr.get(), normalize(m_pos) + x() + RADIUS, y()+h()/2, RADIUS, 0, 2 * M_PI);
	cairo_stroke(cr.get());
	cairo_pattern_destroy(pat);
#endif

	cairo_restore(cr.get());
    }

    Slider::~Slider()
    {
    }


    Label::Label(const std::string& text, const Point& point, const Size& size, int align)
	: Widget(point.x, point.y, size.w, size.h),
	  m_align(align),
	  m_text(text)
    {

    }

    int Label::handle(int event)
    {
	return Widget::handle(event);
    }

    void Label::draw(const Rect& rect)
    {
	draw_text(m_text, TEXT_COLOR, m_align);
    }

    Label::~Label()
    {
    }


    SimpleText::SimpleText(const std::string& text, const Point& point, const Size& size)
	: Widget(point.x, point.y, size.w, size.h),
	  m_text(text)
    {

    }

    int SimpleText::handle(int event)
    {
	switch (event)
	{
	case EVT_MOUSE_DOWN:
	    damage();
	    focus(true);
	    return 1;
	case EVT_KEYDOWN:
	    if (focus())
	    {
		m_text.append(1, (char)key_position());
		damage();
		return 1;
	    }
	    break;
	}

	return Widget::handle(event);
    }

    void SimpleText::draw(const Rect& rect)
    {
	Color background(Color::WHITE);
	auto cr = screen()->context();

	cairo_save(cr.get());

        double rx = x(),
	    ry  = y(),
	    width         = w(),
	    height        = h(),
	    aspect        = 1.0,
	    corner_radius = height / 10.0;

	double radius = corner_radius / aspect;
	double degrees = M_PI / 180.0;

	cairo_new_sub_path (cr.get());
	cairo_arc (cr.get(), rx + width - radius, ry + radius, radius, -90 * degrees, 0 * degrees);
	cairo_arc (cr.get(), rx + width - radius, ry + height - radius, radius, 0 * degrees, 90 * degrees);
	cairo_arc (cr.get(), rx + radius, ry + height - radius, radius, 90 * degrees, 180 * degrees);
	cairo_arc (cr.get(), rx + radius, ry + radius, radius, 180 * degrees, 270 * degrees);
	cairo_close_path (cr.get());

	cairo_set_source_rgb (cr.get(), background.redf(), background.greenf(),background.bluef());
	cairo_fill_preserve (cr.get());

	cairo_set_source_rgba(cr.get(),
			      BORDER_COLOR.redf(),
			      BORDER_COLOR.greenf(),
			      BORDER_COLOR.bluef(),
			      BORDER_COLOR.alphaf());
	cairo_set_line_width (cr.get(), 1.0);
	cairo_stroke (cr.get());

	draw_text(m_text, TEXT_COLOR, ALIGN_CENTER | ALIGN_LEFT);

	cairo_restore(cr.get());
    }

    SimpleText::~SimpleText()
    {
    }


    ImageLabel::ImageLabel(const std::string& image,
			   const std::string& text,
			   const Point& point,
			   const Size& size)
	: Label(text, point, size)
    {
	m_image = shared_cairo_surface_t(cairo_image_surface_create_from_png(image.c_str()),
					 cairo_surface_destroy);
	assert(m_image.get());

	damage();
    }

    void ImageLabel::draw(const Rect& rect)
    {
	draw_image(m_image, ALIGN_LEFT | ALIGN_CENTER);

	auto cr = screen()->context();

	draw_text(m_text, TEXT_COLOR, ALIGN_LEFT | ALIGN_CENTER, 20);
    }

    ImageLabel::~ImageLabel()
    {
    }

}
