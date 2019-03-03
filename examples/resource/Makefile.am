CUSTOM_CXXFLAGS = $(WARN_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	-isystem $(top_srcdir)/external/asio/asio/include \
	-DASIO_STANDALONE -DASIO_DISABLE_STD_FUTURE

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
resource_CXXFLAGS = $(LIBPLANES_CFLAGS) $(CAIRO_CFLAGS) $(LIBDRM_CFLAGS) $(TSLIB_CFLAGS) $(LUA_CFLAGS) $(CJSON_CFLAGS) $(GSTREAMER_CFLAGS) $(LIBJPEG_CFLAGS) $(LIBINPUT_CFLAGS) $(LIBRSVG_CFLAGS) $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
resource_LDADD = $(top_builddir)/src/libegt.la $(LIBPLANES_LIBS) $(CAIRO_LIBS) $(LIBDRM_LIBS) $(TSLIB_LIBS) $(LUA_LIBS) $(CJSON_LIBS) $(GSTREAMER_LIBS) $(LIBJPEG_LIBS) $(LIBINPUT_LIBS) $(LIBRSVG_LIBS)
resourcedir = $(prefix)/share/egt/examples/resource
resource_DATA = $(wildcard $(top_srcdir)/examples/resource/*.png *.bmp)
resource_LDFLAGS = $(AM_LDFLAGS)

EXTRA_DIST = $(wildcard $(top_srcdir)/examples/resource/*.png) \
	$(wildcard $(top_srcdir)/examples/resource/*.bmp)
