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
    grid
};

}


/**
 * Simple x,y coordinate.
 *
 * Typically @ref Point, @ref PointF, @ref DisplayPoint are used as aliases.
 *
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

    /**
     * @param[in] x X value.
     * @param[in] y Y value.
     */
    constexpr PointType(Dim x, Dim y) noexcept
        : m_x(x),
          m_y(y)
    {}

    template<class Dim2>
    constexpr PointType(const PointType<Dim2>& p)
        : m_x(p.x()),
          m_y(p.y())
    {}

    /// PointType operator
    constexpr PointType& operator+=(const PointType& rhs) noexcept
    {
        m_x += rhs.m_x;
        m_y += rhs.m_y;
        return *this;
    }

    /// PointType operator
    constexpr PointType& operator-=(const PointType& rhs) noexcept
    {
        m_x -= rhs.m_x;
        m_y -= rhs.m_y;
        return *this;
    }

    /// PointType operator
    constexpr PointType& operator*=(const PointType& rhs) noexcept
    {
        m_x *= rhs.m_x;
        m_y *= rhs.m_y;
        return *this;
    }

    /// PointType operator
    constexpr PointType& operator/=(const PointType& rhs) noexcept
    {
        m_x /= rhs.m_x;
        m_y /= rhs.m_y;
        return *this;
    }

    /// PointType operator
    template <class T>
    constexpr PointType& operator+=(const T& rhs) noexcept
    {
        m_x += rhs;
        m_y += rhs;
        return *this;
    }

    /// PointType operator
    template <class T>
    constexpr PointType& operator-=(const T& rhs) noexcept
    {
        m_x -= rhs;
        m_y -= rhs;
        return *this;
    }

    /// PointType operator
    template <class T>
    constexpr PointType& operator*=(const T& rhs) noexcept
    {
        m_x *= rhs;
        m_y *= rhs;
        return *this;
    }

    /// PointType operator
    template <class T>
    constexpr PointType& operator/=(const T& rhs) noexcept
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
    PointType point_on_circumference(T radius, T angle) const noexcept
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
    EGT_NODISCARD T angle_to(const PointType& point) const noexcept
    {
        return std::atan2(point.m_x - m_x, m_y - point.m_y);
    }

    /**
     * Calculate the straight line distance to another point.
     *
     * @param point The other point.
     */
    EGT_NODISCARD Dim distance_to(const PointType& point) const noexcept
    {
        return std::hypot(point.m_x - m_x, point.m_y - m_y);
    }

    /// Get the x value.
    EGT_NODISCARD constexpr Dim x() const noexcept { return m_x; }
    /// Get the y value.
    EGT_NODISCARD constexpr Dim y() const noexcept { return m_y; }

    /// Set the x value.
    constexpr void x(Dim x) noexcept { m_x = x; }
    /// Set the y value.
    constexpr void y(Dim y) noexcept { m_y = y; }

protected:
    /// X value
    Dim m_x{0};
    /// Y value
    Dim m_y{0};
};

/// PointType operator
template<class Dim, detail::Compatible DimCompat>
constexpr PointType<Dim, DimCompat> operator-(const PointType<Dim, DimCompat>& a) noexcept
{
    return PointType<Dim, DimCompat>(-a.x(), -a.y());
}

/// PointType operator
template<class Dim, detail::Compatible DimCompat>
constexpr bool operator==(const PointType<Dim, DimCompat>& lhs, const PointType<Dim, DimCompat>& rhs) noexcept
{
    return lhs.x() == rhs.x() && lhs.y() == rhs.y();
}

/// PointType operator
template<detail::Compatible DimCompat>
constexpr bool operator==(const PointType<float, DimCompat>& lhs, const PointType<float, DimCompat>& rhs)
{
    return detail::float_equal(lhs.x(), rhs.x()) &&
           detail::float_equal(lhs.y(), rhs.y());
}

