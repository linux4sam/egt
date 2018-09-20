AM_CXXFLAGS = $(WARN_CFLAGS) -I$(top_srcdir) -I$(top_srcdir)/include -I$(top_srcdir)/include/mui

lib_LTLIBRARIES = libmui.la
libmui_la_CXXFLAGS = $(AM_CXXFLAGS) $(LIBPLANES_CFLAGS) $(CAIRO_CFLAGS) $(LIBDRM_CFLAGS) $(TSLIB_CFLAGS) $(LUA_CFLAGS) $(CJSON_CFLAGS) $(GSTREAMER_CFLAGS)
libmui_la_SOURCES = \
animation.cpp \
chart.cpp \
color.cpp \
event_loop.cpp \
font.cpp \
geometry.cpp \
imagecache.cpp \
input.cpp \
kmsscreen.cpp \
painter.cpp \
palette.cpp \
resource.cpp \
screen.cpp \
sprite.cpp \
timer.cpp \
tools.cpp \
utils.cpp \
video.cpp \
widget.cpp \
window.cpp \
x11screen.cpp

libmuiincludedir = ${includedir}/mui
libmuiinclude_HEADERS = \
$(top_srcdir)/include/mui/animation.h \
$(top_srcdir)/include/mui/chart.h \
$(top_srcdir)/include/mui/color.h \
$(top_srcdir)/include/mui/event_loop.h \
$(top_srcdir)/include/mui/font.h \
$(top_srcdir)/include/mui/geometry.h \
$(top_srcdir)/include/mui/imagecache.h \
$(top_srcdir)/include/mui/input.h \
$(top_srcdir)/include/mui/kmsscreen.h \
$(top_srcdir)/include/mui/painter.h \
$(top_srcdir)/include/mui/palette.h \
$(top_srcdir)/include/mui/piechart.h \
$(top_srcdir)/include/mui/resource.h \
$(top_srcdir)/include/mui/screen.h \
$(top_srcdir)/include/mui/sprite.h \
$(top_srcdir)/include/mui/timer.h \
$(top_srcdir)/include/mui/tools.h \
$(top_srcdir)/include/mui/ui.h \
$(top_srcdir)/include/mui/utils.h \
$(top_srcdir)/include/mui/video.h \
$(top_srcdir)/include/mui/widget.h \
$(top_srcdir)/include/mui/window.h \
$(top_srcdir)/include/mui/x11screen.h


libmui_la_LDFLAGS = -version-number 1:0:0 -no-undefined

pkgconfigdir = @pkgconfigdir@
pkgconfig_DATA = $(top_builddir)/libmui.pc
