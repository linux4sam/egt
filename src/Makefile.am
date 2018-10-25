#AUTOMAKE_OPTIONS = subdir-objects
AM_CXXFLAGS = $(WARN_CFLAGS) -I$(top_srcdir) -I$(top_srcdir)/include -I$(top_srcdir)/src/images/jpeg \
	-I$(top_srcdir)/external/asio/asio/include \
	-DASIO_STANDALONE -DASIO_DISABLE_STD_FUTURE

if HAVE_KPLOT
AM_CXXFLAGS += -I$(top_srcdir)/external/kplot/
endif

lib_LTLIBRARIES = libmui.la
libmui_la_CXXFLAGS = $(AM_CXXFLAGS) $(LIBPLANES_CFLAGS) $(CAIRO_CFLAGS) \
	$(LIBDRM_CFLAGS) $(TSLIB_CFLAGS) $(LUA_CFLAGS) $(CJSON_CFLAGS) \
	$(GSTREAMER_CFLAGS) $(LIBJPEG_CFLAGS) \
	$(LIBINPUT_CFLAGS)
libmui_la_CFLAGS = $(AM_CFLAGS) $(LIBPLANES_CFLAGS) $(CAIRO_CFLAGS) \
	$(LIBDRM_CFLAGS) $(TSLIB_CFLAGS) $(LUA_CFLAGS) $(CJSON_CFLAGS) \
	$(LIBJPEG_CFLAGS) $(LIBINPUT_CFLAGS)

libmui_la_SOURCES = \
animation.cpp \
app.cpp \
button.cpp \
chart.cpp \
color.cpp \
event_loop.cpp \
font.cpp \
frame.cpp \
framebuffer.cpp \
geometry.cpp \
grid.cpp \
image.cpp \
imagecache.cpp \
input/evdev.cpp \
input/input.cpp \
input/libinput.cpp \
input/tslib.cpp \
kmsscreen.cpp \
label.cpp \
list.cpp \
painter.cpp \
palette.cpp \
resource.cpp \
screen.cpp \
sprite.cpp \
text.cpp \
timer.cpp \
tools.cpp \
utils.cpp \
valuewidget.cpp \
video.cpp \
view.cpp \
widget.cpp \
window.cpp \
x11screen.cpp

if HAVE_LUA
libmui_la_SOURCES += \
lua/script.c \
lua/script.h
endif

libmui_la_LIBADD =
if HAVE_KPLOT
libmui_la_LIBADD += libkplot.a
endif

if HAVE_LIBJPEG
libmui_la_SOURCES += \
images/jpeg/cairo_jpg.c \
images/jpeg/cairo_jpg.h
endif

libmuiincludedir = ${includedir}/mui
libmuiinclude_HEADERS = \
$(top_srcdir)/include/mui/animation.h \
$(top_srcdir)/include/mui/app.h \
$(top_srcdir)/include/mui/button.h \
$(top_srcdir)/include/mui/chart.h \
$(top_srcdir)/include/mui/color.h \
$(top_srcdir)/include/mui/event_loop.h \
$(top_srcdir)/include/mui/font.h \
$(top_srcdir)/include/mui/frame.h \
$(top_srcdir)/include/mui/framebuffer.h \
$(top_srcdir)/include/mui/geometry.h \
$(top_srcdir)/include/mui/grid.h \
$(top_srcdir)/include/mui/image.h \
$(top_srcdir)/include/mui/imagecache.h \
$(top_srcdir)/include/mui/input.h \
$(top_srcdir)/include/mui/kmsscreen.h \
$(top_srcdir)/include/mui/label.h \
$(top_srcdir)/include/mui/libinput.h \
$(top_srcdir)/include/mui/list.h \
$(top_srcdir)/include/mui/painter.h \
$(top_srcdir)/include/mui/palette.h \
$(top_srcdir)/include/mui/resource.h \
$(top_srcdir)/include/mui/screen.h \
$(top_srcdir)/include/mui/sprite.h \
$(top_srcdir)/include/mui/text.h \
$(top_srcdir)/include/mui/timer.h \
$(top_srcdir)/include/mui/tools.h \
$(top_srcdir)/include/mui/ui \
$(top_srcdir)/include/mui/utils.h \
$(top_srcdir)/include/mui/valuewidget.h \
$(top_srcdir)/include/mui/video.h \
$(top_srcdir)/include/mui/widget.h \
$(top_srcdir)/include/mui/window.h \
$(top_srcdir)/include/mui/x11screen.h \
$(top_srcdir)/include/mui/object.h \
$(top_srcdir)/include/mui/view.h

libmui_la_LDFLAGS = -version-number 1:0:0 -no-undefined

pkgconfigdir = @pkgconfigdir@
pkgconfig_DATA = $(top_builddir)/libmui.pc

if HAVE_KPLOT
noinst_LIBRARIES = libkplot.a

$(top_builddir)/external/kplot/compat.h: $(top_srcdir)/external/kplot/compat.pre.h $(top_srcdir)/external/kplot/compat.post.h
	( mkdir -p $(top_builddir)/external/kplot/ ; \
	  cat $(top_srcdir)/external/kplot/compat.pre.h ; \
	  CC="$(CC)" CFLAGS="$(CFLAGS)" sh $(top_builddir)/external/kplot/compat.sh reallocarray ; \
	  cat $(top_srcdir)/external/kplot/compat.post.h ; ) >$@

libkplot_a_SOURCES = \
$(top_builddir)/external/kplot/compat.h \
$(top_srcdir)/external/kplot/compat.post.h \
$(top_srcdir)/external/kplot/compat.pre.h \
$(top_srcdir)/external/kplot/colours.c \
$(top_srcdir)/external/kplot/extern.h \
$(top_srcdir)/external/kplot/kplot.h \
$(top_srcdir)/external/kplot/array.c \
$(top_srcdir)/external/kplot/border.c \
$(top_srcdir)/external/kplot/bucket.c \
$(top_srcdir)/external/kplot/buffer.c \
$(top_srcdir)/external/kplot/draw.c \
$(top_srcdir)/external/kplot/grid.c \
$(top_srcdir)/external/kplot/hist.c \
$(top_srcdir)/external/kplot/label.c \
$(top_srcdir)/external/kplot/kdata.c \
$(top_srcdir)/external/kplot/kplot.c \
$(top_srcdir)/external/kplot/margin.c \
$(top_srcdir)/external/kplot/mean.c \
$(top_srcdir)/external/kplot/plotctx.c \
$(top_srcdir)/external/kplot/reallocarray.c \
$(top_srcdir)/external/kplot/stddev.c \
$(top_srcdir)/external/kplot/test-reallocarray.c \
$(top_srcdir)/external/kplot/tic.c \
$(top_srcdir)/external/kplot/vector.c
libkplot_a_CFLAGS = $(AM_CFLAGS) $(CAIRO_CFLAGS) -I$(top_builddir)/external/kplot -fPIC

CLEANFILES = $(top_builddir)/external/kplot/compat.h
BUILT_SOURCES = $(top_builddir)/external/kplot/compat.h

dist-hook:
	for file in $(BUILT_SOURCES) ; do \
		rm -f $(distdir)/$$file ; \
	done

endif