/// PointType operator
template<class Dim, detail::Compatible DimCompat>
constexpr bool operator!=(const PointType<Dim, DimCompat>& lhs, const PointType<Dim, DimCompat>& rhs) noexcept
{
    return !(lhs == rhs);
}

/// PointType operator
template<class Dim, detail::Compatible DimCompat>
constexpr PointType<Dim, DimCompat> operator-(PointType<Dim, DimCompat> lhs, const PointType<Dim, DimCompat>& rhs) noexcept
{
    lhs -= rhs;
    return lhs;
}

/// PointType operator
template<class Dim, detail::Compatible DimCompat>
constexpr PointType<Dim, DimCompat> operator+(PointType<Dim, DimCompat> lhs, const PointType<Dim, DimCompat>& rhs) noexcept
{
    lhs += rhs;
    return lhs;
}

/// PointType operator
template<class Dim, detail::Compatible DimCompat>
constexpr PointType<Dim, DimCompat> operator/(PointType<Dim, DimCompat> lhs, const PointType<Dim, DimCompat>& rhs) noexcept
{
    lhs /= rhs;
    return lhs;
}

/// PointType operator
template<class Dim, detail::Compatible DimCompat>
constexpr PointType<Dim, DimCompat> operator*(PointType<Dim, DimCompat> lhs, const PointType<Dim, DimCompat>& rhs) noexcept
{
    lhs *= rhs;
    return lhs;
}

/// PointType operator
template<class Dim, detail::Compatible DimCompat, class T>
constexpr PointType<Dim, DimCompat> operator-(PointType<Dim, DimCompat> lhs, const T& rhs) noexcept
{
    lhs -= rhs;
    return lhs;
}

/// PointType operator
template<class Dim, detail::Compatible DimCompat, class T>
constexpr PointType<Dim, DimCompat> operator+(PointType<Dim, DimCompat> lhs, const T& rhs) noexcept
{
    lhs += rhs;
    return lhs;
}

/// PointType operator
template<class Dim, detail::Compatible DimCompat, class T>
constexpr PointType<Dim, DimCompat> operator/(PointType<Dim, DimCompat> lhs, const T& rhs) noexcept
{
    lhs /= rhs;
    return lhs;
}

/// PointType operator
template<class Dim, detail::Compatible DimCompat, class T>
constexpr PointType<Dim, DimCompat> operator*(PointType<Dim, DimCompat> lhs, const T& rhs) noexcept
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
 * Helper type alias.
 * @copybrief PointType
 * @ingroup geometry
 */
using Point = PointType<DefaultDim, detail::Compatible::normal>;

static_assert(detail::rule_of_5<Point>(), "must fulfill rule of 5");

/**
 * Helper type alias.
 * @copybrief PointType
 * @ingroup geometry
 */
using PointF = PointType<float, detail::Compatible::normal>;

/**
 * Helper type alias.
 * @copybrief PointType
 * @ingroup geometry
 */
using DisplayPoint = PointType<DefaultDim, detail::Compatible::display>;

