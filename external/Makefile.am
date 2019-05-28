noinst_LTLIBRARIES = libkplot.la libspdlog.la

MRESG_V_GEN = $(mresg_v_GEN_$(V))
mresg_v_GEN_ = $(mresg_v_GEN_$(AM_DEFAULT_VERBOSITY))
mresg_v_GEN_0 = @echo "  GEN     " $@;

$(top_builddir)/external/kplot/compat.h: $(top_srcdir)/external/kplot/compat.pre.h $(top_srcdir)/external/kplot/compat.post.h
	$(MRESG_V_GEN)( mkdir -p $(top_builddir)/external/kplot/ ; \
	  cat $(top_srcdir)/external/kplot/compat.pre.h ; \
	  CC="$(CC)" CFLAGS="$(CFLAGS)" sh $(top_srcdir)/external/kplot/compat.sh reallocarray ; \
	  cat $(top_srcdir)/external/kplot/compat.post.h ; ) >$@

libkplot_la_SOURCES = \
$(top_builddir)/external/kplot/compat.h \
kplot/compat.post.h \
kplot/compat.pre.h \
kplot/colours.c \
kplot/extern.h \
kplot/kplot.h \
kplot/array.c \
kplot/border.c \
kplot/bucket.c \
kplot/buffer.c \
kplot/draw.c \
kplot/grid.c \
kplot/hist.c \
kplot/label.c \
kplot/kdata.c \
kplot/kplot.c \
kplot/margin.c \
kplot/mean.c \
kplot/plotctx.c \
kplot/reallocarray.c \
kplot/stddev.c \
kplot/tic.c \
kplot/vector.c

libkplot_la_CFLAGS = $(CAIRO_CFLAGS) \
	-I$(top_builddir)/external/kplot \
	$(AM_CFLAGS)

libspdlog_la_SOURCES = spdlog/src/spdlog.cpp

libspdlog_la_CXXFLAGS = \
	-I$(top_srcdir)/external/spdlog/include \
	$(AM_CXXFLAGS)

CLEANFILES = $(top_builddir)/external/kplot/compat.h
BUILT_SOURCES = $(top_builddir)/external/kplot/compat.h

dist-hook:
	for file in $(BUILT_SOURCES) ; do \
		rm -f $(distdir)/$$file ; \
	done

EXTRA_DIST = \
	rapidxml \
	layout \
	kplot/compat.sh \
	utfcpp \
	spdlog \
	googletest

if HAVE_SIMD
all-local:
	cd Simd/prj/cmake/ && $(MAKE) $(AM_MAKEFLAGS) all

clean-local:
	cd Simd/prj/cmake/ && $(MAKE) $(AM_MAKEFLAGS) clean
endif
