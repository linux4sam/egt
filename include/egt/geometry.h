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
#include <egt/detail/meta.h>
#include <ostream>
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

namespace detail
{

/**
 * Geometry types compatibility type.
 *
 * This is used to create strong incompatible types as a template argument.
 */
enum class compatible
{
    normal,
    display,
};

}


/**
 * Simple x,y coordinate.
 * @ingroup geometry
 */
template <class dim_t, detail::compatible dim_c = detail::compatible::normal>
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

    inline PointType<dim_t, dim_c>& operator+=(const PointType<dim_t, dim_c>& rhs)
    {
        m_x += rhs.m_x;
        m_y += rhs.m_y;
        return *this;
    }

    inline PointType<dim_t, dim_c>& operator-=(const PointType<dim_t, dim_c>& rhs)
    {
        m_x -= rhs.m_x;
        m_y -= rhs.m_y;
        return *this;
    }

    inline PointType<dim_t, dim_c>& operator*=(const PointType<dim_t, dim_c>& rhs)
    {
        m_x *= rhs.m_x;
        m_y *= rhs.m_y;
        return *this;
    }

    inline PointType<dim_t, dim_c>& operator/=(const PointType<dim_t, dim_c>& rhs)
    {
        m_x /= rhs.m_x;
        m_y /= rhs.m_y;
        return *this;
    }

    template <class T>
    inline PointType<dim_t, dim_c>& operator+=(const T& rhs)
    {
        m_x += rhs;
        m_y += rhs;
        return *this;
    }

    template <class T>
    inline PointType<dim_t, dim_c>& operator-=(const T& rhs)
    {
        m_x -= rhs;
        m_y -= rhs;
        return *this;
    }

    template <class T>
    inline PointType<dim_t, dim_c>& operator*=(const T& rhs)
    {
        m_x *= rhs;
        m_y *= rhs;
        return *this;
    }

    template <class T>
    inline PointType<dim_t, dim_c>& operator/=(const T& rhs)
    {
        m_x /= rhs;
        m_y /= rhs;
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
template<class dim_t, detail::compatible dim_c>
inline bool operator==(const PointType<dim_t, dim_c>& lhs, const PointType<dim_t, dim_c>& rhs)
{
    return lhs.x() == rhs.x() && lhs.y() == rhs.y();
}

template<detail::compatible dim_c>
inline bool operator==(const PointType<float, dim_c>& lhs, const PointType<float, dim_c>& rhs)
{
    return detail::float_compare(lhs.x(), rhs.x()) &&
           detail::float_compare(lhs.y(), rhs.y());
}

/// Compares two @c PointType objects for inequality.
template<class dim_t, detail::compatible dim_c>
inline bool operator!=(const PointType<dim_t, dim_c>& lhs, const PointType<dim_t, dim_c>& rhs)
{
    return !(lhs == rhs);
}

template<class dim_t, detail::compatible dim_c>
inline PointType<dim_t, dim_c> operator-(PointType<dim_t, dim_c> lhs, const PointType<dim_t, dim_c>& rhs)
{
    lhs -= rhs;
    return lhs;
}

template<class dim_t, detail::compatible dim_c>
inline PointType<dim_t, dim_c> operator+(PointType<dim_t, dim_c> lhs, const PointType<dim_t, dim_c>& rhs)
{
    lhs += rhs;
    return lhs;
}

template<class dim_t, detail::compatible dim_c>
inline PointType<dim_t, dim_c> operator/(PointType<dim_t, dim_c> lhs, const PointType<dim_t, dim_c>& rhs)
{
    lhs /= rhs;
    return lhs;
}

template<class dim_t, detail::compatible dim_c>
inline PointType<dim_t, dim_c> operator*(PointType<dim_t, dim_c> lhs, const PointType<dim_t, dim_c>& rhs)
{
    lhs *= rhs;
    return lhs;
}

template<class dim_t, detail::compatible dim_c, class T>
inline PointType<dim_t, dim_c> operator-(PointType<dim_t, dim_c> lhs, const T& rhs)
{
    lhs -= rhs;
    return lhs;
}

template<class dim_t, detail::compatible dim_c, class T>
inline PointType<dim_t, dim_c> operator+(PointType<dim_t, dim_c> lhs, const T& rhs)
{
    lhs += rhs;
    return lhs;
}

template<class dim_t, detail::compatible dim_c, class T>
inline PointType<dim_t, dim_c> operator/(PointType<dim_t, dim_c> lhs, const T& rhs)
{
    lhs /= rhs;
    return lhs;
}

template<class dim_t, detail::compatible dim_c, class T>
inline PointType<dim_t, dim_c> operator*(PointType<dim_t, dim_c> lhs, const T& rhs)
{
    lhs *= rhs;
    return lhs;
}

template<class dim_t, detail::compatible dim_c>
std::ostream& operator<<(std::ostream& os, const PointType<dim_t, dim_c>& point)
{
    os << "[" << point.x() << "," << point.y() << "]";
    return os;
}

/**
 * Helper type for a default point.
 */
using Point = PointType<default_dim_type, detail::compatible::normal>;

static_assert(detail::rule_of_5<Point>(),
              "Point : must fulfill rule of 5");

/**
 * Helper type for a floating point.
 */
using PointF = PointType<float, detail::compatible::normal>;

/**
 * Helper type for a default display point.
 */
using DisplayPoint = PointType<default_dim_type, detail::compatible::display>;

/**
 * Simple width and height.
 * @ingroup geometry
 */
template<class dim_t, detail::compatible dim_c = detail::compatible::normal>
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
    constexpr SizeType(dim_t width, dim_t height) noexcept
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

    template<class T>
    SizeType<dim_t, dim_c>& operator+=(const T& rhs)
    {
        m_width += rhs;
        m_height += rhs;
        return *this;
    }

    template<class T>
    SizeType<dim_t, dim_c>& operator-=(const T& rhs)
    {
        m_width -= rhs;
        m_height -= rhs;
        return *this;
    }

    template<class T>
    SizeType<dim_t, dim_c>& operator*=(const T& rhs)
    {
        m_width *= rhs;
        m_height *= rhs;
        return *this;
    }

    template<class T>
    SizeType<dim_t, dim_c>& operator/=(const T& rhs)
    {
        m_width /= rhs;
        m_height /= rhs;
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
template<class dim_t, detail::compatible dim_c>
inline bool operator==(const SizeType<dim_t, dim_c>& lhs, const SizeType<dim_t, dim_c>& rhs)
{
    return lhs.width() == rhs.width() && lhs.height() == rhs.height();
}

template<detail::compatible dim_c>
inline bool operator==(const SizeType<float, dim_c>& lhs, const SizeType<float, dim_c>& rhs)
{
    return detail::float_compare(lhs.width(), rhs.width()) &&
           detail::float_compare(lhs.height(), rhs.height());
}

/// Compares two @c SizeType objects for inequality.
template<class dim_t, detail::compatible dim_c>
inline bool operator!=(const SizeType<dim_t, dim_c>& lhs, const SizeType<dim_t, dim_c>& rhs)
{
    return !(lhs == rhs);
}

template<class dim_t, detail::compatible dim_c>
inline SizeType<dim_t, dim_c> operator-(SizeType<dim_t, dim_c> lhs, const SizeType<dim_t, dim_c>& rhs)
{
    lhs -= rhs;
    return lhs;
}

template<class dim_t, detail::compatible dim_c>
inline SizeType<dim_t, dim_c> operator+(SizeType<dim_t, dim_c> lhs, const SizeType<dim_t, dim_c>& rhs)
{
    lhs += rhs;
    return lhs;
}

template<class dim_t, detail::compatible dim_c>
inline SizeType<dim_t, dim_c> operator*(SizeType<dim_t, dim_c> lhs, const SizeType<dim_t, dim_c>& rhs)
{
    lhs *= rhs;
    return lhs;
}

template<class dim_t, detail::compatible dim_c>
inline SizeType<dim_t, dim_c> operator/(SizeType<dim_t, dim_c> lhs, const SizeType<dim_t, dim_c>& rhs)
{
    lhs /= rhs;
    return lhs;
}

template<class dim_t, detail::compatible dim_c, class T>
inline SizeType<dim_t, dim_c> operator-(SizeType<dim_t, dim_c> lhs, const T& rhs)
{
    lhs -= rhs;
    return lhs;
}

template<class dim_t, detail::compatible dim_c, class T>
inline SizeType<dim_t, dim_c> operator+(SizeType<dim_t, dim_c> lhs, const T& rhs)
{
    lhs += rhs;
    return lhs;
}

template<class dim_t, detail::compatible dim_c, class T>
inline SizeType<dim_t, dim_c> operator*(SizeType<dim_t, dim_c> lhs, const T& rhs)
{
    lhs *= rhs;
    return lhs;
}

template<class dim_t, detail::compatible dim_c, class T>
inline SizeType<dim_t, dim_c> operator/(SizeType<dim_t, dim_c> lhs, const T& rhs)
{
    lhs /= rhs;
    return lhs;
}

template<class dim_t, detail::compatible dim_c>
std::ostream& operator<<(std::ostream& os, const SizeType<dim_t, dim_c>& size)
{
    os << size.width() << "x" << size.height();
    return os;
}

/**
 * Helper type for a default size.
 */
using Size = SizeType<default_dim_type, detail::compatible::normal>;

static_assert(detail::rule_of_5<Size>(),
              "Size : must fulfill rule of 5");

/**
 * Helper type for a float size.
 */
using SizeF = SizeType<float, detail::compatible::normal>;

/**
 * A rectangle.
 * @ingroup geometry
 *
 * A point and a size.
 *
 * All rectangle points are at the top left.
 */
template<class dim_t, detail::compatible dim_c = detail::compatible::normal>
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
        : m_origin(point),
          m_size(size)
    {
        assert(width() >= 0);
        assert(height() >= 0);
    }

    /**
     * Construct a rectangle with the specified size and the point being at
     * the default origin 0,0.
     */
    // cppcheck-suppress noExplicitConstructor
    RectType(const SizeType<dim_t, dim_c>& size) noexcept
        : m_size(size)
    {
        assert(width() >= 0);
        assert(height() >= 0);
    }

    /**
     * @param x X origin coordinate.
     * @param y Y origin coordinate.
     * @param width Width
     * @param height Height
     */
    RectType(dim_t x, dim_t y, dim_t width, dim_t height) noexcept
        : m_origin(x, y),
          m_size(width, height)
    {
        assert(width >= 0);
        assert(height >= 0);
    }

    RectType<dim_t, dim_c>& operator+=(const SizeType<dim_t, dim_c>& rhs)
    {
        m_size += rhs;
        return *this;
    }

    RectType<dim_t, dim_c>& operator-=(const SizeType<dim_t, dim_c>& rhs)
    {
        m_size -= rhs;
        return *this;
    }

    RectType<dim_t, dim_c>& operator+=(const PointType<dim_t, dim_c>& rhs)
    {
        m_origin += rhs;
        return *this;
    }

    RectType<dim_t, dim_c>& operator-=(const PointType<dim_t, dim_c>& rhs)
    {
        m_origin -= rhs;
        return *this;
    }

    /**
     * Calculate the area of the rectangle.
     */
    inline dim_t area() const
    {
        return width() * height();
    }

    /**
     * Return the center point of the rectangle.
     */
    inline PointType<dim_t, dim_c> center() const
    {
        return PointType<dim_t, dim_c>(x() + (width() / 2.), y() + (height() / 2.));
    }

    /**
     * Move the rectangle's center to the specified point.
     */
    inline void move_to_center(const PointType<dim_t, dim_c>& center)
    {
        const PointType<dim_t, dim_c> pos(center.x() - width() / 2.,
                                          center.y() - height() / 2.);

        m_origin = pos;
    }

    /**
     * Grow the rectangle around its center by the specified radius.
     *
     * @param radius Relative radius to change by.
     */
    inline void grow_around_center(dim_t radius)
    {
        m_origin -= (radius / 2);
        m_size += radius;
    }

    /**
     * Shrink the rectangle around its center by the specified radius.
     *
     * @param radius Relative radius to change by.
     */
    inline void shrink_around_center(dim_t radius)
    {
        m_origin += (radius / 2);
        m_size -= radius;
    }

    /**
     * Get the PointType of the rectangle.
     */
    inline PointType<dim_t, dim_c> point() const
    {
        return m_origin;
    }

    /**
     * Set the rectangle's origin to the specified point.
     */
    inline void set_point(const PointType<dim_t, dim_c>& p)
    {
        m_origin = p;
    }

    /**
     * Get the SizeType of the rectangle.
     */
    inline SizeType<dim_t, dim_c> size() const
    {
        return m_size;
    }

    /**
     * Set the SizeType of the rectangle.
     */
    inline void set_size(const SizeType<dim_t, dim_c>& size)
    {
        m_size = size;
    }

    /**
     * Get the top side of the rectangle.
     */
    constexpr inline dim_t top() const
    {
        return m_origin.y();
    }

    /**
     * Get the left side the rectangle.
     */
    constexpr inline dim_t left() const
    {
        return m_origin.x();
    }

    /**
     * Get the bottom side the rectangle.
     */
    constexpr inline dim_t bottom() const
    {
        return top() + height();
    }

    /**
     * Get the right side the rectangle.
     */
    constexpr inline dim_t right() const
    {
        return left() + width();
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
        m_origin = {};
        m_size = {};
    }

    /**
     * Returns true if the rectangle has no width or height.
     */
    inline bool empty() const
    {
        return width() <= 0 || height() <= 0;
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
    static inline RectType<dim_t, dim_c> merge(const RectType<dim_t, dim_c>& lhs,
            const RectType<dim_t, dim_c>& rhs) noexcept
    {
        const auto xmin = std::min(lhs.x(), rhs.x());
        const auto xmax = std::max(lhs.x() + lhs.width(),
                                   rhs.x() + rhs.width());
        const auto ymin = std::min(lhs.y(), rhs.y());
        const auto ymax = std::max(lhs.y() + lhs.height(),
                                   rhs.y() + rhs.height());

        return RectType<dim_t, dim_c>(xmin, ymin, xmax - xmin, ymax - ymin);
    }

    /**
     * Return the intersecting rectangle of two rectangles, if any.
     */
    static inline RectType<dim_t, dim_c> intersection(const RectType<dim_t, dim_c>& lhs,
            const RectType<dim_t, dim_c>& rhs) noexcept
    {
        const auto x0 = std::max(lhs.x(), rhs.x());
        const auto y0 = std::max(lhs.y(), rhs.y());
        const auto w0 = std::min(lhs.x() + lhs.width(), rhs.x() + rhs.width()) - x0;
        const auto h0 = std::min(lhs.y() + lhs.height(), rhs.y() + rhs.height()) - y0;

        if (w0 < 0 || h0 < 0)
            return RectType<dim_t, dim_c>();

        return RectType<dim_t, dim_c>(x0, y0, w0, h0);
    }

    /**
     * Returns true if this rectangle contains the specified one.
     *
     * This is different than intersect().  The specified rectangle must be
     * completely inside this one.
     */
    inline bool contains(const RectType<dim_t, dim_c>& rhs) const
    {
        return rhs.right() < right() &&
               rhs.bottom() < bottom() &&
               rhs.left() > left() &&
               rhs.top() > top();
    }

    constexpr inline dim_t x() const { return m_origin.x(); }
    constexpr inline dim_t y() const { return m_origin.y(); }

    inline void set_x(dim_t x) { m_origin.set_x(x); }
    inline void set_y(dim_t y) { m_origin.set_y(y); }

    constexpr inline dim_t width() const { return m_size.width(); }
    constexpr inline dim_t height() const { return m_size.height(); }

    inline void set_width(dim_t width) { m_size.set_width(width); }
    inline void set_height(dim_t height) { m_size.set_height(height); }

protected:

    /** Origin */
    PointType<dim_t, dim_c> m_origin;
    /** Size */
    SizeType<dim_t, dim_c> m_size;
};

template<class dim_t, detail::compatible dim_c>
std::ostream& operator<<(std::ostream& os, const RectType<dim_t, dim_c>& rect)
{
    os << "["  << rect.x() << "," << rect.y() << "-" <<
       rect.width() << "x" << rect.height() << "]";
    return os;
}

template<class dim_t, detail::compatible dim_c>
inline RectType<dim_t, dim_c> operator-(RectType<dim_t, dim_c> lhs, const SizeType<dim_t, dim_c>& rhs)
{
    lhs.set_size(lhs.size() - rhs);
    return lhs;
}

template<class dim_t, detail::compatible dim_c>
inline RectType<dim_t, dim_c> operator+(RectType<dim_t, dim_c> lhs, const SizeType<dim_t, dim_c>& rhs)
{
    lhs.set_size(lhs.size() + rhs);
    return lhs;
}

template<class dim_t, detail::compatible dim_c>
inline RectType<dim_t, dim_c> operator*(RectType<dim_t, dim_c> lhs, const SizeType<dim_t, dim_c>& rhs)
{
    lhs.set_size(lhs.size() * rhs);
    return lhs;
}

template<class dim_t, detail::compatible dim_c>
inline RectType<dim_t, dim_c> operator/(RectType<dim_t, dim_c> lhs, const SizeType<dim_t, dim_c>& rhs)
{
    lhs.set_size(lhs.size() / rhs);
    return lhs;
}

template<class dim_t, detail::compatible dim_c>
inline RectType<dim_t, dim_c> operator+(RectType<dim_t, dim_c> lhs, const PointType<dim_t, dim_c>& rhs)
{
    lhs.set_point(lhs.point() + rhs);
    return lhs;
}

template<class dim_t, detail::compatible dim_c>
inline RectType<dim_t, dim_c> operator-(RectType<dim_t, dim_c> lhs, const PointType<dim_t, dim_c>& rhs)
{
    lhs.set_point(lhs.point() - rhs);
    return lhs;
}

template<class dim_t, detail::compatible dim_c>
inline RectType<dim_t, dim_c> operator*(RectType<dim_t, dim_c> lhs, const PointType<dim_t, dim_c>& rhs)
{
    lhs.set_point(lhs.point() * rhs);
    return lhs;
}

template<class dim_t, detail::compatible dim_c>
inline RectType<dim_t, dim_c> operator/(RectType<dim_t, dim_c> lhs, const PointType<dim_t, dim_c>& rhs)
{
    lhs.set_point(lhs.point() / rhs);
    return lhs;
}

/// Compares two @c Rect objects for equality.
template<class dim_t, detail::compatible dim_c>
inline bool operator==(const RectType<dim_t, dim_c>& lhs, const RectType<dim_t, dim_c>& rhs)
{
    return lhs.point() == rhs.point() &&
           lhs.size() == rhs.size();
}

/// Compares two @c Rect objects for inequality.
template<class dim_t, detail::compatible dim_c>
inline bool operator!=(const RectType<dim_t, dim_c>& lhs, const RectType<dim_t, dim_c>& rhs)
{
    return !(lhs == rhs);
}

/**
 * Helper type for a default rect.
 */
using Rect = RectType<default_dim_type, detail::compatible::normal>;

static_assert(detail::rule_of_5<Rect>(),
              "Rect : must fulfill rule of 5");

/**
 * Helper type for a default float rect.
 */
using RectF = RectType<float, detail::compatible::normal>;

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
        const auto x = std::min(m_start.x(), m_end.x());
        const auto y = std::min(m_start.y(), m_end.y());
        const auto x2 = std::max(m_start.x(), m_end.x());
        const auto y2 = std::max(m_start.y(), m_end.y());

        return RectType<dim_t>(x, y, x2 - x, y2 - y);
    }

protected:
    PointType<dim_t> m_start;
    PointType<dim_t> m_end;
};