/**
 * Simple width and height.
 *
 * Typically @ref Size, @ref SizeF are used as aliases.
 *
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

    template<class Dim2>
    constexpr SizeType(const SizeType<Dim2>& s)
        : m_height(s.height()),
          m_width(s.width())
    {}

    /**
     * Returns true if the size has no width or height.
     *
     * This is a special case, and is the default state of a SizeType. This can
     * be used to determine if a size has not been set in many cases.
     */
    EGT_NODISCARD constexpr bool empty() const noexcept
    {
        // cppcheck-suppress unsignedLessThanZero
        return m_width <= 0 || m_height <= 0;
    }

    /**
     * Clear the size.
     *
     * This means to set the width and height to zero.
     */
    constexpr void clear() noexcept
    {
        m_height = m_width = 0;
    }

    /// SizeType operator
    constexpr SizeType& operator+=(const SizeType& rhs) noexcept
    {
        m_width += rhs.m_width;
        m_height += rhs.m_height;
        return *this;
    }

    /// SizeType operator
    constexpr SizeType& operator-=(const SizeType& rhs) noexcept
    {
        m_width -= rhs.m_width;
        m_height -= rhs.m_height;
        return *this;
    }

    /// SizeType operator
    constexpr SizeType& operator*=(const SizeType& rhs) noexcept
    {
        m_width *= rhs.m_width;
        m_height *= rhs.m_height;
        return *this;
    }

    /// SizeType operator
    constexpr SizeType& operator/=(const SizeType& rhs) noexcept
    {
        m_width /= rhs.m_width;
        m_height /= rhs.m_height;
        return *this;
    }

    /// SizeType operator
    template<class T>
    constexpr SizeType& operator+=(const T& rhs) noexcept
    {
        m_width += rhs;
        m_height += rhs;
        return *this;
    }

    /// SizeType operator
    template<class T>
    constexpr SizeType& operator-=(const T& rhs) noexcept
    {
        m_width -= rhs;
        m_height -= rhs;
        return *this;
    }

    /// SizeType operator
    template<class T>
    constexpr SizeType& operator*=(const T& rhs) noexcept
    {
        m_width *= rhs;
        m_height *= rhs;
        return *this;
    }

    /// SizeType operator
    template<class T>
    constexpr SizeType& operator/=(const T& rhs) noexcept
    {
        m_width /= rhs;
        m_height /= rhs;
        return *this;
    }

    /// Get the width value.
    EGT_NODISCARD constexpr Dim width() const  noexcept { return m_width; }
    /// Get the height value.
    EGT_NODISCARD constexpr Dim height() const  noexcept { return m_height; }

    /// Set the width value.
    constexpr void width(Dim width)  noexcept { m_width = width; }
    /// Set the height value.
    constexpr void height(Dim height)  noexcept { m_height = height; }

protected:
    /// Height
    Dim m_height{0};

    /// Width
    Dim m_width{0};
};

/// SizeType operator
template<class Dim, detail::Compatible DimCompat>
constexpr bool operator==(const SizeType<Dim, DimCompat>& lhs, const SizeType<Dim, DimCompat>& rhs) noexcept
{
    return lhs.width() == rhs.width() && lhs.height() == rhs.height();
}

/// SizeType operator
template<detail::Compatible DimCompat>
constexpr bool operator==(const SizeType<float, DimCompat>& lhs, const SizeType<float, DimCompat>& rhs)
{
    return detail::float_equal(lhs.width(), rhs.width()) &&
           detail::float_equal(lhs.height(), rhs.height());
}

/// SizeType operator
template<class Dim, detail::Compatible DimCompat>
constexpr bool operator!=(const SizeType<Dim, DimCompat>& lhs, const SizeType<Dim, DimCompat>& rhs) noexcept
{
    return !(lhs == rhs);
}

/// SizeType operator
template<class Dim, detail::Compatible DimCompat>
constexpr SizeType<Dim, DimCompat> operator-(SizeType<Dim, DimCompat> lhs, const SizeType<Dim, DimCompat>& rhs) noexcept
{
    lhs -= rhs;
    return lhs;
}

/// SizeType operator
template<class Dim, detail::Compatible DimCompat>
constexpr SizeType<Dim, DimCompat> operator+(SizeType<Dim, DimCompat> lhs, const SizeType<Dim, DimCompat>& rhs) noexcept
{
    lhs += rhs;
    return lhs;
}

/// SizeType operator
template<class Dim, detail::Compatible DimCompat>
constexpr SizeType<Dim, DimCompat> operator*(SizeType<Dim, DimCompat> lhs, const SizeType<Dim, DimCompat>& rhs) noexcept
{
    lhs *= rhs;
    return lhs;
}

