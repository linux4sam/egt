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

#if defined _WIN32 || defined __CYGWIN__
#define EGT_HELPER_DLL_IMPORT __declspec(dllimport)
#define EGT_HELPER_DLL_EXPORT __declspec(dllexport)
#define EGT_HELPER_DLL_LOCAL
#else
#if __GNUC__ >= 4
#define EGT_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
#define EGT_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
#define EGT_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define EGT_HELPER_DLL_IMPORT
#define EGT_HELPER_DLL_EXPORT
#define EGT_HELPER_DLL_LOCAL
#endif
#endif

// Now we use the generic helper definitions above to define EGT_API and EGT_LOCAL.
// EGT_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
// EGT_LOCAL is used for non-api symbols.

#define EGT_DLL
#ifdef EGT_DLL // defined if EGT is compiled as a DLL
#ifdef EGT_DLL_EXPORTS // defined if we are building the EGT DLL (instead of using it)
#define EGT_API EGT_HELPER_DLL_EXPORT
#else
#define EGT_API EGT_HELPER_DLL_IMPORT
#endif // EGT_DLL_EXPORTS
#define EGT_LOCAL EGT_HELPER_DLL_LOCAL
#else // EGT_DLL is not defined: this means EGT is a static lib.
#define EGT_API
#define EGT_LOCAL
#endif // EGT_DLL

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
class ReverseRange
{
    T& m_x;
public:
    explicit ReverseRange(T& x) : m_x(x) {}

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
ReverseRange<T> reverse_iterate(T& x)
{
    return ReverseRange<T>(x);
}
#endif

/**
 * Utility base class to make a derived class non-copy-able.
 */
class EGT_API NonCopyable
{
public:
    NonCopyable() = default;
    ~NonCopyable() = default;
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
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
struct EGT_API ScopeExit : public NonCopyable
{
    explicit ScopeExit(std::function<void()> f) noexcept
        : m_f(std::move(f))
    {}

    ~ScopeExit()
    {
        if (m_f)
            m_f();
    }

protected:
    std::function<void()> m_f;
};

template<class T>
constexpr inline T bit(T n)
{
    return 1 << n;
}

}
}
}

#endif
