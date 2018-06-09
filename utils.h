/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */

#ifndef UTILS_H
#define UTILS_H

#include <math.h>
#include <functional>

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

class own_double_less : public std::binary_function<double,double,bool>
{
public:
	own_double_less(double arg_ = 1e-7 ) : epsilon(arg_) {}
	bool operator()(const double &left, const double &right) const
	{
		return (fabs(left - right) > epsilon) && (left < right);
	}
	double epsilon;
};

void LOG_VERBOSE(int verbose);

extern void LOG(const char *format, ...);

#endif