/// SizeType operator
template<class Dim, detail::Compatible DimCompat>
constexpr SizeType<Dim, DimCompat> operator/(SizeType<Dim, DimCompat> lhs, const SizeType<Dim, DimCompat>& rhs) noexcept
{
    lhs /= rhs;
    return lhs;
}

/// SizeType operator
template<class Dim, detail::Compatible DimCompat, class T>
constexpr SizeType<Dim, DimCompat> operator-(SizeType<Dim, DimCompat> lhs, const T& rhs) noexcept
{
    lhs -= rhs;
    return lhs;
}

/// SizeType operator
template<class Dim, detail::Compatible DimCompat, class T>
constexpr SizeType<Dim, DimCompat> operator+(SizeType<Dim, DimCompat> lhs, const T& rhs) noexcept
{
    lhs += rhs;
    return lhs;
}

/// SizeType operator
template<class Dim, detail::Compatible DimCompat, class T>
constexpr SizeType<Dim, DimCompat> operator*(SizeType<Dim, DimCompat> lhs, const T& rhs) noexcept
{
    lhs *= rhs;
    return lhs;
}

/// SizeType operator
template<class Dim, detail::Compatible DimCompat, class T>
constexpr SizeType<Dim, DimCompat> operator/(SizeType<Dim, DimCompat> lhs, const T& rhs) noexcept
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
 * Helper type alias.
 * @copybrief SizeType
 * @ingroup geometry
 */
using Size = SizeType<DefaultDim, detail::Compatible::normal>;

static_assert(detail::rule_of_5<Size>(), "must fulfill rule of 5");

/**
 * Helper type alias.
 * @copybrief SizeType
 * @ingroup geometry
 */
using SizeF = SizeType<float, detail::Compatible::normal>;

