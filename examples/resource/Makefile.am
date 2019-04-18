CUSTOM_CXXFLAGS = $(WARN_CFLAGS) \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	-isystem $(top_srcdir)/external/rapidxml \
	$(CAIRO_CFLAGS) \
	$(GSTREAMER_CFLAGS) \
	$(CODE_COVERAGE_CXXFLAGS)

CUSTOM_LDADD = $(CAIRO_LIBS) \
	$(GSTREAMER_LIBS) \
	$(CODE_COVERAGE_LDFLAGS)

AM_LDFLAGS = -pthread

bin_PROGRAMS = resource

MRESG_V_GEN = $(mresg_v_GEN_$(V))
mresg_v_GEN_ = $(mresg_v_GEN_$(AM_DEFAULT_VERBOSITY))
mresg_v_GEN_0 = @echo "  RES     " $@;

resources = $(wildcard $(top_srcdir)/examples/resource/*.png *.bmp)
rc.cpp: $(resources) Makefile
	$(MRESG_V_GEN)$(top_srcdir)/scripts/mresg $(resources) -o $@
BUILT_SOURCES = rc.cpp
CLEANFILES = $(BUILT_SOURCES)

resource_SOURCES = resource.cpp
nodist_resource_SOURCES = $(BUILT_SOURCES)
resource_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
resource_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
resourcedir = $(prefix)/share/egt/examples/resource
resource_DATA = $(wildcard $(top_srcdir)/examples/resource/*.png *.bmp)
resource_LDFLAGS = $(AM_LDFLAGS)

EXTRA_DIST = $(wildcard $(top_srcdir)/examples/resource/*.png) \
	$(wildcard $(top_srcdir)/examples/resource/*.bmp)
