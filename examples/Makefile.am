AUTOMAKE_OPTIONS = subdir-objects

if HAVE_GSTREAMER
OPTIONAL = video
endif
SUBDIRS = $(OPTIONAL)

AM_CXXFLAGS = $(WARN_CFLAGS) -I$(top_srcdir) -I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	$(LIBPLANES_CFLAGS) $(CAIRO_CFLAGS) $(LIBDRM_CFLAGS) $(TSLIB_CFLAGS) \
	$(LUA_CFLAGS) $(CJSON_CFLAGS) $(GSTREAMER_CFLAGS) $(LIBJPEG_CFLAGS) \
	$(LIBINPUT_CFLAGS) \
	-I$(top_srcdir)/external/asio/asio/include \
	-DASIO_STANDALONE -DASIO_DISABLE_STD_FUTURE

AM_LDFLAGS = -pthread

LDADD = $(LIBPLANES_LIBS) $(CAIRO_LIBS) $(LIBDRM_LIBS) $(TSLIB_LIBS) \
	$(LUA_LIBS) $(CJSON_LIBS) $(GSTREAMER_LIBS) $(LIBJPEG_LIBS) \
	$(LIBINPUT_LIBS)

#CXXFLAGS="$CXXFLAGS -std=c++11 -pedantic -Ofast -flto -fwhole-program"
#LDFLAGS="$LDFLAGS -flto -fwhole-program"

bin_PROGRAMS = \
animations/animations \
basic/basic \
calc/calc \
charts/charts \
drag/drag \
icon/icon \
easing/easing \
floating/floating \
gallery/gallery \
infotain/infotain \
space/space \
thermostat/thermostat \
water/water \
whiteboard/whiteboard \
widgets/widgets

if HAVE_RAPIDXML_HPP
bin_PROGRAMS += \
launcher/launcher \
newsfeed/newsfeed
endif

if HAVE_RAPIDXML_RAPIDXML_HPP
bin_PROGRAMS += \
launcher/launcher \
newsfeed/newsfeed
endif

if HAVE_LIBPLANES
bin_PROGRAMS += \
coin/coin \
sprite/sprite \
brick/brick
endif