/**
 * A rectangle with a point and a size.
 *
 * Typically @ref Rect, @ref RectF are used as aliases.
 *
 * All rectangle points are at the top left.
 *
 * @ingroup geometry
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
    constexpr explicit RectType(const PointType<Dim>& point, const SizeType<Dim, DimCompat>& size) noexcept
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
    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    constexpr RectType(const SizeType<Dim, DimCompat>& size) noexcept
        : RectType( {}, size)
    {}

    /**
     * @param x X origin coordinate.
     * @param y Y origin coordinate.
     * @param width Width
     * @param height Height
     */
    constexpr RectType(Dim x, Dim y, Dim width, Dim height) noexcept
        : RectType( {x, y}, {width, height})
    {}

    template<typename Dim2>
    constexpr RectType(const RectType<Dim2>& r)
        : m_origin(r.point()),
          m_size(r.size())
    {}

    /// RectType operator
    constexpr RectType& operator+=(const SizeType<Dim, DimCompat>& rhs) noexcept
    {
        m_size += rhs;
        return *this;
    }

    /// RectType operator
    constexpr RectType& operator-=(const SizeType<Dim, DimCompat>& rhs) noexcept
    {
        m_size -= rhs;
        return *this;
    }

    /// RectType operator
    constexpr RectType& operator+=(const PointType<Dim, DimCompat>& rhs) noexcept
    {
        m_origin += rhs;
        return *this;
    }

    /// RectType operator
    constexpr RectType& operator-=(const PointType<Dim, DimCompat>& rhs) noexcept
    {
        m_origin -= rhs;
        return *this;
    }

    /**
     * Calculate the area of the rectangle.
     */
    EGT_NODISCARD constexpr Dim area() const noexcept
    {
        return width() * height();
    }

    /**
     * Return the center point of the rectangle.
     */
    EGT_NODISCARD constexpr PointType<Dim, DimCompat> center() const noexcept
    {
        return PointType<Dim, DimCompat>(x() + (width() / 2.f), y() + (height() / 2.f));
    }

    /**
     * Move the rectangle's center to the specified point.
     */
    constexpr void move_to_center(const PointType<Dim, DimCompat>& center) noexcept
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
    constexpr void grow_around_center(Dim radius) noexcept
    {
        m_origin -= (radius / 2);
        m_size += radius;
    }

    /**
     * Shrink the rectangle around its center by the specified radius.
     *
     * @param radius Relative radius to change by.
     */
    constexpr void shrink_around_center(Dim radius) noexcept
    {
        m_origin += (radius / 2);
        m_size -= radius;
    }

    /**
     * Get the PointType of the rectangle.
     */
    EGT_NODISCARD constexpr const PointType<Dim, DimCompat>& point() const noexcept
    {
        return m_origin;
    }

    /**
     * Set the rectangle's origin to the specified point.
     */
    constexpr void point(const PointType<Dim, DimCompat>& p) noexcept
    {
        m_origin = p;
    }

    /**
     * Get the SizeType of the rectangle.
     */
    EGT_NODISCARD constexpr const SizeType<Dim, DimCompat>& size() const noexcept
    {
        return m_size;
    }

    /**
     * Set the SizeType of the rectangle.
     */
    constexpr void size(const SizeType<Dim, DimCompat>& size) noexcept
    {
        m_size = size;
    }

    /**
     * Get the top side of the rectangle.
     */
    EGT_NODISCARD constexpr Dim top() const noexcept
    {
        return m_origin.y();
    }

    /**
     * Get the left side the rectangle.
     */
    EGT_NODISCARD constexpr Dim left() const noexcept
    {
        return m_origin.x();
    }

    /**
     * Get the bottom side the rectangle.
     */
    EGT_NODISCARD constexpr Dim bottom() const noexcept
    {
        return top() + height();
    }

    /**
     * Get the right side the rectangle.
     */
    EGT_NODISCARD constexpr Dim right() const noexcept
    {
        return left() + width();
    }

    /**
     * Get the top left point of the rectangle.
     *
     * @note This is usually the origin/point of the rectangle.
     */
    EGT_NODISCARD constexpr PointType<Dim, DimCompat> top_left() const noexcept
    {
        return PointType<Dim, DimCompat>(left(), top());
    }

    /**
     * Get the top right point of the rectangle.
     */
    EGT_NODISCARD constexpr PointType<Dim, DimCompat> top_right() const noexcept
    {
        return PointType<Dim, DimCompat>(right(), top());
    }

    /**
     * Get the bottom left point of the rectangle.
     */
    EGT_NODISCARD constexpr PointType<Dim, DimCompat> bottom_left() const noexcept
    {
        return PointType<Dim, DimCompat>(left(), bottom());
    }

    /**
     * Get the bottom right point of the rectangle.
     */
    EGT_NODISCARD constexpr PointType<Dim, DimCompat> bottom_right() const noexcept
    {
        return PointType<Dim, DimCompat>(right(), bottom());
    }

    /**
     * Clear the rectangle by giving it no width or height.
     */
    constexpr void clear() noexcept
    {
        m_origin = {};
        m_size = {};
    }

    /**
     * Returns true if the rectangle has no width or height.
     */
    EGT_NODISCARD constexpr bool empty() const noexcept
    {
        return width() <= 0 || height() <= 0;
    }

    /**
     * Returns true if the specified point is inside the rectangle.
     * @param point The point to test.
     */
    EGT_NODISCARD constexpr bool intersect(const PointType<Dim, DimCompat>& point) const noexcept
    {
        return (point.x() <= right() && point.x() >= left() &&
                point.y() <= bottom() && point.y() >= top());
    }

    /**
     * Determine if two rectangles intersect, or, overlap.
     * @param rect The rect to test.
     */
    EGT_NODISCARD constexpr bool intersect(const RectType& rect) const noexcept
    {
        return (x() < rect.x() + rect.width() && x() + width() > rect.x() &&
                y() < rect.y() + rect.height() && y() + height() > rect.y());
    }

    /**
     * Merge two rectangles together into one super rectangle that contains
     * them both.
     */
    static constexpr RectType merge(const RectType& lhs,
                                    const RectType& rhs) noexcept
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
    static constexpr RectType intersection(const RectType& lhs,
                                           const RectType& rhs) noexcept
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
    EGT_NODISCARD constexpr bool contains(const RectType& rhs) const noexcept
    {
        return rhs.right() < right() &&
               rhs.bottom() < bottom() &&
               rhs.left() > left() &&
               rhs.top() > top();
    }

    /// Get the x value.
    EGT_NODISCARD constexpr Dim x() const noexcept { return m_origin.x(); }
    /// Get the y value.
    EGT_NODISCARD constexpr Dim y() const noexcept { return m_origin.y(); }

    /// Set the x value.
    constexpr void x(Dim x) noexcept { m_origin.x(x); }
    /// Set the y value.
    constexpr void y(Dim y) noexcept { m_origin.y(y); }

    /// Get the width value.
    EGT_NODISCARD constexpr Dim width() const noexcept { return m_size.width(); }
    /// Get the height value.
    EGT_NODISCARD constexpr Dim height() const noexcept { return m_size.height(); }

    /// Set the width value.
    constexpr void width(Dim width) noexcept { m_size.width(width); }
    /// Set the height value.
    constexpr void height(Dim height) noexcept { m_size.height(height); }

