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
#include "font.h"
#include <cairo.h>
#include <cassert>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <iostream>

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

	/**
	 * Damage the box() of the widget.
	 */
	virtual void damage();

	virtual void damage(const Rect& rect) {}

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

	inline Point center() const
	{
	    return Point(x() + w()/2, y() + h()/2);
	}

	virtual ~Widget();

    protected:

	Widget* parent()
	{
	    if (!m_parent)
	    {
		std::cout << "bad parent pointer" << std::endl;
	    	while(1);
	    }
	    assert(m_parent);
	    return m_parent;
	}
	virtual IScreen* screen() { return parent()->screen(); }

	void draw_text(const std::string& text,
		       const Color& color = Color::BLACK,
		       int align = ALIGN_CENTER,
		       int standoff = 5,
		       const Font& font = Font());
	void draw_basic_text(const std::string& text,
			     const Rect& rect,
			     const Color& color = Color::BLACK,
			     int align = ALIGN_CENTER,
			     int standoff = 5,
			     const Font& font = Font());

	void draw_image(shared_cairo_surface_t image,
				int align = ALIGN_CENTER, int standoff = 0);
	void draw_basic_box(const Rect& rect,
			    const Color& border = BORDER_COLOR,
			    const Color& bg = GLOW_COLOR);
	void draw_gradient_box(const Rect& rect,
			       const Color& border = BORDER_COLOR,
			       const Color& bg = GLOW_COLOR,
			       bool active = false);

	Rect m_box;
	bool m_visible;
	bool m_focus;
	bool m_active;
	Widget* m_parent;

	friend class SimpleWindow;
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

    class ImageButton : public Button
    {
    public:
	ImageButton(const std::string& image,
	       const Point& point = Point(),
	       const Size& size = Size());

	virtual void draw(const Rect& rect);

	virtual ~ImageButton();

    protected:
	shared_cairo_surface_t m_image;
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
	enum
	{
	    ORIENTATION_HORIZONTAL,
	    ORIENTATION_VERTICAL,
	};

	Slider(int min, int max, const Point& point = Point(),
	       const Size& size = Size(),
	       int orientation = ORIENTATION_HORIZONTAL);

	int handle(int event);

	virtual void draw(const Rect& rect);

	int position() const { return m_pos; }

	inline void position(int pos)
	{
	    if (pos <= m_max && pos >= m_min && pos != m_pos)
	    {
		m_pos = pos;
		damage();
	    }
	}

	virtual ~Slider();

    protected:

	inline int normalize(int pos)
	{
	    if (m_orientation == ORIENTATION_HORIZONTAL)
	    {
		int dim = h();
		return float(w() - dim) / float(m_max - m_min)  * float(pos);
	    }
	    else
	    {
		int dim = w();
		return float(h() - dim) / float(m_max - m_min)  * float(pos);
	    }
	}

        inline int denormalize(int diff)
	{
	    if (m_orientation == ORIENTATION_HORIZONTAL)
	    {
		int dim = h();
		return float(m_max - m_min) / float(w() - dim) * float(diff);
	    }
	    else
	    {
		int dim = w();
		return float(m_max - m_min) / float(h() - dim) * float(diff);
	    }
	}

	int m_min;
	int m_max;
	int m_pos;
	int m_moving_x;
	int m_start_pos;
	int m_orientation;
    };

    class Label : public Widget
    {
    public:
	Label(const std::string& text, const Point& point = Point(),
	      const Size& size = Size(), int align = ALIGN_CENTER,
	      const Font& font = Font());

	int handle(int event);

	void fgcolor(const Color& color) { m_fgcolor = color; }

	virtual void draw(const Rect& rect);

	virtual ~Label();

    protected:
	int m_align;
	std::string m_text;
	Font m_font;
	Color m_fgcolor;
    };

    class SimpleText : public Widget
    {
    public:
	SimpleText(const std::string& text = std::string(),
		   const Point& point = Point(), const Size& size = Size());

	int handle(int event);

	virtual void draw(const Rect& rect);

	virtual ~SimpleText();

    protected:
	std::string m_text;
    };


    class CheckBox : public Label
    {
    public:
	CheckBox(const std::string& text = std::string(),
		 const Point& point = Point(),
		 const Size& size = Size());

	bool checked() const { return m_checked; }
	void checked(bool c) { m_checked = c; }

	int handle(int event);

	virtual void draw(const Rect& rect);

	virtual ~CheckBox();
    protected:
	bool m_checked;
    };

    class ImageLabel : public Label
    {
    public:
	ImageLabel(const std::string& image,
		   const std::string& text = std::string(),
		   const Point& point = Point(),
		   const Size& size = Size(),
		   const Font& font = Font());

	virtual void draw(const Rect& rect);

	virtual ~ImageLabel();
    protected:
	shared_cairo_surface_t m_image;
    };

    class ListBox : public Widget
    {
    public:
	ListBox(const std::vector<std::string>& items,
		 const Point& point = Point(),
		 const Size& size = Size());

	virtual int handle(int event);

	virtual void draw(const Rect& rect);

	void selected(uint32_t index);
	uint32_t selected() const { return m_selected; }

	virtual ~ListBox();
    protected:

	virtual void on_selected(int index) {}

	Rect item_rect(uint32_t index) const;

	std::vector<std::string> m_items;
	uint32_t m_selected;
    };

    class Radial : public Widget
    {
    public:
	Radial(const Point&, const Size& size);

	void label(const std::string& label) { m_label = label; }

	virtual int handle(int event);

	virtual void draw(const Rect& rect);

    protected:
	std::string m_label;
	float m_angle;
    };
}

#endif
