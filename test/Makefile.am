AUTOMAKE_OPTIONS = subdir-objects

CUSTOM_CXXFLAGS = $(WARN_CFLAGS) \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	$(cairo_CFLAGS) \
	$(CODE_COVERAGE_CXXFLAGS)

CUSTOM_LDADD = $(cairo_LIBS) $(CODE_COVERAGE_LDFLAGS)

check_LTLIBRARIES = libgtest.la
libgtest_la_SOURCES = ../external/googletest/googletest/src/gtest-all.cc
libgtest_la_CPPFLAGS = -I$(top_srcdir)/external/googletest/googletest/include -I$(top_srcdir)/external/googletest/googletest
libgtest_la_LDFLAGS = -pthread

check_PROGRAMS = \
test

test_SOURCES = \
main.cpp

if HAVE_GSTREAMER
test_SOURCES += \
audio/audio.cpp \
video/video.cpp
endif

test_CPPFLAGS = -I$(top_srcdir)/external/googletest/googletest/include -I$(top_srcdir)/external/googletest/googletest -pthread
test_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
test_LDADD = libgtest.la $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
test_LDFLAGS = $(AM_LDFLAGS)

TESTS = test
