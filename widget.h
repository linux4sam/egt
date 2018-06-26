/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef WIDGET_H
#define WIDGET_H

#include "font.h"
#include "geometry.h"
#include "input.h"
#include "palette.h"
#include "screen.h"
#include "utils.h"
#include <cairo.h>
#include <cassert>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace mui
{
    enum
    {
	/**
	 * Do not draw the background color.
	 *
	 * The background color is usually drawn by default. The background will
	 * essentially be transparent.
	 */
	FLAG_NO_BACKGROUND = (1<<0),

	/**
	 * This is an overlay plane window.
	 */
	FLAG_PLANE_WINDOW = (1<<1),

	/**
	 * Don't draw any border.
	 */
	FLAG_NO_BORDER = (1<<2),

	/**
	 * Widget always requires a full redraw.
	 */
	FLAG_FULL_REDRAW = (1<<3),

	/**
	 * This is a window widget.
	 */
	FLAG_WINDOW = (1<<4),

	/**
	 * Default window flags.
	 */
	FLAG_WINDOW_DEFAULT = FLAG_WINDOW,
    };

    /**
     * Base widget class.
     */
    class Widget
    {
    public:

	enum
	{
	    /**
	     * Center alignment is a weak alignment both horizontal and
	     * vertical. To break one of those dimensions to another
	     * alignment, specify it in addiiton to ALIGN_CENTER.  If both
	     * are broken, ALIGN_CENTER has no effect.
	     */
	    ALIGN_CENTER = (1<<0),
	    ALIGN_LEFT = (1<<1),
	    ALIGN_RIGHT = (1<<2),
	    ALIGN_TOP = (1<<3),
	    ALIGN_BOTTOM = (1<<4),
	};

	/**
	 * Construct a widget.
	 */
	Widget(int x = 0, int y = 0, int w = 0, int h = 0, uint32_t flags = 0);

	/**
	 * Draw the widget.
	 *
	 * @note Do not call this directly.
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
	inline void position(const Point& point) { position(point.x, point.y); }

	/**
	 * Set the size of the widget.
	 */
	virtual void size(int w, int h);
	inline void size(const Size& s) { size(s.w, s.h); }

	/**
	 * Resize the widget.
	 * This will cause a redraw of the widget.
	 */
	virtual void resize(int w, int h);
	inline void resize(const Size& s) { resize(s.w, s.h); }

	/**
	 * Move the widget.
	 * This will cause a redraw of the widget.
	 */
	virtual void move(int x, int y);
	inline void move(const Point& point) { move(point.x, point.y); }

	/**
	 * Hide the widget.  This will prevent draw() calls.
	 */
	virtual void hide() { m_visible = false; }

	/**
	 * Show the widget.  This will allow draw() calls.
	 */
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

	Size size() const { return m_box.size(); }

	/*
	 * Bounding box dimensions.
	 */
	inline int w() const { return m_box.w; }
	inline int h() const { return m_box.h; }
	inline int x() const { return m_box.x; }
	inline int y() const { return m_box.y; }

	inline Point center() const
	{
	    return Point(x() + w()/2, y() + h()/2);
	}

	const Palette& palette() const
	{
	    if (m_palette.get())
		return *m_palette.get();

	    return global_palette();
	}

	void set_palette(const Palette& palette)
	{
	    m_palette.reset(new Palette(palette));
	}

	virtual ~Widget();

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

	inline bool is_flag_set(uint32_t flag) const
	{
	    return (m_flags & flag) == flag;
	}

	inline bool flag_set(uint32_t flag)
	{
	    return m_flags |= flag;
	}

	inline bool flag_clear(uint32_t flag)
	{
	    return m_flags &= ~flag;
	}

    protected:

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
	std::shared_ptr<Palette> m_palette;
	uint32_t m_flags;

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
		    const std::string& label = "",
		    const Point& point = Point(),
		    const Size& size = Size(),
		    bool border = true);

	virtual void draw(const Rect& rect);

	void fgcolor(const Color& color) { m_fgcolor = color; }

	void font(const Font& font) { m_font = font; }

	virtual ~ImageButton();

    protected:
	shared_cairo_surface_t m_image;
	Font m_font;
	Color m_fgcolor;
	bool m_border;
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
	    if (pos > m_max)
	    {
		m_pos = m_max;
		damage();
	    }
	    else if (pos < m_min)
	    {
		m_pos = m_min;
		damage();
	    }
	    else if (pos != m_pos)
	    {
		m_pos = pos;
		damage();
	    }
	}

	virtual ~Slider();

    protected:

	// position to offset
	inline int normalize(int pos)
	{
	    if (m_orientation == ORIENTATION_HORIZONTAL)
	    {
		int dim = h();
		return float(w() - dim) / float(m_max - m_min) * float(pos);
	    }
	    else
	    {
		int dim = w();
		return float(h() - dim) / float(m_max - m_min) * float(pos);
	    }
	}

	// offset to position
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
	explicit Label(const std::string& text,
		       const Point& point = Point(),
		       const Size& size = Size(), int align = ALIGN_CENTER,
		       const Font& font = Font());

	int handle(int event);

	void text(const std::string& str);

	void fgcolor(const Color& color) { m_fgcolor = color; }

	void font(const Font& font) { m_font = font; }

	virtual void draw(const Rect& rect);

	virtual ~Label();

    protected:
	int m_align;
	std::string m_text;
	Font m_font;
	Color m_fgcolor;
    };

    /**
     * Input text box.
     */
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

    /**
     * Boolean checkbox.
     */
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

	// todo
	void percent(int value) { m_angle = value; damage(); }

	virtual int handle(int event);

	virtual void draw(const Rect& rect);

    protected:
	std::string m_label;
	float m_angle;
    };

    /**
     * A static grid that does not perform any drawing, but controls the
     * size and position of any widget added.
     */
    class StaticGrid : public Widget
    {
    public:
	StaticGrid(int x, int y, int w, int h, int columns, int rows, int border = 0)
	    : Widget(x, y, w, h),
	      m_columns(columns),
	      m_rows(rows),
	      m_border(border)
	{}

	virtual void draw(const Rect& rect) {}
	virtual void damage() {}

	virtual void position(int x, int y)
	{
	    Widget::position(x,y);
	    reposition();
	}

	virtual void size(int w, int h)
	{
	    Widget::size(w,h);
	    reposition();
	}

	virtual void add(Widget* widget, int column, int row)
	{
	    if (column >= (int)m_widgets.size())
		m_widgets.resize(column+1);

	    if (row >= (int)m_widgets[column].size())
		m_widgets[column].resize(row+1);

	    m_widgets[column][row] = widget;
	}

	/**
	 * Reposition all child widgets.
	 */
	virtual void reposition()
	{
	    for (int column = 0; column < (int)m_widgets.size(); column++)
	    {
		for (int row = 0; row < (int)m_widgets[column].size(); row++)
		{
		    Widget* widget = m_widgets[column][row];
		    if (widget)
		    {
			int ix = x() + (column * (w() / m_columns)) + m_border;
			int iy = y() + (row * (h() / m_rows)) + m_border;
			int iw = (w() / m_columns) - (m_border * 2);
			int ih = (h() / m_rows) - (m_border * 2);

			widget->position(ix, iy);
			widget->size(iw, ih);
		    }
		}
	    }
	}

    protected:
	int m_columns;
	int m_rows;
	int m_border;

	std::vector<std::vector<Widget*>> m_widgets;
    };

    class ProgressBar : public Widget
    {
    public:
	ProgressBar(const Point& point = Point(), const Size& size = Size());

	virtual void draw(const Rect& rect);

	/**
	 * Set the percent, from 0 to 100.
	 */
	virtual void percent(int p)
	{
	    if (m_percent != p)
	    {
		m_percent = p;
		if (m_percent < 0)
		    m_percent = 0;
		else if (m_percent > 100)
		    m_percent = 100;
		damage();
	    }
	}

    protected:
	int m_percent;
    };

    class LevelMeter : public ProgressBar
    {
    public:
	LevelMeter(const Point& point = Point(), const Size& size = Size());

	virtual void draw(const Rect& rect);
    };

    class AnalogMeter : public ProgressBar
    {
    public:
	AnalogMeter(const Point& point = Point(), const Size& size = Size());

	virtual void draw(const Rect& rect);
    };

    class SpinProgress : public ProgressBar
    {
    public:
	SpinProgress(const Point& point = Point(), const Size& size = Size());

	virtual void draw(const Rect& rect);
    };

    class ScrollWheel : public Widget
    {
    public:
	ScrollWheel(const Point& point = Point(), const Size& size = Size());

	virtual int handle(int event);

	virtual void draw(const Rect& rect);

	int position() const { return m_pos; }

	inline void position(int pos)
	{
	    if (pos < (int)m_values.size())
	    {
		m_pos = pos;
		damage();
	    }
	}

	void values(const std::vector<std::string>& v) { m_values = v; }

    protected:
	std::vector<std::string> m_values;
	int m_pos;
	int m_moving_x;
	int m_start_pos;
    };
}

#endif
