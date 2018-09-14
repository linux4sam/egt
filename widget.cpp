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
#include "painter.h"
#include "resource.h"
#include "imagecache.h"
#include <sstream>

using namespace std;

namespace mui
{
    Widget::Widget(int x, int y, int w, int h, uint32_t flags)
	: m_box(x, y, w, h),
	  m_visible(true),
	  m_focus(false),
	  m_active(false),
	  m_disabled(false),
	  m_parent(0),
	  m_flags(flags)
    {}

    int Widget::handle(int event)
    {
	// do nothing
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
	    corner_radius = 50/*height*/ / 10.0;

	double radius = corner_radius / aspect;
	double degrees = M_PI / 180.0;

	cairo_new_sub_path (cr.get());
	cairo_arc(cr.get(), rx + width - radius, ry + radius, radius, -90 * degrees, 0 * degrees);
	cairo_arc(cr.get(), rx + width - radius, ry + height - radius, radius, 0 * degrees, 90 * degrees);
	cairo_arc(cr.get(), rx + radius, ry + height - radius, radius, 90 * degrees, 180 * degrees);
	cairo_arc(cr.get(), rx + radius, ry + radius, radius, 180 * degrees, 270 * degrees);
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
	cairo_set_line_width(cr.get(), 1.0);
	cairo_stroke(cr.get());

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
	    corner_radius = 50/*height*/ / 10.0;

	double radius = corner_radius / aspect;
	double degrees = M_PI / 180.0;

	cairo_new_sub_path (cr.get());
	cairo_arc(cr.get(), rx + width - radius, ry + radius, radius, -90 * degrees, 0 * degrees);
	cairo_arc(cr.get(), rx + width - radius, ry + height - radius, radius, 0 * degrees, 90 * degrees);
	cairo_arc(cr.get(), rx + radius, ry + height - radius, radius, 90 * degrees, 180 * degrees);
	cairo_arc(cr.get(), rx + radius, ry + radius, radius, 180 * degrees, 270 * degrees);
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

    void Widget::draw_text(const std::string& text, const Rect& rect,
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

	Rect target = align_algorithm(Size(textext.width, textext.height),
				      rect,
				      align,
				      standoff);

	cairo_set_source_rgba(cr.get(),
			      color.redf(),
			      color.greenf(),
			      color.bluef(),
			      color.alphaf());
	cairo_move_to(cr.get(), target.x + textext.x_bearing, target.y - textext.y_bearing);
	cairo_show_text(cr.get(), text.c_str());
	cairo_stroke(cr.get());

	cairo_restore(cr.get());
    }

    void Widget::draw_image(shared_cairo_surface_t image, int align, int standoff, bool bw)
    {
	auto width = cairo_image_surface_get_width(image.get());
	auto height = cairo_image_surface_get_height(image.get());

	Rect target = align_algorithm(Size(width, height), box(), align, standoff);

	Painter painter(screen()->context());
#if 1
	painter.draw_image(target.point(), image, bw);
#else
	painter.paint_surface_with_drop_shadow(
	  image.get(),
	  5,
	  0.2,
	  0.4,
	  target.x,
	  target.y,
	  width,
	  height,
	  target.x,
	  target.y);
#endif
    }

    Widget::~Widget()
    {}

    Image::Image(const string& filename, int x, int y)
	: m_filename(filename),
	  m_hscale(1.0),
	  m_vscale(1.0)
    {
	m_image = image_cache.get(filename, 1.0);
	assert(m_image.get());

	m_box = Rect(x,y,
		     cairo_image_surface_get_width(m_image.get()),
		     cairo_image_surface_get_height(m_image.get()));
    }

    void Image::scale(double hscale, double vscale)
    {
	if (m_hscale != hscale || m_vscale != vscale)
	{
	    damage();

	    m_image = image_cache.get(m_filename, hscale, vscale, false);

	    size(cairo_image_surface_get_width(m_image.get()),
		 cairo_image_surface_get_height(m_image.get()));
	    m_hscale = hscale;
	    m_vscale = vscale;

	    damage();
	}
    }

    void Image::draw(const Rect& rect)
    {
	// TODO: this needs to apply to all draw() calls.  Don't give a widget a
	// rectangle that is outside of its own box.
	//Rect r = Rect::intersect(rect, box());

	Painter painter(screen()->context());
	//painter.draw_image(Rect(0,0,w(),h()), box().point(), m_image);
	painter.draw_image(Rect(rect.point() - box().point(),rect.size()), rect.point(), m_image);
	//screen()->blit(m_image.get(), rect.x, rect.y, rect.w, rect.h, box().x, box().y);
    }

    Image::~Image()
    {

    }

    Button::Button(const string& label, const Point& point, const Size& size)
	: Widget(point.x, point.y, size.w, size.h),
	  m_label(label)
    {}

    int Button::handle(int event)
    {
	switch (event)
	{
	case EVT_MOUSE_DOWN:
	    if (!active())
	    {
		damage();
		active(true);
		invoke_handlers();
		return 1;
	    }
	    break;
	case EVT_MOUSE_UP:
	    if (active())
	    {
		damage();
		active(false);
		invoke_handlers();
		return 1;
	    }
	    break;
	case EVT_MOUSE_MOVE:
	    break;
	case EVT_KEY_DOWN:
	    break;
	}

	return Widget::handle(event);
    }

    void Button::draw(const Rect& rect)
    {
	// box
	draw_gradient_box(box(), BORDER_COLOR, GLOW_COLOR, active());

	// text
	draw_text(m_label, box());
    }

    Button::~Button()
    {
    }

    ImageButton::ImageButton(const string& image,
			     const string& label,
			     const Point& point,
			     const Size& size,
			     bool border)
	: Button(label, point, size),
	  m_fgcolor(TEXT_COLOR),
	  m_border(border),
	  m_image_align(ALIGN_CENTER),
	  m_label_align(ALIGN_CENTER | ALIGN_BOTTOM)
    {
	if (!image.empty())
	    set_image(image);
    }

    void ImageButton::set_image(const std::string& image)
    {
	m_image = image_cache.get(image, 1.0);
	assert(m_image.get());
	assert(cairo_surface_status(m_image.get()) == CAIRO_STATUS_SUCCESS);

	auto width = cairo_image_surface_get_width(m_image.get());
	auto height = cairo_image_surface_get_height(m_image.get());
	//resize(width,height);
	m_box.w = width;
	m_box.h = height;
    }

    void ImageButton::draw(const Rect& rect)
    {
	if (m_border)
	    draw_gradient_box(box(), BORDER_COLOR, GLOW_COLOR);

	// image
	draw_image(m_image, m_image_align, 10, disabled());

	if (!m_label.empty())
	    draw_text(m_label, box(), m_fgcolor,
			    m_label_align, 10,
			    m_font);
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
	    corner_radius = 50/*height*/ / 10.0;

	double radius = corner_radius / aspect;
	double degrees = M_PI / 180.0;

	cairo_new_sub_path (cr.get());
	cairo_arc(cr.get(), rx + width - radius, ry + radius, radius, -90 * degrees, 0 * degrees);
	cairo_arc(cr.get(), rx + width - radius, ry + height - radius, radius, 0 * degrees, 90 * degrees);
	cairo_arc(cr.get(), rx + radius, ry + height - radius, radius, 90 * degrees, 180 * degrees);
	cairo_arc(cr.get(), rx + radius, ry + radius, radius, 180 * degrees, 270 * degrees);
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
	draw_text(m_label, box(), TEXT_COLOR, ALIGN_LEFT|ALIGN_CENTER);

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
	{
	    Rect bounding;

	    if (m_orientation == ORIENTATION_HORIZONTAL)
	    {
		bounding = Rect(x() + normalize(m_pos) + 1,
				y() + 1,
				h() - 2,
				h() - 2);
	    }
	    else
	    {
		bounding = Rect(x() + 1,
				y() + normalize(m_pos) + 1,
				w() - 2,
				w() - 2);
	    }

	    if (Rect::point_inside(screen_to_window(mouse_position()), bounding))
	    {
		if (m_orientation == ORIENTATION_HORIZONTAL)
		    m_moving_x = screen_to_window(mouse_position()).x;
		else
		    m_moving_x = screen_to_window(mouse_position()).y;
		m_start_pos = position();
		active(true);
		return 1;
	    }

	    break;
	}
	case EVT_MOUSE_UP:
	    active(false);
	    return 1;
	case EVT_MOUSE_MOVE:
	    if (active())
	    {
		if (m_orientation == ORIENTATION_HORIZONTAL)
		{
		    int diff = screen_to_window(mouse_position()).x - m_moving_x;
		    position(m_start_pos + denormalize(diff));
		}
		else
		{
		    int diff = screen_to_window(mouse_position()).y - m_moving_x;
		    position(m_start_pos + denormalize(diff));
		}
		return 1;
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
			      palette().color(Palette::HIGHLIGHT).redf(),
			      palette().color(Palette::HIGHLIGHT).greenf(),
			      palette().color(Palette::HIGHLIGHT).bluef(),
			      palette().color(Palette::HIGHLIGHT).alphaf());

	if (m_orientation == ORIENTATION_HORIZONTAL)
	{
	    cairo_set_line_width(cr.get(), h()/5.0);

	    // line
	    cairo_move_to(cr.get(), x(), y() + h()/2);
	    cairo_line_to(cr.get(), x() + normalize(m_pos), y() + h()/2);
	    cairo_stroke(cr.get());

	    cairo_set_source_rgba(cr.get(),
				  BORDER_COLOR.redf(),
				  BORDER_COLOR.greenf(),
				  BORDER_COLOR.bluef(),
				  BORDER_COLOR.alphaf());

	    cairo_move_to(cr.get(), x() + normalize(m_pos) + 1, y() + h()/2);
	    cairo_line_to(cr.get(), x() + w(), y() + h()/2);
	    cairo_stroke(cr.get());

	    // handle
	    draw_gradient_box(Rect(x() + normalize(m_pos)+ 1,
				   y() + 1,
				   h() - 2,
				   h() - 2));
	}
	else
	{
	    cairo_set_line_width(cr.get(), w()/5.0);

	    // line
	    cairo_move_to(cr.get(), x() + w()/2, y() + h());
	    cairo_line_to(cr.get(), x() + w()/2, y() + normalize(m_pos));
	    cairo_stroke(cr.get());

	    cairo_set_source_rgba(cr.get(),
				  BORDER_COLOR.redf(),
				  BORDER_COLOR.greenf(),
				  BORDER_COLOR.bluef(),
				  BORDER_COLOR.alphaf());

	    cairo_move_to(cr.get(), x() + w()/2, y() + normalize(m_pos) + 1);
	    cairo_line_to(cr.get(), x() + w()/2, y());
	    cairo_stroke(cr.get());

	    // handle
	    draw_gradient_box(Rect(x() + 1,
				   y() + normalize(m_pos) + 1,
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

    void Label::text(const std::string& str)
    {
	if (m_text != str)
	{
	    m_text = str;
	    damage();
	}
    }

    void Label::draw(const Rect& rect)
    {
	draw_text(m_text, box(), m_fgcolor, m_align, 5, m_font);
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
	case EVT_KEY_DOWN:
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
	draw_text(m_text, box(), TEXT_COLOR, ALIGN_CENTER | ALIGN_LEFT);
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
	    draw_basic_box(r, palette().color(Palette::BORDER),
			   palette().color(Palette::HIGHLIGHT));

	    static const int OFFSET = 5;
	    auto cr = screen()->context();

	    cairo_set_source_rgba(cr.get(),
				  palette().color(Palette::DARK).redf(),
				  palette().color(Palette::DARK).greenf(),
				  palette().color(Palette::DARK).bluef(),
				  palette().color(Palette::DARK).alphaf());

	    cairo_move_to(cr.get(), r.x + OFFSET, r.y + OFFSET);
	    cairo_line_to(cr.get(), r.x + r.w - OFFSET, r.y + r.h - OFFSET);
	    cairo_move_to(cr.get(), r.x + r.w - OFFSET, r.y + OFFSET);
	    cairo_line_to(cr.get(), r.x + OFFSET, r.y + r.h - OFFSET);
	    cairo_set_line_width(cr.get(), 2.0);
	    cairo_stroke(cr.get());
	}
	else
	{
	    draw_gradient_box(r);
	}

	// text
	draw_text(m_text,
		  box(),
		  palette().color(Palette::TEXT),
		  ALIGN_LEFT | ALIGN_CENTER,
		  h());
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
	m_image = image_cache.get(image, 1.0);
	assert(m_image.get());
	assert(cairo_surface_status(m_image.get()) == CAIRO_STATUS_SUCCESS);
    }

    void ImageLabel::draw(const Rect& rect)
    {
	// image
	draw_image(m_image, ALIGN_LEFT | ALIGN_CENTER);

	// text
	auto width = cairo_image_surface_get_width(m_image.get());
	draw_text(m_text, box(), m_fgcolor, ALIGN_LEFT | ALIGN_CENTER, width + 5, m_font);
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
		if (Rect::point_inside(screen_to_window(mouse_position()), item_rect(i)))
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
#if 0
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

	    draw_text(m_items[i], item_rect(i), Color::BLACK, ALIGN_CENTER);
	}
#else
	auto cr = screen()->context();

	Font font;
	cairo_text_extents_t textext;
	cairo_select_font_face(cr.get(),
			       font.face().c_str(),
			       CAIRO_FONT_SLANT_NORMAL,
			       CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr.get(), font.size());
	cairo_text_extents(cr.get(),"a", &textext);

	draw_basic_box(Rect(x(), y(), w(), 40 * m_items.size()));

	if (!m_items.empty())
	{
	    draw_basic_box(item_rect(m_selected), BORDER_COLOR, Color::ORANGE);

	    for (uint32_t i = 0; i < m_items.size(); i++)
	    {
		draw_text(m_items[i], item_rect(i), Color::BLACK, ALIGN_CENTER);
	    }
	}
#endif
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
	: ValueWidget<float>(point, size, -1.0)
    {}

    static int norm(float angle)
    {
	int a = (int)(angle / (M_PI/180.0));
	if (a > 180)
	    a -= 180;
	else
	    a+= 180;
	if (a == 360)
	    a = 0;
	return a;
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
		float angle = atan2f(screen_to_window(mouse_position()).y - c.y,
				     screen_to_window(mouse_position()).x - c.x);
		angle = angle * (180.0 / M_PI);
		angle = (angle > 0.0 ? angle : (360.0 + angle));
		angle = 180 - angle;

		value(angle);
		return 1;
	    }
	    break;
	}

	return Widget::handle(event);
    }

    static float radians(float zero, float degrees)
    {
	degrees = 360 - degrees - zero;
	return degrees * (M_PI/180.0);
    }

    void Radial::draw(const Rect& rect)
    {
	float linew = 40;

	Color color1(Color::LIGHTGRAY);
	color1.alpha(0x55);

	float radius = w() / 2 - (linew / 2);
	double angle1 = radians(180, 0);
	double angle2 = radians(180, value());

	int a = norm(angle2);

	Color color2(Color::OLIVE);
	if (a > 180)
	    color2 = Color::ORANGE;

	Point c = center();

	auto cr = screen()->context();

	cairo_save(cr.get());

	// bottom full circle
	cairo_set_source_rgba(cr.get(),
			      color1.redf(),
			      color1.greenf(),
			      color1.bluef(),
			      color1.alphaf());
	cairo_set_line_width(cr.get(), linew);

	cairo_arc(cr.get(), c.x, c.y, radius, 0, 2 * M_PI);
	cairo_stroke(cr.get());

	// top position arc
	cairo_set_source_rgb(cr.get(),
			     color2.redf(),
			     color2.greenf(),
			     color2.bluef());
	cairo_set_line_width(cr.get(), linew - (linew/3));
	cairo_arc(cr.get(), c.x, c.y, radius, angle1, angle2);
	cairo_stroke(cr.get());

	string text = std::to_string(a) + m_label;
	draw_text(text, box(), color2, ALIGN_CENTER, 0, Font(72));

	cairo_restore(cr.get());
    }

    ProgressBar::ProgressBar(const Point& point, const Size& size)
	: ValueRangeWidget<int>(point, size, 0, 100, 0)
    {
    }

    void ProgressBar::draw(const Rect& rect)
    {
	auto cr = screen()->context();

	cairo_save(cr.get());

	Color bg = palette().color(Palette::BG);
	cairo_set_source_rgba(cr.get(),
			      bg.redf(),
			      bg.greenf(),
			      bg.bluef(),
			      bg.alphaf());
	cairo_rectangle(cr.get(), x(), y(), w(), h());
	cairo_fill(cr.get());

	Color color = palette().color(Palette::HIGHLIGHT);
	cairo_set_source_rgb(cr.get(), color.redf(), color.greenf(), color.bluef());
	cairo_rectangle(cr.get(), x(),  y(), w() * value() / 100., h());
	cairo_fill(cr.get());

	cairo_rectangle(cr.get(), x(), y(), w(), h());
	Color border = palette().color(Palette::BORDER);
	cairo_set_source_rgba(cr.get(),
			      border.redf(),
			      border.greenf(),
			      border.bluef(),
			      border.alphaf());
	cairo_set_line_width(cr.get(), 1.0);
	cairo_stroke(cr.get());

	cairo_restore(cr.get());
    }

    LevelMeter::LevelMeter(const Point& point, const Size& size)
	: ValueRangeWidget<int>(point, size, 0, 100, 0)
    {
    }

    void LevelMeter::draw(const Rect& rect)
    {
	auto cr = screen()->context();

	cairo_save(cr.get());

	int limit = 20 - value() / 5;
	int barheight = h() / 20;

	for (int i = 0; i < 20; i++)
	{
	    if (i > limit)
		cairo_set_source_rgb(cr.get(), 0.6, 1.0, 0);
	    else
		cairo_set_source_rgb(cr.get(), 0.2, 0.4, 0);

	    cairo_rectangle(cr.get(), x(),  y() + i * barheight, w(), barheight - 2);
	    cairo_fill(cr.get());
	}

	cairo_restore(cr.get());
    }

    AnalogMeter::AnalogMeter(const Point& point, const Size& size)
	: ValueRangeWidget<int>(point, size, 0, 100, 0)
    {
    }

    void AnalogMeter::draw(const Rect& rect)
    {
	auto cr = screen()->context();

	cairo_save(cr.get());

	cairo_set_source_rgb(cr.get(), 0, 0, 0);
	cairo_set_line_width(cr.get(), 1.0);

        // Set origin as middle of bottom edge of the drawing area.
	// Window is not resizable so "magic numbers" will work here.
	cairo_translate(cr.get(),x()+w()/2,y()+h());

	float hw = w() / 2.0 - 40.0;

	// Draw the black radial scale marks and labels
	for(double marks = 0.0; marks <= 100.0; marks += 10.0)
	{
	    // Draw the radial marks
	    cairo_move_to(cr.get(),
			  hw * cos(M_PI * marks * 0.01),
			  -hw * sin(M_PI * marks * 0.01) );
	    cairo_line_to(cr.get(),
			  (hw + 10.0) * cos(M_PI * marks * 0.01),
			  -(hw + 10.0) * sin(M_PI * marks * 0.01) );

	    // Set the text to print
	    char text[10];
	    sprintf(text,"%2.0f", marks);

	    cairo_text_extents_t textext;
	    cairo_select_font_face(cr.get(), "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	    cairo_set_font_size(cr.get(), 16);
	    cairo_text_extents(cr.get(),text, &textext);

	    int width = textext.width;
	    int height = textext.height;
	    // Position the text at the end of the radial marks
	    cairo_move_to(cr.get(),
			  (-(hw + 30.0) * cos(M_PI * marks * 0.01)) - ((double)width/2.0),
			  (-(hw + 30.0) * sin(M_PI * marks * 0.01)) + ((double)height/2.0));

	    cairo_show_text(cr.get(), text);
	}
	cairo_stroke(cr.get());

	// Retrieve the new slider value
	float value = this->value();

	cairo_set_source_rgb(cr.get(), 1.0, 0, 0);
	cairo_set_line_width(cr.get(), 1.5);

	// Draw the meter pointer
	cairo_move_to(cr.get(), 0.0,0.0);
	cairo_line_to(cr.get(),
		      -hw * cos(M_PI *value * 0.01),
		      -hw * sin(M_PI *value * 0.01));
	cairo_stroke(cr.get());

	cairo_restore(cr.get());
    }

    SpinProgress::SpinProgress(const Point& point, const Size& size)
	: ValueRangeWidget<int>(point, size, 0, 100, 0)
    {
    }

    void SpinProgress::draw(const Rect& rect)
    {
	float linew = 5;

	Color color2(Color::ORANGE);

	float radius = w() / 2 - (linew / 2);
	double angle1 = radians(180, 0);
	double angle2 = radians(180, value() / 100. * 360.);

	Point c = center();

	auto cr = screen()->context();

	cairo_save(cr.get());

	cairo_set_source_rgba(cr.get(), 0, 0, 0, 0);
	cairo_rectangle(cr.get(), x(),  y(), w(), h());
	cairo_fill(cr.get());

	cairo_set_source_rgb(cr.get(),
			     color2.redf(),
			     color2.greenf(),
			     color2.bluef());
	cairo_set_line_width(cr.get(), linew - (linew/3));
	cairo_arc(cr.get(), c.x, c.y, radius, angle1, angle2);
	cairo_stroke(cr.get());

	cairo_restore(cr.get());
    }

#ifdef DEVELOPMENT
    ScrollWheel::ScrollWheel(const Point& point, const Size& size)
	: Widget(point.x, point.y, size.w, size.h),
	  m_pos(0),
	  m_moving_x(0)
    {}

    int ScrollWheel::handle(int event)
    {
	switch (event)
	{
	case EVT_MOUSE_DOWN:
	    m_moving_x = screen_to_window(mouse_position()).y;
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
		int diff = screen_to_window(mouse_position()).y - m_moving_x;
		position(m_start_pos + diff);
	    }
	    break;
	}

	return Widget::handle(event);
    }

    void ScrollWheel::draw(const Rect& rect)
    {
	Color border(Color::BLACK);
	Color glass(0x00115555);
	Color color(0x4169E1ff);

	auto cr = screen()->context();

	cairo_save(cr.get());

	cairo_text_extents_t textext;
	cairo_select_font_face(cr.get(), "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr.get(), 16);
	cairo_text_extents(cr.get(),"a", &textext);

	cairo_set_source_rgb(cr.get(), border.redf(), border.greenf(), border.bluef());
	cairo_set_line_width(cr.get(), 3.0);
	cairo_move_to(cr.get(), x(), y());
	cairo_line_to(cr.get(), x(), y()+h());
	cairo_move_to(cr.get(), x()+w(), y());
	cairo_line_to(cr.get(), x()+w(), y()+h());
	cairo_stroke(cr.get());

	cairo_set_source_rgba(cr.get(), glass.redf(), glass.greenf(), glass.bluef(), glass.alphaf());
	cairo_rectangle(cr.get(), x(),  y() - textext.height + (1 * h()/3), w(), (1 * h()/3));
	cairo_fill (cr.get());

	cairo_pattern_t *pat;
	pat = cairo_pattern_create_linear(x(), y(), x(), y()+h()/2);

	Color step = Color(Color::GRAY);
	cairo_pattern_add_color_stop_rgb(pat, 0, step.redf(), step.greenf(), step.bluef());
	step = Color(Color::WHITE);
	cairo_pattern_add_color_stop_rgb(pat, 0.5, step.redf(), step.greenf(), step.bluef());
	step = Color(Color::GRAY);
	cairo_pattern_add_color_stop_rgb(pat, 1.0, step.redf(), step.greenf(), step.bluef());

	cairo_set_source(cr.get(), pat);

	int offset = y() + textext.height;
	for (int index = position();
	     index < (int)m_values.size() && index < position()+3; index++)
	{
	    cairo_move_to(cr.get(), x(), offset);
	    cairo_show_text(cr.get(), m_values[index].c_str());

	    offset += h()/3;//textext.height + 10;
	}

	//cairo_stroke(cr);
	cairo_pattern_destroy(pat);

	cairo_restore(cr.get());
    }
#endif
}
