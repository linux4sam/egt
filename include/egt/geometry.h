/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_GEOMETRY_H
#define EGT_GEOMETRY_H

/**
 * @file
 * @brief Working with geometry.
 */

#include <algorithm>
#include <cassert>
#include <cmath>
#include <egt/detail/floatingpoint.h>
#include <iostream>
#include <string>

namespace egt
{
inline namespace v1
{
/**
 * @defgroup geometry Geometry
 * @brief Objects for working with geometry.
 */

using default_dim_type = int;

enum class compatible
{
    normal,
    display,
    tuple,
};

/**
 * @brief Simple x,y coordinate.
 * @ingroup geometry
 */
template <class dim_t, compatible dim_c = compatible::normal>
class PointType
{
public:

    using dim_type = dim_t;

    PointType() noexcept
    {}

    PointType(dim_t x_, dim_t y_) noexcept
        : x(x_),
          y(y_)
    {}

    PointType<dim_t, dim_c>(const PointType<dim_t, dim_c>&) = default;
    PointType<dim_t, dim_c>(PointType<dim_t, dim_c>&&) = default;
    PointType<dim_t, dim_c>& operator=(const PointType<dim_t, dim_c>&) = default;
    PointType<dim_t, dim_c>& operator=(PointType<dim_t, dim_c>&&) = default;
    ~PointType<dim_t, dim_c>() = default;

    PointType<dim_t, dim_c>& operator+=(const PointType<dim_t, dim_c>& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    PointType<dim_t, dim_c>& operator-=(const PointType<dim_t, dim_c>& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    PointType<dim_t, dim_c>& operator/=(const PointType<dim_t, dim_c>& rhs)
    {
        x /= rhs.x;
        y /= rhs.y;
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
    PointType<dim_t, dim_c> point_on_circumference(T radius, T angle) const
    {
        return PointType(x + radius * std::cos(angle),
                         y + radius * std::sin(angle));
    }

    /**
     * Return the angle in radians from this point to get to another.
     *
     * @param point The other point.
     */
    template <class T>
    T angle_to(const PointType<dim_t, dim_c>& point) const
    {
        return std::atan2(point.x - x, y - point.y);
    }

    /**
     * Calculate the straight line distance to another point.
     *
     * @param point The other point.
     */
    template <class T>
    T distance_to(const PointType<dim_t, dim_c>& point) const
    {
#if 0
        return std::sqrt(T((point.x - x) * (point.x - x)) +
                         T((point.y - y) * (point.y - y)));
#else
        // maybe slower...
        return std::hypot(T(point.x - x), T(point.y - y));
#endif
    }

    dim_t x{0};
    dim_t y{0};
};

/// Compares two @c PointType objects for equality.
template<class dim_t, compatible dim_c>
inline bool operator==(const PointType<dim_t, dim_c>& lhs, const PointType<dim_t, dim_c>& rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

/// Compares two @c PointType objects for inequality.
template<class dim_t, compatible dim_c>
inline bool operator!=(const PointType<dim_t, dim_c>& lhs, const PointType<dim_t, dim_c>& rhs)
{
    return !(lhs == rhs);
}

template<class dim_t, compatible dim_c>
inline PointType<dim_t, dim_c> operator-(PointType<dim_t, dim_c> lhs, const PointType<dim_t, dim_c>& rhs)
{
    lhs -= rhs;
    return lhs;
}

template<class dim_t, compatible dim_c>
inline PointType<dim_t, dim_c> operator+(PointType<dim_t, dim_c> lhs, const PointType<dim_t, dim_c>& rhs)
{
    lhs += rhs;
    return lhs;
}

template<class dim_t, compatible dim_c>
inline PointType<dim_t, dim_c> operator/(PointType<dim_t, dim_c> lhs, const PointType<dim_t, dim_c>& rhs)
{
    lhs /= rhs;
    return lhs;
}

template<class dim_t, compatible dim_c>
std::ostream& operator<<(std::ostream& os, const PointType<dim_t, dim_c>& point)
{
    os << point.x << "," << point.y;
    return os;
}

using Point = PointType<default_dim_type, compatible::normal>;
using PointF = PointType<float, compatible::normal>;
using DisplayPoint = PointType<default_dim_type, compatible::display>;

/**
 * @brief Simple width and height.
 * @ingroup geometry
 */
template<class dim_t, compatible dim_c = compatible::normal>
class SizeType
{
public:

    using dim_type = dim_t;

    SizeType() noexcept
    {}

    explicit SizeType(dim_t w_, dim_t h_) noexcept
        : h(h_),
          w(w_)
    {}

    SizeType<dim_t, dim_c>(const SizeType<dim_t, dim_c>&) = default;
    SizeType<dim_t, dim_c>(SizeType<dim_t, dim_c>&&) = default;
    SizeType<dim_t, dim_c>& operator=(const SizeType<dim_t, dim_c>&) = default;
    SizeType<dim_t, dim_c>& operator=(SizeType<dim_t, dim_c>&&) = default;
    ~SizeType<dim_t, dim_c>() = default;

    /**
     * Returns true if the size has no width or height.
     *
     * This is a special case, and is the default state of a SizeType. This can
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

    SizeType<dim_t, dim_c>& operator+=(const SizeType<dim_t, dim_c>& rhs)
    {
        w += rhs.w;
        h += rhs.h;
        return *this;
    }

    SizeType<dim_t, dim_c>& operator-=(const SizeType<dim_t, dim_c>& rhs)
    {
        w -= rhs.w;
        h -= rhs.h;
        return *this;
    }

    dim_t h{0};
    dim_t w{0};
};

/// Compares two @c SizeType objects for equality.
template<class dim_t, compatible dim_c>
inline bool operator==(const SizeType<dim_t, dim_c>& lhs, const SizeType<dim_t, dim_c>& rhs)
{
    return lhs.w == rhs.w && lhs.h == rhs.h;
}

/// Compares two @c SizeType objects for inequality.
template<class dim_t, compatible dim_c>
inline bool operator!=(const SizeType<dim_t, dim_c>& lhs, const SizeType<dim_t, dim_c>& rhs)
{
    return !(lhs == rhs);
}

template<class dim_t, compatible dim_c>
inline SizeType<dim_t, dim_c> operator-(SizeType<dim_t, dim_c> lhs, const SizeType<dim_t, dim_c>& rhs)
{
    lhs -= rhs;
    return lhs;
}

template<class dim_t, compatible dim_c>
inline SizeType<dim_t, dim_c> operator+(SizeType<dim_t, dim_c> lhs, const SizeType<dim_t, dim_c>& rhs)
{
    lhs += rhs;
    return lhs;
}

template<class dim_t, compatible dim_c>
inline SizeType<dim_t, dim_c> operator*(SizeType<dim_t, dim_c> lhs, const SizeType<dim_t, dim_c>& rhs)
{
    lhs.w *= rhs.w;
    lhs.h *= rhs.h;
    return lhs;
}

template<class dim_t, compatible dim_c>
inline SizeType<dim_t, dim_c> operator*(SizeType<dim_t, dim_c> lhs, float rhs)
{
    lhs.w *= rhs;
    lhs.h *= rhs;
    return lhs;
}

template<class dim_t, compatible dim_c>
inline SizeType<dim_t, dim_c> operator/(SizeType<dim_t, dim_c> lhs, const SizeType<dim_t, dim_c>& rhs)
{
    lhs.w /= rhs.w;
    lhs.h /= rhs.h;
    return lhs;
}

template<class dim_t, compatible dim_c>
inline SizeType<dim_t, dim_c> operator/(SizeType<dim_t, dim_c> lhs, float rhs)
{
    lhs.w /= rhs;
    lhs.h /= rhs;
    return lhs;
}

template<class dim_t, compatible dim_c>
std::ostream& operator<<(std::ostream& os, const SizeType<dim_t, dim_c>& size)
{
    os << size.w << "," << size.h;
    return os;
}

using Size = SizeType<default_dim_type, compatible::normal>;
using SizeF = SizeType<float, compatible::normal>;
using Tuple = SizeType<int, compatible::tuple>;

/**
 * @brief A rectangle.
 * @ingroup geometry
 *
 * A point and a size.
 *
 * All rectangle points are at the top left.
 */
template<class dim_t, compatible dim_c = compatible::normal>
class RectType
{
public:

    using dim_type = dim_t;

    RectType() noexcept
    {}

    /**
     * Construct a rectangle with an explicit point and size.
     */
    explicit RectType(const Point& point, const SizeType<dim_t, dim_c>& size) noexcept
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
    // cppcheck-suppress noExplicitConstructor
    RectType(const SizeType<dim_t, dim_c>& size) noexcept
        : w(size.w),
          h(size.h)
    {
        assert(w >= 0);
        assert(h >= 0);
    }

    RectType(dim_t x_, dim_t y_, dim_t w_, dim_t h_) noexcept
        : x(x_),
          y(y_),
          w(w_),
          h(h_)
    {
        assert(w >= 0);
        assert(h >= 0);
    }

    RectType<dim_t, dim_c>(const RectType<dim_t, dim_c>&) = default;
    RectType<dim_t, dim_c>(RectType<dim_t, dim_c>&&) = default;
    RectType<dim_t, dim_c>& operator=(const RectType<dim_t, dim_c>&) = default;
    RectType<dim_t, dim_c>& operator=(RectType<dim_t, dim_c>&&) = default;
    ~RectType<dim_t, dim_c>() = default;

    RectType<dim_t, dim_c>& operator+=(const SizeType<dim_t, dim_c>& rhs)
    {
        w += rhs.w;
        h += rhs.h;
        return *this;
    }

    RectType<dim_t, dim_c>& operator-=(const SizeType<dim_t, dim_c>& rhs)
    {
        w -= rhs.w;
        h -= rhs.h;
        return *this;
    }

    RectType<dim_t, dim_c>& operator+=(const Point& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    RectType<dim_t, dim_c>& operator-=(const Point& rhs)
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
        return Point(x + (w / 2.), y + (h / 2.));
    }

    /**
     * Move the rectangle's center to the specified point.
     */
    inline void move_to_center(const Point& center)
    {
        Point pos(center.x - w / 2.,
                  center.y - h / 2.);

        x = pos.x;
        y = pos.y;
    }

    inline void grow_around_center(dim_t value)
    {
        x -= value / 2.;
        y -= value / 2.;
        w += value;
        h += value;
    }

    /**
     * Get the PointType of the rectangle.
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
     * Get the SizeType of the rectangle.
     */
    inline SizeType<dim_t, dim_c> size() const
    {
        return SizeType<dim_t, dim_c>(w, h);
    }

    /**
     * Set the SizeType of the rectangle.
     */
    inline void size(const SizeType<dim_t, dim_c>& s)
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

    /**
     * Get the top left point of the rectangle.
     *
     * @note This is usually the origin/point of the rectangle.
     */
    inline Point top_left() const
    {
        return Point(left(), top());
    }

    /**
     * Get the top right point of the rectangle.
     */
    inline Point top_right() const
    {
        return Point(right(), top());
    }

    /**
     * Get the bottom left point of the rectangle.
     */
    inline Point bottom_left() const
    {
        return Point(left(), bottom());
    }

    /**
     * Get the bottom right point of the rectangle.
     */
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

    /**
     * Returns true if the specified point is inside the rectangle.
     * @param point The point to test.
     */
    inline bool point_inside(const Point& point) const
    {
        return point_inside(point, *this);
    }

    /**
     * Determine if the specified point is inside of the rectangle.
     */
    static inline bool point_inside(const Point& point, const RectType<dim_t, dim_c>& rhs)
    {
        return (point.x <= rhs.right() && point.x >= rhs.left() &&
                point.y <= rhs.bottom() && point.y >= rhs.top());
    }

    /**
     * Determine if two rectangles intersect, or, overlap.
     */
    static inline bool intersect(const RectType<dim_t, dim_c>& lhs, const RectType<dim_t, dim_c>& rhs)
    {
        return (lhs.x < rhs.x + rhs.w && lhs.x + lhs.w > rhs.x &&
                lhs.y < rhs.y + rhs.h && lhs.y + lhs.h > rhs.y);
    }

    /**
     * Determine if a point intersects with a rectangle.
     */
    static inline bool intersect(const RectType<dim_t, dim_c>& lhs, const Point& rhs)
    {
        return (lhs.x < rhs.x && lhs.x + lhs.w > rhs.x &&
                lhs.y < rhs.y && lhs.y + lhs.h > rhs.y);
    }

    /**
     * Merge two rectangles together into one super rectangle that contains
     * them both.
     */
    static inline RectType<dim_t, dim_c> merge(const RectType<dim_t, dim_c>& lhs, const RectType<dim_t, dim_c>& rhs) noexcept
    {
        auto xmin = std::min(lhs.x, rhs.x);
        auto xmax = std::max(lhs.x + lhs.w,
                             rhs.x + rhs.w);
        auto ymin = std::min(lhs.y, rhs.y);
        auto ymax = std::max(lhs.y + lhs.h,
                             rhs.y + rhs.h);

        return RectType<dim_t, dim_c>(xmin, ymin, xmax - xmin, ymax - ymin);
    }

    inline RectType<dim_t, dim_c> intersection(const RectType<dim_t, dim_c>& rhs) noexcept
    {
        return intersection(*this, rhs);
    }

    /**
     * Return the intersecting rectangle of two rectangles, if any.
     */
    static inline RectType<dim_t, dim_c> intersection(const RectType<dim_t, dim_c>& lhs, const RectType<dim_t, dim_c>& rhs) noexcept
    {
        auto x0 = std::max(lhs.x, rhs.x);
        auto y0 = std::max(lhs.y, rhs.y);
        auto w0 = std::min(lhs.x + lhs.w, rhs.x + rhs.w) - x0;
        auto h0 = std::min(lhs.y + lhs.h, rhs.y + rhs.h) - y0;

        if (w0 < 0 || h0 < 0)
            return RectType<dim_t, dim_c>();

        return RectType<dim_t, dim_c>(x0, y0, w0, h0);
    }

    /**
    * Returns true if this rectangle contains the specified one.
     */
    inline bool contains(const RectType<dim_t, dim_c>& rhs) const
    {
        return rhs.right() < right() &&
               rhs.bottom() < bottom() &&
               rhs.left() > left() &&
               rhs.top() > top();
    }

    inline std::string tostring() const
    {
        return "[" + std::to_string(x) + "," + std::to_string(y) +
               " - " + std::to_string(w) + "," + std::to_string(h) + "]";
    }

    dim_t x{0};
    dim_t y{0};
    dim_t w{0};
    dim_t h{0};
};

template<class dim_t, compatible dim_c>
std::ostream& operator<<(std::ostream& os, const RectType<dim_t, dim_c>& rect)
{
    os << rect.tostring();
    return os;
}

template<class dim_t, compatible dim_c>
inline RectType<dim_t, dim_c> operator-(RectType<dim_t, dim_c> lhs, const SizeType<dim_t, dim_c>& rhs)
{
    lhs.size(lhs.size() - rhs);
    return lhs;
}

template<class dim_t, compatible dim_c>
inline RectType<dim_t, dim_c> operator+(RectType<dim_t, dim_c> lhs, const SizeType<dim_t, dim_c>& rhs)
{
    lhs.size(lhs.size() + rhs);
    return lhs;
}

template<class dim_t, compatible dim_c>
inline RectType<dim_t, dim_c> operator+(RectType<dim_t, dim_c> lhs, const Point& rhs)
{
    lhs.point(lhs.point() + rhs);
    return lhs;
}

template<class dim_t, compatible dim_c>
inline RectType<dim_t, dim_c> operator-(RectType<dim_t, dim_c> lhs, const Point& rhs)
{
    lhs.point(lhs.point() - rhs);
    return lhs;
}

/// Compares two @c Rect objects for equality.
template<class dim_t, compatible dim_c>
inline bool operator==(const RectType<dim_t, dim_c>& lhs, const RectType<dim_t, dim_c>& rhs)
{
    return lhs.x == rhs.x &&
           lhs.y == rhs.y &&
           lhs.w == rhs.w &&
           lhs.h == rhs.h;
}

/// Compares two @c Rect objects for inequality.
template<class dim_t, compatible dim_c>
inline bool operator!=(const RectType<dim_t, dim_c>& lhs, const RectType<dim_t, dim_c>& rhs)
{
    return !(lhs == rhs);
}

using Rect = RectType<default_dim_type, compatible::normal>;

/**
 * @brief A line, with a starting and ending point.
 * @ingroup geometry
 */
template<class dim_t>
class LineType
{
public:

    using dim_type = dim_t;

    explicit LineType(const Point& start, const Point& end) noexcept
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
        auto x = std::min(m_start.x, m_end.x);
        auto y = std::min(m_start.y, m_end.y);
        auto x2 = std::max(m_start.x, m_end.x);
        auto y2 = std::max(m_start.y, m_end.y);

        return Rect(x, y, x2 - x, y2 - y);
    }

protected:
    Point m_start;
    Point m_end;
};

using Line = LineType<default_dim_type>;

/**
 * @brief An Arc consists of a radius and two angles.
 * @ingroup geometry
 */
template<class dim_t>
class ArcType
{
public:

    using dim_type = dim_t;

    /**
     * Construct Arc object.
     */
    ArcType(const Point& c = Point(), float r = 0.0f,
            float a1 = 0.0f, float a2 = 0.0f)
        : center(c),
          radius(r),
          angle1(a1),
          angle2(a2)
    {
    }

    Point center;
    float radius{0.0f};
    float angle1;
    float angle2;
};

/**
 * @brief Arc
 * This is an Arc.
 */
using Arc = ArcType<default_dim_type>;

/**
 * @brief A basic circle with a center point and radius.
 * @ingroup geometry
 */
template<class dim_t>
class CircleType : public ArcType<dim_t>
{
public:

    using dim_type = dim_t;

    /**
     * Construct a Circle object.
     */
    CircleType(const Point& c = Point(), float r = 0.0f)
        : Arc(c, r, 0.0f, 2 * M_PI)
    {
    }

    bool empty()
    {
        return this->radius < 0.0f ||
               detail::FloatingPoint<float>(this->radius).
               AlmostEquals(detail::FloatingPoint<float>(0.0f));
    }

    Rect rect() const
    {
        Rect r(this->center, Size());
        r.grow_around_center(this->radius);
        return r;
    }
};

using Circle = CircleType<default_dim_type>;

}
}

#endif
