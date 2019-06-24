AUTOMAKE_OPTIONS = subdir-objects

OPTIONAL =
if HAVE_GSTREAMER
OPTIONAL += video
endif
SUBDIRS = $(OPTIONAL) i18n resource

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

bin_PROGRAMS = \
animations/animations \
basic/basic \
basicui/basicui \
boards/boards \
brick/brick \
calc/calc \
charts/charts \
colors/colors \
drag/drag \
easing/easing \
floating/floating \
frames/frames \
layout/layout \
sizers/sizers \
space/space \
sprite/sprite \
squares/squares \
virtualkeyboard/virtualkeyboard \
water/water \
whiteboard/whiteboard \
widgets/widgets

if HAVE_LIBCURL
bin_PROGRAMS += \
newsfeed/newsfeed
endif

if HAVE_GSTREAMER
bin_PROGRAMS += \
audioplayer/audioplayer
if HAVE_LIBPLANES
bin_PROGRAMS += \
gallery/gallery
endif
endif

if HAVE_EXPERIMENTAL_FILESYSTEM
bin_PROGRAMS += \
dialog/dialog
endif

if HAVE_LIBRSVG
bin_PROGRAMS += \
gauge/gauge \
dash/dash
endif

animations_animations_SOURCES = animations/animations.cpp
animations_animations_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
animations_animations_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
animations_animationsdir = $(prefix)/share/egt/examples/animations
dist_animations_animations_DATA = $(wildcard $(top_srcdir)/examples/animations/*.png)
animations_animations_LDFLAGS = $(AM_LDFLAGS)

brick_brick_SOURCES = brick/brick.cpp
brick_brick_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
brick_brick_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
brick_brickdir = $(prefix)/share/egt/examples/brick
dist_brick_brick_DATA = $(wildcard $(top_srcdir)/examples/brick/*.png)
brick_brick_LDFLAGS = $(AM_LDFLAGS)

calc_calc_SOURCES = calc/calc.cpp
calc_calc_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
calc_calc_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
calc_calcdir = $(prefix)/share/egt/examples/calc
calc_calc_LDFLAGS = $(AM_LDFLAGS)

charts_charts_SOURCES = charts/charts.cpp
charts_charts_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
charts_charts_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
charts_chartsdir = $(prefix)/share/egt/examples/charts
dist_charts_charts_DATA = $(wildcard $(top_srcdir)/examples/charts/*.png)
charts_charts_LDFLAGS = $(AM_LDFLAGS)

easing_easing_SOURCES = easing/easing.cpp
easing_easing_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
easing_easing_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
easing_easingdir = $(prefix)/share/egt/examples/easing
dist_easing_easing_DATA = $(wildcard $(top_srcdir)/examples/easing/*.png)
easing_easing_LDFLAGS = $(AM_LDFLAGS)

floating_floating_SOURCES = floating/floating.cpp
floating_floating_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
floating_floating_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
floating_floatingdir = $(prefix)/share/egt/examples/floating
dist_floating_floating_DATA = $(wildcard $(top_srcdir)/examples/floating/*.png)
floating_floating_LDFLAGS = $(AM_LDFLAGS)

gallery_gallery_SOURCES = gallery/gallery.cpp
gallery_gallery_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
gallery_gallery_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
gallery_gallerydir = $(prefix)/share/egt/examples/gallery
dist_gallery_gallery_DATA = $(wildcard $(top_srcdir)/examples/gallery/*.png) \
	$(wildcard $(top_srcdir)/examples/gallery/movies/*.png) \
	$(wildcard $(top_srcdir)/examples/gallery/movies/*.jpg)
gallery_gallery_LDFLAGS = $(AM_LDFLAGS)

space_space_SOURCES = space/space.cpp
space_space_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
space_space_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
space_spacedir = $(prefix)/share/egt/examples/space
dist_space_space_DATA = $(wildcard $(top_srcdir)/examples/space/*.png)
space_space_LDFLAGS = $(AM_LDFLAGS)

sprite_sprite_SOURCES = sprite/sprite.cpp
sprite_sprite_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
sprite_sprite_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
sprite_spritedir = $(prefix)/share/egt/examples/sprite
dist_sprite_sprite_DATA = $(wildcard $(top_srcdir)/examples/sprite/*.png)
sprite_sprite_LDFLAGS = $(AM_LDFLAGS)

water_water_SOURCES = water/water.cpp
water_water_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
water_water_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
water_waterdir = $(prefix)/share/egt/examples/water
dist_water_water_DATA = $(wildcard $(top_srcdir)/examples/water/*.png)
water_water_LDFLAGS = $(AM_LDFLAGS)

widgets_widgets_SOURCES = widgets/widgets.cpp
widgets_widgets_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
widgets_widgets_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
widgets_widgetsdir = $(prefix)/share/egt/examples/widgets
dist_widgets_widgets_DATA = $(wildcard $(top_srcdir)/examples/widgets/*.png)
widgets_widgets_LDFLAGS = $(AM_LDFLAGS)

basic_basic_SOURCES = basic/basic.cpp
basic_basic_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
basic_basic_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
basic_basicdir = $(prefix)/share/egt/examples/basic
basic_basic_LDFLAGS = $(AM_LDFLAGS)

whiteboard_whiteboard_SOURCES = whiteboard/whiteboard.cpp
whiteboard_whiteboard_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
whiteboard_whiteboard_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
whiteboard_whiteboarddir = $(prefix)/share/egt/examples/whiteboard
dist_whiteboard_whiteboard_DATA = $(wildcard $(top_srcdir)/examples/whiteboard/*.png)
whiteboard_whiteboard_LDFLAGS = $(AM_LDFLAGS)

newsfeed_newsfeed_SOURCES = newsfeed/newsfeed.cpp
newsfeed_newsfeed_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
newsfeed_newsfeed_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
newsfeed_newsfeeddir = $(prefix)/share/egt/examples/newsfeed
dist_newsfeed_newsfeed_DATA = $(wildcard $(top_srcdir)/examples/newsfeed/*.xml)
newsfeed_newsfeed_LDFLAGS = $(AM_LDFLAGS)

drag_drag_SOURCES = drag/drag.cpp
drag_drag_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
drag_drag_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
drag_dragdir = $(prefix)/share/egt/examples/drag
dist_drag_drag_DATA = $(wildcard $(top_srcdir)/examples/drag/*.png)
drag_drag_LDFLAGS = $(AM_LDFLAGS)

squares_squares_SOURCES = squares/squares.cpp
squares_squares_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
squares_squares_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
squares_squares_LDFLAGS = $(AM_LDFLAGS)

frames_frames_SOURCES = frames/frames.cpp
frames_frames_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
frames_frames_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
frames_frames_LDFLAGS = $(AM_LDFLAGS)

virtualkeyboard_virtualkeyboard_SOURCES = virtualkeyboard/virtualkeyboard.cpp
virtualkeyboard_virtualkeyboard_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
virtualkeyboard_virtualkeyboard_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
virtualkeyboard_virtualkeyboard_LDFLAGS = $(AM_LDFLAGS)

basicui_basicui_SOURCES = basicui/basicui.cpp
basicui_basicui_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
basicui_basicui_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
basicui_basicuidir = $(prefix)/share/egt/examples/basicui
dist_basicui_basicui_DATA = $(wildcard $(top_srcdir)/examples/basicui/*.xml)
basicui_basicui_LDFLAGS = $(AM_LDFLAGS)

audioplayer_audioplayer_SOURCES = audioplayer/audioplayer.cpp
audioplayer_audioplayer_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
audioplayer_audioplayer_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
audioplayer_audioplayerdir = $(prefix)/share/egt/examples/audioplayer
dist_audioplayer_audioplayer_DATA = $(wildcard $(top_srcdir)/examples/audioplayer/*.png) \
	$(wildcard $(top_srcdir)/examples/audioplayer/*.mp3)
audioplayer_audioplayer_LDFLAGS = $(AM_LDFLAGS)

boards_boards_SOURCES = boards/boards.cpp
boards_boards_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
boards_boards_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
boards_boardsdir = $(prefix)/share/egt/examples/boards
dist_boards_boards_DATA = $(wildcard $(top_srcdir)/examples/boards/*.png)
boards_boards_LDFLAGS = $(AM_LDFLAGS)

dialog_dialog_SOURCES = dialog/dialog.cpp
dialog_dialog_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
dialog_dialog_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
dialog_dialogdir = $(prefix)/share/egt/examples/dialog
dialog_dialog_LDFLAGS = $(AM_LDFLAGS)

gauge_gauge_SOURCES = gauge/gauge.cpp
gauge_gauge_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
gauge_gauge_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
gauge_gaugedir = $(prefix)/share/egt/examples/gauge
dist_gauge_gauge_DATA = $(wildcard $(top_srcdir)/examples/gauge/*.svg)
gauge_gauge_LDFLAGS = $(AM_LDFLAGS)

dash_dash_SOURCES = dash/dash.cpp
dash_dash_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
dash_dash_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
dash_dashdir = $(prefix)/share/egt/examples/dash
dist_dash_dash_DATA = $(wildcard $(top_srcdir)/examples/dash/*.svg)
dash_dash_LDFLAGS = $(AM_LDFLAGS)

sizers_sizers_SOURCES = sizers/sizers.cpp
sizers_sizers_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
sizers_sizers_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
sizers_sizers_LDFLAGS = $(AM_LDFLAGS)

layout_layout_SOURCES = layout/layout.cpp
layout_layout_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
layout_layout_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
layout_layoutdir = $(prefix)/share/egt/examples/layout/icons
dist_layout_layout_DATA = $(wildcard $(top_srcdir)/examples/layout/icons/*.png)
layout_layout_LDFLAGS = $(AM_LDFLAGS)

colors_colors_SOURCES = colors/colors.cpp
colors_colors_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
colors_colors_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
colors_colorsdir = $(prefix)/share/egt/examples/colors
colors_colors_LDFLAGS = $(AM_LDFLAGS)

TIDY_OPTIONS = $(AM_CPPFLAGS) $(CUSTOM_CXXFLAGS) $(CPPFLAGS) $(CXXFLAGS)

tidy-local: all-am
	@for source in $(SOURCES); do \
		[[ $$source == *.c ]] && continue; \
		[[ $$source == *.hpp ]] && continue; \
		[[ $$source == *.ipp ]] && continue; \
		clang-tidy $$source -- $(TIDY_OPTIONS); \
	done

tidy: tidy-local
