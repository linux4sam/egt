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

    explicit Pattern(Type type, const StepArray& steps = {});

    Pattern(const StepArray& steps,
            const Point& start,
            const Point& end);

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
        return color();
    }

    /**
     * Get the first color of the pattern.
     */
    EGT_PATTERN_CONSTEXPR Color color()
    {
        return m_color;
    }

    /**
     * Get the first color of the pattern.
     */
    EGT_PATTERN_CONSTEXPR const Color& color() const
    {
        return m_color;
    }

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
    /// Set the starting point of the pattern.
    EGT_NODISCARD float starting_radius() const;
    /// Get the ending  point of the pattern.
    EGT_NODISCARD Point ending() const;
    /// Set the ending  point of the pattern.
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