protected:

    /// Origin
    PointType<Dim, DimCompat> m_origin;
    /// Size
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
constexpr RectType<Dim, DimCompat> operator-(RectType<Dim, DimCompat> lhs, const SizeType<Dim, DimCompat>& rhs) noexcept
{
    lhs.size(lhs.size() - rhs);
    return lhs;
}

/// RectType operator
template<class Dim, detail::Compatible DimCompat>
constexpr RectType<Dim, DimCompat> operator+(RectType<Dim, DimCompat> lhs, const SizeType<Dim, DimCompat>& rhs) noexcept
{
    lhs.size(lhs.size() + rhs);
    return lhs;
}

/// RectType operator
template<class Dim, detail::Compatible DimCompat>
constexpr RectType<Dim, DimCompat> operator*(RectType<Dim, DimCompat> lhs, const SizeType<Dim, DimCompat>& rhs) noexcept
{
    lhs.size(lhs.size() * rhs);
    return lhs;
}

/// RectType operator
template<class Dim, detail::Compatible DimCompat>
constexpr RectType<Dim, DimCompat> operator/(RectType<Dim, DimCompat> lhs, const SizeType<Dim, DimCompat>& rhs) noexcept
{
    lhs.size(lhs.size() / rhs);
    return lhs;
}

/// RectType operator
template<class Dim, detail::Compatible DimCompat>
constexpr RectType<Dim, DimCompat> operator+(RectType<Dim, DimCompat> lhs, const PointType<Dim, DimCompat>& rhs) noexcept
{
    lhs.point(lhs.point() + rhs);
    return lhs;
}

/// RectType operator
template<class Dim, detail::Compatible DimCompat>
constexpr RectType<Dim, DimCompat> operator-(RectType<Dim, DimCompat> lhs, const PointType<Dim, DimCompat>& rhs) noexcept
{
    lhs.point(lhs.point() - rhs);
    return lhs;
}

/// RectType operator
template<class Dim, detail::Compatible DimCompat>
constexpr RectType<Dim, DimCompat> operator*(RectType<Dim, DimCompat> lhs, const PointType<Dim, DimCompat>& rhs) noexcept
{
    lhs.point(lhs.point() * rhs);
    return lhs;
}

/// RectType operator
template<class Dim, detail::Compatible DimCompat>
constexpr RectType<Dim, DimCompat> operator/(RectType<Dim, DimCompat> lhs, const PointType<Dim, DimCompat>& rhs) noexcept
{
    lhs.point(lhs.point() / rhs);
    return lhs;
}