template<class dim_t>
std::ostream& operator<<(std::ostream& os, const LineType<dim_t>& line)
{
    os << "[" << line.start() << "-" << line.end() << "]";
    return os;
}

/**
 * Helper type for a default line.
 */
using Line = LineType<default_dim_type>;

static_assert(detail::rule_of_5<Line>(),
              "Line : must fulfill rule of 5");

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
     * @param center Center point of the arc.
     * @param radius Radius of the arc.
     * @param angle1 Angle 1 of the arc in radians.
     * @param angle2 Angle 2 of the arc in radians.
     */
    constexpr explicit ArcType(const PointType<dim_t>& center = {}, dim_t radius = {},
                               float angle1 = 0.0f, float angle2 = 0.0f) noexcept
        : m_center(center),
          m_radius(radius),
          m_angle1(angle1),
          m_angle2(angle2)
    {
    }

    /**
     * Returns true if the arc has no radius.
     */
    bool empty() const;

    inline void set_radius(dim_t radius) { m_radius = radius; }
    inline void set_angle1(float angle) { m_angle1 = angle; }
    inline void set_angle2(float angle) { m_angle2 = angle; }
    inline void set_center(const PointType<dim_t>& center) { m_center = center; }

    constexpr inline dim_t radius() const { return m_radius; }
    constexpr inline float angle1() const { return m_angle1; }
    constexpr inline float angle2() const { return m_angle2; }
    constexpr inline PointType<dim_t> center() const { return m_center; }

