CUSTOM_FLAGS = \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	-isystem $(top_srcdir)/external/rapidxml \
	-isystem $(top_srcdir)/external/utfcpp/source \
	-isystem $(top_srcdir)/external/layout \
	-isystem $(top_srcdir)/external/fmt/include

if HAVE_SIMD
CUSTOM_FLAGS += -isystem $(top_srcdir)/external/Simd/src/
endif

# MinGW env has a problem with using the variable -DDATADIR=\"$(datadir)\"
# resulting in the error:
#    expected unqualified-id before string constant
# Workaround is to use DATAPATH instead.
GIT_VERSION := $(shell cat $(top_builddir)/include/egt/git.version 2> /dev/null)
CUSTOM_FLAGS += -DDATAPATH=\"$(datadir)\" -DSRCDIR=\"$(abs_top_srcdir)\" -DGIT_VERSION=\"${GIT_VERSION}\"

CUSTOM_CXXFLAGS = -DEGT_DLL_EXPORTS -DFMT_HEADER_ONLY

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
checkbox.cpp \
color.cpp \
combo.cpp \
detail/asioallocator.h \
detail/alignment.cpp \
detail/base64.cpp \
detail/base64.h \
detail/collision.cpp \
detail/dump.h \
detail/egtlog.cpp \
detail/egtlog.h \
detail/eraw.cpp \
detail/eraw.h \
detail/erawimage.h \
detail/filesystem.cpp \
detail/fmt.h \
detail/image.cpp \
detail/imagecache.cpp \
detail/input/inputkeyboard.cpp \
detail/input/inputkeyboard.h \
detail/layout.cpp \
detail/mousegesture.cpp \
detail/priorityqueue.h \
detail/screen/composerscreen.cpp \
detail/screen/flipthread.h \
detail/screen/memoryscreen.cpp \
detail/spriteimpl.h \
detail/string.cpp \
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
gauge.cpp \
geometry.cpp \
grid.cpp \
image.cpp \
images/bmp/cairo_bmp.c \
images/bmp/cairo_bmp.h \
input.cpp \
keycode.cpp \
label.cpp \
list.cpp \
notebook.cpp \
object.cpp \
painter.cpp \
palette.cpp \
pattern.cpp \
progressbar.cpp \
radial.cpp \
radiobox.cpp \
resource.cpp \
respath.cpp \
screen.cpp \
script.cpp \
scrollwheel.cpp \
serialize.cpp \
shapes.cpp \
sideboard.cpp \
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
uri.cpp \
utils.cpp \
view.cpp \
widget.cpp \
widgetflags.cpp \
window.cpp

if HAVE_LUA
libegt_la_SOURCES += \
detail/lua/script.c \
detail/lua/script.h
endif

if HAVE_LIBJPEG
libegt_la_SOURCES += \
images/jpeg/cairo_jpg.c \
images/jpeg/cairo_jpg.h
endif

if CPU_ARM
libegt_la_SOURCES += \
detail/memset32.S
endif

