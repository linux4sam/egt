/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_UTILS_H
#define MUI_UTILS_H

/**
 * @file
 * @brief Working with basic utilities.
 */

#include <cmath>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>

#ifdef DEBUG
#define DBG(x) do { std::cout << x << std::endl; } while (0)
#else
#define DBG(x) do {                             \
        if (mui::detail::globalloglevel() > 1)  \
            std::cout << x << std::endl;        \
    } while(0)
#endif

#define INFO(x) do {                            \
        if (mui::detail::globalloglevel() > 0)  \
            std::cout << x << std::endl;        \
    } while(0)

#define ERR(x) do { std::cerr << x << std::endl; } while (0)

namespace mui
{
    template <typename T>
    void ignoreparam(T&&)
    {}

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
    }

    namespace experimental
    {
        /**
         * Toy lua evaluation.
         */
        double lua_evaluate(const std::string& expr);
    }

}

#endif
