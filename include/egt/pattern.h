/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_PATTERN_H
#define EGT_PATTERN_H

/**
 * @file
 * @brief Working with patterns.
 */

#include <cairo.h>
#include <cassert>
#include <egt/color.h>
#include <egt/detail/meta.h>
#include <egt/geometry.h>
#include <egt/types.h>
#include <vector>

namespace egt
{
inline namespace v1
{

/*
 * gcc < 7.2 with c++14 support contains a bug dealing with constexpr on some
 * member functions.  clang seems to handle this correctly.
 *
 *     pattern.h:88:27: error: enclosing class of constexpr non-static member
 *         function ‘egt::v1::Pattern::operator egt::v1::Color() const’ is not a literal type
 *
 * This is further explained here:  https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66297
 *
 * Conditionally use constexpr in this case.
 */
#if defined(__clang__)
#  define EGT_PATTERN_CONSTEXPR constexpr
#elif defined(__GNUC__)
#  if (__GNUC__ < 7 || (__GNUC__ == 7 && __GNUC_MINOR__ < 2))
#    define EGT_PATTERN_CONSTEXPR
#  else
#    define EGT_PATTERN_CONSTEXPR constexpr
#  endif
#else
#  define EGT_PATTERN_CONSTEXPR constexpr
#endif

/**
 * A Pattern which can store one or more colors at different offsets (steps)
 * which can be used to create complex gradients.
 */
class EGT_API Pattern
{
public:

    /// Type of pattern.
    enum class Type
    {
        solid,
        linear,
        linear_vertical,
        radial
    };

    /// Scalar type for pattern steps.
    using StepScaler = float;

    /// Step array type.
    using StepArray = std::vector<std::pair<float, Color>>;

    Pattern() noexcept = default;

    Pattern(const Pattern& rhs);
    Pattern(Pattern&& rhs) noexcept;
    Pattern& operator=(const Pattern& rhs);
    Pattern& operator=(Pattern&& rhs) noexcept;

    ~Pattern() noexcept;

    // cppcheck-suppress noExplicitConstructor
    // NOLINTNEXTLINE(hicpp-explicit-conversions, google-explicit-constructor)
    constexpr Pattern(const Color& color) noexcept
        : m_color(color)
    {}

    /**
     * Construct a pattern with the specified type and steps.
     */
    explicit Pattern(Type type, const StepArray& steps = {});

    /**
     * Construct a linear pattern with the specified steps.
     *
     * A linear pattern is made between a starting point and an ending point.
     *
     * @param steps The steps of the gradient.
     * @param start The starting point of the pattern.
     * @param end The ending point of the pattern.
     */
    Pattern(const StepArray& steps,
            const Point& start,
            const Point& end);

    /**
     * Construct a radial pattern with the specified steps.
     *
     * A radial pattern is made between a starting circle and an ending circle.
     *
     * @param steps The steps of the gradient.
     * @param start The center of the starting circle.
     * @param start_radius The starting radius of the pattern.
     * @param end The center of the ending circle.
     * @param end_radius The ending radius of the pattern.
     */
    Pattern(const StepArray& steps,
            const Point& start,
            float start_radius,
            const Point& end,
            float end_radius);

    /**
     * Get the first color of the pattern.
     */
    // NOLINTNEXTLINE(hicpp-explicit-conversions, google-explicit-constructor)
    EGT_PATTERN_CONSTEXPR operator Color() const
    {
        return solid();
    }

    /**
     * Get the solid color.
     *
     * Calling this when type() != Type::Solid will throw.
     */
    EGT_PATTERN_CONSTEXPR Color solid() const
    {
        if (m_type != Type::solid)
            throw std::runtime_error("requested solid color in pattern that is not a solid color");
        return m_color;
    }

    /**
     * Get the first color of the pattern.
     * @deprecated
     */
    EGT_DEPRECATED EGT_NODISCARD Color color() const
    {
        return first();
    }

    /**
     * Get the first color of the pattern.
     */
    EGT_NODISCARD Color first() const;

    /**
     * Add a step to the gradient.
     */
    Pattern& step(StepScaler offset, const Color& color);

    /**
     * Get the type of pattern.
     */
    EGT_NODISCARD Type type() const { return m_type; }

    /**
     * Create a linear gradient from start to end.
     */
    void linear(const Point& start, const Point& end);

    /**
     * Create a radial gradient from start to end.
     */
    void radial(const Point& start, float start_radius,
                const Point& end, float end_radius);

    /// Get the starting point of the pattern.
    EGT_NODISCARD Point starting() const;
    /// Get the starting radius of the pattern.
    EGT_NODISCARD float starting_radius() const;
    /// Get the ending point of the pattern.
    EGT_NODISCARD Point ending() const;
    /// Get the ending radius of the pattern.
    EGT_NODISCARD float ending_radius() const;

    /// Get all of the steps of the pattern
    EGT_NODISCARD const StepArray& steps() const;

    /// Get internal pattern representation.
    EGT_NODISCARD cairo_pattern_t* pattern() const
    {
        if (!m_pattern)
            create_pattern();
        assert(m_pattern.get());
        return m_pattern.get();
    }

protected:

    /// @private
    void create_pattern() const;

    /// @private
    static bool sort_by_first(const std::pair<float, Color>& a,
                              const std::pair<float, Color>& b)
    {
        return (a.first < b.first);
    }

    /// Type of the pattern.
    Type m_type{Type::solid};

    /// Solid color of the pattern
    Color m_color;

    struct PatternImpl;
    /// Implementation details for non-solid color patterns
    PatternImpl* m_impl{nullptr};

    /// Internal pattern representation.
    mutable shared_cairo_pattern_t m_pattern;
};

static_assert(detail::rule_of_5<Pattern>(), "must fulfill rule of 5");

}
}

#endif
