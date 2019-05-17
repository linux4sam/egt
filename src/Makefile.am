CUSTOM_FLAGS = $(WARN_CFLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	-isystem $(top_srcdir)/external/rapidxml \
	-isystem $(top_srcdir)/external/utfcpp/source \
	-isystem $(top_srcdir)/external/layout \
	-isystem $(top_srcdir)/external/kplot

AM_CXXFLAGS = -DDATADIR=\"$(datadir)\"

lib_LTLIBRARIES = libegt.la
libegt_la_CXXFLAGS = \
	$(CUSTOM_FLAGS) \
	@LIBEGT_EXTRA_CXXFLAGS@ \
	$(CODE_COVERAGE_CXXFLAGS) \
	$(AM_CXXFLAGS)
libegt_la_CPPFLAGS = \
	$(CUSTOM_FLAGS) \
	@LIBEGT_EXTRA_CXXFLAGS@ \
	$(CODE_COVERAGE_CPPFLAGS) \
	$(AM_CPPFLAGS)
libegt_la_CFLAGS = \
	$(CUSTOM_FLAGS) \
	@LIBEGT_EXTRA_CFLAGS@ \
	$(CODE_COVERAGE_CFLAGS) \
	$(AM_CFLAGS)

libegt_la_SOURCES = \
animation.cpp \
app.cpp \
button.cpp \
buttongroup.cpp \
canvas.cpp \
chart.cpp \
checkbox.cpp \
color.cpp \
combo.cpp \
detail/alignment.cpp \
detail/collision.cpp \
detail/filesystem.cpp \
detail/imagecache.cpp \
detail/layout.cpp \
detail/mousegesture.cpp \
detail/object.cpp \
detail/resource.cpp \
detail/string.cpp \
detail/window/basicwindow.cpp \
detail/window/basicwindow.h \
detail/window/windowimpl.cpp \
detail/window/windowimpl.h \
dialog.cpp \
easing.cpp \
event.cpp \
eventloop.cpp \
font.cpp \
frame.cpp \
geometry.cpp \
grid.cpp \
guage.cpp \
image.cpp \
images/bmp/cairo_bmp.c \
images/bmp/cairo_bmp.h \
input.cpp \
keyboard.cpp \
label.cpp \
list.cpp \
notebook.cpp \
painter.cpp \
palette.cpp \
progressbar.cpp \
radiobox.cpp \
screen.cpp \
scrollwheel.cpp \
shapes.cpp \
sizer.cpp \
slider.cpp \
sprite.cpp \
text.cpp \
textwidget.cpp \
theme.cpp \
themes/midnight.cpp \
themes/shamrock.cpp \
themes/sky.cpp \
timer.cpp \
tools.cpp \
types.cpp \
uiloader.cpp \
utils.cpp \
view.cpp \
widget.cpp \
widgetflags.cpp \
window.cpp

if HAVE_LUA
libegt_la_SOURCES += \
lua/script.c \
lua/script.h
endif

if HAVE_LIBJPEG
libegt_la_SOURCES += \
images/jpeg/cairo_jpg.c \
images/jpeg/cairo_jpg.h
endif

libegt_la_LIBADD = $(CODE_COVERAGE_LDFLAGS)
libegt_la_LIBADD += $(top_builddir)/external/libkplot.la

libegtincludedir = ${includedir}
nobase_libegtinclude_HEADERS = \
$(top_builddir)/include/egt/ui \
$(top_builddir)/include/egt/version.h \
$(top_srcdir)/include/egt/animation.h \
$(top_srcdir)/include/egt/app.h \
$(top_srcdir)/include/egt/bitmask.h \
$(top_srcdir)/include/egt/button.h \
$(top_srcdir)/include/egt/buttongroup.h \
$(top_srcdir)/include/egt/canvas.h \
$(top_srcdir)/include/egt/chart.h \
$(top_srcdir)/include/egt/checkbox.h \
$(top_srcdir)/include/egt/color.h \
$(top_srcdir)/include/egt/combo.h \
$(top_srcdir)/include/egt/dialog.h \
$(top_srcdir)/include/egt/detail/alignment.h \
$(top_srcdir)/include/egt/detail/collision.h \
$(top_srcdir)/include/egt/detail/floatingpoint.h \
$(top_srcdir)/include/egt/detail/imagecache.h \
$(top_srcdir)/include/egt/detail/filesystem.h \
$(top_srcdir)/include/egt/detail/math.h \
$(top_srcdir)/include/egt/detail/mousegesture.h \
$(top_srcdir)/include/egt/detail/object.h \
$(top_srcdir)/include/egt/detail/priorityqueue.h \
$(top_srcdir)/include/egt/detail/resource.h \
$(top_srcdir)/include/egt/detail/serialize.h \
$(top_srcdir)/include/egt/detail/spriteimpl.h \
$(top_srcdir)/include/egt/detail/string.h \
$(top_srcdir)/include/egt/detail/layout.h \
$(top_srcdir)/include/egt/easing.h \
$(top_srcdir)/include/egt/event.h \
$(top_srcdir)/include/egt/eventloop.h \
$(top_srcdir)/include/egt/flags.h \
$(top_srcdir)/include/egt/font.h \
$(top_srcdir)/include/egt/form.h \
$(top_srcdir)/include/egt/frame.h \
$(top_srcdir)/include/egt/geometry.h \
$(top_srcdir)/include/egt/grid.h \
$(top_srcdir)/include/egt/guage.h \
$(top_srcdir)/include/egt/image.h \
$(top_srcdir)/include/egt/input.h \
$(top_srcdir)/include/egt/keyboard.h \
$(top_srcdir)/include/egt/keycode.h \
$(top_srcdir)/include/egt/label.h \
$(top_srcdir)/include/egt/list.h \
$(top_srcdir)/include/egt/notebook.h \
$(top_srcdir)/include/egt/painter.h \
$(top_srcdir)/include/egt/palette.h \
$(top_srcdir)/include/egt/popup.h \
$(top_srcdir)/include/egt/progressbar.h \
$(top_srcdir)/include/egt/radial.h \
$(top_srcdir)/include/egt/radiobox.h \
$(top_srcdir)/include/egt/screen.h \
$(top_srcdir)/include/egt/scrollwheel.h \
$(top_srcdir)/include/egt/sideboard.h \
$(top_srcdir)/include/egt/sizer.h \
$(top_srcdir)/include/egt/shapes.h \
$(top_srcdir)/include/egt/slider.h \
$(top_srcdir)/include/egt/sprite.h \
$(top_srcdir)/include/egt/text.h \
$(top_srcdir)/include/egt/textwidget.h \
$(top_srcdir)/include/egt/theme.h \
$(top_srcdir)/include/egt/themes/midnight.h \
$(top_srcdir)/include/egt/themes/shamrock.h \
$(top_srcdir)/include/egt/themes/sky.h \
$(top_srcdir)/include/egt/timer.h \
$(top_srcdir)/include/egt/tools.h \
$(top_srcdir)/include/egt/types.h \
$(top_srcdir)/include/egt/uiloader.h \
$(top_srcdir)/include/egt/utils.h \
$(top_srcdir)/include/egt/valuewidget.h \
$(top_srcdir)/include/egt/view.h \
$(top_srcdir)/include/egt/widget.h \
$(top_srcdir)/include/egt/widgetflags.h \
$(top_srcdir)/include/egt/window.h

include Makefile.asio

nobase_libegtinclude_HEADERS += $(ASIO_SOURCE_FILES)

if HAVE_TSLIB
libegt_la_SOURCES += \
detail/input/inputtslib.cpp

nobase_libegtinclude_HEADERS += \
$(top_srcdir)/include/egt/detail/input/inputtslib.h
endif

if HAVE_LIBINPUT
libegt_la_SOURCES += \
detail/input/inputlibinput.cpp

nobase_libegtinclude_HEADERS += \
$(top_srcdir)/include/egt/detail/input/inputlibinput.h
endif


if HAVE_GSTREAMER
libegt_la_SOURCES += \
audio.cpp \
video.cpp \
detail/video/gstdecoderimpl.cpp \
detail/video/gstappsinkimpl.cpp \
camera.cpp

if HAVE_LIBPLANES
libegt_la_SOURCES += \
detail/video/gstkmssinkimpl.cpp

nobase_libegtinclude_HEADERS += \
$(top_srcdir)/include/egt/detail/video/gstkmssinkimpl.h
endif

nobase_libegtinclude_HEADERS += \
$(top_srcdir)/include/egt/audio.h \
$(top_srcdir)/include/egt/video.h \
$(top_srcdir)/include/egt/detail/video/gstdecoderimpl.h \
$(top_srcdir)/include/egt/detail/video/gstappsinkimpl.h \
$(top_srcdir)/include/egt/camera.h

endif

if HAVE_LIBPLANES
libegt_la_SOURCES += \
detail/window/planewindow.cpp \
detail/window/planewindow.h \
detail/screen/kmsoverlay.cpp \
detail/screen/kmsscreen.cpp

nobase_libegtinclude_HEADERS += \
$(top_srcdir)/include/egt/detail/screen/kmsoverlay.h \
$(top_srcdir)/include/egt/detail/screen/kmsscreen.h
endif

if HAVE_X11
libegt_la_SOURCES += \
detail/screen/keysym_to_unicode.cpp \
detail/screen/keysym_to_unicode.h \
detail/screen/keyboard_code_conversion_x.h \
detail/screen/keyboard_code_conversion_x.cpp \
detail/screen/x11screen.cpp \
detail/screen/x11wrap.h

nobase_libegtinclude_HEADERS += \
$(top_srcdir)/include/egt/detail/screen/x11screen.h
endif

if HAVE_LIBCURL
libegt_la_SOURCES += network/http.cpp

nobase_libegtinclude_HEADERS += \
$(top_srcdir)/include/egt/network/http.h
endif

if HAVE_EXPERIMENTAL_FILESYSTEM
libegt_la_SOURCES += \
filedialog.cpp

nobase_libegtinclude_HEADERS += \
$(top_srcdir)/include/egt/filedialog.h
endif

if HAVE_LIBRSVG
libegt_la_SOURCES += \
detail/svg.cpp

nobase_libegtinclude_HEADERS += \
$(top_srcdir)/include/egt/detail/svg.h
endif

if HAVE_FBDEV
libegt_la_SOURCES += \
detail/screen/framebuffer.cpp

nobase_libegtinclude_HEADERS += \
$(top_srcdir)/include/egt/detail/screen/framebuffer.h
endif

if HAVE_LINUX_INPUT_H
libegt_la_SOURCES += \
detail/input/inputevdev.cpp

nobase_libegtinclude_HEADERS += \
$(top_srcdir)/include/egt/detail/input/inputevdev.h
endif

if OS_WIN32
libegt_la_LIBADD += -lws2_32 -lintl
endif

libegt_la_LDFLAGS = $(AM_LDFLAGS) @LIBEGT_EXTRA_LDFLAGS@ -version-number 1:0:0 -no-undefined

pkgconfigdir = @pkgconfigdir@
pkgconfig_DATA = $(top_builddir)/libegt.pc

dist-hook:
	for file in $(BUILT_SOURCES) ; do \
		rm -f $(distdir)/$$file ; \
	done

BUILT_SOURCES = $(top_builddir)/include/egt/version.h $(top_builddir)/include/egt/ui
EXTRA_DIST = $(top_srcdir)/include/egt/version.h.in $(top_srcdir)/include/egt/ui.in

TIDY_OPTIONS = \
-checks="-*\
,clang-analyzer-*\
,cppcoreguidelines-* \
,-cppcoreguidelines-pro-bounds-constant-array-index\
,-cppcoreguidelines-pro-type-vararg\
,-cppcoreguidelines-pro-type-reinterpret-cast\
,-cppcoreguidelines-pro-type-union-access\
,-cppcoreguidelines-pro-bounds-array-to-pointer-decay\
,-cppcoreguidelines-pro-bounds-pointer-arithmetic\
,modernize-*\
,-modernize-pass-by-value\
,-modernize-use-default\
,-modernize-use-override\
,readability-*\
,-readability-braces-around-statements\
,-readability-else-after-return\
,-readability-implicit-bool-cast\
,-readability-named-parameter\
,misc-redundant-expression\
,-clang-analyzer-alpha.deadcode.UnreachableCode\
,-clang-analyzer-alpha.core.CastToStruct\
,-clang-analyzer-alpha.unix.PthreadLock\
,-clang-analyzer-alpha.cplusplus.VirtualCall" \
-header-filter=egt/*.h \
-- $(AM_CPPFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(libegt_la_CXXFLAGS)

tidy: all-am
	@for source in $(libegt_la_SOURCES) $(nobase_libegtinclude_HEADERS); do \
		[[ $$source == *.c ]] && continue; \
		[[ $$source == *.h ]] && continue; \
		echo $$source; \
		clang-tidy $$source $(TIDY_OPTIONS); \
	done
