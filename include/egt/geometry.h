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
 * Define the default dimension type used for geometry.
 */
using DefaultDim = int;

namespace detail
{

/**
 * Geometry types compatibility type.
 *
 * This is used to create strong incompatible types as a template argument.
 */
enum class Compatible
{
    normal,
    display,
};

}


/**
 * Simple x,y coordinate.
 * @ingroup geometry
 */
template <class Dim, detail::Compatible DimCompat = detail::Compatible::normal>
class PointType
{
public:

    /**
     * Helper to reference the dimension type.
     */
    using DimType = Dim;

    constexpr PointType() noexcept = default;

    constexpr PointType(Dim x, Dim y) noexcept
        : m_x(x),
          m_y(y)
    {}

    /// PointType operator
    inline PointType<Dim, DimCompat>& operator+=(const PointType<Dim, DimCompat>& rhs)
    {
        m_x += rhs.m_x;
        m_y += rhs.m_y;
        return *this;
    }

    /// PointType operator
    inline PointType<Dim, DimCompat>& operator-=(const PointType<Dim, DimCompat>& rhs)
    {
        m_x -= rhs.m_x;
        m_y -= rhs.m_y;
        return *this;
    }

    /// PointType operator
    inline PointType<Dim, DimCompat>& operator*=(const PointType<Dim, DimCompat>& rhs)
    {
        m_x *= rhs.m_x;
        m_y *= rhs.m_y;
        return *this;
    }

    /// PointType operator
    inline PointType<Dim, DimCompat>& operator/=(const PointType<Dim, DimCompat>& rhs)
    {
        m_x /= rhs.m_x;
        m_y /= rhs.m_y;
        return *this;
    }

    /// PointType operator
    template <class T>
    inline PointType<Dim, DimCompat>& operator+=(const T& rhs)
    {
        m_x += rhs;
        m_y += rhs;
        return *this;
    }

    /// PointType operator
    template <class T>
    inline PointType<Dim, DimCompat>& operator-=(const T& rhs)
    {
        m_x -= rhs;
        m_y -= rhs;
        return *this;
    }

    /// PointType operator
    template <class T>
    inline PointType<Dim, DimCompat>& operator*=(const T& rhs)
    {
        m_x *= rhs;
        m_y *= rhs;
        return *this;
    }

    /// PointType operator
    template <class T>
    inline PointType<Dim, DimCompat>& operator/=(const T& rhs)
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
    PointType<Dim, DimCompat> point_on_circumference(T radius, T angle) const
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
    T angle_to(const PointType<Dim, DimCompat>& point) const
    {
        return std::atan2(point.m_x - m_x, m_y - point.m_y);
    }

    /**
     * Calculate the straight line distance to another point.
     *
     * @param point The other point.
     */
    Dim distance_to(const PointType<Dim, DimCompat>& point) const
    {
        return std::hypot(point.m_x - m_x, point.m_y - m_y);
    }

    constexpr inline Dim x() const { return m_x; }
    constexpr inline Dim y() const { return m_y; }

    inline void x(Dim x) { m_x = x; }
    inline void y(Dim y) { m_y = y; }

protected:
    Dim m_x{0};
    Dim m_y{0};
};

/// PointType operator
template<class Dim, detail::Compatible DimCompat>
inline bool operator==(const PointType<Dim, DimCompat>& lhs, const PointType<Dim, DimCompat>& rhs)
{
    return lhs.x() == rhs.x() && lhs.y() == rhs.y();
}

/// PointType operator
template<detail::Compatible DimCompat>
inline bool operator==(const PointType<float, DimCompat>& lhs, const PointType<float, DimCompat>& rhs)
{
    return detail::float_equal(lhs.x(), rhs.x()) &&
           detail::float_equal(lhs.y(), rhs.y());
}

