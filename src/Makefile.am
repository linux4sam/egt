#AUTOMAKE_OPTIONS = subdir-objects
AM_CXXFLAGS = $(WARN_CFLAGS) -I$(top_srcdir) -I$(top_srcdir)/include -I$(top_builddir)/include -I$(top_srcdir)/src/images/jpeg \
	-I$(top_srcdir)/external/asio/asio/include \
	-DASIO_STANDALONE -DASIO_DISABLE_STD_FUTURE

if HAVE_KPLOT
AM_CXXFLAGS += -I$(top_srcdir)/external/kplot/
endif

lib_LTLIBRARIES = libegt.la
libegt_la_CXXFLAGS = $(AM_CXXFLAGS) $(LIBPLANES_CFLAGS) $(CAIRO_CFLAGS) \
	$(LIBDRM_CFLAGS) $(TSLIB_CFLAGS) $(LUA_CFLAGS) $(CJSON_CFLAGS) \
	$(GSTREAMER_CFLAGS) $(LIBJPEG_CFLAGS) \
	$(LIBINPUT_CFLAGS)
libegt_la_CFLAGS = $(AM_CFLAGS) $(LIBPLANES_CFLAGS) $(CAIRO_CFLAGS) \
	$(LIBDRM_CFLAGS) $(TSLIB_CFLAGS) $(LUA_CFLAGS) $(CJSON_CFLAGS) \
	$(LIBJPEG_CFLAGS) $(LIBINPUT_CFLAGS)

libegt_la_SOURCES = \
animation.cpp \
app.cpp \
button.cpp \
canvas.cpp \
chart.cpp \
color.cpp \
eventloop.cpp \
font.cpp \
frame.cpp \
geometry.cpp \
grid.cpp \
image.cpp \
imagecache.cpp \
input/evdev.cpp \
input/input.cpp \
input/libinput.cpp \
input/tslib.cpp \
label.cpp \
list.cpp \
object.cpp \
painter.cpp \
palette.cpp \
planewindow.cpp \
resource.cpp \
screen/framebuffer.cpp \
screen/kmsoverlay.cpp \
screen/kmsscreen.cpp \
screen/screen.cpp \
screen/x11screen.cpp \
sprite.cpp \
text.cpp \
timer.cpp \
tools.cpp \
uiloader.cpp \
utils.cpp \
valuewidget.cpp \
video.cpp \
view.cpp \
widget.cpp \
window.cpp

if HAVE_LUA
libegt_la_SOURCES += \
lua/script.c \
lua/script.h
endif

libegt_la_LIBADD =
if HAVE_KPLOT
libegt_la_LIBADD += libkplot.a
endif

if HAVE_LIBJPEG
libegt_la_SOURCES += \
images/jpeg/cairo_jpg.c \
images/jpeg/cairo_jpg.h
endif

libegtincludedir = ${includedir}/egt
libegtinclude_HEADERS = \
$(top_srcdir)/include/egt/animation.h \
$(top_srcdir)/include/egt/app.h \
$(top_srcdir)/include/egt/button.h \
$(top_srcdir)/include/egt/canvas.h \
$(top_srcdir)/include/egt/chart.h \
$(top_srcdir)/include/egt/color.h \
$(top_srcdir)/include/egt/eventloop.h \
$(top_srcdir)/include/egt/detail/floatingpoint.h \
$(top_srcdir)/include/egt/font.h \
$(top_srcdir)/include/egt/frame.h \
$(top_srcdir)/include/egt/framebuffer.h \
$(top_srcdir)/include/egt/geometry.h \
$(top_srcdir)/include/egt/grid.h \
$(top_srcdir)/include/egt/image.h \
$(top_srcdir)/include/egt/imagecache.h \
$(top_srcdir)/include/egt/input.h \
$(top_srcdir)/include/egt/kmsoverlay.h \
$(top_srcdir)/include/egt/kmsscreen.h \
$(top_srcdir)/include/egt/label.h \
$(top_srcdir)/include/egt/libinput.h \
$(top_srcdir)/include/egt/list.h \
$(top_srcdir)/include/egt/object.h \
$(top_srcdir)/include/egt/painter.h \
$(top_srcdir)/include/egt/palette.h \
$(top_srcdir)/include/egt/planewindow.h \
$(top_srcdir)/include/egt/resource.h \
$(top_srcdir)/include/egt/screen.h \
$(top_srcdir)/include/egt/sprite.h \
$(top_srcdir)/include/egt/text.h \
$(top_srcdir)/include/egt/timer.h \
$(top_srcdir)/include/egt/tools.h \
$(top_srcdir)/include/egt/ui \
$(top_srcdir)/include/egt/uiloader.h \
$(top_srcdir)/include/egt/utils.h \
$(top_srcdir)/include/egt/valuewidget.h \
$(top_srcdir)/include/egt/video.h \
$(top_srcdir)/include/egt/view.h \
$(top_srcdir)/include/egt/widget.h \
$(top_srcdir)/include/egt/window.h \
$(top_srcdir)/include/egt/x11screen.h

libegt_la_LDFLAGS = -version-number 1:0:0 -no-undefined

pkgconfigdir = @pkgconfigdir@
pkgconfig_DATA = $(top_builddir)/libegt.pc

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

EXTRA_DIST = $(top_srcdir)/include/egt/version.h.in
