/*
 * Copyright (C) 2024 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SRC_DETAIL_CAIROABSTRACTION_H
#define EGT_SRC_DETAIL_CAIROABSTRACTION_H

/**
 * @file
 * @brief Cairo abstraction layer.
 */

#include <cairo.h>
#include <memory>

namespace egt
{
inline namespace v1
{

namespace detail
{

template<typename T, void (D)(T*)>
class CairoPointerAbstraction
{
public:
    CairoPointerAbstraction(T* ptr = nullptr)
        : m_ptr(ptr, D)
    {}

    explicit operator bool() const noexcept { return static_cast<bool>(m_ptr); }

    operator T* () const noexcept { return m_ptr.get(); }

    T* get() const noexcept { return m_ptr.get(); }

private:
    std::unique_ptr<T, decltype(D)> m_ptr;
};

using CairoScaledFontAbstraction = CairoPointerAbstraction<cairo_scaled_font_t, cairo_scaled_font_destroy>;

class InternalFont : public CairoScaledFontAbstraction
{
public:
    using CairoScaledFontAbstraction::CairoScaledFontAbstraction;
};

using CairoPatternAbstraction = CairoPointerAbstraction<cairo_pattern_t, cairo_pattern_destroy>;

class InternalPattern : public CairoPatternAbstraction
{
public:
    using CairoPatternAbstraction::CairoPatternAbstraction;
};

}
}
}

#endif
