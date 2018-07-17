/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <iosfwd>
#include <cassert>
#include <algorithm>

namespace mui
{
    /**
     * Simple x,y coordinate.
     *
     * @image html coordsys.png
     */
    class Point
    {
    public:
	Point()
	    : x(0),
	      y(0)
	{}

	explicit Point(int x_, int y_)
	    : x(x_),
	      y(y_)
	{}

	int x;
	int y;

	Point& operator+=(const Point& rhs)
	{
	    x += rhs.x;
	    y += rhs.y;
	    return *this;
	}

	Point& operator-=(const Point& rhs)
	{
	    x -= rhs.x;
	    y -= rhs.y;
	    return *this;
	}

	friend Point operator-(Point lhs, const Point& rhs)
	{
	    lhs -= rhs;
	    return lhs;
	}

	friend Point operator+(Point lhs, const Point& rhs)
	{
	    lhs += rhs;
	    return lhs;
	}

    };

    std::ostream& operator<<(std::ostream& os, const Point& point);

    /**
     * Simple width,height size.
     */
    class Size
    {
    public:
	Size()
	    : h(0),
	      w(0)
	{}

	explicit Size(int w_, int h_)
	    : h(h_),
	      w(w_)
	{}

	int h;
	int w;
    };

    std::ostream& operator<<(std::ostream& os, const Size& size);

    /**
     * A rectangle. A point and a size from that same point.
     *
     * All rectangle points are at the top left.
     * @image html coordsys.png
     */
    class Rect
    {
    public:
	Rect()
	    : x(0),
	      y(0),
	      w(0),
	      h(0)
	{}

	Rect(const Point& point, const Size& size)
	    : x(point.x),
	      y(point.y),
	      w(size.w),
	      h(size.h)
	{
	    assert(w >= 0);
	    assert(h >= 0);
	}

	Rect(int x_, int y_, int w_, int h_)
	    : x(x_),
	      y(y_),
	      w(w_),
	      h(h_)
	{
	    assert(w >= 0);
	    assert(h >= 0);
	}

	int x;
	int y;
	int w;
	int h;

	Point center() const
	{
	    return Point(x + (w/2), y + (h/2));
	}

	Point point() const
	{
	    return Point(x,y);
	}

	Size size() const
	{
	    return Size(w,h);
	}

	inline void clear()
	{
	    x = y = w = h = 0;
	}

	inline bool is_clear() const
	{
	    return w == 0 || h == 0;
	}

	/**
	 * Determine if the specified point is inside of the rectangle.
	 */
	static inline bool point_inside(const Point& point, const Rect& rhs)
	{
	    return (point.x <= rhs.x + rhs.w && point.x >= rhs.x &&
		    point.y <= rhs.y + rhs.h && point.y >= rhs.y);
	}

	/**
	 * Determine if two rectangles intersect, or, overlap.
	 */
	static inline bool is_intersect(const Rect& lhs, const Rect& rhs)
	{
	    return (lhs.x <= rhs.x + rhs.w && lhs.x + lhs.w >= rhs.x &&
		    lhs.y <= rhs.y + rhs.h && lhs.y + lhs.h >= rhs.y);
	}

	/**
	 * Merge two rectangles together into one super rectangle that contains
	 * them both.
	 */
	static inline Rect merge(const Rect& lhs, const Rect& rhs)
	{
	    int xmin = std::min(lhs.x, rhs.x);
	    int xmax = std::max(lhs.x + lhs.w + 1, rhs.x + rhs.w + 1);
	    int ymin = std::min(lhs.y, rhs.y);
	    int ymax = std::max(lhs.y + lhs.h + 1, rhs.y + rhs.h + 1);

	    return Rect(xmin, ymin, xmax-xmin, ymax-ymin);
	}

	/**
	 * Return the intersecting rectangle of two rectangles, if any.
	 */
	static inline Rect intersect(const Rect& lhs, const Rect& rhs)
	{
	    int x = std::max(lhs.x, rhs.x);
	    int y = std::max(lhs.y, rhs.y);
	    int w = std::min(lhs.x + lhs.w, rhs.x + rhs.w) - x;
	    int h = std::min(lhs.y + lhs.h, rhs.y + rhs.h) - y;

	    if (w < 0 || h < 0)
		return Rect();

	    return Rect(x, y, w, h);
	}
    };

    std::ostream& operator<< (std::ostream& os, const Rect& rect);

}

#endif