/// PointType operator
template<class Dim, detail::Compatible DimCompat>
inline bool operator!=(const PointType<Dim, DimCompat>& lhs, const PointType<Dim, DimCompat>& rhs)
{
    return !(lhs == rhs);
}

/// PointType operator
template<class Dim, detail::Compatible DimCompat>
inline PointType<Dim, DimCompat> operator-(PointType<Dim, DimCompat> lhs, const PointType<Dim, DimCompat>& rhs)
{
    lhs -= rhs;
    return lhs;
}

/// PointType operator
template<class Dim, detail::Compatible DimCompat>
inline PointType<Dim, DimCompat> operator+(PointType<Dim, DimCompat> lhs, const PointType<Dim, DimCompat>& rhs)
{
    lhs += rhs;
    return lhs;
}

/// PointType operator
template<class Dim, detail::Compatible DimCompat>
inline PointType<Dim, DimCompat> operator/(PointType<Dim, DimCompat> lhs, const PointType<Dim, DimCompat>& rhs)
{
    lhs /= rhs;
    return lhs;
}

/// PointType operator
template<class Dim, detail::Compatible DimCompat>
inline PointType<Dim, DimCompat> operator*(PointType<Dim, DimCompat> lhs, const PointType<Dim, DimCompat>& rhs)
{
    lhs *= rhs;
    return lhs;
}

/// PointType operator
template<class Dim, detail::Compatible DimCompat, class T>
inline PointType<Dim, DimCompat> operator-(PointType<Dim, DimCompat> lhs, const T& rhs)
{
    lhs -= rhs;
    return lhs;
}

/// PointType operator
template<class Dim, detail::Compatible DimCompat, class T>
inline PointType<Dim, DimCompat> operator+(PointType<Dim, DimCompat> lhs, const T& rhs)
{
    lhs += rhs;
    return lhs;
}

/// PointType operator
template<class Dim, detail::Compatible DimCompat, class T>
inline PointType<Dim, DimCompat> operator/(PointType<Dim, DimCompat> lhs, const T& rhs)
{
    lhs /= rhs;
    return lhs;
}

/// PointType operator
template<class Dim, detail::Compatible DimCompat, class T>
inline PointType<Dim, DimCompat> operator*(PointType<Dim, DimCompat> lhs, const T& rhs)
{
    lhs *= rhs;
    return lhs;
}

/// Overloaded std::ostream insertion operator
template<class Dim, detail::Compatible DimCompat>
std::ostream& operator<<(std::ostream& os, const PointType<Dim, DimCompat>& point)
{
    os << "[" << point.x() << "," << point.y() << "]";
    return os;
}

/**
 * Helper type for a default point.
 */
using Point = PointType<DefaultDim, detail::Compatible::normal>;

static_assert(detail::rule_of_5<Point>(),
              "Point : must fulfill rule of 5");

/**
 * Helper type for a float point.
 */
using PointF = PointType<float, detail::Compatible::normal>;

/**
 * Helper type for a default display point.
 */
using DisplayPoint = PointType<DefaultDim, detail::Compatible::display>;

/**
 * Simple width and height.
 * @ingroup geometry
 */
template<class Dim, detail::Compatible DimCompat = detail::Compatible::normal>
class SizeType
{
public:

    /**
     * Helper to reference the dimension type.
     */
    using DimType = Dim;

    constexpr SizeType() noexcept = default;

    /**
     * @param width Width
     * @param height Height
     */
    constexpr SizeType(Dim width, Dim height) noexcept
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

    /// SizeType operator
    SizeType<Dim, DimCompat>& operator+=(const SizeType<Dim, DimCompat>& rhs)
    {
        m_width += rhs.m_width;
        m_height += rhs.m_height;
        return *this;
    }

    /// SizeType operator
    SizeType<Dim, DimCompat>& operator-=(const SizeType<Dim, DimCompat>& rhs)
    {
        m_width -= rhs.m_width;
        m_height -= rhs.m_height;
        return *this;
    }