libegt_la_LIBADD = $(CODE_COVERAGE_LDFLAGS)
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
../include/egt/button.h \
../include/egt/buttongroup.h \
../include/egt/canvas.h \
../include/egt/checkbox.h \
../include/egt/color.h \
../include/egt/combo.h \
../include/egt/detail/alignment.h \
../include/egt/detail/collision.h \
../include/egt/detail/cow.h \
../include/egt/detail/enum.h \
../include/egt/detail/filesystem.h \
../include/egt/detail/image.h \
../include/egt/detail/imagecache.h \
../include/egt/detail/incbin.h \
../include/egt/detail/layout.h \
../include/egt/detail/math.h \
../include/egt/detail/meta.h \
../include/egt/detail/mousegesture.h \
../include/egt/detail/screen/composerscreen.h \
../include/egt/detail/screen/memoryscreen.h \
../include/egt/detail/string.h \
../include/egt/detail/stringhash.h \
../include/egt/dialog.h \
../include/egt/easing.h \
../include/egt/embed.h \
../include/egt/event.h \
../include/egt/eventloop.h \
../include/egt/fixedvector.h \
../include/egt/flags.h \
../include/egt/flagsbase.h \
../include/egt/font.h \
../include/egt/form.h \
../include/egt/frame.h \
../include/egt/gauge.h \
../include/egt/geometry.h \
../include/egt/grid.h \
../include/egt/image.h \
../include/egt/input.h \
../include/egt/imageholder.h \
../include/egt/keycode.h \
../include/egt/label.h \
../include/egt/list.h \
../include/egt/notebook.h \
../include/egt/object.h \
../include/egt/painter.h \
../include/egt/palette.h \
../include/egt/pattern.h \
../include/egt/popup.h \
../include/egt/progressbar.h \
../include/egt/radial.h \
../include/egt/radiobox.h \
../include/egt/resource.h \
../include/egt/respath.h \
../include/egt/screen.h \
../include/egt/script.h \
../include/egt/scrollwheel.h \
../include/egt/serialize.h \
../include/egt/shapes.h \
../include/egt/sideboard.h \
../include/egt/signal.h \
../include/egt/sizer.h \
../include/egt/slider.h \
../include/egt/sprite.h \
../include/egt/string.h \
../include/egt/text.h \
../include/egt/textwidget.h \
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
../include/egt/uri.h \
../include/egt/utils.h \
../include/egt/value.h \
../include/egt/valuewidget.h \
../include/egt/view.h \
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
detail/camera/gstcaptureimpl.h \
detail/video/gstmeta.h

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

if HAVE_SDL2
libegt_la_SOURCES += \
detail/screen/sdlscreen.cpp

nobase_libegtinclude_HEADERS += \
../include/egt/detail/screen/sdlscreen.h
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
detail/svg.h \
svgimage.cpp

nobase_libegtinclude_HEADERS += \
../include/egt/svgimage.h
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

if HAVE_PLPLOT
libegt_la_SOURCES += \
chart.cpp \
detail/charts/plplotimpl.cpp \
detail/charts/plplotimpl.h

nobase_libegtinclude_HEADERS += \
../include/egt/chart.h
endif

if ENABLE_VIRTUALKEYBOARD
libegt_la_SOURCES += \
virtualkeyboard.cpp

nobase_libegtinclude_HEADERS += \
../include/egt/virtualkeyboard.h
endif

if OS_WIN32
libegt_la_LIBADD += -lws2_32 -lintl
endif

libegt_la_LDFLAGS = $(AM_LDFLAGS) @LIBEGT_EXTRA_LDFLAGS@ -version-info 4:0:0 -no-undefined

pkgconfigdir = @pkgconfigdir@
pkgconfig_DATA = $(top_builddir)/libegt.pc

dist-hook:
	for file in $(BUILT_SOURCES) ; do \
		rm -f $(distdir)/$$file ; \
	done

BUILT_SOURCES = $(top_builddir)/include/egt/version.h $(top_builddir)/include/egt/ui $(top_builddir)/include/egt/git.version
EXTRA_DIST = $(top_srcdir)/include/egt/version.h.in $(top_srcdir)/include/egt/ui.in

TIDY_FLAGS = $(libegt_la_CXXFLAGS)
include Makefile.tidy

checkheaders:
	@$(top_srcdir)/scripts/checkheaders.sh \
		$(top_srcdir)/include \
		$(top_builddir)/include \
		$(AM_CPPFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(libegt_la_CXXFLAGS)

$(top_builddir)/include/egt/git.version: .FORCE
	@switch_user="eval"; \
	if test -d "$(top_srcdir)/.git"; then \
		if test $$(id -u) -eq 0; then \
			if test $$(id -u) -ne $$(stat -c %u $(top_srcdir)/.git); then \
				git_tree_user=$$(stat -c %U $(top_srcdir)/.git); \
				switch_user="su $$git_tree_user -c"; \
			fi \
		fi \
	fi; \
	$$switch_user '$(top_srcdir)/scripts/git_version.sh $(top_srcdir) > $(top_builddir)/include/egt/git.version'; \
	$$switch_user 'touch $(top_srcdir)/src/utils.cpp';

.PHONY: .FORCE
.FORCE:
