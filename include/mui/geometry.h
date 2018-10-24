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

        using dim_t = int;

        Point() noexcept
        {}

        explicit Point(dim_t x_, dim_t y_) noexcept
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
         *
         * @param radius The radius of the circle.
         * @param angle The angle in radians.
         */
        template <class T>
        T point_on_circumference(T radius, T angle)
        {
            return Point(x + radius * std::sin(angle),
                         y - radius * std::cos(angle));
        }

        /**
         * Return the angle from this point to get to another.
         *
         * @param point The other point.
         */
        template <class T>
        T angle_to(const Point& point)
        {
            return std::atan2(point.x - x, y - point.y);
        }

        /**
         * Calculate the straight line distance to another point.
         *
         * @param point The other point.
         */
        template <class T>
        T distance_to(const Point& point)
        {
            /*
              return std::sqrt(T((point.x - x) * (point.x - x)) +
              T((point.y - y) * (point.y - y)));
            */
            return std::hypot(T(point.x - x), T(point.y - y));
        }

        dim_t x{0};
        dim_t y{0};
    };

    inline bool operator==(const Point& lhs, const Point& rhs)
    {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    inline bool operator!=(const Point& lhs, const Point& rhs)
    {
        return !(lhs == rhs);
    }

    inline Point operator-(Point lhs, const Point& rhs)
    {
        lhs -= rhs;
        return lhs;
    }

    inline Point operator+(Point lhs, const Point& rhs)
    {
        lhs += rhs;
        return lhs;
    }


    std::ostream& operator<<(std::ostream& os, const Point& point);

    /**
     * Simple width and height size.
     */
    class Size
    {
    public:

        using dim_t = int;

        Size() noexcept
        {}

        explicit Size(dim_t w_, dim_t h_) noexcept
            : h(h_),
              w(w_)
        {}

        /**
         * Returns true if the size has no width or height.
         *
         * This is a special case, and is the default state of a Size. This can
         * be used to determine if a size has not be set in many cases.
         */
        inline bool empty() const
        {
            return w <= 0 || h <= 0;
        }

        inline void clear()
        {
            h = w = 0;
        }

        Size& operator+=(const Size& rhs)
        {
            w += rhs.w;
            h += rhs.h;
            return *this;
        }

        Size& operator-=(const Size& rhs)
        {
            w -= rhs.w;
            h -= rhs.h;
            return *this;
        }

        dim_t h{0};
        dim_t w{0};
    };

    inline bool operator==(const Size& lhs, const Size& rhs)
    {
        return lhs.w == rhs.w && rhs.h == rhs.h;
    }

    inline bool operator!=(const Size& lhs, const Size& rhs)
    {
        return !(lhs == rhs);
    }

    inline Size operator-(Size lhs, const Size& rhs)
    {
        lhs -= rhs;
        return lhs;
    }

    inline Size operator+(Size lhs, const Size& rhs)
    {
        lhs += rhs;
        return lhs;
    }

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

        using dim_t = int;

        Rect() noexcept
        {}

        /**
         * Construct a rectangle with an explicit point and size.
         */
        explicit Rect(const Point& point, const Size& size) noexcept
            : x(point.x),
              y(point.y),
              w(size.w),
              h(size.h)
        {
            assert(w >= 0);
            assert(h >= 0);
        }

        /**
         * Construct a rectangle with the specified size and the point being at
         * the default origin 0,0.
         */
        explicit Rect(const Size& size) noexcept
            : w(size.w),
              h(size.h)
        {
            assert(w >= 0);
            assert(h >= 0);
        }

        explicit Rect(dim_t x_, dim_t y_, dim_t w_, dim_t h_) noexcept
            : x(x_),
              y(y_),
              w(w_),
              h(h_)
        {
            assert(w >= 0);
            assert(h >= 0);
        }

        Rect& operator+=(const Size& rhs)
        {
            w += rhs.w;
            h += rhs.h;
            return *this;
        }

        Rect& operator-=(const Size& rhs)
        {
            w -= rhs.w;
            h -= rhs.h;
            return *this;
        }

        Rect& operator+=(const Point& rhs)
        {
            x += rhs.x;
            y += rhs.y;
            return *this;
        }

        Rect& operator-=(const Point& rhs)
        {
            x -= rhs.x;
            y -= rhs.y;
            return *this;
        }


        inline dim_t area() const
        {
            return w * h;
        }

        /**
         * Return the center point of the rectangle.
         */
        inline Point center() const
        {
            return Point(x + (w / 2), y + (h / 2));
        }

        /**
         * Get the Point of the rectangle.
         */
        inline Point point() const
        {
            return Point(x, y);
        }

        inline void point(const Point& p)
        {
            x = p.x;
            y = p.y;
        }

        /**
         * Get the Size of the rectangle.
         */
        inline Size size() const
        {
            return Size(w, h);
        }

        inline void size(const Size& s)
        {
            w = s.w;
            h = s.h;
        }

        /**
         * Get the top side of the rectangle.
         */
        inline dim_t top() const
        {
            return y;
        }

        /**
         * Get the left side the rectangle.
         */
        inline dim_t left() const
        {
            return x;
        }

        /**
         * Get the bottom side the rectangle.
         */
        inline dim_t bottom() const
        {
            return y + h;
        }

        /**
         * Get the right side the rectangle.
         */
        inline dim_t right() const
        {
            return x + w;
        }

        inline Point top_left() const
        {
            return Point(left(), top());
        }

        inline Point top_right() const
        {
            return Point(right(), top());
        }

        inline Point bottom_left() const
        {
            return Point(left(), bottom());
        }

        inline Point bottom_right() const
        {
            return Point(right(), bottom());
        }

        /**
         * Clear the rectangle by giving it no with or height.
         */
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

        inline bool point_inside(const Point& point) const
        {
            return point_inside(point, *this);
        }

        /**
         * Determine if the specified point is inside of the rectangle.
         */
        static inline bool point_inside(const Point& point, const Rect& rhs)
        {
            return (point.x <= rhs.right() && point.x >= rhs.left() &&
                    point.y <= rhs.bottom() && point.y >= rhs.top());
        }

        /**
         * Determine if two rectangles intersect, or, overlap.
         */
        static inline bool intersect(const Rect& lhs, const Rect& rhs)
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
            dim_t xmin = std::min(lhs.x, rhs.x);
            dim_t xmax = std::max(lhs.x + lhs.w + static_cast<dim_t>(1),
                                  rhs.x + rhs.w + static_cast<dim_t>(1));
            dim_t ymin = std::min(lhs.y, rhs.y);
            dim_t ymax = std::max(lhs.y + lhs.h + static_cast<dim_t>(1),
                                  rhs.y + rhs.h + static_cast<dim_t>(1));

            return Rect(xmin, ymin, xmax - xmin, ymax - ymin);
        }

        inline Rect intersection(const Rect& rhs)
        {
            return intersection(*this, rhs);
        }

        /**
         * Return the intersecting rectangle of two rectangles, if any.
         */
        static inline Rect intersection(const Rect& lhs, const Rect& rhs)
        {
            dim_t x = std::max(lhs.x, rhs.x);
            dim_t y = std::max(lhs.y, rhs.y);
            dim_t w = std::min(lhs.x + lhs.w, rhs.x + rhs.w) - x;
            dim_t h = std::min(lhs.y + lhs.h, rhs.y + rhs.h) - y;

            if (w < 0 || h < 0)
                return Rect();

            return Rect(x, y, w, h);
        }

        dim_t x{0};
        dim_t y{0};
        dim_t w{0};
        dim_t h{0};
    };

    std::ostream& operator<<(std::ostream& os, const Rect& rect);

    inline Rect operator-(Rect lhs, const Size& rhs)
    {
        lhs.size(lhs.size() - rhs);
        return lhs;
    }

    inline Rect operator+(Rect lhs, const Size& rhs)
    {
        lhs.size(lhs.size() + rhs);
        return lhs;
    }

    inline bool operator==(const Rect& lhs, const Rect& rhs)
    {
        return lhs.x == rhs.x &&
               lhs.y == rhs.y &&
               lhs.w == rhs.w &&
               lhs.h == rhs.h;
    }

    inline bool operator!=(const Rect& lhs, const Rect& rhs)
    {
        return !(lhs == rhs);
    }

    class Line
    {
    public:
        using dim_t = int;

        explicit Line(const Point& start, const Point& end) noexcept
            : m_start(start),
              m_end(end)
        {}

        inline Point start() const { return m_start; }
        inline Point end() const { return m_end; }

        /**
         * Returns a rectangle containing the line.
         */
        inline Rect rect()
        {
            dim_t x = std::min(m_start.x, m_end.x);
            dim_t y = std::min(m_start.y, m_end.y);
            dim_t x2 = std::max(m_start.x, m_end.x);
            dim_t y2 = std::max(m_start.y, m_end.y);

            return Rect(x, y, x2 - x, y2 - y);
        }

    protected:
        Point m_start;
        Point m_end;
    };
}

#endif