    /// SizeType operator
    SizeType<Dim, DimCompat>& operator*=(const SizeType<Dim, DimCompat>& rhs)
    {
        m_width *= rhs.m_width;
        m_height *= rhs.m_height;
        return *this;
    }

    /// SizeType operator
    SizeType<Dim, DimCompat>& operator/=(const SizeType<Dim, DimCompat>& rhs)
    {
        m_width /= rhs.m_width;
        m_height /= rhs.m_height;
        return *this;
    }

    /// SizeType operator
    template<class T>
    SizeType<Dim, DimCompat>& operator+=(const T& rhs)
    {
        m_width += rhs;
        m_height += rhs;
        return *this;
    }

    /// SizeType operator
    template<class T>
    SizeType<Dim, DimCompat>& operator-=(const T& rhs)
    {
        m_width -= rhs;
        m_height -= rhs;
        return *this;
    }

    /// SizeType operator
    template<class T>
    SizeType<Dim, DimCompat>& operator*=(const T& rhs)
    {
        m_width *= rhs;
        m_height *= rhs;
        return *this;
    }

    /// SizeType operator
    template<class T>
    SizeType<Dim, DimCompat>& operator/=(const T& rhs)
    {
        m_width /= rhs;
        m_height /= rhs;
        return *this;
    }

    constexpr inline Dim width() const { return m_width; }
    constexpr inline Dim height() const { return m_height; }

    inline void width(Dim width) { m_width = width; }
    inline void height(Dim height) { m_height = height; }

protected:
    /**
     * Height
     */
    Dim m_height{0};

    /**
     * Width
     */
    Dim m_width{0};
};

/// SizeType operator
template<class Dim, detail::Compatible DimCompat>
inline bool operator==(const SizeType<Dim, DimCompat>& lhs, const SizeType<Dim, DimCompat>& rhs)
{
    return lhs.width() == rhs.width() && lhs.height() == rhs.height();
}

/// SizeType operator
template<detail::Compatible DimCompat>
inline bool operator==(const SizeType<float, DimCompat>& lhs, const SizeType<float, DimCompat>& rhs)
{
    return detail::float_equal(lhs.width(), rhs.width()) &&
           detail::float_equal(lhs.height(), rhs.height());
}

/// SizeType operator
template<class Dim, detail::Compatible DimCompat>
inline bool operator!=(const SizeType<Dim, DimCompat>& lhs, const SizeType<Dim, DimCompat>& rhs)
{
    return !(lhs == rhs);
}

/// SizeType operator
template<class Dim, detail::Compatible DimCompat>
inline SizeType<Dim, DimCompat> operator-(SizeType<Dim, DimCompat> lhs, const SizeType<Dim, DimCompat>& rhs)
{
    lhs -= rhs;
    return lhs;
}

/// SizeType operator
template<class Dim, detail::Compatible DimCompat>
inline SizeType<Dim, DimCompat> operator+(SizeType<Dim, DimCompat> lhs, const SizeType<Dim, DimCompat>& rhs)
{
    lhs += rhs;
    return lhs;
}

/// SizeType operator
template<class Dim, detail::Compatible DimCompat>
inline SizeType<Dim, DimCompat> operator*(SizeType<Dim, DimCompat> lhs, const SizeType<Dim, DimCompat>& rhs)
{
    lhs *= rhs;
    return lhs;
}

/// SizeType operator
template<class Dim, detail::Compatible DimCompat>
inline SizeType<Dim, DimCompat> operator/(SizeType<Dim, DimCompat> lhs, const SizeType<Dim, DimCompat>& rhs)
{
    lhs /= rhs;
    return lhs;
}

/// SizeType operator
template<class Dim, detail::Compatible DimCompat, class T>
inline SizeType<Dim, DimCompat> operator-(SizeType<Dim, DimCompat> lhs, const T& rhs)
{
    lhs -= rhs;
    return lhs;
}

