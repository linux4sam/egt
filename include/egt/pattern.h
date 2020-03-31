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

#include <egt/color.h>
#include <egt/detail/meta.h>
#include <egt/geometry.h>
#include <vector>

namespace egt
{
inline namespace v1
{

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

    // cppcheck-suppress noExplicitConstructor
    // NOLINTNEXTLINE(hicpp-explicit-conversions, google-explicit-constructor)
    Pattern(const Color& color);

    explicit Pattern(Type type, StepArray steps = {});

    Pattern(StepArray steps,
            const Point& start,
            const Point& end);

    Pattern(StepArray steps,
            const Point& start,
            float start_radius,
            const Point& end,
            float end_radius);

    /**
     * Get the first color of the pattern.
     */
    // NOLINTNEXTLINE(hicpp-explicit-conversions, google-explicit-constructor)
    operator Color() const
    {
        return color();
    }

    /**
     * Get the first color of the pattern.
     */
    Color color();

    /**
     * Get the first color of the pattern.
     */
    const Color& color() const;

    /**
     * Add a step to the gradient.
     */
    Pattern& step(StepScaler offset, const Color& color);

    /**
     * Get the type of pattern.
     */
    Type type() const { return m_type; }

    /**
     * Create a linear gradient from @ref start to @ref end.
     */
    void linear(const Point& start, const Point& end);

    /**
     * Create a radial gradient from @ref start to @ref end.
     */
    void radial(const Point& start, float start_radius,
                const Point& end, float end_radius);

    /// Get the starting point of the pattern.
    Point start() const { return m_start; }
    /// Set the starting point of the pattern.
    float start_radius() const { return m_start_radius; }
    /// Get the ending  point of the pattern.
    Point end() const { return m_end; }
    /// Set the ending  point of the pattern.
    float end_radius() const { return m_end_radius; }

    /// Get all of the steps of the pattern
    const StepArray& steps() const;

protected:

    /// @private
    static bool sort_by_first(const std::pair<float, Color>& a,
                              const std::pair<float, Color>& b)
    {
        return (a.first < b.first);
    }

    /// Steps of the pattern
    StepArray m_steps;
    /// Type of the pattern.
    Type m_type{Type::solid};
    /// Starting point of the pattern.
    Point m_start;
    /// Starting radius of the pattern.
    float m_start_radius{0};
    /// Ending point of the pattern.
    Point m_end;
    /// Ending radius of the pattern.
    float m_end_radius{0};
};

static_assert(detail::rule_of_5<Pattern>(), "must fulfill rule of 5");

}
}

#endif
