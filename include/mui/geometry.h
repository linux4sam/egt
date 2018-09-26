/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_GEOMETRY_H
#define MUI_GEOMETRY_H

/**
 * @file
 * @brief Working with geometry.
 */

#include <iosfwd>
#include <cassert>
#include <algorithm>
#include <cmath>

namespace mui
{
    /**
     * Simple x,y coordinate.
     */
    class Point
    {
    public:
        Point() noexcept
            : x(0),
              y(0)
        {}

        explicit Point(int x_, int y_) noexcept
            : x(x_),
              y(y_)
        {}

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

        /**
         * If this point is the center of a circle, return a new point that is
         * on the circumference of the circle at the specified angle.
         */
        template <class T>
        T point_on_circumference(T radius, T angle)
        {
            return Point(x + radius * std::sin(angle),
                         y - radius * std::cos(angle));
        }

        /**
         * Return the angle from this point to get to another.
         * @param point The other point.
         */
        template <class T>
        T angle_to_point(const Point& point)
        {
            return std::atan2(point.x - x, y - point.y);
        }

        int x;
        int y;

        friend Point operator-(Point lhs, const Point& rhs);
        friend Point operator+(Point lhs, const Point& rhs);
    };

    Point operator-(Point lhs, const Point& rhs);
    Point operator+(Point lhs, const Point& rhs);

    std::ostream& operator<<(std::ostream& os, const Point& point);

    /**
     * Simple width,height size.
     */
    class Size
    {
    public:
        Size() noexcept
            : h(0),
              w(0)
        {}

        explicit Size(int w_, int h_) noexcept
            : h(h_),
              w(w_)
        {}

        /**
         * Returns true if the size has no width or height.
         */
        inline bool empty() const
        {
            return w <= 0 || h <= 0;
        }

        inline void clear()
        {
            h = w = 0;
        }

        int h;
        int w;
    };

    std::ostream& operator<<(std::ostream& os, const Size& size);

    /**
     * A rectangle.
     *
     * A point and a size.
     *
     * All rectangle points are at the top left.
     */
    class Rect
    {
    public:
        Rect() noexcept
            : x(0),
              y(0),
              w(0),
              h(0)
        {}

        Rect(const Point& point, const Size& size) noexcept
            : x(point.x),
              y(point.y),
              w(size.w),
              h(size.h)
        {
            assert(w >= 0);
            assert(h >= 0);
        }

        Rect(int x_, int y_, int w_, int h_) noexcept
            : x(x_),
              y(y_),
              w(w_),
              h(h_)
        {
            assert(w >= 0);
            assert(h >= 0);
        }

        int area() const
        {
            return w * h;
        }

        Point center() const
        {
            return Point(x + (w / 2), y + (h / 2));
        }

        Point point() const
        {
            return Point(x, y);
        }

        Size size() const
        {
            return Size(w, h);
        }

        inline int top() const
        {
            return y;
        }

        inline int left() const
        {
            return x;
        }

        inline int bottom() const
        {
            return y + h;
        }

        inline int right() const
        {
            return x + w;
        }

        inline void clear()
        {
            x = y = w = h = 0;
        }

        /**
         * Returns true if the rectangle has no width or height.
         */
        inline bool empty() const
        {
            return w <= 0 || h <= 0;
        }

        inline bool operator==(const Rect& rhs) const
        {
            return x == rhs.x &&
                   y == rhs.y &&
                   w == rhs.w &&
                   h == rhs.h;
        }

        inline bool point_inside(const Point& point) const
        {
            return point_inside(point, *this);
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

            return Rect(xmin, ymin, xmax - xmin, ymax - ymin);
        }

        inline Rect intersect(const Rect& rhs)
        {
            return intersect(*this, rhs);
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

        int x;
        int y;
        int w;
        int h;
    };

    std::ostream& operator<<(std::ostream& os, const Rect& rect);

}

#endif
