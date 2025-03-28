CUSTOM_CXXFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	-isystem $(top_srcdir)/external/rapidxml \
	-isystem $(top_srcdir)/external/cxxopts/include \
	$(CODE_COVERAGE_CXXFLAGS)

CUSTOM_LDADD = $(CODE_COVERAGE_LDFLAGS)

AM_LDFLAGS = -pthread

bin_PROGRAMS = video camera capture

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
video_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS) \
	-DEXAMPLEDATA=\"$(datadir)/egt/examples/video\"
video_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
videodir = $(prefix)/share/egt/examples/video
video_DATA = $(wildcard $(top_srcdir)/examples/video/*.png) \
	$(wildcard $(top_srcdir)/examples/video/*.jpg) \
	video.xml
video_LDFLAGS = $(AM_LDFLAGS)

camera_SOURCES = camera.cpp
nodist_camera_SOURCES = $(BUILT_SOURCES)
camera_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS) \
	-DEXAMPLEDATA=\"$(datadir)/egt/examples/video\"
camera_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
camera_LDFLAGS = $(AM_LDFLAGS)

capture_SOURCES = capture.cpp
nodist_capture_SOURCES = $(BUILT_SOURCES)
capture_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS) \
	-DEXAMPLEDATA=\"$(datadir)/egt/examples/video\"
capture_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
capture_LDFLAGS = $(AM_LDFLAGS)

TIDY_FLAGS = $(CUSTOM_CXXFLAGS)
include ../../src/Makefile.tidy

EXTRA_DIST = $(wildcard $(top_srcdir)/examples/video/*.png) \
	video.xml