animations_animations_SOURCES = animations/animations.cpp
animations_animations_CXXFLAGS = $(AM_CXXFLAGS)
animations_animations_LDADD = $(top_builddir)/src/libegt.la $(LDADD)
animations_animationsdir = $(prefix)/share/egt/examples/animations
animations_animations_DATA = $(wildcard $(top_srcdir)/examples/animations/*.png)
animations_animations_LDFLAGS = $(AM_LDFLAGS)

brick_brick_SOURCES = brick/brick.cpp
brick_brick_CXXFLAGS = $(AM_CXXFLAGS)
brick_brick_LDADD = $(top_builddir)/src/libegt.la $(LDADD)
brick_brickdir = $(prefix)/share/egt/examples/brick
brick_brick_DATA = $(wildcard $(top_srcdir)/examples/brick/*.png)
brick_brick_LDFLAGS = $(AM_LDFLAGS)

calc_calc_SOURCES = calc/calc.cpp
calc_calc_CXXFLAGS = $(AM_CXXFLAGS)
calc_calc_LDADD = $(top_builddir)/src/libegt.la $(LDADD)
calc_calcdir = $(prefix)/share/egt/examples/calc
calc_calc_LDFLAGS = $(AM_LDFLAGS)

charts_charts_SOURCES = charts/charts.cpp
charts_charts_CXXFLAGS = $(AM_CXXFLAGS)
charts_charts_LDADD = $(top_builddir)/src/libegt.la $(LDADD)
charts_chartsdir = $(prefix)/share/egt/examples/charts
charts_charts_DATA = $(wildcard $(top_srcdir)/examples/charts/*.png)
charts_charts_LDFLAGS = $(AM_LDFLAGS)

coin_coin_SOURCES = coin/coin.cpp
coin_coin_CXXFLAGS = $(AM_CXXFLAGS)
coin_coin_LDADD = $(top_builddir)/src/libegt.la $(LDADD)
coin_coindir = $(prefix)/share/egt/examples/coin
coin_coin_DATA = $(wildcard $(top_srcdir)/examples/coin/*.png)
coin_coin_LDFLAGS = $(AM_LDFLAGS)

easing_easing_SOURCES = easing/easing.cpp
easing_easing_CXXFLAGS = $(AM_CXXFLAGS)
easing_easing_LDADD = $(top_builddir)/src/libegt.la $(LDADD)
easing_easingdir = $(prefix)/share/egt/examples/easing
easing_easing_DATA = $(wildcard $(top_srcdir)/examples/easing/*.png)
easing_easing_LDFLAGS = $(AM_LDFLAGS)

floating_floating_SOURCES = floating/floating.cpp
floating_floating_CXXFLAGS = $(AM_CXXFLAGS)
floating_floating_LDADD = $(top_builddir)/src/libegt.la $(LDADD)
floating_floatingdir = $(prefix)/share/egt/examples/floating
floating_floating_DATA = $(wildcard $(top_srcdir)/examples/floating/*.png)
floating_floating_LDFLAGS = $(AM_LDFLAGS)

gallery_gallery_SOURCES = gallery/gallery.cpp
gallery_gallery_CXXFLAGS = $(AM_CXXFLAGS)
gallery_gallery_LDADD = $(top_builddir)/src/libegt.la $(LDADD)
gallery_gallerydir = $(prefix)/share/egt/examples/gallery
gallery_gallery_DATA = $(wildcard $(top_srcdir)/examples/gallery/*.png) \
	$(wildcard $(top_srcdir)/examples/gallery/movies/*.png) \
	$(wildcard $(top_srcdir)/examples/gallery/movies/*.jpg)
gallery_gallery_LDFLAGS = $(AM_LDFLAGS)

launcher_launcher_SOURCES = launcher/launcher.cpp
launcher_launcher_CXXFLAGS = $(AM_CXXFLAGS)
launcher_launcher_LDADD = $(top_builddir)/src/libegt.la $(LDADD)
launcher_launcherdir = $(prefix)/share/egt/examples/launcher
launcher_launcher_DATA = $(wildcard $(top_srcdir)/examples/launcher/*.png) \
	$(top_srcdir)/examples/launcher/menu.xml \
	$(top_srcdir)/examples/launcher/background.jpg
launcher_launcher_LDFLAGS = $(AM_LDFLAGS)
launcher_launcher_SCRIPTS = $(wildcard $(top_srcdir)/examples/launcher/*.sh)

space_space_SOURCES = space/space.cpp
space_space_CXXFLAGS = $(AM_CXXFLAGS)
space_space_LDADD = $(top_builddir)/src/libegt.la $(LDADD)
space_spacedir = $(prefix)/share/egt/examples/space
space_space_DATA = $(wildcard $(top_srcdir)/examples/space/*.png)
space_space_LDFLAGS = $(AM_LDFLAGS)

sprite_sprite_SOURCES = sprite/sprite.cpp
sprite_sprite_CXXFLAGS = $(AM_CXXFLAGS)
sprite_sprite_LDADD = $(top_builddir)/src/libegt.la $(LDADD)
sprite_spritedir = $(prefix)/share/egt/examples/sprite
sprite_sprite_DATA = $(wildcard $(top_srcdir)/examples/sprite/*.png)
sprite_sprite_LDFLAGS = $(AM_LDFLAGS)

water_water_SOURCES = water/water.cpp
water_water_CXXFLAGS = $(AM_CXXFLAGS)
water_water_LDADD = $(top_builddir)/src/libegt.la $(LDADD)
water_waterdir = $(prefix)/share/egt/examples/water
water_water_DATA = $(wildcard $(top_srcdir)/examples/water/*.png)
water_water_LDFLAGS = $(AM_LDFLAGS)

infotain_infotain_SOURCES = infotain/infotain.cpp
infotain_infotain_CXXFLAGS = $(AM_CXXFLAGS)
infotain_infotain_LDADD = $(top_builddir)/src/libegt.la $(LDADD)
infotain_infotaindir = $(prefix)/share/egt/examples/infotain
infotain_infotain_DATA = $(wildcard $(top_srcdir)/examples/infotain/res/*.png)
infotain_infotain_LDFLAGS = $(AM_LDFLAGS)

widgets_widgets_SOURCES = widgets/widgets.cpp
widgets_widgets_CXXFLAGS = $(AM_CXXFLAGS)
widgets_widgets_LDADD = $(top_builddir)/src/libegt.la $(LDADD)
widgets_widgetsdir = $(prefix)/share/egt/examples/widgets
widgets_widgets_DATA = $(wildcard $(top_srcdir)/examples/widgets/*.png)
widgets_widgets_LDFLAGS = $(AM_LDFLAGS)

basic_basic_SOURCES = basic/basic.cpp
basic_basic_CXXFLAGS = $(AM_CXXFLAGS)
basic_basic_LDADD = $(top_builddir)/src/libegt.la $(LDADD)
basic_basicdir = $(prefix)/share/egt/examples/basic
basic_basic_DATA = $(wildcard $(top_srcdir)/examples/basic/*.xml)
basic_basic_LDFLAGS = $(AM_LDFLAGS)

whiteboard_whiteboard_SOURCES = whiteboard/whiteboard.cpp
whiteboard_whiteboard_CXXFLAGS = $(AM_CXXFLAGS)
whiteboard_whiteboard_LDADD = $(top_builddir)/src/libegt.la $(LDADD)
whiteboard_whiteboarddir = $(prefix)/share/egt/examples/whiteboard
whiteboard_whiteboard_DATA = $(wildcard $(top_srcdir)/examples/whiteboard/*.png)
whiteboard_whiteboard_LDFLAGS = $(AM_LDFLAGS)

newsfeed_newsfeed_SOURCES = newsfeed/newsfeed.cpp
newsfeed_newsfeed_CXXFLAGS = $(AM_CXXFLAGS)
newsfeed_newsfeed_LDADD = $(top_builddir)/src/libegt.la $(LDADD)
newsfeed_newsfeeddir = $(prefix)/share/egt/examples/newsfeed
newsfeed_newsfeed_DATA = $(wildcard $(top_srcdir)/examples/newsfeed/*.xml)
newsfeed_newsfeed_LDFLAGS = $(AM_LDFLAGS)

thermostat_thermostat_SOURCES = thermostat/thermostat.cpp
thermostat_thermostat_CXXFLAGS = $(AM_CXXFLAGS)
thermostat_thermostat_LDADD = $(top_builddir)/src/libegt.la $(LDADD)
thermostat_thermostatdir = $(prefix)/share/egt/examples/thermostat
thermostat_thermostat_DATA = $(wildcard $(top_srcdir)/examples/thermostat/*.png)
thermostat_thermostat_LDFLAGS = $(AM_LDFLAGS)

icon_icon_SOURCES = icon/icon.cpp
icon_icon_CXXFLAGS = $(AM_CXXFLAGS)
icon_icon_LDADD = $(top_builddir)/src/libegt.la $(LDADD)
icon_icondir = $(prefix)/share/egt/examples/icon
icon_icon_DATA = $(wildcard $(top_srcdir)/examples/icon/*.png)
icon_icon_LDFLAGS = $(AM_LDFLAGS)

drag_drag_SOURCES = drag/drag.cpp
drag_drag_CXXFLAGS = $(AM_CXXFLAGS)
drag_drag_LDADD = $(top_builddir)/src/libegt.la $(LDADD)
drag_dragdir = $(prefix)/share/egt/examples/drag
drag_drag_DATA = $(wildcard $(top_srcdir)/examples/drag/*.png)
drag_drag_LDFLAGS = $(AM_LDFLAGS)