protected:

    /**
     * Center point of the arc.
     */
    PointType<dim_t> m_center;

    /**
     * Radius of the arc.
     */
    dim_t m_radius{0};

    /**
     * Angle of the arc in radians.
     */
    float m_angle1{};

    /**
     * Angle of the arc in radians.
     */
    float m_angle2{};
};

/**
 * Helper type for a default arc.
 */
using Arc = ArcType<default_dim_type>;

static_assert(detail::rule_of_5<Arc>(),
              "Arc : must fulfill rule of 5");

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
     * @param center Center point of the arc.
     * @param radius Radius of the arc.
     */
    constexpr explicit CircleType(const PointType<dim_t>& center = {}, dim_t radius = {}) noexcept
        : ArcType<dim_t>(center, radius, 0, 2.f * detail::pi<float>())
    {
    }

    /**
     * Get a Rect that covers the circle.
     */
    inline RectType<dim_t> rect() const
    {
        RectType<dim_t> r(this->center(), SizeType<dim_t>());
        r.grow_around_center(this->radius());
        return r;
    }
};

template<class dim_t>
std::ostream& operator<<(std::ostream& os, const CircleType<dim_t>& circle)
{
    os << "[" << circle.center() << "-" << circle.radius() << "]";
    return os;
}

/**
 * Helper type for a default circle.
 */
