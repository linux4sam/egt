CUSTOM_CXXFLAGS = $(WARN_CFLAGS) \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	-isystem $(top_srcdir)/external/rapidxml \
	$(LIBPLANES_CFLAGS) \
	$(CAIRO_CFLAGS) \
	$(LIBDRM_CFLAGS) \
	$(TSLIB_CFLAGS) \
	$(LUA_CFLAGS) \
	$(GSTREAMER_CFLAGS) \
	$(LIBJPEG_CFLAGS) \
	$(LIBINPUT_CFLAGS) \
	$(LIBCURL_CFLAGS) \
	$(LIBRSVG_CFLAGS) \
	$(CODE_COVERAGE_CXXFLAGS)

CUSTOM_LDADD = $(LIBPLANES_LIBS) $(CAIRO_LIBS) $(LIBDRM_LIBS) $(TSLIB_LIBS) \
	$(LUA_LIBS) $(GSTREAMER_LIBS) $(LIBJPEG_LIBS) \
	$(LIBINPUT_LIBS) $(MAGIC_LIB) $(LIBRSVG_LIBS) $(LIBCURL_LIBS) \
	$(CODE_COVERAGE_LDFLAGS)

AM_LDFLAGS = -pthread

bin_PROGRAMS = video camera

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
video_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
video_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
videodir = $(prefix)/share/egt/examples/video
video_DATA = $(wildcard $(top_srcdir)/examples/video/*.png) \
	video.xml
video_LDFLAGS = $(AM_LDFLAGS)

camera_SOURCES = camera.cpp
nodist_camera_SOURCES = $(BUILT_SOURCES)
camera_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
camera_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
cameradir = $(prefix)/share/egt/examples/camera
camera_DATA = $(wildcard $(top_srcdir)/examples/video/*.png)
camera_LDFLAGS = $(AM_LDFLAGS)

EXTRA_DIST = $(wildcard $(top_srcdir)/examples/video/*.png) \
	video.xml