/// SizeType operator
template<class Dim, detail::Compatible DimCompat, class T>
inline SizeType<Dim, DimCompat> operator+(SizeType<Dim, DimCompat> lhs, const T& rhs)
{
    lhs += rhs;
    return lhs;
}

/// SizeType operator
template<class Dim, detail::Compatible DimCompat, class T>
inline SizeType<Dim, DimCompat> operator*(SizeType<Dim, DimCompat> lhs, const T& rhs)
{
    lhs *= rhs;
    return lhs;
}

/// SizeType operator
template<class Dim, detail::Compatible DimCompat, class T>
inline SizeType<Dim, DimCompat> operator/(SizeType<Dim, DimCompat> lhs, const T& rhs)
{
    lhs /= rhs;
    return lhs;
}

/// Overloaded std::ostream insertion operator
template<class Dim, detail::Compatible DimCompat>
std::ostream& operator<<(std::ostream& os, const SizeType<Dim, DimCompat>& size)
{
    os << size.width() << "x" << size.height();
    return os;
}

/**
 * Helper type for a default size.
 */
using Size = SizeType<DefaultDim, detail::Compatible::normal>;

static_assert(detail::rule_of_5<Size>(),
              "Size : must fulfill rule of 5");

/**
 * Helper type for a float size.
 */
using SizeF = SizeType<float, detail::Compatible::normal>;

/**
 * A rectangle.
 * @ingroup geometry
 *
 * A point and a size.
 *
 * All rectangle points are at the top left.
 */
template<class Dim, detail::Compatible DimCompat = detail::Compatible::normal>
class RectType
{
public:

    /**
     * Helper to reference the dimension type.
     */
    using DimType = Dim;

    constexpr RectType() noexcept = default;

    /**
     * Construct a rectangle with an explicit point and size.
     */
    explicit RectType(const PointType<Dim>& point, const SizeType<Dim, DimCompat>& size) noexcept
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
    RectType(const SizeType<Dim, DimCompat>& size) noexcept
        : RectType( {}, size)
    {}

    /**
     * @param x X origin coordinate.
     * @param y Y origin coordinate.
     * @param width Width
     * @param height Height
     */
    RectType(Dim x, Dim y, Dim width, Dim height) noexcept
        : RectType( {x, y}, {width, height})
    {}

    /// RectType operator
    RectType<Dim, DimCompat>& operator+=(const SizeType<Dim, DimCompat>& rhs)
    {
        m_size += rhs;
        return *this;
    }

    /// RectType operator
    RectType<Dim, DimCompat>& operator-=(const SizeType<Dim, DimCompat>& rhs)
    {
        m_size -= rhs;
        return *this;
    }

    /// RectType operator
    RectType<Dim, DimCompat>& operator+=(const PointType<Dim, DimCompat>& rhs)
    {
        m_origin += rhs;
        return *this;
    }

    /// RectType operator
    RectType<Dim, DimCompat>& operator-=(const PointType<Dim, DimCompat>& rhs)
    {
        m_origin -= rhs;
        return *this;
    }

    /**
     * Calculate the area of the rectangle.
     */
    inline Dim area() const
    {
        return width() * height();
    }

    /**
     * Return the center point of the rectangle.
     */
    inline PointType<Dim, DimCompat> center() const
    {
        return PointType<Dim, DimCompat>(x() + (width() / 2.), y() + (height() / 2.));
    }

    /**
     * Move the rectangle's center to the specified point.
     */
    inline void move_to_center(const PointType<Dim, DimCompat>& center)
    {
        const PointType<Dim, DimCompat> pos(center.x() - width() / 2.,
                                            center.y() - height() / 2.);

        m_origin = pos;
    }

    /**
     * Grow the rectangle around its center by the specified radius.
     *
     * @param radius Relative radius to change by.
     */
    inline void grow_around_center(Dim radius)
    {
        m_origin -= (radius / 2);
        m_size += radius;
    }

