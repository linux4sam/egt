/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/color.h"
#include "egt/detail/math.h"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <sstream>

namespace egt
{
inline namespace v1
{

std::string Color::hex() const
{
    RGBAType value = ((red() & 0xff) << 24) |
                     ((green() & 0xff) << 16) |
                     ((blue() & 0xff) << 8) |
                     (alpha() & 0xff);

    std::ostringstream ss;
    ss << std::hex << std::setw(8) << std::setfill('0') << value;
    return ss.str();
}

std::ostream& operator<<(std::ostream& os, const Color& color)
{
    return os << fmt::format("({},{},{},{})", color.red(), color.green(), color.blue(), color.alpha());
}

struct hsv
{
    double h; // angle in degrees
    double s; // a fraction between 0 and 1
    double v; // a fraction between 0 and 1
};

static hsv rgb2hsv(const Color& in)
{
    hsv out{};

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
    Color out;

    if (in.s <= 0.0)
    {
        out.redf(in.v);
        out.greenf(in.v);
        out.bluef(in.v);
        return out;
    }

    double hh = in.h;
    if (hh >= 360.0)
        hh = 0.0;
    hh /= 60.0;
    auto i = (int64_t)hh;
    double ff = hh - i;
    double p = in.v * (1.0 - in.s);
    double q = in.v * (1.0 - (in.s * ff));
    double t = in.v * (1.0 - (in.s * (1.0 - ff)));

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

struct hsl
{
    double h; // angle in degrees
    double s; // a fraction between 0 and 1
    double l; // a fraction between 0 and 1
};

static hsl rgb2hsl(const Color& rgb)
{
    const auto cmax = std::max(std::max(rgb.redf(), rgb.greenf()), rgb.bluef());
    const auto cmin = std::min(std::min(rgb.redf(), rgb.greenf()), rgb.bluef());
    const auto delta = cmax - cmin;

    hsl hsl =
    {
        0.f,
        0.f,
        (cmax + cmin) / 2.f
    };

    if (detail::float_equal(delta, 0.f))
        return hsl;

    if (detail::float_equal(cmax, rgb.redf()))
    {
        hsl.h = fmodf((rgb.greenf() - rgb.bluef()) / delta, 6.f);
    }
    else if (detail::float_equal(cmax, rgb.greenf()))
    {
        hsl.h = (rgb.bluef() - rgb.redf()) / delta + 2.f;
    }
    else
    {
        hsl.h = (rgb.redf() - rgb.greenf()) / delta + 4.f;
    }

    hsl.s = delta / (1.f - std::abs(2.f * hsl.l - 1.f));
    hsl.h /= 6.f;
    hsl.h = fmodf(hsl.h + 1.f, 1.f);

    return hsl;
}

static Color hsl2rgb(hsl hsl)
{
    const float k = hsl.h * 6.f;
    const float C = (1.f - std::abs(2.f * hsl.l - 1.f)) * hsl.s;
    const float X = C * (1.f - std::abs(fmodf(k, 2.f) - 1.f));
    const float m = hsl.l - C / 2.f;
    const auto d = int(floorf(k));

    Color out;

    switch (d)
    {
    case 0: out = Color::rgbaf(C, X, 0.f); break;
    case 1: out = Color::rgbaf(X, C, 0.f); break;
    case 2: out = Color::rgbaf(0.f, C, X); break;
    case 3: out = Color::rgbaf(0.f, X, C); break;
    case 4: out = Color::rgbaf(X, 0.f, C); break;
    default: out = Color::rgbaf(C, 0.f, X); break;
    }

    out = out + m;
    return out;
}

void Color::get_hsvf(float& h, float& s, float& v, float* alpha)
{
    hsv c = rgb2hsv(*this);
    h = c.h;
    s = c.s;
    v = c.v;
    if (alpha)
        *alpha = alphaf();
}

void Color::get_hslf(float& h, float& s, float& l, float* alpha)
{
    hsl c = rgb2hsl(*this);
    h = c.h;
    s = c.s;
    l = c.l;
    if (alpha)
        *alpha = alphaf();
}

void Color::get_rgbaf(float& r, float& g, float& b, float* alpha)
{
    r = redf();
    g = greenf();
    b = bluef();
    if (alpha)
        *alpha = alphaf();
}

Color Color::hsvf(float h, float s, float v, float alpha)
{
    auto c = hsv2rgb({h, s, v});
    c.alphaf(alpha);
    return c;
}

Color Color::hslf(float h, float s, float l, float alpha)
{
    auto c = hsl2rgb({h, s, l});
    c.alphaf(alpha);
    return c;
}

template<class T>
static constexpr T linear_interpolator(T a, T b, T t)
{
    return a * (1 - t) + b * t;
}

Color Color::interp_hsv(const Color& a, const Color& b, float t)
{
    hsv ca = rgb2hsv(a);
    hsv cb = rgb2hsv(b);
    hsv final{};

    final.h = linear_interpolator(ca.h, cb.h, static_cast<double>(t));
    final.s = linear_interpolator(ca.s, cb.s, static_cast<double>(t));
    final.v = linear_interpolator(ca.v, cb.v, static_cast<double>(t));

    return {hsv2rgb(final),
            static_cast<ComponentType>(a.alpha() + (b.alpha() - a.alpha()) * t)};
}

Color Color::interp_hsl(const Color& a, const Color& b, float t)
{
    hsl hsla = rgb2hsl(a);
    hsl hslb = rgb2hsl(b);

    hsl hslaa = hsla;

    if (std::abs(hslaa.h - hslb.h) > 0.5f)
    {
        if (hslaa.h > hslb.h)
        {
            hslaa.h -= 1.f;
        }
        else
        {
            hslaa.h += 1.f;
        }
    }

    hsl hsl{};
    hsl.h = hslaa.h + t * (hslb.h - hslaa.h);
    hsl.s = hslaa.s + t * (hslb.s - hslaa.s);
    hsl.l = hslaa.l + t * (hslb.l - hslaa.l);
    hsl.h = fmodf(hsl.h + 1.f, 1.f);

    return {hsl2rgb(hsl),
            static_cast<ComponentType>(a.alpha() + (b.alpha() - a.alpha()) * t)};
}

Color Color::interp_rgba(const Color& a, const Color& b, float t)
{
    return Color::rgbaf(a.redf() + (b.redf() - a.redf()) * t,
                        a.greenf() + (b.greenf() - a.greenf()) * t,
                        a.bluef() + (b.bluef() - a.bluef()) * t,
                        a.alphaf() + (b.alphaf() - a.alphaf()) * t);

}

namespace experimental
{

Color ColorMap::interp(float t) const
{
    if (empty())
        return {};

    const size_t steps = m_steps.size() - 1;
    const float sf = detail::clamp<float>(t, 0.f, 1.f) * steps;
    const size_t k = floorf(sf);

    if (k + 1 >= m_steps.size())
        return m_steps.back();

    const auto u = fmodf(sf, 1.f);

    Color result;
    switch (m_interp)
    {
    case Interpolation::rgba:
        result = Color::interp_rgba(m_steps[k], m_steps[k + 1], u);
        break;
    case Interpolation::hsv:
        result = Color::interp_hsv(m_steps[k], m_steps[k + 1], u);
        break;
    case Interpolation::hsl:
        result = Color::interp_hsl(m_steps[k], m_steps[k + 1], u);
        break;
    default:
        assert(!"unhandled Interpolation type");
        break;
    }

    return result;
}

Color ColorMap::interp_cached(float t) const
{
    if (empty())
        return {};

    const size_t steps = m_steps.size() - 1;
    const float sf = detail::clamp<float>(t, 0.f, 1.f) * steps;
    const size_t k = floorf(sf);

    if (k + 1 >= m_steps.size())
        return m_steps.back();

    const auto u = fmodf(sf, 1.f);

    const size_t index = (k + u) * 10000;
    auto i = m_cache[static_cast<size_t>(m_interp)].find(index);
    if (i != m_cache[static_cast<size_t>(m_interp)].end())
        return i->second;

    auto result = ColorMap::interp(t);
    m_cache[static_cast<size_t>(m_interp)].insert(std::make_pair(index, result));
    return result;
}

}

}
}
