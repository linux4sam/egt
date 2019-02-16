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
#include <vector>

#ifdef DEBUG
#define DBG(x) do { std::cout << x << std::endl; } while (0)
#elif defined(NDEBUG)
#define DBG(x)
#else
#define DBG(x) do {                             \
        if (egt::detail::globalloglevel() > 1)  \
            std::cout << x << std::endl;        \
    } while(0)
#endif

#define INFO(x) do {                            \
        if (egt::detail::globalloglevel() > 0)  \
            std::cout << x << std::endl;        \
    } while(0)

#define ERR(x) do { std::cerr << x << std::endl; } while (0)

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

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
int& globalloglevel();

template <typename T>
class reverse_range
{
    T& x;

public:
    explicit reverse_range(T& x) : x(x) {}

    auto begin() const -> decltype(this->x.rbegin())
    {
        return x.rbegin();
    }

    auto end() const -> decltype(this->x.rend())
    {
        return x.rend();
    }
};

template <typename T>
reverse_range<T> reverse_iterate(T& x)
{
    return reverse_range<T>(x);
}

/**
 * Utility base class to make a derived class non-copyable.
 */
class noncopyable
{
public:
    noncopyable() = default;
    ~noncopyable() = default;

private:
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
};

std::string replace_all(std::string str, const std::string& from,
                        const std::string& to);

std::string exe_pwd();

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

}

namespace experimental
{
/**
 * Toy lua evaluation.
 */
double lua_evaluate(const std::string& expr);

std::vector<std::string> glob(const std::string& pattern);

class Fps
{
public:

    Fps()
    {
        start();
    }

    /**
     * Start/reset the counter.
     */
    void start()
    {
        m_start = std::chrono::steady_clock::now();
        m_frames = 0;
    }

    /**
     * Call at the end of every frame.
     */
    void end_frame()
    {
        m_frames++;

        auto now = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration<double>(now - m_start).count();
        if (diff > 1.0)
        {
            m_fps = m_frames / diff;
            m_ready = true;
            start();
        }
    }

    bool ready() const { return m_ready && m_frames > 0; }

    /**
     * Retrieve the current FPS value.
     */
    float fps()
    {
        m_ready = false;
        return m_fps;
    }


protected:
    std::chrono::time_point<std::chrono::steady_clock> m_start{};
    uint64_t m_frames{0};
    float m_fps{0.};
    bool m_ready{false};
};

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