    /**
     * Shrink the rectangle around its center by the specified radius.
     *
     * @param radius Relative radius to change by.
     */
    inline void shrink_around_center(Dim radius)
    {
        m_origin += (radius / 2);
        m_size -= radius;
    }

    /**
     * Get the PointType of the rectangle.
     */
    inline const PointType<Dim, DimCompat>& point() const
    {
        return m_origin;
    }

    /**
     * Set the rectangle's origin to the specified point.
     */
    inline void point(const PointType<Dim, DimCompat>& p)
    {
        m_origin = p;
    }

    /**
     * Get the SizeType of the rectangle.
     */
    inline const SizeType<Dim, DimCompat>& size() const
    {
        return m_size;
    }

    /**
     * Set the SizeType of the rectangle.
     */
    inline void size(const SizeType<Dim, DimCompat>& size)
    {
        m_size = size;
    }

    /**
     * Get the top side of the rectangle.
     */
    constexpr inline Dim top() const
    {
        return m_origin.y();
    }

    /**
     * Get the left side the rectangle.
     */
    constexpr inline Dim left() const
    {
        return m_origin.x();
    }

    /**
     * Get the bottom side the rectangle.
     */
    constexpr inline Dim bottom() const
    {
        return top() + height();
    }

    /**
     * Get the right side the rectangle.
     */
    constexpr inline Dim right() const
    {
        return left() + width();
    }

    /**
     * Get the top left point of the rectangle.
     *
     * @note This is usually the origin/point of the rectangle.
     */
    inline PointType<Dim, DimCompat> top_left() const
    {
        return PointType<Dim, DimCompat>(left(), top());
    }

    /**
     * Get the top right point of the rectangle.
     */
    inline PointType<Dim, DimCompat> top_right() const
    {
        return PointType<Dim, DimCompat>(right(), top());
    }

    /**
     * Get the bottom left point of the rectangle.
     */
    inline PointType<Dim, DimCompat> bottom_left() const
    {
        return PointType<Dim, DimCompat>(left(), bottom());
    }

