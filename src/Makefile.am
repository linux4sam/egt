CUSTOM_FLAGS = $(WARN_CFLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	-isystem $(top_srcdir)/external/rapidxml \
	-isystem $(top_srcdir)/external/utfcpp/source \
	-isystem $(top_srcdir)/external/layout \
	-isystem $(top_srcdir)/external/kplot \
	-isystem $(top_builddir)/external/kplot \
	-isystem $(top_srcdir)/external/spdlog/include

CUSTOM_CXXFLAGS = -Woverloaded-virtual

if HAVE_SIMD
CUSTOM_FLAGS += -isystem $(top_srcdir)/external/Simd/src/
endif

AM_CXXFLAGS = -DDATADIR=\"$(datadir)\"

lib_LTLIBRARIES = libegt.la
libegt_la_CXXFLAGS = \
	$(CUSTOM_FLAGS) \
	$(CUSTOM_CXXFLAGS) \
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
detail/input/inputkeyboard.cpp \
detail/input/inputkeyboard.h \
detail/layout.cpp \
detail/mousegesture.cpp \
detail/object.cpp \
detail/resource.cpp \
detail/screen/flipthread.h \
detail/spriteimpl.h \
detail/string.cpp \
detail/textwidget.cpp \
detail/utf8text.cpp \
detail/utf8text.h \
detail/window/basicwindow.cpp \
detail/window/basicwindow.h \
detail/window/windowimpl.cpp \
detail/window/windowimpl.h \
dialog.cpp \
easing.cpp \
event.cpp \
eventloop.cpp \
font.cpp \
form.cpp \
frame.cpp \
geometry.cpp \
grid.cpp \
gauge.cpp \
image.cpp \
images/bmp/cairo_bmp.c \
images/bmp/cairo_bmp.h \
input.cpp \
keycode.cpp \
label.cpp \
list.cpp \
notebook.cpp \
painter.cpp \
palette.cpp \
progressbar.cpp \
radiobox.cpp \
respath.cpp \
screen.cpp \
scrollwheel.cpp \
shapes.cpp \
sideboard.cpp \
sizer.cpp \
slider.cpp \
sprite.cpp \
text.cpp \
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
virtualkeyboard.cpp \
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
libegt_la_LIBADD += $(top_builddir)/external/libkplot.la $(top_builddir)/external/libspdlog.la
if HAVE_SIMD
libegt_la_LIBADD += $(top_builddir)/external/Simd/prj/cmake/libSimd.a
endif
if ENABLE_LUA_BINDINGS
libegt_la_LIBADD += $(top_builddir)/lua/libegtlua.la
endif

libegtincludedir = ${includedir}
nobase_libegtinclude_HEADERS = \
$(top_builddir)/include/egt/ui \
$(top_builddir)/include/egt/version.h \
../include/egt/animation.h \
../include/egt/app.h \
../include/egt/bitmask.h \
../include/egt/button.h \
../include/egt/buttongroup.h \
../include/egt/canvas.h \
../include/egt/chart.h \
../include/egt/checkbox.h \
../include/egt/color.h \
../include/egt/combo.h \
../include/egt/dialog.h \
../include/egt/detail/alignment.h \
../include/egt/detail/collision.h \
../include/egt/detail/cow.h \
../include/egt/detail/dump.h \
../include/egt/detail/enum.h \
../include/egt/detail/filesystem.h \
../include/egt/detail/flagsbase.h \
../include/egt/detail/imagecache.h \
../include/egt/detail/math.h \
../include/egt/detail/meta.h \
../include/egt/detail/mousegesture.h \
../include/egt/detail/object.h \
../include/egt/detail/priorityqueue.h \
../include/egt/detail/resource.h \
../include/egt/detail/serialize.h \
../include/egt/detail/string.h \
../include/egt/detail/textwidget.h \
../include/egt/detail/layout.h \
../include/egt/easing.h \
../include/egt/event.h \
../include/egt/eventloop.h \
../include/egt/flags.h \
../include/egt/font.h \
../include/egt/form.h \
../include/egt/frame.h \
../include/egt/geometry.h \
../include/egt/grid.h \
../include/egt/gauge.h \
../include/egt/image.h \
../include/egt/input.h \
../include/egt/keycode.h \
../include/egt/label.h \
../include/egt/list.h \
../include/egt/notebook.h \
../include/egt/painter.h \
../include/egt/palette.h \
../include/egt/popup.h \
../include/egt/progressbar.h \
../include/egt/radial.h \
../include/egt/radiobox.h \
../include/egt/respath.h \
../include/egt/screen.h \
../include/egt/scrollwheel.h \
../include/egt/sideboard.h \
../include/egt/sizer.h \
../include/egt/shapes.h \
../include/egt/slider.h \
../include/egt/sprite.h \
../include/egt/text.h \
../include/egt/theme.h \
../include/egt/themes/coconut.h \
../include/egt/themes/lapis.h \
../include/egt/themes/midnight.h \
../include/egt/themes/shamrock.h \
../include/egt/themes/sky.h \
../include/egt/themes/ultraviolet.h \
../include/egt/timer.h \
../include/egt/tools.h \
../include/egt/types.h \
../include/egt/uiloader.h \
../include/egt/utils.h \
../include/egt/value.h \
../include/egt/valuewidget.h \
../include/egt/view.h \
../include/egt/virtualkeyboard.h \
../include/egt/widget.h \
../include/egt/widgetflags.h \
../include/egt/window.h

include Makefile.asio

nobase_libegtinclude_HEADERS += $(ASIO_SOURCE_FILES)

if HAVE_TSLIB
libegt_la_SOURCES += \
detail/input/inputtslib.cpp

nobase_libegtinclude_HEADERS += \
../include/egt/detail/input/inputtslib.h
endif

if HAVE_LIBINPUT
libegt_la_SOURCES += \
detail/input/inputlibinput.cpp

nobase_libegtinclude_HEADERS += \
../include/egt/detail/input/inputlibinput.h
endif


if HAVE_GSTREAMER
libegt_la_SOURCES += \
audio.cpp \
video.cpp \
detail/video/gstappsinkimpl.cpp \
detail/video/gstappsinkimpl.h \
detail/video/gstdecoderimpl.cpp \
detail/video/gstdecoderimpl.h \
camera.cpp \
detail/camera/gstcameraimpl.cpp \
detail/camera/gstcameraimpl.h \
capture.cpp \
detail/camera/gstcaptureimpl.cpp \
detail/camera/gstcaptureimpl.h

if HAVE_LIBPLANES
libegt_la_SOURCES += \
detail/video/gstkmssinkimpl.cpp \
detail/video/gstkmssinkimpl.h
endif

nobase_libegtinclude_HEADERS += \
../include/egt/audio.h \
../include/egt/video.h \
../include/egt/camera.h \
../include/egt/capture.h

endif

if HAVE_LIBPLANES
libegt_la_SOURCES += \
detail/window/planewindow.cpp \
detail/window/planewindow.h \
detail/screen/kmsoverlay.cpp \
detail/screen/kmsscreen.cpp

nobase_libegtinclude_HEADERS += \
../include/egt/detail/screen/kmsoverlay.h \
../include/egt/detail/screen/kmsscreen.h \
../include/egt/detail/screen/kmstype.h
endif

if HAVE_X11
libegt_la_SOURCES += \
detail/screen/x11screen.cpp \
detail/screen/x11wrap.h \
detail/screen/keyboard_code_conversion_x.h \
detail/screen/keyboard_code_conversion_x.cpp

nobase_libegtinclude_HEADERS += \
../include/egt/detail/screen/x11screen.h
endif

if HAVE_LIBCURL
libegt_la_SOURCES += network/http.cpp

nobase_libegtinclude_HEADERS += \
../include/egt/network/http.h
endif

if HAVE_EXPERIMENTAL_FILESYSTEM
libegt_la_SOURCES += \
filedialog.cpp

nobase_libegtinclude_HEADERS += \
../include/egt/filedialog.h
endif

if HAVE_LIBRSVG
libegt_la_SOURCES += \
detail/svg.cpp \
svgimage.cpp

nobase_libegtinclude_HEADERS += \
../include/egt/detail/svg.h \
../include/egt/svgimage.h
endif

if HAVE_FBDEV
libegt_la_SOURCES += \
detail/screen/framebuffer.cpp

nobase_libegtinclude_HEADERS += \
../include/egt/detail/screen/framebuffer.h
endif

if HAVE_LINUX_INPUT_H
libegt_la_SOURCES += \
detail/input/inputevdev.cpp

nobase_libegtinclude_HEADERS += \
../include/egt/detail/input/inputevdev.h
endif

if ENABLE_LUA_BINDINGS
libegt_la_SOURCES += \
luaapp.cpp

nobase_libegtinclude_HEADERS += \
../include/egt/luaapp.h
endif

if HAVE_ALSA
libegt_la_SOURCES += \
sound.cpp

nobase_libegtinclude_HEADERS += \
../include/egt/sound.h
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

TIDY_OPTIONS = $(AM_CPPFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(libegt_la_CXXFLAGS)

tidy-local: all-am
	@for source in $(SOURCES); do \
		[[ $$source == *.c ]] && continue; \
		[[ $$source == *.h ]] && continue; \
		$(CLANG_TIDY_BIN) $(srcdir)/$$source -- $(TIDY_OPTIONS); \
	done

tidy: tidy-local

checkheaders:
	@$(top_srcdir)/scripts/checkheaders.sh \
		$(top_srcdir)/include \
		$(top_builddir)/include \
		$(AM_CPPFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(libegt_la_CXXFLAGS)
