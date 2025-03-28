AUTOMAKE_OPTIONS = subdir-objects

GZIP_V_GEN = $(gzip_v_GEN_$(V))
gzip_v_GEN_ = $(gzip_v_GEN_$(AM_DEFAULT_VERBOSITY))
gzip_v_GEN_0 = @echo "  GZIP    " $@;

OPTIONAL =
if HAVE_GSTREAMER
OPTIONAL += video
endif

if HAVE_MSGFMT
if HAVE_LIBINTL
OPTIONAL += i18n
endif
if ENABLE_SNIPPETS
OPTIONAL += snippets
endif
if HAVE_LIBRSVG
OPTIONAL += resource
endif
SUBDIRS = $(OPTIONAL)
endif

CUSTOM_CXXFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	-isystem $(top_srcdir)/external/rapidxml \
	-isystem $(top_srcdir)/external/cxxopts/include \
	$(CODE_COVERAGE_CXXFLAGS)

CUSTOM_LDADD = $(CODE_COVERAGE_LDFLAGS)

bin_PROGRAMS = \
basic/basic \
basicui/basicui \
boards/boards \
colors/colors \
dialog/dialog \
drag/drag \
floating/floating \
frames/frames \
grid/grid \
imagebutton/imagebutton \
imagestack/imagestack \
keys/keys \
listboxmulti/listboxmulti \
press/press \
sizers/sizers \
space/space \
sprite/sprite \
squares/squares \
whiteboard/whiteboard \
widgets/widgets

if HAVE_LUA
bin_PROGRAMS += \
calc/calc
endif

if ENABLE_LUA_BINDINGS
bin_PROGRAMS += \
luarepl/luarepl
endif

if HAVE_GSTREAMER
bin_PROGRAMS += \
audioplayer/audioplayer
endif

if HAVE_LIBRSVG
bin_PROGRAMS += \
dash/dash \
gauge/gauge \
sevensegment/sevensegment
endif

if HAVE_ALSA
if HAVE_SNDFILE
bin_PROGRAMS += \
drummachine/drummachine
endif
endif

if HAVE_PLPLOT
bin_PROGRAMS += \
charts/charts \
easing/easing \
monitor/monitor
endif

if HAVE_LINUX_GPIO_H
bin_PROGRAMS += \
gpio/gpio
endif

if ENABLE_VIRTUALKEYBOARD
bin_PROGRAMS += \
virtualkeyboard/virtualkeyboard
endif

if ENABLE_SVGDESERIAL
bin_PROGRAMS += \
motorcycledash/motorcycledash
endif

EXTRA_DIST =
CLEANFILES =

audioplayer_audioplayer_SOURCES = audioplayer/audioplayer.cpp
audioplayer_audioplayer_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS) \
	-DEXAMPLEDATA=\"$(datadir)/egt/examples/audioplayer\"