    /**
     * Get the bottom right point of the rectangle.
     */
    inline PointType<Dim, DimCompat> bottom_right() const
    {
        return PointType<Dim, DimCompat>(right(), bottom());
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
    inline bool intersect(const PointType<Dim, DimCompat>& point) const
    {
        return (point.x() <= right() && point.x() >= left() &&
                point.y() <= bottom() && point.y() >= top());
    }

    /**
     * Determine if two rectangles intersect, or, overlap.
     * @param rect The rect to test.
     */
    inline bool intersect(const RectType<Dim, DimCompat>& rect) const
    {
        return (x() < rect.x() + rect.width() && x() + width() > rect.x() &&
                y() < rect.y() + rect.height() && y() + height() > rect.y());
    }

    /**
     * Merge two rectangles together into one super rectangle that contains
     * them both.
     */
    static inline RectType<Dim, DimCompat> merge(const RectType<Dim, DimCompat>& lhs,
            const RectType<Dim, DimCompat>& rhs) noexcept
    {
        const auto xmin = std::min(lhs.x(), rhs.x());
        const auto xmax = std::max(lhs.x() + lhs.width(),
                                   rhs.x() + rhs.width());
        const auto ymin = std::min(lhs.y(), rhs.y());
        const auto ymax = std::max(lhs.y() + lhs.height(),
                                   rhs.y() + rhs.height());

        return RectType<Dim, DimCompat>(xmin, ymin, xmax - xmin, ymax - ymin);
    }

    /**
     * Return the intersecting rectangle of two rectangles, if any.
     */
    static inline RectType<Dim, DimCompat> intersection(const RectType<Dim, DimCompat>& lhs,
            const RectType<Dim, DimCompat>& rhs) noexcept
    {
        const auto x0 = std::max(lhs.x(), rhs.x());
        const auto y0 = std::max(lhs.y(), rhs.y());
        const auto w0 = std::min(lhs.x() + lhs.width(), rhs.x() + rhs.width()) - x0;
        const auto h0 = std::min(lhs.y() + lhs.height(), rhs.y() + rhs.height()) - y0;

        if (w0 < 0 || h0 < 0)
            return RectType<Dim, DimCompat>();

        return RectType<Dim, DimCompat>(x0, y0, w0, h0);
    }

    /**
     * Returns true if this rectangle contains the specified one.
     *
     * This is different than intersect().  The specified rectangle must be
     * completely inside this one.
     */
    inline bool contains(const RectType<Dim, DimCompat>& rhs) const
    {
        return rhs.right() < right() &&
               rhs.bottom() < bottom() &&
               rhs.left() > left() &&
               rhs.top() > top();
    }

    constexpr inline Dim x() const { return m_origin.x(); }
    constexpr inline Dim y() const { return m_origin.y(); }

    inline void x(Dim x) { m_origin.x(x); }
    inline void y(Dim y) { m_origin.y(y); }

    constexpr inline Dim width() const { return m_size.width(); }
    constexpr inline Dim height() const { return m_size.height(); }

    inline void width(Dim width) { m_size.width(width); }
    inline void height(Dim height) { m_size.height(height); }

protected:

    /** Origin */
    PointType<Dim, DimCompat> m_origin;
    /** Size */
    SizeType<Dim, DimCompat> m_size;
};

/// Overloaded std::ostream insertion operator
template<class Dim, detail::Compatible DimCompat>
std::ostream& operator<<(std::ostream& os, const RectType<Dim, DimCompat>& rect)
{
    os << "["  << rect.x() << "," << rect.y() << "-" <<
       rect.width() << "x" << rect.height() << "]";
    return os;
}

/// RectType operator
template<class Dim, detail::Compatible DimCompat>
inline RectType<Dim, DimCompat> operator-(RectType<Dim, DimCompat> lhs, const SizeType<Dim, DimCompat>& rhs)
{
    lhs.size(lhs.size() - rhs);
    return lhs;
}

/// RectType operator
template<class Dim, detail::Compatible DimCompat>
inline RectType<Dim, DimCompat> operator+(RectType<Dim, DimCompat> lhs, const SizeType<Dim, DimCompat>& rhs)
{
    lhs.size(lhs.size() + rhs);
    return lhs;
}

/// RectType operator
template<class Dim, detail::Compatible DimCompat>
inline RectType<Dim, DimCompat> operator*(RectType<Dim, DimCompat> lhs, const SizeType<Dim, DimCompat>& rhs)
{
    lhs.size(lhs.size() * rhs);
    return lhs;
}

/// RectType operator
template<class Dim, detail::Compatible DimCompat>
inline RectType<Dim, DimCompat> operator/(RectType<Dim, DimCompat> lhs, const SizeType<Dim, DimCompat>& rhs)
{
    lhs.size(lhs.size() / rhs);
    return lhs;
}

/// RectType operator
template<class Dim, detail::Compatible DimCompat>
inline RectType<Dim, DimCompat> operator+(RectType<Dim, DimCompat> lhs, const PointType<Dim, DimCompat>& rhs)
{
    lhs.point(lhs.point() + rhs);
    return lhs;
}

/// RectType operator
template<class Dim, detail::Compatible DimCompat>
inline RectType<Dim, DimCompat> operator-(RectType<Dim, DimCompat> lhs, const PointType<Dim, DimCompat>& rhs)
{
    lhs.point(lhs.point() - rhs);
    return lhs;
}

/// RectType operator
template<class Dim, detail::Compatible DimCompat>
inline RectType<Dim, DimCompat> operator*(RectType<Dim, DimCompat> lhs, const PointType<Dim, DimCompat>& rhs)
{
    lhs.point(lhs.point() * rhs);
    return lhs;
}

/// RectType operator
template<class Dim, detail::Compatible DimCompat>
inline RectType<Dim, DimCompat> operator/(RectType<Dim, DimCompat> lhs, const PointType<Dim, DimCompat>& rhs)
{
    lhs.point(lhs.point() / rhs);
    return lhs;
}

/// RectType operator
template<class Dim, detail::Compatible DimCompat>
inline bool operator==(const RectType<Dim, DimCompat>& lhs, const RectType<Dim, DimCompat>& rhs)
{
    return lhs.point() == rhs.point() &&
           lhs.size() == rhs.size();
}

/// RectType operator
template<class Dim, detail::Compatible DimCompat>
inline bool operator!=(const RectType<Dim, DimCompat>& lhs, const RectType<Dim, DimCompat>& rhs)
{
    return !(lhs == rhs);
}

/**
 * Helper type for a default rect.
 */
using Rect = RectType<DefaultDim, detail::Compatible::normal>;

static_assert(detail::rule_of_5<Rect>(),
              "Rect : must fulfill rule of 5");

/**
 * Helper type for a float rect.
 */
using RectF = RectType<float, detail::Compatible::normal>;

/**
 * A line, with a starting and ending point.
 * @ingroup geometry
 */
template<class Dim>
class LineType
{
public:

