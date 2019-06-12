/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_UTILS_H
#define EGT_UTILS_H

/**
 * @file
 * @brief Working with basic utilities.
 */

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#define likely(x)       __builtin_expect((x), 1)
#define unlikely(x)     __builtin_expect((x), 0)

namespace egt
{
inline namespace v1
{
template <typename T>
void ignoreparam(T&&)
{}

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&& ... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

namespace detail
{

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
    if (std::fabs(old - to) > .00001)
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

namespace experimental
{
/**
 * Toy lua evaluation.
 */
double lua_evaluate(const std::string& expr);

#ifdef NDEBUG
inline void code_timer(bool, const std::string&, std::function<void ()> callback)
{
    callback();
}
#else
inline void code_timer(bool enable, const std::string& prefix, std::function<void ()> callback)
{
    if (enable)
    {
        auto start = std::chrono::steady_clock::now();

        callback();

        auto end = std::chrono::steady_clock::now();
        auto diff = end - start;

        std::cout << prefix <<
                  std::chrono::duration<double, std::milli>(diff).count() << std::endl;
    }
    else
    {
        callback();
    }
}
#endif

}

}
}

#endif