audioplayer_audioplayer_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
audioplayer_audioplayerdir = $(prefix)/share/egt/examples/audioplayer
dist_audioplayer_audioplayer_DATA = $(wildcard $(top_srcdir)/examples/audioplayer/*.png) \
	$(wildcard $(top_srcdir)/examples/audioplayer/*.mp3) \
	$(wildcard $(top_srcdir)/examples/audioplayer/*.xml)
audioplayer_audioplayer_LDFLAGS = $(AM_LDFLAGS)

basic_basic_SOURCES = basic/basic.cpp
basic_basic_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
basic_basic_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
basic_basicdir = $(prefix)/share/egt/examples/basic
basic_basic_LDFLAGS = $(AM_LDFLAGS)

basicui_basicui_SOURCES = basicui/basicui.cpp
basicui_basicui_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS) \
	-DEXAMPLEDATA=\"$(datadir)/egt/examples/basicui\"
basicui_basicui_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
basicui_basicuidir = $(prefix)/share/egt/examples/basicui
dist_basicui_basicui_DATA = $(wildcard $(top_srcdir)/examples/basicui/*.xml)
basicui_basicui_LDFLAGS = $(AM_LDFLAGS)

boards_boards_SOURCES = boards/boards.cpp
boards_boards_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
boards_boards_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
boards_boardsdir = $(prefix)/share/egt/examples/boards
dist_boards_boards_DATA = $(wildcard $(top_srcdir)/examples/boards/*.png) \
	$(top_srcdir)/examples/boards/boards.xml
boards_boards_LDFLAGS = $(AM_LDFLAGS)
EXTRA_DIST += boards/boards.xml

calc_calc_SOURCES = calc/calc.cpp
calc_calc_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
calc_calc_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
calc_calcdir = $(prefix)/share/egt/examples/calc
dist_calc_calc_DATA = $(wildcard $(top_srcdir)/examples/calc/*.png) \
	$(top_srcdir)/examples/calc/calc.xml
calc_calc_LDFLAGS = $(AM_LDFLAGS)
EXTRA_DIST += calc/calc.xml

charts_charts_SOURCES = charts/charts.cpp
charts_charts_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS) \
	-DEXAMPLEDATA=\"$(datadir)/egt/examples/charts\"
charts_charts_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
charts_chartsdir = $(prefix)/share/egt/examples/charts
dist_charts_charts_DATA = $(wildcard $(top_srcdir)/examples/charts/*.png) \
	$(top_srcdir)/examples/charts/charts.xml
charts_charts_LDFLAGS = $(AM_LDFLAGS)
EXTRA_DIST += charts/charts.xml

colors_colors_SOURCES = colors/colors.cpp
colors_colors_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
colors_colors_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
colors_colorsdir = $(prefix)/share/egt/examples/colors
colors_colors_LDFLAGS = $(AM_LDFLAGS)

dash_dash_SOURCES = dash/dash.cpp
dash_dash_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS) \
	-DEXAMPLEDATA=\"$(datadir)/egt/examples/dash\"
dash_dash_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
dash_dashdir = $(prefix)/share/egt/examples/dash
dist_dash_dash_DATA = $(wildcard $(top_srcdir)/examples/dash/*.svg) \
	$(wildcard $(top_srcdir)/examples/dash/*.png) \
	$(top_srcdir)/examples/dash/dash.xml
dash_dash_LDFLAGS = $(AM_LDFLAGS)
dash_dash_DEPENDENCIES = dash/dash_background.svg.gz
dash/dash_background.svg.gz : $(top_srcdir)/examples/dash/dash_background.svg
	@$(GZIP_V_GEN)gzip -c -9 $< > $@
CLEANFILES += dash/dash_background.svg.gz
EXTRA_DIST += dash/dash.xml

dialog_dialog_SOURCES = dialog/dialog.cpp
dialog_dialog_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
dialog_dialog_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
dialog_dialogdir = $(prefix)/share/egt/examples/dialog
dialog_dialog_LDFLAGS = $(AM_LDFLAGS)

drag_drag_SOURCES = drag/drag.cpp
drag_drag_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS) \
	-DEXAMPLEDATA=\"$(datadir)/egt/examples/drag\"
drag_drag_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
drag_dragdir = $(prefix)/share/egt/examples/drag
dist_drag_drag_DATA = $(wildcard $(top_srcdir)/examples/drag/*.png) \
	$(top_srcdir)/examples/drag/drag.xml
drag_drag_LDFLAGS = $(AM_LDFLAGS)

drummachine_drummachine_SOURCES = drummachine/drummachine.cpp
drummachine_drummachine_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS) \
	-DEXAMPLEDATA=\"$(datadir)/egt/examples/drummachine\"
drummachine_drummachine_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
drummachine_drummachinedir = $(prefix)/share/egt/examples/drummachine
drummachine_drummachine_DATA = $(wildcard $(top_srcdir)/examples/drummachine/*.wav)
drummachine_drummachine_LDFLAGS = $(AM_LDFLAGS)

easing_easing_SOURCES = easing/easing.cpp
easing_easing_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS) \
	-DEXAMPLEDATA=\"$(datadir)/egt/examples/easing\"
easing_easing_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
easing_easingdir = $(prefix)/share/egt/examples/easing
dist_easing_easing_DATA = $(wildcard $(top_srcdir)/examples/easing/*.png) \
	$(top_srcdir)/examples/easing/easing.xml
easing_easing_LDFLAGS = $(AM_LDFLAGS)
EXTRA_DIST += easing/easing.xml

floating_floating_SOURCES = floating/floating.cpp
floating_floating_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS) \
	-DEXAMPLEDATA=\"$(datadir)/egt/examples/floating\"
floating_floating_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
floating_floatingdir = $(prefix)/share/egt/examples/floating
dist_floating_floating_DATA = $(wildcard $(top_srcdir)/examples/floating/*.png)
floating_floating_LDFLAGS = $(AM_LDFLAGS)

frames_frames_SOURCES = frames/frames.cpp
frames_frames_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
frames_frames_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
frames_frames_LDFLAGS = $(AM_LDFLAGS)

gauge_gauge_SOURCES = gauge/gauge.cpp
gauge_gauge_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS) \
	-DEXAMPLEDATA=\"$(datadir)/egt/examples/gauge\"
gauge_gauge_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
gauge_gaugedir = $(prefix)/share/egt/examples/gauge
dist_gauge_gauge_DATA = $(wildcard $(top_srcdir)/examples/gauge/*.svg) \
	$(wildcard $(top_srcdir)/examples/gauge/*.png) \
	$(top_srcdir)/examples/gauge/gauge.xml
gauge_gauge_LDFLAGS = $(AM_LDFLAGS)
EXTRA_DIST += gauge/gauge.xml

gpio_gpio_SOURCES = gpio/gpio.cpp
gpio_gpio_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
gpio_gpio_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
gpio_gpiodir = $(prefix)/share/egt/examples/gpio
gpio_gpio_LDFLAGS = $(AM_LDFLAGS)

grid_grid_SOURCES = grid/grid.cpp
grid_grid_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
grid_grid_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
grid_griddir = $(prefix)/share/egt/examples/grid
grid_grid_LDFLAGS = $(AM_LDFLAGS)

imagebutton_imagebutton_SOURCES = imagebutton/imagebutton.cpp
imagebutton_imagebutton_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
imagebutton_imagebutton_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
imagebutton_imagebuttondir = $(prefix)/share/egt/examples/imagebutton
imagebutton_imagebutton_LDFLAGS = $(AM_LDFLAGS)

imagestack_imagestack_SOURCES = imagestack/imagestack.cpp
imagestack_imagestack_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS) \
	-DEXAMPLEDATA=\"$(datadir)/egt/examples/imagestack\"
imagestack_imagestack_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
imagestack_imagestackdir = $(prefix)/share/egt/examples/imagestack/images
imagestack_imagestack_DATA = $(wildcard $(top_srcdir)/examples/imagestack/images/*.png) \
	$(wildcard $(top_srcdir)/examples/imagestack/*.png) \
	$(top_srcdir)/examples/imagestack/imagestack.xml
imagestack_imagestack_LDFLAGS = $(AM_LDFLAGS)
EXTRA_DIST += imagestack/imagestack.xml

keys_keys_SOURCES = keys/keys.cpp
keys_keys_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
keys_keys_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
keys_keysdir = $(prefix)/share/egt/examples/keys
keys_keys_LDFLAGS = $(AM_LDFLAGS)

listboxmulti_listboxmulti_SOURCES = listboxmulti/listboxmulti.cpp
listboxmulti_listboxmulti_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
listboxmulti_listboxmulti_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
listboxmulti_listboxmultidir = $(prefix)/share/egt/examples/listboxmulti
listboxmulti_listboxmulti_LDFLAGS = $(AM_LDFLAGS)

luarepl_luarepl_SOURCES = luarepl/luarepl.cpp
luarepl_luarepl_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
luarepl_luarepl_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
luarepl_luarepldir = $(prefix)/share/egt/examples/luarepl
luarepl_luarepl_LDFLAGS = $(AM_LDFLAGS)

monitor_monitor_SOURCES = monitor/monitor.cpp
monitor_monitor_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
monitor_monitor_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
monitor_monitordir = $(prefix)/share/egt/examples/monitor
monitor_monitor_DATA = $(wildcard $(top_srcdir)/examples/monitor/*.png) \
	$(top_srcdir)/examples/monitor/monitor.xml
monitor_monitor_LDFLAGS = $(AM_LDFLAGS)
EXTRA_DIST += monitor/monitor.xml

press_press_SOURCES = press/press.cpp
press_press_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
press_press_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
press_pressdir = $(prefix)/share/egt/examples/press
press_press_LDFLAGS = $(AM_LDFLAGS)

sevensegment_sevensegment_SOURCES = sevensegment/sevensegment.cpp
sevensegment_sevensegment_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS) \
	-DEXAMPLEDATA=\"$(datadir)/egt/examples/sevensegment\"
sevensegment_sevensegment_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
sevensegment_sevensegmentdir = $(prefix)/share/egt/examples/sevensegment
sevensegment_sevensegment_DATA = $(wildcard $(top_srcdir)/examples/sevensegment/*.svg)
sevensegment_sevensegment_LDFLAGS = $(AM_LDFLAGS)

sizers_sizers_SOURCES = sizers/sizers.cpp
sizers_sizers_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
sizers_sizers_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
sizers_sizers_LDFLAGS = $(AM_LDFLAGS)

space_space_SOURCES = space/space.cpp
space_space_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS) \
	-DEXAMPLEDATA=\"$(datadir)/egt/examples/space\"
space_space_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
space_spacedir = $(prefix)/share/egt/examples/space
dist_space_space_DATA = $(wildcard $(top_srcdir)/examples/space/*.png) \
	$(top_srcdir)/examples/space/space.xml
space_space_LDFLAGS = $(AM_LDFLAGS)
EXTRA_DIST += space/space.xml

sprite_sprite_SOURCES = sprite/sprite.cpp
sprite_sprite_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS) \
	-DEXAMPLEDATA=\"$(datadir)/egt/examples/sprite\"
sprite_sprite_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
sprite_spritedir = $(prefix)/share/egt/examples/sprite
dist_sprite_sprite_DATA = $(wildcard $(top_srcdir)/examples/sprite/*.png)
sprite_sprite_LDFLAGS = $(AM_LDFLAGS)

squares_squares_SOURCES = squares/squares.cpp
squares_squares_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
squares_squares_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
squares_squares_LDFLAGS = $(AM_LDFLAGS)

whiteboard_whiteboard_SOURCES = whiteboard/whiteboard.cpp
whiteboard_whiteboard_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS) \
	-DEXAMPLEDATA=\"$(datadir)/egt/examples/whiteboard\"
whiteboard_whiteboard_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
whiteboard_whiteboarddir = $(prefix)/share/egt/examples/whiteboard
dist_whiteboard_whiteboard_DATA = $(wildcard $(top_srcdir)/examples/whiteboard/*.png) \
	$(top_srcdir)/examples/whiteboard/whiteboard.xml
whiteboard_whiteboard_LDFLAGS = $(AM_LDFLAGS)
EXTRA_DIST += whiteboard/whiteboard.xml

widgets_widgets_SOURCES = widgets/widgets.cpp
widgets_widgets_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
widgets_widgets_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
widgets_widgetsdir = $(prefix)/share/egt/examples/widgets
dist_widgets_widgets_DATA = $(wildcard $(top_srcdir)/examples/widgets/*.png) \
	$(top_srcdir)/examples/widgets/widgets.xml
widgets_widgets_LDFLAGS = $(AM_LDFLAGS)
EXTRA_DIST += widgets/widgets.xml

virtualkeyboard_virtualkeyboard_SOURCES = virtualkeyboard/virtualkeyboard.cpp
virtualkeyboard_virtualkeyboard_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
virtualkeyboard_virtualkeyboard_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
virtualkeyboard_virtualkeyboard_LDFLAGS = $(AM_LDFLAGS)

motorcycledash_motorcycledash_SOURCES = motorcycledash/motorcycledash.cpp
motorcycledash_motorcycledash_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS) \
	-DEXAMPLEDATA=\"$(datadir)/egt/examples/motorcycledash\"
motorcycledash_motorcycledash_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
motorcycledash_motorcycledashdir = $(prefix)/share/egt/examples/motorcycledash
motorcycledash_motorcycledash_DATA = $(wildcard $(top_srcdir)/examples/motorcycledash/eraw.bin) \
  $(wildcard $(top_srcdir)/examples/motorcycledash/*.png)
motorcycledash_motorcycledash_LDFLAGS = $(AM_LDFLAGS)

TIDY_FLAGS = $(CUSTOM_CXXFLAGS)
include ../src/Makefile.tidy