    /**
     * Helper to reference the dimension type.
     */
    using DimType = Dim;

    constexpr explicit LineType(const PointType<Dim>& start, const PointType<Dim>& end) noexcept
        : m_start(start),
          m_end(end)
    {}

    inline PointType<Dim> start() const { return m_start; }
    inline PointType<Dim> end() const { return m_end; }

    inline void start(const PointType<Dim>& start) { m_start = start; }
    inline void end(const PointType<Dim>& end) { m_end = end; }

    /**
     * Returns a rectangle containing the line.
     */
    inline RectType<Dim> rect() const
    {
        const auto x = std::min(m_start.x(), m_end.x());
        const auto y = std::min(m_start.y(), m_end.y());
        const auto x2 = std::max(m_start.x(), m_end.x());
        const auto y2 = std::max(m_start.y(), m_end.y());

        return RectType<Dim>(x, y, x2 - x, y2 - y);
    }

protected:
    PointType<Dim> m_start;
    PointType<Dim> m_end;
};

/// Overloaded std::ostream insertion operator
template<class Dim>
std::ostream& operator<<(std::ostream& os, const LineType<Dim>& line)
{
    os << "[" << line.start() << "-" << line.end() << "]";
    return os;
}

/**
 * Helper type for a default line.
 */
using Line = LineType<DefaultDim>;

static_assert(detail::rule_of_5<Line>(),
              "Line : must fulfill rule of 5");

/**
 * An Arc consists of a radius and two angles.
 * @ingroup geometry
 */
template<class Dim>
class ArcType
{
public:

    /**
     * Helper to reference the dimension type.
     */
    using DimType = Dim;

