#include "utils.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static int m_v = 0;

void LOG_VERBOSE(int verbose)
{
	m_v = verbose;
}

static int envset = -1;

void LOG(const char *format, ...)
{
	va_list ap;

	if (envset < 0)
		envset = !!getenv("LIBPLANES_DEBUG");

	if (m_v || envset)
	{
		va_start(ap, format);
		vfprintf(stderr, format, ap);
		va_end(ap);
	}
}
