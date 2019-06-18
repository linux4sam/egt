/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/color.h"
#include <cassert>
#include <iostream>
#include <cmath>

namespace egt
{
inline namespace v1
{

std::ostream& operator<<(std::ostream& os, const Color& color)
{
    os << "(" << color.red() << "," << color.green() << "," <<
       color.blue() << "," << color.alpha() << ")";
    return os;
}

typedef struct
{
    double h; // angle in degrees
    double s; // a fraction between 0 and 1
    double v; // a fraction between 0 and 1
} hsv;

static hsv rgb2hsv(Color in)
{
    hsv out;

    auto min = in.redf() < in.greenf() ? in.redf() : in.greenf();
    min = min  < in.bluef() ? min  : in.bluef();

    auto max = in.redf() > in.greenf() ? in.redf() : in.greenf();
    max = max  > in.bluef() ? max  : in.bluef();

    out.v = max;
    auto delta = max - min;
    if (delta < 0.00001)
    {
        out.s = 0;
        out.h = 0;
        return out;
    }

    if (max > 0.0)
    {
        out.s = (delta / max);
    }
    else
    {
        out.s = 0.0;
        out.h = NAN;
        return out;
    }

    if (in.redf() >= max)
        out.h = (in.greenf() - in.bluef()) / delta;
    else if (in.greenf() >= max)
        out.h = 2.0 + (in.bluef() - in.redf()) / delta;
    else
        out.h = 4.0 + (in.redf() - in.greenf()) / delta;

    out.h *= 60.0;

    if (out.h < 0.0)
        out.h += 360.0;

    return out;
}

static Color hsv2rgb(hsv in)
{
    double      hh, p, q, t, ff;
    long        i;
    Color         out;

    if (in.s <= 0.0)
    {
        out.redf(in.v);
        out.greenf(in.v);
        out.bluef(in.v);
        return out;
    }

    hh = in.h;
    if (hh >= 360.0)
        hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch (i)
    {
    case 0:
        out.redf(in.v);
        out.greenf(t);
        out.bluef(p);
        break;
    case 1:
        out.redf(q);
        out.greenf(in.v);
        out.bluef(p);
        break;
    case 2:
        out.redf(p);
        out.greenf(in.v);
        out.bluef(t);
        break;

    case 3:
        out.redf(p);
        out.greenf(q);
        out.bluef(in.v);
        break;
    case 4:
        out.redf(t);
        out.greenf(p);
        out.bluef(in.v);
        break;
    case 5:
    default:
        out.redf(in.v);
        out.greenf(p);
        out.bluef(q);
        break;
    }
    return out;
}

void Color::get_hsvf(float* h, float* s, float* v, float* alpha)
{
    hsv c = rgb2hsv(*this);
    if (h)
        *h = c.h;
    if (s)
        *s = c.s;
    if (v)
        *v = c.v;
    if (alpha)
        *alpha = alphaf();
}

void Color::get_rgbaf(float* r, float* g, float* b, float* alpha)
{
    if (r)
        *r = redf();
    if (g)
        *g = greenf();
    if (b)
        *b = bluef();
    if (alpha)
        *alpha = alphaf();
}

Color Color::hsvf(float h, float s, float v, float alpha)
{
    auto c = hsv2rgb({h, s, v});
    c.alphaf(alpha);
    return c;
}

template<class T>
static inline T linear_interpolator(T a, T b, T t)
{
    return a * (1 - t) + b * t;
}

Color Color::interp_hsv(const Color& a, const Color& b, float t)
{
    hsv ca = rgb2hsv(a);
    hsv cb = rgb2hsv(b);
    hsv final;

    final.h = linear_interpolator(ca.h, cb.h, static_cast<double>(t));
    final.s = linear_interpolator(ca.s, cb.s, static_cast<double>(t));
    final.v = linear_interpolator(ca.v, cb.v, static_cast<double>(t));

    return Color(hsv2rgb(final),
                 a.alpha() + (b.alpha() - a.alpha()) * t);
}

Color Color::interp_rgba(const Color& a, const Color& b, float t)
{
    //return a + (b - a) * t;

    return Color::rgbaf(a.redf() + (b.redf() - a.redf()) * t,
                        a.greenf() + (b.greenf() - a.greenf()) * t,
                        a.bluef() + (b.bluef() - a.bluef()) * t,
                        a.alphaf() + (b.alphaf() - a.alphaf()) * t);

}

namespace experimental
{

template<class T>
constexpr const T& clamp(const T& v, const T& lo, const T& hi)
{
    return assert(!(hi < lo)),
           (v < lo) ? lo : (hi < v) ? hi : v;
}


Color ColorMap::interp(float t) const
{
    if (empty())
        return {};

    const size_t steps = m_steps.size() - 1;
    const float s = clamp(t, 0.f, 1.f);
    const float sf = s * steps;
    const size_t k = floorf(sf);

    if (k + 1 == m_steps.size())
        return m_steps.back();

    const auto u = fmodf(sf, 1.f);

    switch (m_interp)
    {
    case interpolation::rgba:
        return Color::interp_rgba(m_steps[k], m_steps[k + 1], u);
    case interpolation::hsv:
        return Color::interp_hsv(m_steps[k], m_steps[k + 1], u);
    }

    return {};
}

}

}
}
