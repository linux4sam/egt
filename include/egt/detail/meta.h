/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_META_H
#define EGT_DETAIL_META_H

/**
 * @file
 * @brief Meta tools for compilation, debug, and language support.
 */

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <egt/detail/math.h>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#define likely(x)       __builtin_expect((x), 1)
#define unlikely(x)     __builtin_expect((x), 0)

namespace egt
{
inline namespace v1
{
namespace detail
{

/**
 * Utility function to safely ignore a parameter to a function.
 */
template <typename T>
void ignoreparam(T&&)
{}

/**
 * C++11 does not provide std::make_unique(), so we provide a basic implementation.
 */
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&& ... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...)); // NOLINT
}

#ifndef SWIG
/**
 * Range class to work with C++11 range based for loops in a reverse order.
 * @see reverse_iterate
 */
template <typename T>
class reverse_range
{
    T& m_x;
public:
    explicit reverse_range(T& x) : m_x(x) {}

    auto begin() const -> decltype(this->m_x.rbegin())
    {
        return m_x.rbegin();
    }

    auto end() const -> decltype(this->m_x.rend())
    {
        return m_x.rend();
    }
};

/**
 * Reverse iterator to work with C++11 range based for loops in a reverse order.
 *
 * @code{.cpp}
 * for (auto& child : detail::reverse_iterate(children))
 * {
 *     ...
 * }
 * @endcode
 */
template <typename T>
reverse_range<T> reverse_iterate(T& x)
{
    return reverse_range<T>(x);
}
#endif

/**
 * Utility base class to make a derived class non-copy-able.
 */
class noncopyable
{
public:
    noncopyable() = default;
    ~noncopyable() = default;
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
};

/**
 * Rule of 5 static assert all in one.
 *
 * Example usage is intended for static_assert:
 * @code{.cpp}
 * static_assert(detail::rule_of_5<Color>(),
 *               "Color : must fulfill rule of 5");
 * @endcode
 */
template <class T>
constexpr bool rule_of_5()
{
    static_assert(std::is_destructible<T>::value, "T : must be destructible");
    static_assert(std::is_copy_constructible<T>::value, "T : must be copy constructible");
    static_assert(std::is_move_constructible<T>::value, "T : must be move constructible");
    static_assert(std::is_copy_assignable<T>::value, "T : must be copy assignable");
    static_assert(std::is_move_assignable<T>::value, "T : must be move assignable");

    return std::is_destructible<T>::value && std::is_copy_constructible<T>::value
           && std::is_move_constructible<T>::value && std::is_copy_assignable<T>::value
           && std::is_move_assignable<T>::value;
}

template<class T>
inline void code_timer(bool enable, const std::string& prefix, const T& callback)
{
    if (enable)
    {
        const auto start = std::chrono::steady_clock::now();

        callback();

        const auto end = std::chrono::steady_clock::now();
        const auto diff = end - start;

        std::cout << prefix <<
                  std::chrono::duration<double, std::milli>(diff).count() << std::endl;
    }
    else
    {
        callback();
    }
}

/**
 * Utility to test and change a value if it is different.
 *
 * This is a very common pattern used by Widget functions to determine whether
 * damage() should be called when a property is changed.
 */
template<class T>
inline bool change_if_diff(T& old, const T& to)
{
    if (old != to)
    {
        old = to;
        return true;
    }

    return false;
}

template<>
inline bool change_if_diff(float& old, const float& to)
{
    if (!detail::float_compare(old, to))
    {
        old = to;
        return true;
    }

    return false;
}

template<>
inline bool change_if_diff(double& old, const double& to)
{
    if (!detail::float_compare(old, to))
    {
        old = to;
        return true;
    }

    return false;
}

/**
 * Utility to run a callback std::function when this object goes out of scope.
 *
 * This can be used to run a function when an instance of a scope_exit goes out
 * of scope or is deleted.
 */
struct scope_exit : public noncopyable
{
    explicit scope_exit(std::function<void()> f) noexcept
        : m_f(std::move(f))
    {}

    ~scope_exit()
    {
        if (m_f)
            m_f();
    }

protected:
    std::function<void()> m_f;
};

}
}
}

#endif
