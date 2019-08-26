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
#include <egt/detail/math.h>
#include <iostream>
#include <string>

namespace egt
{
inline namespace v1
{
/**
 * @defgroup geometry Geometry
 * Objects for working with geometry.
 */

/**
 * Helper type to define default dimension type used for geometry.
 */
using default_dim_type = int;

/**
 * Geometry types compatibility type.
 *
 * This is used to create strong incompatible types as a template argument.
 */
enum class compatible
{
    normal,
    display,
    tuple,
};

/**
 * Simple x,y coordinate.
 * @ingroup geometry
 */
template <class dim_t, compatible dim_c = compatible::normal>
class PointType
{
public:

    /**
     * Helper to reference the dimension type.
     */
    using dim_type = dim_t;

    constexpr PointType() noexcept = default;

    constexpr PointType(dim_t x, dim_t y) noexcept
        : m_x(x),
          m_y(y)
    {}

    PointType<dim_t, dim_c>& operator+=(const PointType<dim_t, dim_c>& rhs)
    {
        m_x += rhs.m_x;
        m_y += rhs.m_y;
        return *this;
    }

    PointType<dim_t, dim_c>& operator-=(const PointType<dim_t, dim_c>& rhs)
    {
        m_x -= rhs.m_x;
        m_y -= rhs.m_y;
        return *this;
    }

    PointType<dim_t, dim_c>& operator/=(const PointType<dim_t, dim_c>& rhs)
    {
        m_x /= rhs.m_x;
        m_y /= rhs.m_y;
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
        return PointType(m_x + radius * std::cos(angle),
                         m_y + radius * std::sin(angle));
    }

    /**
     * Return the angle in radians from this point to get to another.
     *
     * @param point The other point.
     */
    template <class T>
    T angle_to(const PointType<dim_t, dim_c>& point) const
    {
        return std::atan2(point.m_x - m_x, m_y - point.m_y);
    }

    /**
     * Calculate the straight line distance to another point.
     *
     * @param point The other point.
     */
    dim_t distance_to(const PointType<dim_t, dim_c>& point) const
    {
        return std::hypot(point.m_x - m_x, point.m_y - m_y);
    }

    constexpr inline dim_t x() const { return m_x; }
    constexpr inline dim_t y() const { return m_y; }

    inline void set_x(dim_t x) { m_x = x; }
    inline void set_y(dim_t y) { m_y = y; }

protected:
    dim_t m_x{0};
    dim_t m_y{0};
};

/// Compares two @c PointType objects for equality.
template<class dim_t, compatible dim_c>
inline bool operator==(const PointType<dim_t, dim_c>& lhs, const PointType<dim_t, dim_c>& rhs)
{
    return lhs.x() == rhs.x() && lhs.y() == rhs.y();
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
    os << "[" << point.x() << "," << point.y() << "]";
    return os;
}

/**
 * Helper type for a default point.
 */
using Point = PointType<default_dim_type, compatible::normal>;

/**
 * Helper type for a floating point.
 */
using PointF = PointType<float, compatible::normal>;

/**
 * Helper type for a default display point.
 */
using DisplayPoint = PointType<default_dim_type, compatible::display>;

/**
 * Simple width and height.
 * @ingroup geometry
 */
template<class dim_t, compatible dim_c = compatible::normal>
class SizeType
{
public:

    /**
     * Helper to reference the dimension type.
     */
    using dim_type = dim_t;

    constexpr SizeType() noexcept = default;

    /**
     * @param width Width
     * @param height Height
     */
    constexpr explicit SizeType(dim_t width, dim_t height) noexcept
        : m_height(height),
          m_width(width)
    {}

    /**
     * Returns true if the size has no width or height.
     *
     * This is a special case, and is the default state of a SizeType. This can
     * be used to determine if a size has not be set in many cases.
     */
    inline bool empty() const
    {
        return m_width <= 0 || m_height <= 0;
    }

    /**
     * Clear the size.
     *
     * This means to set the width and height to zero.
     */
    inline void clear()
    {
        m_height = m_width = 0;
    }

    SizeType<dim_t, dim_c>& operator+=(const SizeType<dim_t, dim_c>& rhs)
    {
        m_width += rhs.m_width;
        m_height += rhs.m_height;
        return *this;
    }

    SizeType<dim_t, dim_c>& operator-=(const SizeType<dim_t, dim_c>& rhs)
    {
        m_width -= rhs.m_width;
        m_height -= rhs.m_height;
        return *this;
    }

    SizeType<dim_t, dim_c>& operator*=(const SizeType<dim_t, dim_c>& rhs)
    {
        m_width *= rhs.m_width;
        m_height *= rhs.m_height;
        return *this;
    }

    SizeType<dim_t, dim_c>& operator/=(const SizeType<dim_t, dim_c>& rhs)
    {
        m_width /= rhs.m_width;
        m_height /= rhs.m_height;
        return *this;
    }

    constexpr inline dim_t width() const { return m_width; }
    constexpr inline dim_t height() const { return m_height; }

    inline void set_width(dim_t width) { m_width = width; }
    inline void set_height(dim_t height) { m_height = height; }

protected:
    /**
     * Height
     */
    dim_t m_height{0};

    /**
     * Width
     */
    dim_t m_width{0};
};

/// Compares two @c SizeType objects for equality.
template<class dim_t, compatible dim_c>
inline bool operator==(const SizeType<dim_t, dim_c>& lhs, const SizeType<dim_t, dim_c>& rhs)
{
    return lhs.width() == rhs.width() && lhs.height() == rhs.height();
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
    lhs.set_width(lhs.width() * rhs.width());
    lhs.set_height(lhs.height() * rhs.height());
    return lhs;
}

template<class dim_t, compatible dim_c>
inline SizeType<dim_t, dim_c> operator*(SizeType<dim_t, dim_c> lhs, float rhs)
{
    lhs.set_width(lhs.width() * rhs);
    lhs.set_height(lhs.height() * rhs);
    return lhs;
}

template<class dim_t, compatible dim_c>
inline SizeType<dim_t, dim_c> operator/(SizeType<dim_t, dim_c> lhs, const SizeType<dim_t, dim_c>& rhs)
{
    lhs.set_width(lhs.width() / rhs.width());
    lhs.set_height(lhs.height() / rhs.height());
    return lhs;
}

template<class dim_t, compatible dim_c>
inline SizeType<dim_t, dim_c> operator/(SizeType<dim_t, dim_c> lhs, float rhs)
{
    lhs.set_width(lhs.width() / rhs);
    lhs.set_height(lhs.height() / rhs);
    return lhs;
}

template<class dim_t, compatible dim_c>
std::ostream& operator<<(std::ostream& os, const SizeType<dim_t, dim_c>& size)
{
    os << size.width() << "x" << size.height();
    return os;
}

/**
 * Helper type for a default size.
 */
using Size = SizeType<default_dim_type, compatible::normal>;
/**
 * Helper type for a float size.
 */
using SizeF = SizeType<float, compatible::normal>;
/**
 * Helper type for a tuple.
 */
using Tuple = SizeType<int, compatible::tuple>;

/**
 * A rectangle.
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

    /**
     * Helper to reference the dimension type.
     */
    using dim_type = dim_t;

    constexpr RectType() noexcept = default;

    /**
     * Construct a rectangle with an explicit point and size.
     */
    explicit RectType(const PointType<dim_t>& point, const SizeType<dim_t, dim_c>& size) noexcept
        : m_x(point.x()),
          m_y(point.y()),
          m_width(size.width()),
          m_height(size.height())
    {
        assert(m_width >= 0);
        assert(m_height >= 0);
    }

    /**
     * Construct a rectangle with the specified size and the point being at
     * the default origin 0,0.
     */
    // cppcheck-suppress noExplicitConstructor
    RectType(const SizeType<dim_t, dim_c>& size) noexcept
        : m_width(size.width()),
          m_height(size.height())
    {
        assert(m_width >= 0);
        assert(m_height >= 0);
    }

    /**
     * @param x X origin coordinate.
     * @param y Y origin coordinate.
     * @param width Width
     * @param height Height
     */
    RectType(dim_t x, dim_t y, dim_t width, dim_t height) noexcept
        : m_x(x),
          m_y(y),
          m_width(width),
          m_height(height)
    {
        assert(m_width >= 0);
        assert(m_height >= 0);
    }

    RectType<dim_t, dim_c>& operator+=(const SizeType<dim_t, dim_c>& rhs)
    {
        m_width += rhs.width();
        m_height += rhs.height();
        return *this;
    }

    RectType<dim_t, dim_c>& operator-=(const SizeType<dim_t, dim_c>& rhs)
    {
        m_width -= rhs.width();
        m_height -= rhs.height();
        return *this;
    }

    RectType<dim_t, dim_c>& operator+=(const PointType<dim_t, dim_c>& rhs)
    {
        m_x += rhs.x();
        m_y += rhs.y();
        return *this;
    }

    RectType<dim_t, dim_c>& operator-=(const PointType<dim_t, dim_c>& rhs)
    {
        m_x -= rhs.x();
        m_y -= rhs.y();
        return *this;
    }

    /**
     * Calculate the area of the rectangle.
     */
    inline dim_t area() const
    {
        return m_width * m_height;
    }

    /**
     * Return the center point of the rectangle.
     */
    inline PointType<dim_t, dim_c> center() const
    {
        return PointType<dim_t, dim_c>(m_x + (m_width / 2.), m_y + (m_height / 2.));
    }

    /**
     * Move the rectangle's center to the specified point.
     */
    inline void move_to_center(const PointType<dim_t, dim_c>& center)
    {
        PointType<dim_t, dim_c> pos(center.x() - m_width / 2.,
                                    center.y() - m_height / 2.);

        m_x = pos.x();
        m_y = pos.y();
    }

    /**
     * Grow the rectangle around its center by the specified radius.
     *
     * @param radius Relative radius to change by.
     */
    inline void grow_around_center(dim_t radius)
    {
        m_x -= radius / 2.;
        m_y -= radius / 2.;
        m_width += radius;
        m_height += radius;
    }

    /**
     * Shrink the rectangle around its center by the specified radius.
     *
     * @param radius Relative radius to change by.
     */
    inline void shrink_around_center(dim_t radius)
    {
        m_x += radius / 2.;
        m_y += radius / 2.;
        m_width -= radius;
        m_height -= radius;
    }

    /**
     * Get the PointType of the rectangle.
     */
    inline PointType<dim_t, dim_c> point() const
    {
        return PointType<dim_t, dim_c>(m_x, m_y);
    }

    /**
     * Set the rectangle's origin to the specified point.
     */
    inline void set_point(const PointType<dim_t, dim_c>& p)
    {
        m_x = p.x();
        m_y = p.y();
    }

    /**
     * Get the SizeType of the rectangle.
     */
    inline SizeType<dim_t, dim_c> size() const
    {
        return SizeType<dim_t, dim_c>(m_width, m_height);
    }

    /**
     * Set the SizeType of the rectangle.
     */
    inline void set_size(const SizeType<dim_t, dim_c>& s)
    {
        m_width = s.width();
        m_height = s.height();
    }

    /**
     * Get the top side of the rectangle.
     */
    inline dim_t top() const
    {
        return m_y;
    }

    /**
     * Get the left side the rectangle.
     */
    inline dim_t left() const
    {
        return m_x;
    }

    /**
     * Get the bottom side the rectangle.
     */
    inline dim_t bottom() const
    {
        return m_y + m_height;
    }

    /**
     * Get the right side the rectangle.
     */
    inline dim_t right() const
    {
        return m_x + m_width;
    }

    /**
     * Get the top left point of the rectangle.
     *
     * @note This is usually the origin/point of the rectangle.
     */
    inline PointType<dim_t, dim_c> top_left() const
    {
        return PointType<dim_t, dim_c>(left(), top());
    }

    /**
     * Get the top right point of the rectangle.
     */
    inline PointType<dim_t, dim_c> top_right() const
    {
        return PointType<dim_t, dim_c>(right(), top());
    }

    /**
     * Get the bottom left point of the rectangle.
     */
    inline PointType<dim_t, dim_c> bottom_left() const
    {
        return PointType<dim_t, dim_c>(left(), bottom());
    }

    /**
     * Get the bottom right point of the rectangle.
     */
    inline PointType<dim_t, dim_c> bottom_right() const
    {
        return PointType<dim_t, dim_c>(right(), bottom());
    }

    /**
     * Clear the rectangle by giving it no with or height.
     */
    inline void clear()
    {
        m_x = m_y = m_width = m_height = 0;
    }

    /**
     * Returns true if the rectangle has no width or height.
     */
    inline bool empty() const
    {
        return m_width <= 0 || m_height <= 0;
    }

    /**
     * Returns true if the specified point is inside the rectangle.
     * @param point The point to test.
     */
    inline bool intersect(const PointType<dim_t, dim_c>& point) const
    {
        return (point.x() <= right() && point.x() >= left() &&
                point.y() <= bottom() && point.y() >= top());
    }

    /**
     * Determine if two rectangles intersect, or, overlap.
     * @param rect The rect to test.
     */
    inline bool intersect(const RectType<dim_t, dim_c>& rect) const
    {
        return (x() < rect.x() + rect.width() && x() + width() > rect.x() &&
                y() < rect.y() + rect.height() && y() + height() > rect.y());
    }

    /**
     * Merge two rectangles together into one super rectangle that contains
     * them both.
     */
    static inline RectType<dim_t, dim_c> merge(const RectType<dim_t, dim_c>& lhs, const RectType<dim_t, dim_c>& rhs) noexcept
    {
        auto xmin = std::min(lhs.x(), rhs.x());
        auto xmax = std::max(lhs.x() + lhs.width(),
                             rhs.x() + rhs.width());
        auto ymin = std::min(lhs.y(), rhs.y());
        auto ymax = std::max(lhs.y() + lhs.height(),
                             rhs.y() + rhs.height());

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
        auto x0 = std::max(lhs.x(), rhs.x());
        auto y0 = std::max(lhs.y(), rhs.y());
        auto w0 = std::min(lhs.x() + lhs.width(), rhs.x() + rhs.width()) - x0;
        auto h0 = std::min(lhs.y() + lhs.height(), rhs.y() + rhs.height()) - y0;

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

    constexpr inline dim_t x() const { return m_x; }
    constexpr inline dim_t y() const { return m_y; }

    inline void set_x(dim_t x) { m_x = x; }
    inline void set_y(dim_t y) { m_y = y; }

    constexpr inline dim_t width() const { return m_width; }
    constexpr inline dim_t height() const { return m_height; }

    inline void set_width(dim_t width) { m_width = width; }
    inline void set_height(dim_t height) { m_height = height; }

protected:

    /**
     * X origin coordinate.
     */
    dim_t m_x{0};
    /**
     * Y origin coordinate.
     */
    dim_t m_y{0};
    /**
     * Width
     */
    dim_t m_width{0};
    /**
     * Height
     */
    dim_t m_height{0};
};

template<class dim_t, compatible dim_c>
std::ostream& operator<<(std::ostream& os, const RectType<dim_t, dim_c>& rect)
{
    os << "["  << rect.x() << "," << rect.y() << "-" <<
       rect.width() << "x" << rect.height() << "]";
    return os;
}

template<class dim_t, compatible dim_c>
inline RectType<dim_t, dim_c> operator-(RectType<dim_t, dim_c> lhs, const SizeType<dim_t, dim_c>& rhs)
{
    lhs.set_size(lhs.size() - rhs);
    return lhs;
}

template<class dim_t, compatible dim_c>
inline RectType<dim_t, dim_c> operator+(RectType<dim_t, dim_c> lhs, const SizeType<dim_t, dim_c>& rhs)
{
    lhs.set_size(lhs.size() + rhs);
    return lhs;
}

template<class dim_t, compatible dim_c>
inline RectType<dim_t, dim_c> operator+(RectType<dim_t, dim_c> lhs, const PointType<dim_t, dim_c>& rhs)
{
    lhs.set_point(lhs.point() + rhs);
    return lhs;
}

template<class dim_t, compatible dim_c>
inline RectType<dim_t, dim_c> operator-(RectType<dim_t, dim_c> lhs, const PointType<dim_t, dim_c>& rhs)
{
    lhs.set_point(lhs.point() - rhs);
    return lhs;
}

/// Compares two @c Rect objects for equality.
template<class dim_t, compatible dim_c>
inline bool operator==(const RectType<dim_t, dim_c>& lhs, const RectType<dim_t, dim_c>& rhs)
{
    return lhs.x() == rhs.x() &&
           lhs.y() == rhs.y() &&
           lhs.width() == rhs.width() &&
           lhs.height() == rhs.height();
}

/// Compares two @c Rect objects for inequality.
template<class dim_t, compatible dim_c>
inline bool operator!=(const RectType<dim_t, dim_c>& lhs, const RectType<dim_t, dim_c>& rhs)
{
    return !(lhs == rhs);
}

/**
 * Helper type for a default rect.
 */
using Rect = RectType<default_dim_type, compatible::normal>;
/**
 * Helper type for a default float rect.
 */
using RectF = RectType<float, compatible::normal>;

/**
 * A line, with a starting and ending point.
 * @ingroup geometry
 */
template<class dim_t>
class LineType
{
public:

    /**
     * Helper to reference the dimension type.
     */
    using dim_type = dim_t;

    constexpr explicit LineType(const PointType<dim_t>& start, const PointType<dim_t>& end) noexcept
        : m_start(start),
          m_end(end)
    {}

    inline PointType<dim_t> start() const { return m_start; }
    inline PointType<dim_t> end() const { return m_end; }

    inline void set_start(const PointType<dim_t>& start) { m_start = start; }
    inline void set_end(const PointType<dim_t>& end) { m_end = end; }

    /**
     * Returns a rectangle containing the line.
     */
    inline RectType<dim_t> rect() const
    {
        auto x = std::min(m_start.x(), m_end.x());
        auto y = std::min(m_start.y(), m_end.y());
        auto x2 = std::max(m_start.x(), m_end.x());
        auto y2 = std::max(m_start.y(), m_end.y());

        return RectType<dim_t>(x, y, x2 - x, y2 - y);
    }

protected:
    PointType<dim_t> m_start;
    PointType<dim_t> m_end;
};

/**
 * Helper type for a default line.
 */
using Line = LineType<default_dim_type>;

/**
 * An Arc consists of a radius and two angles.
 * @ingroup geometry
 */
template<class dim_t>
class ArcType
{
public:

    /**
     * Helper to reference the dimension type.
     */
    using dim_type = dim_t;

    /**
     * Construct an Arc object.
     *
     * @param c Center point of the arc.
     * @param r Radius of the arc.
     * @param a1 Angle 1 of the arc in radians.
     * @param a2 Angle 2 of the arc in radians.
     */
    constexpr explicit ArcType(const PointType<dim_t>& c = {}, float r = 0.0f,
                               float a1 = 0.0f, float a2 = 0.0f) noexcept
        : center(c),
          radius(r),
          angle1(a1),
          angle2(a2)
    {
    }

    /**
     * Returns true if the arc has no radius.
     */
    bool empty() const;

    /**
     * Center point of the arc.
     */
    PointType<dim_t> center;

    /**
     * Radius of the arc.
     */
    float radius{0.0f};

    /**
     * Angle of the arc in radians.
     */
    float angle1;

    /**
     * Angle of the arc in radians.
     */
    float angle2;
};

/**
 * Helper type for a default arc.
 */
using Arc = ArcType<default_dim_type>;

/**
 * A basic circle with a center point and radius.
 * @ingroup geometry
 */
template<class dim_t>
class CircleType : public ArcType<dim_t>
{
public:

    /**
     * Helper to reference the dimension type.
     */
    using dim_type = dim_t;

    /**
     * Construct a Circle object.
     *
     * @param c Center point of the arc.
     * @param r Radius of the arc.
     */
    constexpr explicit CircleType(const PointType<dim_t>& c = {}, float r = 0.0f) noexcept
        : ArcType<dim_t>(c, r, 0.0f, 2 * detail::pi<float>())
    {
    }

    /**
     * Get a Rect that covers the circle.
     */
    inline RectType<dim_t> rect() const
    {
        RectType<dim_t> r(this->center, SizeType<dim_t>());
        r.grow_around_center(this->radius);
        return r;
    }
};

/**
 * Helper type for a default circle.
 */
using Circle = CircleType<default_dim_type>;

using CircleF = CircleType<float>;

}
}

#endif
