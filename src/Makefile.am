CUSTOM_CXXFLAGS = $(WARN_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/src/images/jpeg \
	-I$(top_srcdir)/external/asio/asio/include \
	-DASIO_STANDALONE -DASIO_DISABLE_STD_FUTURE

CUSTOM_CFLAGS = $(CUSTOM_CXXFLAGS)

if HAVE_KPLOT
CUSTOM_CXXFLAGS += -I$(top_srcdir)/external/kplot/
endif

lib_LTLIBRARIES = libegt.la
libegt_la_CXXFLAGS = $(LIBPLANES_CFLAGS) $(CAIRO_CFLAGS) \
	$(LIBDRM_CFLAGS) $(TSLIB_CFLAGS) $(LUA_CFLAGS) $(CJSON_CFLAGS) \
	$(GSTREAMER_CFLAGS) $(LIBJPEG_CFLAGS) \
	$(LIBINPUT_CFLAGS) \
	$(CUSTOM_CXXFLAGS) \
	$(AM_CXXFLAGS)
libegt_la_CFLAGS = $(LIBPLANES_CFLAGS) $(CAIRO_CFLAGS) \
	$(LIBDRM_CFLAGS) $(TSLIB_CFLAGS) $(LUA_CFLAGS) $(CJSON_CFLAGS) \
	$(LIBJPEG_CFLAGS) $(LIBINPUT_CFLAGS) \
	$(CUSTOM_CFLAGS) \
	$(AM_CFLAGS)

libegt_la_SOURCES = \
animation.cpp \
app.cpp \
audio.cpp \
button.cpp \
buttongroup.cpp \
canvas.cpp \
chart.cpp \
checkbox.cpp \
color.cpp \
combo.cpp \
detail/basicwindow.cpp \
detail/mousegesture.cpp \
detail/windowimpl.cpp \
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
notebook.cpp \
object.cpp \
painter.cpp \
palette.cpp \
progressbar.cpp \
radiobox.cpp \
resource.cpp \
screen/framebuffer.cpp \
screen/screen.cpp \
screen/x11screen.cpp \
slider.cpp \
sprite.cpp \
text.cpp \
textwidget.cpp \
theme.cpp \
timer.cpp \
tools.cpp \
uiloader.cpp \
utils.cpp \
video.cpp \
view.cpp \
widget.cpp \
window.cpp

if HAVE_LIBPLANES
libegt_la_SOURCES += \
detail/planewindow.cpp \
screen/kmsoverlay.cpp \
screen/kmsscreen.cpp
endif

if HAVE_LUA
libegt_la_SOURCES += \
lua/script.c \
lua/script.h
endif

libegt_la_LIBADD =
if HAVE_KPLOT
libegt_la_LIBADD += $(top_builddir)/external/libkplot.la
endif

if HAVE_LIBJPEG
libegt_la_SOURCES += \
images/jpeg/cairo_jpg.c \
images/jpeg/cairo_jpg.h
endif

libegtincludedir = ${includedir}/egt
libegtinclude_HEADERS = \
$(top_builddir)/include/egt/version.h \
$(top_srcdir)/include/egt/animation.h \
$(top_srcdir)/include/egt/app.h \
$(top_srcdir)/include/egt/audio.h \
$(top_srcdir)/include/egt/bitmask.h \
$(top_srcdir)/include/egt/button.h \
$(top_srcdir)/include/egt/buttongroup.h \
$(top_srcdir)/include/egt/canvas.h \
$(top_srcdir)/include/egt/chart.h \
$(top_srcdir)/include/egt/checkbox.h \
$(top_srcdir)/include/egt/color.h \
$(top_srcdir)/include/egt/combo.h \
$(top_srcdir)/include/egt/detail/floatingpoint.h \
$(top_srcdir)/include/egt/detail/basicwindow.h \
$(top_srcdir)/include/egt/detail/math.h \
$(top_srcdir)/include/egt/detail/mousegesture.h \
$(top_srcdir)/include/egt/detail/planewindow.h \
$(top_srcdir)/include/egt/eventloop.h \
$(top_srcdir)/include/egt/font.h \
$(top_srcdir)/include/egt/frame.h \
$(top_srcdir)/include/egt/framebuffer.h \
$(top_srcdir)/include/egt/geometry.h \
$(top_srcdir)/include/egt/grid.h \
$(top_srcdir)/include/egt/image.h \
$(top_srcdir)/include/egt/imagecache.h \
$(top_srcdir)/include/egt/input.h \
$(top_srcdir)/include/egt/keyboard.h \
$(top_srcdir)/include/egt/kmsoverlay.h \
$(top_srcdir)/include/egt/kmsscreen.h \
$(top_srcdir)/include/egt/label.h \
$(top_srcdir)/include/egt/libinput.h \
$(top_srcdir)/include/egt/list.h \
$(top_srcdir)/include/egt/notebook.h \
$(top_srcdir)/include/egt/object.h \
$(top_srcdir)/include/egt/painter.h \
$(top_srcdir)/include/egt/palette.h \
$(top_srcdir)/include/egt/popup.h \
$(top_srcdir)/include/egt/progressbar.h \
$(top_srcdir)/include/egt/radiobox.h \
$(top_srcdir)/include/egt/resource.h \
$(top_srcdir)/include/egt/screen.h \
$(top_srcdir)/include/egt/sizer.h \
$(top_srcdir)/include/egt/slider.h \
$(top_srcdir)/include/egt/sprite.h \
$(top_srcdir)/include/egt/text.h \
$(top_srcdir)/include/egt/textwidget.h \
$(top_srcdir)/include/egt/theme.h \
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

dist-hook:
	for file in $(BUILT_SOURCES) ; do \
		rm -f $(distdir)/$$file ; \
	done

BUILT_SOURCES = $(top_builddir)/include/egt/version.h
EXTRA_DIST = $(top_srcdir)/include/egt/version.h.in
