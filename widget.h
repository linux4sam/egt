/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef WIDGET_H
#define WIDGET_H

#include "geometry.h"
#include "input.h"
#include "screen.h"
#include "utils.h"
#include <cairo.h>
#include <cassert>
#include <map>
#include <string>
#include <vector>
#include <memory>

namespace mui
{

    /**
     * Base widget class.
     */
    class Widget
    {
    public:

	enum
	{
	    FLAG_NO_BORDER = (1<<0),
	    FLAG_FULL_REDRAW = (1<<1),
	};

	enum
	{
	    ALIGN_CENTER = 1<<0,
	    ALIGN_LEFT = 1<<1,
	    ALIGN_RIGHT = 1<<2,
	    ALIGN_TOP = 1<<3,
	    ALIGN_BOTTOM = 1<<4,
	};

	/**
	 * Construct a widget.
	 */
	Widget(int x = 0, int y = 0, int w = 0, int h = 0);

	/**
	 * Draw the widget.
	 *
	 * Do not call this directly.  When implementing this function, only
	 * re-draw within rect when possible.
	 */
	virtual void draw(const Rect& rect) = 0;

	/**
	 * Handle an event.
	 *
	 * Only the event ID is passed to this function. To get data associated
	 * with the event, you have to call other functions.
	 */
	virtual int handle(int event);

	/**
	 * Set the position of the widget.
	 */
	virtual void position(int x, int y);

	/**
	 * Set the size of the widget.
	 */
	virtual void size(int w, int h);

	/**
	 * Resize the widget.
	 * This will cause a redraw of the widget.
	 */
	virtual void resize(int w, int h);

	/**
	 * Move the widget.
	 * This will cause a redraw of the widget.
	 */
	virtual void move(int x, int y);

	virtual void hide() { m_visible = false; }
	virtual void show() { m_visible = true; damage(); }
	virtual bool visible() const { return m_visible; }
	virtual bool focus() const { return m_focus; }
	virtual void focus(bool value) { m_focus = value; }

	virtual bool active() const { return m_active; }
	virtual void active(bool value) { m_active = value; }

	virtual void damage();

	/**
	 * Bounding box for the widgets.
	 */
	const Rect& box() const { return m_box; }

	/**
	 * Bounding box width.
	 */
	inline int w() const { return m_box.w; }
	inline int h() const { return m_box.h; }
	inline int x() const { return m_box.x; }
	inline int y() const { return m_box.y; }

	virtual ~Widget();

    protected:


	void draw_text(const std::string& text,
		       const Color& color = Color::BLACK, int align = ALIGN_CENTER,
		       int standoff = 5);

	void draw_image(shared_cairo_surface_t image,
				int align = ALIGN_CENTER, int standoff = 0);

	Rect m_box;
	bool m_visible;
	bool m_focus;
	bool m_active;
    };

    using shared_cairo_surface_t =
			   std::shared_ptr<cairo_surface_t>;

    using shared_cairo_t =
			   std::shared_ptr<cairo_t>;

    class Image : public Widget
    {
    public:
	Image(const std::string& filename, int x = 0, int y = 0);

	virtual void draw(const Rect& rect);

	void scale(double scale);

	double scale() const { return m_scale; }

	virtual ~Image();

    protected:

	shared_cairo_surface_t m_image;
	shared_cairo_surface_t m_back;
	std::string m_filename;
	double m_scale;
	std::map<int,shared_cairo_surface_t> m_cache;
    };

    class Button : public Widget
    {
    public:
	Button(const std::string& label, const Point& point = Point(),
	       const Size& size = Size());

	int handle(int event);

	virtual void draw(const Rect& rect);

	virtual ~Button();

    protected:
	std::string m_label;
    };

    class Combo : public Widget
    {
    public:
	Combo(const std::string& label, const Point& point = Point(),
	      const Size& size = Size());

	int handle(int event);

	virtual void draw(const Rect& rect);

	virtual ~Combo();

    protected:
	std::string m_label;
    };


    class Slider : public Widget
    {
    public:
	Slider(int min, int max, const Point& point = Point(),
	       const Size& size = Size());

	int handle(int event);

	virtual void draw(const Rect& rect);

	int position() const { return m_pos; }

	inline void position(int pos)
	{
	    if (pos <= m_max && pos >= m_min)
	    {
		m_pos = pos;
		damage();
	    }
	}

	virtual ~Slider();

    protected:

	constexpr static int RADIUS = 10;

	inline int normalize(int pos)
	{
	    return float(w() - RADIUS * 4) / float(m_max - m_min)  * float(pos);
	}

        inline int denormalize(int diff)
	{
	    return float(m_max - m_min)  / float(w() - RADIUS * 4) * float(diff);
	}

	int m_min;
	int m_max;
	int m_pos;
	int m_moving_x;
	int m_start_pos;
    };

    class Label : public Widget
    {
    public:
	Label(const std::string& text, const Point& point = Point(), const Size& size = Size(), int align = ALIGN_CENTER);

	int handle(int event);

	virtual void draw(const Rect& rect);

	virtual ~Label();

    protected:
	int m_align;
	std::string m_text;
    };

    class SimpleText : public Widget
    {
    public:
	SimpleText(const std::string& text = std::string(), const Point& point = Point(), const Size& size = Size());

	int handle(int event);

	virtual void draw(const Rect& rect);

	virtual ~SimpleText();

    protected:
	std::string m_text;
    };


    class CheckBox : public Widget
    {
    public:
	CheckBox(const std::string& text = std::string(),
		 const Point& point = Point(),
		 const Size& size = Size());

	int handle(int event);

	virtual void draw(const Rect& rect);

	virtual ~CheckBox();
    };

    class ImageLabel : public Label
    {
    public:
	ImageLabel(const std::string& image,
		   const std::string& text = std::string(),
		   const Point& point = Point(),
		   const Size& size = Size());

	virtual void draw(const Rect& rect);

	virtual ~ImageLabel();
    protected:
	shared_cairo_surface_t m_image;
    };
}

#endif