    /**
     * Construct an Arc object.
     *
     * @param center Center point of the arc.
     * @param radius Radius of the arc.
     * @param angle1 Angle 1 of the arc in radians.
     * @param angle2 Angle 2 of the arc in radians.
     */
    constexpr explicit ArcType(const PointType<Dim>& center = {}, Dim radius = {},
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
    EGT_API bool empty() const;

    /** Set the radius. */
    inline void radius(Dim radius) { m_radius = radius; }
    /** Set the angle1. */
    inline void angle1(float angle) { m_angle1 = angle; }
    /** Set the angle2. */
    inline void angle2(float angle) { m_angle2 = angle; }
    /** Set the center point. */
    inline void center(const PointType<Dim>& center) { m_center = center; }

    /** Get the radius. */
    constexpr inline Dim radius() const { return m_radius; }
    /** Get the angle1. */
    constexpr inline float angle1() const { return m_angle1; }
    /** Get the angle2. */
    constexpr inline float angle2() const { return m_angle2; }
    /** Get the center point. */
    constexpr inline PointType<Dim> center() const { return m_center; }

protected:

    /**
     * Center point of the arc.
     */
    PointType<Dim> m_center;

    /**
     * Radius of the arc.
     */
    Dim m_radius{0};

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
using Arc = ArcType<DefaultDim>;

static_assert(detail::rule_of_5<Arc>(),
              "Arc : must fulfill rule of 5");

/**
 * A basic circle with a center point and radius.
 * @ingroup geometry
 */
template<class Dim>
class CircleType : public ArcType<Dim>
{
public:

    /**
     * Helper to reference the dimension type.
     */
    using DimType = Dim;

    /**
     * Construct a Circle object.
     *
     * @param center Center point of the arc.
     * @param radius Radius of the arc.
     */
    constexpr explicit CircleType(const PointType<Dim>& center = {}, Dim radius = {}) noexcept
        : ArcType<Dim>(center, radius, 0, 2.f * detail::pi<float>())
    {
    }

    /**
     * Get a Rect that covers the circle.
     */
    inline RectType<Dim> rect() const
    {
        RectType<Dim> r(this->center() - Point(this->radius(), this->radius()),
                        SizeType<Dim>(this->radius() * 2, this->radius() * 2));
        return r;
    }
};

/// Overloaded std::ostream insertion operator
template<class Dim>
std::ostream& operator<<(std::ostream& os, const CircleType<Dim>& circle)
{
    os << "[" << circle.center() << "-" << circle.radius() << "]";
    return os;
}

/**
 * Helper type for a default circle.
 */
using Circle = CircleType<DefaultDim>;

static_assert(detail::rule_of_5<Circle>(),
              "Circle : must fulfill rule of 5");

/**
 * Helper type for a float circle.
 */
using CircleF = CircleType<float>;

/**
 * A basic ellipse with a center and 2 radiuses.
 * @ingroup geometry
 */
template<class Dim>
class EllipseType
{
public:
    /**
    * Helper to reference the dimension type.
    */
    using DimType = Dim;

    constexpr EllipseType(const PointType<Dim>& center = {},
                          Dim radiusa = {},
                          Dim radiusb = {}) noexcept
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
    inline Dim perimeter() const
    {
        return 2.f * detail::pi<float>() * std::sqrt((m_radiusa * m_radiusa +
                m_radiusb * m_radiusb) / 2.f);
    }

    /**
     * @param angle The angle in degrees on the ellipse, with the center in the
     * middle of the ellipse.
     */
    inline PointType<Dim> point_on_perimeter(float angle)
    {
        const auto x = m_radiusa * std::cos(detail::to_radians(0.f, angle));
        const auto y = m_radiusb * std::sin(detail::to_radians(0.f, angle));
        return center() + PointType<Dim>(x, y);
    }

    inline void radiusa(const Dim radiusa) { m_radiusa = radiusa; }
    inline void radiusb(const Dim radiusb) { m_radiusb = radiusb; }
    inline void center(const PointType<Dim>& center) { m_center = center; }

    constexpr inline Dim radiusa() const { return m_radiusa; }
    constexpr inline Dim radiusb() const { return m_radiusb; }
    constexpr inline PointType<Dim> center() const { return m_center; }

protected:

    /**
     * Center point of the arc.
     */
    PointType<Dim> m_center;

    /**
     * A radius.
     */
    Dim m_radiusa{0};

    /**
     * B radius.
     */
    Dim m_radiusb{0};
};

/// Overloaded std::ostream insertion operator
template<class Dim>
std::ostream& operator<<(std::ostream& os, const EllipseType<Dim>& ellipse)
{
    os << "[" << ellipse.center() << "-" << ellipse.radiusa() <<
       "-" << ellipse.radiusb() << "]";
    return os;
}

}
}

#endif