/// RectType operator
template<class Dim, detail::Compatible DimCompat>
constexpr bool operator==(const RectType<Dim, DimCompat>& lhs, const RectType<Dim, DimCompat>& rhs) noexcept
{
    return lhs.point() == rhs.point() &&
           lhs.size() == rhs.size();
}

/// RectType operator
template<class Dim, detail::Compatible DimCompat>
constexpr bool operator!=(const RectType<Dim, DimCompat>& lhs, const RectType<Dim, DimCompat>& rhs) noexcept
{
    return !(lhs == rhs);
}

/**
 * Helper type alias.
 * @copybrief RectType
 * @ingroup geometry
 */
using Rect = RectType<DefaultDim, detail::Compatible::normal>;

static_assert(detail::rule_of_5<Rect>(), "must fulfill rule of 5");

/**
 * Helper type alias.
 * @copybrief RectType
 * @ingroup geometry
 */
using RectF = RectType<float, detail::Compatible::normal>;

/**
 * A line, with a starting and ending point.
 *
 * Typically @ref Line, @ref LineF are used as aliases.
 *
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

    /**
     * @param[in] start Start point of the line.
     * @param[in] end End point of the line.
     */
    constexpr explicit LineType(const PointType<Dim>& start, const PointType<Dim>& end) noexcept
        : m_start(start),
          m_end(end)
    {}

    /// Get the start point of the line.
    EGT_NODISCARD constexpr PointType<Dim> start() const noexcept { return m_start; }
    /// Get the end point of the line.
    EGT_NODISCARD constexpr PointType<Dim> end() const noexcept { return m_end; }

    /// Set the start point of the line.
    constexpr void start(const PointType<Dim>& start) noexcept { m_start = start; }
    /// Set the end point of the line.
    constexpr void end(const PointType<Dim>& end) noexcept { m_end = end; }

    /**
     * Returns a rectangle containing the line.
     */
    EGT_NODISCARD constexpr RectType<Dim> rect() const noexcept
    {
        const auto x = std::min(m_start.x(), m_end.x());
        const auto y = std::min(m_start.y(), m_end.y());
        const auto x2 = std::max(m_start.x(), m_end.x());
        const auto y2 = std::max(m_start.y(), m_end.y());

        return RectType<Dim>(x, y, x2 - x, y2 - y);
    }

protected:
    /// Start point of the line.
    PointType<Dim> m_start;
    /// End point of the line.
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
 * Helper type alias.
 * @copybrief LineType
 * @ingroup geometry
 */
using Line = LineType<DefaultDim>;

/**
 * Helper type alias.
 * @copybrief LineType
 * @ingroup geometry
 */
using LineF = LineType<float>;

static_assert(detail::rule_of_5<Line>(), "must fulfill rule of 5");

/**
 * An Arc consists of a radius and two angles.
 *
 * Typically @ref Arc is used as an alias.
 *
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
    EGT_NODISCARD EGT_API bool empty() const noexcept;

    /** Set the radius. */
    constexpr void radius(Dim radius) noexcept { m_radius = radius; }
    /** Set the angle1. */
    constexpr void angle1(float angle) noexcept { m_angle1 = angle; }
    /** Set the angle2. */
    constexpr void angle2(float angle) noexcept { m_angle2 = angle; }
    /** Set the center point. */
    constexpr void center(const PointType<Dim>& center) noexcept { m_center = center; }

    /** Get the radius. */
    EGT_NODISCARD constexpr Dim radius() const noexcept { return m_radius; }
    /** Get the angle1. */
    EGT_NODISCARD constexpr float angle1() const noexcept { return m_angle1; }
    /** Get the angle2. */
    EGT_NODISCARD constexpr float angle2() const noexcept { return m_angle2; }
    /** Get the center point. */
    EGT_NODISCARD constexpr PointType<Dim> center() const noexcept { return m_center; }

