/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <iostream>

namespace mui
{

/**
 *
 */
    class Point
    {
    public:
	explicit Point(int x_ = 0, int y_ = 0)
	    : x(x_),
	      y(y_)
	{}

	int x;
	int y;
    };

    inline std::ostream& operator << (std::ostream& os, const Point& point)
    {
	os << point.x << "," << point.y;
	return os;
    }

/**
 *
 */
    class Size
    {
    public:
	Size(int w_ = 0, int h_ = 0)
	    : h(h_),
	      w(w_)
	{}

	int h;
	int w;
    };

/**
 *
 */
    class Rect
    {
    public:
	Rect(int x_ = 0, int y_ = 0, int w_ = 0, int h_ = 0)
	    : x(x_),
	      y(y_),
	      w(w_),
	      h(h_)
	{}

	int x;
	int y;
	int w;
	int h;

	void clear()
	{
	    x = y = w = h = 0;
	}

	bool is_clear() const
	{
	    return w == 0 || h == 0;
	}

	static bool point_inside(const Point& point, const Rect& rhs)
	{
	    return  (point.x <= rhs.x + rhs.w && point.x >= rhs.x &&
		     point.y <= rhs.y + rhs.h && point.y >= rhs.y);
	}

	static bool is_intersect(const Rect& lhs, const Rect& rhs)
	{
	    return  (lhs.x <= rhs.x + rhs.w && lhs.x + lhs.w >= rhs.x &&
		     lhs.y <= rhs.y + rhs.h && lhs.y + lhs.h >= rhs.y);
	}

	static Rect merge(const Rect& lhs, const Rect& rhs)
	{
	    int xmin = std::min(lhs.x, rhs.x);
	    int xmax = std::max(lhs.x + lhs.w + 1, rhs.x + rhs.w + 1);
	    int ymin = std::min(lhs.y, rhs.y);
	    int ymax = std::max(lhs.y + lhs.h + 1, rhs.y + rhs.h + 1);

	    return Rect(xmin, ymin, xmax-xmin, ymax-ymin);
	}


	static Rect intersect(const Rect& lhs, const Rect& rhs)
	{
	    /*
	      intersection is empty unless
	      intersect.Right > intersect.Left
	      && intersect.Bottom > intersect.Top
	    */

	    int x = std::max(lhs.x, rhs.x);
	    int y = std::max(lhs.y, rhs.y);
	    int r = std::min(lhs.x + lhs.w, rhs.x + rhs.w);
	    int b = std::min(lhs.y + lhs.h, rhs.y + rhs.h);

	    return Rect(x,y,r-x,b-y);
	}
    };

    inline std::ostream& operator << (std::ostream& os, const Rect& rect)
    {
	os << "[" << rect.x << "," << rect.y << " - " << rect.w << "," << rect.h << "]";
	return os;
    }

}

#endif
