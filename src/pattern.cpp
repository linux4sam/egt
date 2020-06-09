/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/pattern.h"

namespace egt
{
inline namespace v1
{

Pattern::Pattern(const Color& color)
    : m_steps(1, std::make_pair(0.f, color))
{
}

Pattern::Pattern(Type type, StepArray steps)
    : m_steps(std::move(steps)),
      m_type(type)
{
    std::sort(m_steps.begin(), m_steps.end(), sort_by_first);
}

Pattern::Pattern(StepArray steps,
                 const Point& start,
                 const Point& end)
    : m_steps(std::move(steps)),
      m_type(Type::linear),
      m_start(start),
      m_end(end)
{
    std::sort(m_steps.begin(), m_steps.end(), sort_by_first);
}

Pattern::Pattern(StepArray steps,
                 const Point& start,
                 float start_radius,
                 const Point& end,
                 float end_radius)
    : m_steps(std::move(steps)),
      m_type(Type::radial),
      m_start(start),
      m_start_radius(start_radius),
      m_end(end),
      m_end_radius(end_radius)
{
    std::sort(m_steps.begin(), m_steps.end(), sort_by_first);
}

Color Pattern::color()
{
    if (!m_steps.empty())
        return m_steps.begin()->second;

    return {};
}

const Color& Pattern::color() const
{
    if (!m_steps.empty())
        return m_steps.begin()->second;

    static Color tmp;
    return tmp;
}

Pattern& Pattern::step(StepScaler offset, const Color& color)
{
    m_steps.emplace_back(offset, color);
    std::sort(m_steps.begin(), m_steps.end(), sort_by_first);
    m_pattern.reset();
    return *this;
}

void Pattern::linear(const Point& start, const Point& end)
{
    m_type = Type::linear;
    m_start = start;
    m_end = end;
    m_pattern.reset();
}

void Pattern::radial(const Point& start, float start_radius,
                     const Point& end, float end_radius)
{
    m_type = Type::radial;
    m_start = start;
    m_start_radius = start_radius;
    m_end = end;
    m_end_radius = end_radius;
    m_pattern.reset();
}

const Pattern::StepArray& Pattern::steps() const
{
    return m_steps;
}

void Pattern::create_pattern() const
{
    switch (type())
    {
    case Pattern::Type::linear:
    {
        m_pattern =
            shared_cairo_pattern_t(cairo_pattern_create_linear(starting().x(),
                                   starting().y(),
                                   ending().x(),
                                   ending().y()),
                                   cairo_pattern_destroy);

        for (const auto& step : steps())
        {
            cairo_pattern_add_color_stop_rgba(m_pattern.get(),
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
        m_pattern =
            shared_cairo_pattern_t(cairo_pattern_create_radial(starting().x(),
                                   starting().y(),
                                   starting_radius(),
                                   ending().x(),
                                   ending().y(),
                                   ending_radius()),
                                   cairo_pattern_destroy);
        for (const auto& step : steps())
        {
            cairo_pattern_add_color_stop_rgba(m_pattern.get(),
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
        m_pattern =
            shared_cairo_pattern_t(cairo_pattern_create_rgba(color().redf(),
                                   color().greenf(),
                                   color().bluef(),
                                   color().alphaf()),
                                   cairo_pattern_destroy);
        break;
    }
    }

}

}
}