using Circle = CircleType<default_dim_type>;

static_assert(detail::rule_of_5<Circle>(),
              "Circle : must fulfill rule of 5");

using CircleF = CircleType<float>;

template<class dim_t>
class EllipseType
{
public:
    /**
    * Helper to reference the dimension type.
    */
    using dim_type = dim_t;

    constexpr EllipseType(const PointType<dim_t>& center = {},
                          dim_t radiusa = {},
                          dim_t radiusb = {}) noexcept
        : m_center(center),
          m_radiusa(radiusa),
          m_radiusb(radiusb)
    {
    }

    /**
     * Get the total perimeter of the ellipse.
     *
     * This is technically just an approximation.
     */
    inline dim_t perimeter() const
    {
        return 2.f * detail::pi<float>() * std::sqrt((m_radiusa * m_radiusa +
                m_radiusb * m_radiusb) / 2.f);
    }

    /**
     * @param angle The angle in degrees on the ellipse, with the center in the
     * middle of the ellipse.
     */
    inline PointType<dim_t> point_on_perimeter(float angle)
    {
        const auto x = m_radiusa * std::cos(detail::to_radians(0.f, angle));
        const auto y = m_radiusb * std::sin(detail::to_radians(0.f, angle));
        return center() + PointType<dim_t>(x, y);
    }

    inline void set_radiusa(const dim_t radiusa) { m_radiusa = radiusa; }
    inline void set_radiusb(const dim_t radiusb) { m_radiusb = radiusb; }
    inline void set_center(const PointType<dim_t>& center) { m_center = center; }

    constexpr inline dim_t radiusa() const { return m_radiusa; }
    constexpr inline dim_t radiusb() const { return m_radiusb; }
    constexpr inline PointType<dim_t> center() const { return m_center; }

protected:

    /**
     * Center point of the arc.
     */
    PointType<dim_t> m_center;

    /**
     * A radius.
     */
    dim_t m_radiusa{0};

    /**
     * B radius.
     */
    dim_t m_radiusb{0};
};

template<class dim_t>
std::ostream& operator<<(std::ostream& os, const EllipseType<dim_t>& ellipse)
{
    os << "[" << ellipse.center() << "-" << ellipse.radiusa() <<
       "-" << ellipse.radiusb() << "]";
    return os;
}

}
}

#endif
