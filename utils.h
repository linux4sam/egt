/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */

#ifndef UTILS_H
#define UTILS_H

#include <math.h>
#include <functional>
#include <stdlib.h>

template <typename T>
class reverse_range
{
    T &x;

public:
    reverse_range(T &x) : x(x) {}

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
reverse_range<T> reverse_iterate(T &x)
{
    return reverse_range<T>(x);
}

void LOG_VERBOSE(int verbose);

extern void LOG(const char *format, ...);

#ifdef DEBUG
#define DBG(x) do { std::cout << x << std::endl; } while (0)
#else
#define DBG(x) do {						\
		extern int globalenvset;			\
		if (globalenvset < 0)				\
			globalenvset = !!getenv("MUI_DEBUG");	\
		if (globalenvset)				\
			std::cout << x << std::endl;		\
	} while(0)
#endif

#define ERR(x) do { std::cerr << x << std::endl; } while (0)

#endif