protected:

    /// Center point of the arc.
    PointType<Dim> m_center;

    /// Radius of the arc.
    Dim m_radius{0};

    /// Angle of the arc in radians.
    float m_angle1{};

    /// Angle of the arc in radians.
    float m_angle2{};
};

/**
 * Helper type alias.
 * @copybrief ArcType
 * @ingroup geometry
 */
using Arc = ArcType<DefaultDim>;

static_assert(detail::rule_of_5<Arc>(), "must fulfill rule of 5");

/**
 * A basic circle with a center point and radius.
 *
 * Typically @ref Circle, @ref CircleF are used as aliases.
 *
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
    EGT_NODISCARD constexpr RectType<Dim> rect() const noexcept
    {
        RectType<Dim> r(this->center() - Point(this->radius(), this->radius()),
                        SizeType<Dim>(this->radius() * 2, this->radius() * 2));
        return r;
    }

    /**
     * Return the point on the circumference of the circle at the given angle.
     *
     * @param angle The angle in radians.
     */
    template <class T>
    PointType<Dim> point_on_circumference(T angle) const noexcept
    {
        return PointType<Dim>(this->x() + this->radius() * std::cos(angle),
                              this->y() + this->radius() * std::sin(angle));
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
 * Helper type alias.
 * @copybrief CircleType
 * @ingroup geometry
 */
using Circle = CircleType<DefaultDim>;

static_assert(detail::rule_of_5<Circle>(), "must fulfill rule of 5");

/**
 * Helper type alias.
 * @copybrief CircleType
 * @ingroup geometry
 */
using CircleF = CircleType<float>;

/**
 * A basic ellipse with a center and 2 radii.
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

    /**
     * @param[in] center Center of the ellipse.
     * @param[in] radiusa First radius of the ellipse.
     * @param[in] radiusb Second radius of the ellipse.
     */
    explicit constexpr EllipseType(const PointType<Dim>& center = {},
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
    EGT_NODISCARD constexpr Dim perimeter() const noexcept
    {
        return 2.f * detail::pi<float>() * std::sqrt((m_radiusa * m_radiusa +
                m_radiusb * m_radiusb) / 2.f);
    }

    /**
     * Return the point on the circumference of the ellipse at the given angle.
     *
     * @param angle The angle in radians on the ellipse, with the center in the
     * middle of the ellipse.
     */
    template <class T>
    PointType<Dim> point_on_circumference(T angle) noexcept
    {
        const auto x = m_radiusa * std::cos(angle);
        const auto y = m_radiusb * std::sin(angle);
        return center() + PointType<Dim>(x, y);
    }

    /// Set the first radius of the ellipse.
    constexpr void radiusa(const Dim radiusa) noexcept { m_radiusa = radiusa; }
    /// Set the second radius of the ellipse.
    constexpr void radiusb(const Dim radiusb) noexcept { m_radiusb = radiusb; }
    /// Set the center of the ellipse.
    constexpr void center(const PointType<Dim>& center) noexcept { m_center = center; }

    /// Get the first radius of the ellipse.
    EGT_NODISCARD constexpr Dim radiusa() const noexcept { return m_radiusa; }
    /// Get the second radius of the ellipse.
    EGT_NODISCARD constexpr Dim radiusb() const noexcept { return m_radiusb; }
    /// Get the center of the ellipse.
    EGT_NODISCARD constexpr PointType<Dim> center() const noexcept { return m_center; }

protected:

    /// Center point of the arc.
    PointType<Dim> m_center;

    /// A radius.
    Dim m_radiusa{0};

    /// B radius.
    Dim m_radiusb{0};
};

/**
 * Helper type alias.
 * @copybrief EllipseType
 * @ingroup geometry
 */
using Ellipse = EllipseType<DefaultDim>;

static_assert(detail::rule_of_5<Ellipse>(), "must fulfill rule of 5");

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
