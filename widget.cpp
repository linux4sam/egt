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
	  m_active(false),
	  m_parent(0)
    {}

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
	}
    }

    void Widget::damage()
    {
	parent()->damage(box());
    }

    void Widget::draw_basic_box(const Rect& rect,
				const Color& border,
				const Color& bg)
    {
	auto cr = screen()->context();

	cairo_save(cr.get());

        double rx = rect.x,
	    ry = rect.y,
	    width = rect.w,
	    height = rect.h,
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

	cairo_set_source_rgba(cr.get(),
			      bg.redf(),
			      bg.greenf(),
			      bg.bluef(),
			      bg.alphaf());
	cairo_fill_preserve (cr.get());

	cairo_set_source_rgba(cr.get(),
			      border.redf(),
			      border.greenf(),
			      border.bluef(),
			      border.alphaf());
	cairo_set_line_width (cr.get(), 1.0);
	cairo_stroke (cr.get());

	cairo_restore(cr.get());
    }

    void Widget::draw_gradient_box(const Rect& rect,
				   const Color& border,
				   const Color& bg,
				   bool active)
    {
	auto cr = screen()->context();

	cairo_save(cr.get());

        double rx = rect.x,
	    ry = rect.y,
	    width = rect.w,
	    height = rect.h,
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

	cairo_pattern_t *pat;
	pat = cairo_pattern_create_linear(rx + width / 2, ry, rx+width/2, ry + height);

	if (!active)
	{
	    Color step = bg;
	    cairo_pattern_add_color_stop_rgb(pat, 0, step.redf(), step.greenf(), step.bluef());
	    step = bg.tint(.9);
	    cairo_pattern_add_color_stop_rgb(pat, 0.43, step.redf(), step.greenf(), step.bluef());
	    step = bg.tint(.82);
	    cairo_pattern_add_color_stop_rgb(pat, 0.5, step.redf(), step.greenf(), step.bluef());
	    step = bg.tint(.95);
	    cairo_pattern_add_color_stop_rgb(pat, 1.0, step.redf(), step.greenf(), step.bluef());

	    cairo_set_line_width (cr.get(), 1.0);
	}
	else
	{
	    Color step = bg;
	    cairo_pattern_add_color_stop_rgb(pat, 1, step.redf(), step.greenf(), step.bluef());
	    step = bg.tint(.9);
	    cairo_pattern_add_color_stop_rgb(pat, 0.5, step.redf(), step.greenf(), step.bluef());
	    step = bg.tint(.82);
	    cairo_pattern_add_color_stop_rgb(pat, 0.43, step.redf(), step.greenf(), step.bluef());
	    step = bg.tint(.95);
	    cairo_pattern_add_color_stop_rgb(pat, 0, step.redf(), step.greenf(), step.bluef());

	    cairo_set_line_width (cr.get(), 2.0);
	}

	cairo_set_source(cr.get(), pat);
	cairo_fill_preserve(cr.get());

	cairo_set_source_rgba(cr.get(),
			      border.redf(),
			      border.greenf(),
			      border.bluef(),
			      border.alphaf());
	cairo_set_line_width(cr.get(), 1.0);
	cairo_stroke(cr.get());

	cairo_restore(cr.get());
    }

    void Widget::draw_text(const std::string& text, const Color& color,
			   int align, int standoff, const Font& font)
    {
	auto cr = screen()->context();

	cairo_save(cr.get());

	cairo_text_extents_t textext;
	cairo_select_font_face(cr.get(),
			       font.face().c_str(),
			       CAIRO_FONT_SLANT_NORMAL,
			       CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr.get(), font.size());
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

	cairo_set_source_rgba(cr.get(),
			      color.redf(),
			      color.greenf(),
			      color.bluef(),
			      color.alphaf());
	cairo_move_to(cr.get(), p.x, p.y);
	cairo_show_text(cr.get(), text.c_str());
	cairo_stroke(cr.get());

	cairo_restore(cr.get());
    }

    void Widget::draw_basic_text(const std::string& text,
				 const Rect& rect,
				 const Color& color, int align, int standoff,
				 const Font& font)
    {
	auto cr = screen()->context();

	cairo_save(cr.get());

	cairo_text_extents_t textext;
	cairo_select_font_face(cr.get(),
			       font.face().c_str(),
			       CAIRO_FONT_SLANT_NORMAL,
			       CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr.get(), font.size());
	cairo_text_extents(cr.get(),text.c_str(), &textext);

	Point p;
	if (align & ALIGN_CENTER)
	{
	    p.x = rect.x + (rect.w/2) - (textext.width/2) + textext.x_bearing;
	    p.y = rect.y + (rect.h/2) + (textext.height/2);
	}

	if (align & ALIGN_LEFT)
	    p.x = rect.x + standoff;
	if (align & ALIGN_RIGHT)
	    p.x = rect.x + rect.w - textext.width - standoff;
	if (align & ALIGN_TOP)
	    p.y = rect.y + textext.height + standoff;
	if (align & ALIGN_BOTTOM)
	    p.y = rect.y + rect.h - standoff;


	cairo_set_source_rgba(cr.get(),
			      color.redf(),
			      color.greenf(),
			      color.bluef(),
			      color.alphaf());
	cairo_move_to(cr.get(), p.x, p.y);
	cairo_show_text(cr.get(), text.c_str());
	cairo_stroke(cr.get());

	cairo_restore(cr.get());
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

	//position(x, y);

	//size(cairo_image_surface_get_width(m_image.get()),
	//   cairo_image_surface_get_height(m_image.get()));
	m_box = Rect(x,y,
		     cairo_image_surface_get_width(m_image.get()),
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

	//damage();
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
	    if (!active())
	    {
		damage();
		active(true);
		return 1;
	    }
	    break;
	case EVT_MOUSE_UP:
	    if (active())
	    {
		damage();
		active(false);
		return 1;
	    }
	    break;
	case EVT_MOUSE_MOVE:
	    break;
	case EVT_KEYDOWN:
	    break;
	}

	return Widget::handle(event);
    }

    void Button::draw(const Rect& rect)
    {
	// box
	draw_gradient_box(box(), BORDER_COLOR, GLOW_COLOR, active());

	// text
	draw_text(m_label);
    }

    Button::~Button()
    {
    }


    ImageButton::ImageButton(const string& image,
			     const Point& point,
			     const Size& size)
	: Button("", point, size)
    {
	m_image = shared_cairo_surface_t(cairo_image_surface_create_from_png(image.c_str()),
					 cairo_surface_destroy);
	assert(m_image.get());
	assert(cairo_surface_status(m_image.get()) == CAIRO_STATUS_SUCCESS);

	//damage();
    }

    void ImageButton::draw(const Rect& rect)
    {
	// image
	draw_image(m_image, ALIGN_LEFT | ALIGN_CENTER);
    }

    ImageButton::~ImageButton()
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

    Slider::Slider(int min, int max, const Point& point,
		   const Size& size, int orientation)
	: Widget(point.x, point.y, size.w, size.h),
	  m_min(min),
	  m_max(max),
	  m_pos(min),
	  m_moving_x(0),
	  m_orientation(orientation)
    {
    }

    int Slider::handle(int event)
    {
	switch (event)
	{
	case EVT_MOUSE_DOWN:
	    if (m_orientation == ORIENTATION_HORIZONTAL)
		m_moving_x = mouse_position().x;
	    else
		m_moving_x = mouse_position().y;
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
		if (m_orientation == ORIENTATION_HORIZONTAL)
		{
		    int diff = mouse_position().x - m_moving_x;
		    position(m_start_pos + denormalize(diff));
		}
		else
		{
		    int diff = mouse_position().y - m_moving_x;
		    position(m_start_pos + denormalize(diff));
		}
	    }
	    break;
	}

	return Widget::handle(event);
    }

    void Slider::draw(const Rect& rect)
    {
	auto cr = screen()->context();

	cairo_save(cr.get());

	cairo_set_source_rgba(cr.get(),
			      BORDER_COLOR.redf(),
			      BORDER_COLOR.greenf(),
			      BORDER_COLOR.bluef(),
			      BORDER_COLOR.alphaf());
	cairo_set_line_width(cr.get(), 4.0);

	if (m_orientation == ORIENTATION_HORIZONTAL)
	{
	    int dim = h();

	    // line
	    cairo_move_to(cr.get(), x() + dim/2, y() + h()/2);
	    cairo_line_to(cr.get(), x() + w() - dim/2, y() + h()/2);
	    cairo_stroke(cr.get());

	    // handle
	    draw_gradient_box(Rect(x() + normalize(m_pos) + 1,
				   y() + 1,
				   h() - 2,
				   h() - 2));
	}
	else
	{
	    int dim = w();

	    // line
	    cairo_move_to(cr.get(), x() + w()/2, y() + dim/2);
	    cairo_line_to(cr.get(), x() + w()/2, y() + h() - dim/2);
	    cairo_stroke(cr.get());

	    // handle
	    draw_gradient_box(Rect(x() + 1,
				   y() + normalize(m_pos) + 2,
				   w() - 2,
				   w() - 2));
	}

	cairo_restore(cr.get());
    }

    Slider::~Slider()
    {
    }

    Label::Label(const std::string& text, const Point& point, const Size& size,
		 int align, const Font& font)
	: Widget(point.x, point.y, size.w, size.h),
	  m_align(align),
	  m_text(text),
	  m_font(font),
	  m_fgcolor(TEXT_COLOR)
    {

    }

    int Label::handle(int event)
    {
	return Widget::handle(event);
    }

    void Label::draw(const Rect& rect)
    {
	draw_text(m_text, m_fgcolor, m_align, 5, m_font);
    }

    Label::~Label()
    {
    }

    SimpleText::SimpleText(const std::string& text, const Point& point, const Size& size)
	: Widget(point.x, point.y, size.w, size.h),
	  m_text(text)
    {}

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
	// box
	draw_basic_box(box());

	// text
	draw_text(m_text, TEXT_COLOR, ALIGN_CENTER | ALIGN_LEFT);
    }

    SimpleText::~SimpleText()
    {
    }


    CheckBox::CheckBox(const std::string& text,
		       const Point& point,
		       const Size& size)
	: Label(text, point, size),
	  m_checked(false)
    {

    }

    int CheckBox::handle(int event)
    {
	switch (event)
	{
	case EVT_MOUSE_DOWN:
	    damage();
	    checked(!checked());
	    return 1;
	}

	return Widget::handle(event);
    }

    void CheckBox::draw(const Rect& rect)
    {
	static const int STANDOFF = 5;

	// image
	Rect r(x() + STANDOFF,
	       y() + STANDOFF,
	       h() - STANDOFF * 2,
	       h() - STANDOFF * 2);

	if (checked())
	{
	    draw_basic_box(r, BORDER_COLOR, Color::ORANGE);
#if 1
	    static const int OFFSET = 5;
	    auto cr = screen()->context();
	    cairo_move_to(cr.get(), r.x + OFFSET, r.y + OFFSET);
	    cairo_line_to(cr.get(), r.x + r.w - OFFSET, r.y + r.h - OFFSET);
	    cairo_move_to(cr.get(), r.x + r.w - OFFSET, r.y + OFFSET);
	    cairo_line_to(cr.get(), r.x + OFFSET, r.y + r.h - OFFSET);
	    cairo_set_line_width(cr.get(), 2.0);
	    cairo_stroke(cr.get());
#endif
	}
	else
	{
	    draw_gradient_box(r);
	}

	// text
	draw_text(m_text, TEXT_COLOR, ALIGN_LEFT | ALIGN_CENTER,
		  STANDOFF + h() - STANDOFF);
    }

    CheckBox::~CheckBox()
    {
    }

    ImageLabel::ImageLabel(const std::string& image,
			   const std::string& text,
			   const Point& point,
			   const Size& size,
			   const Font& font)
	: Label(text, point, size, ALIGN_CENTER, font)
    {
	m_image = shared_cairo_surface_t(cairo_image_surface_create_from_png(image.c_str()),
					 cairo_surface_destroy);
	assert(m_image.get());
	assert(cairo_surface_status(m_image.get()) == CAIRO_STATUS_SUCCESS);

	//damage();
    }

    void ImageLabel::draw(const Rect& rect)
    {
	// image
	draw_image(m_image, ALIGN_LEFT | ALIGN_CENTER);

	// text
	auto width = cairo_image_surface_get_width(m_image.get());
	draw_text(m_text, m_fgcolor, ALIGN_LEFT | ALIGN_CENTER, width + 5, m_font);
    }

    ImageLabel::~ImageLabel()
    {
    }

    ListBox::ListBox(const std::vector<string>& items,
		     const Point& point,
		     const Size& size)
	: Widget(point.x, point.y, size.w, size.h),
	  m_items(items),
	  m_selected(0)
    {

    }

    Rect ListBox::item_rect(uint32_t index) const
    {
	int height = 40;
	Rect r(box().x, box().y, box().w, height);
	r.y += (height * index);
	return r;
    }

    int ListBox::handle(int event)
    {
	switch (event)
	{
	case EVT_MOUSE_DOWN:
	{
	    for (uint32_t i = 0; i < m_items.size(); i++)
	    {
		if (Rect::point_inside(mouse_position(), item_rect(i)))
		{
		    if (m_selected != i)
		    {
			m_selected = i;
			damage();
			on_selected(i);
		    }

		    break;
		}
	    }

	    return 1;
	}
	}

	return Widget::handle(event);
    }

    void ListBox::draw(const Rect& rect)
    {
	for (uint32_t i = 0; i < m_items.size(); i++)
	{
	    if (i == m_selected)
	    {
		draw_basic_box(item_rect(i), BORDER_COLOR, Color::ORANGE);
	    }
	    else
	    {
		draw_gradient_box(item_rect(i));
	    }

	    draw_basic_text(m_items[i], item_rect(i), Color::BLACK, ALIGN_CENTER);
	}
    }

    void ListBox::selected(uint32_t index)
    {
	if (m_selected != index)
	{
	    m_selected = index;
	    damage();
	    on_selected(index);
	}
    }

    ListBox::~ListBox()
    {
    }


    Radial::Radial(const Point& point, const Size& size)
	: Widget(point.x, point.y, size.w, size.h),
	  m_angle(-1.0)
    {

    }

    int Radial::handle(int event)
    {
	switch (event)
	{
	case EVT_MOUSE_DOWN:
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
		Point c = center();
		m_angle = atan2(mouse_position().y - c.y,
				mouse_position().x - c.x);
		damage();

		return 1;
	    }
	    break;
	}

	return Widget::handle(event);
    }

    void Radial::draw(const Rect& rect)
    {
	float linew = 40;

	Color color1(Color::LIGHTGRAY);
	color1.alpha(0x55);
	Color color2(Color::ORANGE);
	if (m_angle > 0)
	    color2 = Color::GREEN;

	float radius = w() / 2 - (linew / 2);
	double angle1 = 0;// * (M_PI/180.0);
	double angle2 = m_angle;

	Point c = center();

	auto cr = screen()->context();

	cairo_save(cr.get());

	cairo_set_source_rgba(cr.get(),
			      color1.redf(),
			      color1.greenf(),
			      color1.bluef(),
			      color1.alphaf());
	cairo_set_line_width(cr.get(), linew);

	cairo_arc(cr.get(), c.x, c.y, radius, 0, 2 * M_PI);
	cairo_stroke(cr.get());

	cairo_set_source_rgb(cr.get(),
			     color2.redf(),
			     color2.greenf(),
			     color2.bluef());
	cairo_set_line_width(cr.get(), linew - (linew/3));
	cairo_arc_negative(cr.get(), c.x, c.y, radius, angle1, angle2);
	cairo_stroke(cr.get());

	string text = std::to_string((int)(angle2 / (M_PI/180.0))) + m_label;
	draw_text(text, color2, ALIGN_CENTER, 0, Font(72));

	cairo_restore(cr.get());
    }
}
