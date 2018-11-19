AUTOMAKE_OPTIONS = subdir-objects

AM_CXXFLAGS = $(WARN_CFLAGS) -I$(top_srcdir) -I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/external/asio/asio/include \
	-DASIO_STANDALONE -DASIO_DISABLE_STD_FUTURE

bin_PROGRAMS = video

MRESG_V_GEN = $(mresg_v_GEN_$(V))
mresg_v_GEN_ = $(mresg_v_GEN_$(AM_DEFAULT_VERBOSITY))
mresg_v_GEN_0 = @echo "  RES     " $@;

resources = $(wildcard $(top_srcdir)/examples/video/*.png)
rc.cpp: $(resources) Makefile
	$(MRESG_V_GEN)$(top_srcdir)/scripts/mresg $(resources) -o $@
BUILT_SOURCES = rc.cpp
CLEANFILES = $(BUILT_SOURCES)

video_SOURCES = video.cpp
nodist_video_SOURCES = $(BUILT_SOURCES)
video_CXXFLAGS = $(AM_CXXFLAGS) $(LIBPLANES_CFLAGS) $(CAIRO_CFLAGS) $(LIBDRM_CFLAGS) $(TSLIB_CFLAGS) $(LUA_CFLAGS) $(CJSON_CFLAGS) $(GSTREAMER_CFLAGS) $(LIBJPEG_CFLAGS) $(LIBINPUT_CFLAGS)
video_LDADD = $(top_builddir)/src/libegt.la $(LIBPLANES_LIBS) $(CAIRO_LIBS) $(LIBDRM_LIBS) $(TSLIB_LIBS) $(LUA_LIBS) $(CJSON_LIBS) $(GSTREAMER_LIBS) $(LIBJPEG_LIBS) $(LIBINPUT_LIBS)
videodir = $(prefix)/share/egt/examples/video
video_DATA = $(wildcard $(top_srcdir)/examples/video/*.png)
video_LDFLAGS = $(AM_LDFLAGS)

EXTRA_DIST = $(wildcard $(top_srcdir)/examples/video/*.png)
