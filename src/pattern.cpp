/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/cairoabstraction.h"
#include "egt/pattern.h"

namespace egt
{
inline namespace v1
{

struct Pattern::PatternImpl
{
    PatternImpl() = default;

    explicit PatternImpl(StepArray steps)
        : m_steps(std::move(steps))
    {}

    PatternImpl(StepArray steps,
                const Point& start,
                const Point& end)
        : m_steps(std::move(steps)),
          m_start(start),
          m_end(end)
    {}

    PatternImpl(StepArray steps,
                const Point& start,
                float start_radius,
                const Point& end,
                float end_radius)
        : m_steps(std::move(steps)),
          m_start(start),
          m_start_radius(start_radius),
          m_end(end),
          m_end_radius(end_radius)
    {}

    bool operator==(const Pattern::PatternImpl& rhs) const
    {
        return m_steps == rhs.m_steps &&
               m_start == rhs.m_start &&
               detail::float_equal(m_start_radius, rhs.m_start_radius) &&
               m_end == rhs.m_end &&
               detail::float_equal(m_end_radius, rhs.m_end_radius);
    }

    /// Steps of the pattern
    StepArray m_steps;
    /// Starting point of the pattern.
    Point m_start;
    /// Starting radius of the pattern.
    float m_start_radius{0};
    /// Ending point of the pattern.
    Point m_end;
    /// Ending radius of the pattern.
    float m_end_radius{0};
};

Pattern::Pattern(Type type, const StepArray& steps)
    : m_type(type),
      m_impl(new PatternImpl(steps))
{
    assert(type != Type::solid);
    std::sort(m_impl->m_steps.begin(), m_impl->m_steps.end(), sort_by_first);
}

Pattern::Pattern(const StepArray& steps,
                 const Point& start,
                 const Point& end)
    : m_type(Type::linear),
      m_impl(new PatternImpl(steps, start, end))
{
    std::sort(m_impl->m_steps.begin(), m_impl->m_steps.end(), sort_by_first);
}

Pattern::Pattern(const StepArray& steps,
                 const Point& start,
                 float start_radius,
                 const Point& end,
                 float end_radius)
    : m_type(Type::radial),
      m_impl(new PatternImpl(steps, start, start_radius, end, end_radius))
{
    std::sort(m_impl->m_steps.begin(), m_impl->m_steps.end(), sort_by_first);
}

Pattern::Pattern(const Pattern& rhs)
    : m_type(rhs.m_type),
      m_color(rhs.m_color),
      m_pattern(rhs.m_pattern)
{
    if (rhs.m_impl)
    {
        // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
        m_impl = new PatternImpl();
        m_impl->m_steps = rhs.m_impl->m_steps;
        m_impl->m_start = rhs.m_impl->m_start;
        m_impl->m_start_radius = rhs.m_impl->m_start_radius;
        m_impl->m_end = rhs.m_impl->m_end;
        m_impl->m_end_radius = rhs.m_impl->m_end_radius;
    }
}

Pattern::Pattern(Pattern&& rhs) noexcept
    : m_type(rhs.m_type),
      m_color(rhs.m_color),
      m_impl(std::exchange(rhs.m_impl, nullptr)),
      m_pattern(std::move(rhs.m_pattern))
{}

Pattern& Pattern::operator=(const Pattern& rhs)
{
    if (this != &rhs)
    {
        m_type = rhs.m_type;
        m_color = rhs.m_color;
        if (m_impl)
        {
            delete m_impl;
            m_impl = nullptr;
        }
        if (rhs.m_impl)
        {
            // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
            m_impl = new PatternImpl();
            m_impl->m_steps = rhs.m_impl->m_steps;
            m_impl->m_start = rhs.m_impl->m_start;
            m_impl->m_start_radius = rhs.m_impl->m_start_radius;
            m_impl->m_end = rhs.m_impl->m_end;
            m_impl->m_end_radius = rhs.m_impl->m_end_radius;
        }
        m_pattern = rhs.m_pattern;
    }
    return *this;
}

Pattern& Pattern::operator=(Pattern&& rhs) noexcept
{
    m_type = rhs.m_type;
    m_color = rhs.m_color;
    m_impl = rhs.m_impl;
    rhs.m_impl = nullptr;
    m_pattern = std::move(rhs.m_pattern);
    return *this;
}

bool Pattern::operator==(const Pattern& rhs) const
{
    if (m_type != rhs.m_type)
        return false;

    if (m_type == Type::solid)
        return m_color == rhs.m_color;

    if (m_impl == rhs.m_impl)
        return true;

    if (!m_impl || !rhs.m_impl)
        return false;

    return *m_impl == *rhs.m_impl;
}

Pattern& Pattern::step(StepScaler offset, const Color& color)
{
    if (m_type == Type::solid)
        throw std::runtime_error("pattern is not a solid color");

    if (!m_impl)
        // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
        m_impl = new PatternImpl();
    m_impl->m_steps.emplace_back(offset, color);
    std::sort(m_impl->m_steps.begin(), m_impl->m_steps.end(), sort_by_first);

    m_pattern.reset();
    return *this;
}

void Pattern::linear(const Point& start, const Point& end)
{
    if (!m_impl)
        // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
        m_impl = new PatternImpl();

    if ((m_type != Type::linear) && (m_type != Type::linear_vertical))
        m_type = Type::linear;
    m_impl->m_start = start;
    m_impl->m_end = end;

    m_pattern.reset();
}

void Pattern::radial(const Point& start, float start_radius,
                     const Point& end, float end_radius)
{
    if (!m_impl)
        // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
        m_impl = new PatternImpl();

    m_type = Type::radial;
    m_impl->m_start = start;
    m_impl->m_start_radius = start_radius;
    m_impl->m_end = end;
    m_impl->m_end_radius = end_radius;

    m_pattern.reset();
}

Color Pattern::first() const
{
    if (m_type != Type::solid && !m_impl->m_steps.empty())
        return m_impl->m_steps[0].second;
    return m_color;
}

Point Pattern::starting() const
{
    if (m_impl)
        return m_impl->m_start;
    return {};
}

float Pattern::starting_radius() const
{
    if (m_impl)
        return m_impl->m_start_radius;
    return 0;
}

Point Pattern::ending() const
{
    if (m_impl)
        return m_impl->m_end;
    return {};
}

float Pattern::ending_radius() const
{
    if (m_impl)
        return m_impl->m_end_radius;
    return 0;
}

const Pattern::StepArray& Pattern::steps() const
{
    if (m_impl)
        return m_impl->m_steps;

    throw std::runtime_error("pattern is not a solid color");
}

void Pattern::create_pattern() const
{
    switch (type())
    {
    case Pattern::Type::linear:
    case Pattern::Type::linear_vertical:
    {
        m_pattern = std::make_shared<detail::InternalPattern>(
                        cairo_pattern_create_linear(starting().x(),
                                starting().y(),
                                ending().x(),
                                ending().y()));

        for (const auto& step : steps())
        {
            cairo_pattern_add_color_stop_rgba(*m_pattern,
                                              step.first,
                                              step.second.redf(),
                                              step.second.greenf(),
                                              step.second.bluef(),
                                              step.second.alphaf());
        }
        break;
    }
    case Pattern::Type::radial:
    {
        m_pattern = std::make_shared<detail::InternalPattern>(
                        cairo_pattern_create_radial(starting().x(),
                                starting().y(),
                                starting_radius(),
                                ending().x(),
                                ending().y(),
                                ending_radius()));
        for (const auto& step : steps())
        {
            cairo_pattern_add_color_stop_rgba(*m_pattern,
                                              step.first,
                                              step.second.redf(),
                                              step.second.greenf(),
                                              step.second.bluef(),
                                              step.second.alphaf());
        }
        break;
    }
    case Pattern::Type::solid:
    {
        m_pattern = std::make_shared<detail::InternalPattern>(
                        cairo_pattern_create_rgba(solid().redf(),
                                solid().greenf(),
                                solid().bluef(),
                                solid().alphaf()));
        break;
    }
    }

}

Pattern::~Pattern() noexcept
{
    delete m_impl;
}

}
}
