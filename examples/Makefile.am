AUTOMAKE_OPTIONS = subdir-objects

if HAVE_GSTREAMER
OPTIONAL = video
endif
SUBDIRS = $(OPTIONAL)

AM_CXXFLAGS = $(WARN_CFLAGS) -I$(top_srcdir) -I$(top_srcdir)/include \
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
easing/easing \
floating/floating \
gallery/gallery \
infotain/infotain \
space/space \
water/water \
widgets/widgets \
whiteboard/whiteboard

if HAVE_RAPIDXML_HPP
bin_PROGRAMS += \
launcher/launcher
endif

if HAVE_RAPIDXML_RAPIDXML_HPP
bin_PROGRAMS += \
launcher/launcher
endif

if HAVE_LIBPLANES
bin_PROGRAMS += \
coin/coin \
sprite/sprite \
brick/brick
endif

animations_animations_SOURCES = animations/animations.cpp
animations_animations_CXXFLAGS = $(AM_CXXFLAGS)
animations_animations_LDADD = $(top_builddir)/src/libmui.la $(LDADD)
animations_animationsdir = $(prefix)/share/mui/examples/animations
animations_animations_DATA = $(wildcard $(top_srcdir)/examples/animations/*.png)
animations_animations_LDFLAGS = $(AM_LDFLAGS)

brick_brick_SOURCES = brick/brick.cpp
brick_brick_CXXFLAGS = $(AM_CXXFLAGS)
brick_brick_LDADD = $(top_builddir)/src/libmui.la $(LDADD)
brick_brickdir = $(prefix)/share/mui/examples/brick
brick_brick_DATA = $(wildcard $(top_srcdir)/examples/brick/*.png)
brick_brick_LDFLAGS = $(AM_LDFLAGS)

calc_calc_SOURCES = calc/calc.cpp
calc_calc_CXXFLAGS = $(AM_CXXFLAGS)
calc_calc_LDADD = $(top_builddir)/src/libmui.la $(LDADD)
calc_calcdir = $(prefix)/share/mui/examples/calc
calc_calc_LDFLAGS = $(AM_LDFLAGS)

coin_coin_SOURCES = coin/coin.cpp
coin_coin_CXXFLAGS = $(AM_CXXFLAGS)
coin_coin_LDADD = $(top_builddir)/src/libmui.la $(LDADD)
coin_coindir = $(prefix)/share/mui/examples/coin
coin_coin_DATA = $(wildcard $(top_srcdir)/examples/coin/*.png)
coin_coin_LDFLAGS = $(AM_LDFLAGS)

easing_easing_SOURCES = easing/easing.cpp
easing_easing_CXXFLAGS = $(AM_CXXFLAGS)
easing_easing_LDADD = $(top_builddir)/src/libmui.la $(LDADD)
easing_easingdir = $(prefix)/share/mui/examples/easing
easing_easing_DATA = $(wildcard $(top_srcdir)/examples/easing/*.png)
easing_easing_LDFLAGS = $(AM_LDFLAGS)

floating_floating_SOURCES = floating/floating.cpp
floating_floating_CXXFLAGS = $(AM_CXXFLAGS)
floating_floating_LDADD = $(top_builddir)/src/libmui.la $(LDADD)
floating_floatingdir = $(prefix)/share/mui/examples/floating
floating_floating_DATA = $(wildcard $(top_srcdir)/examples/floating/*.png)
floating_floating_LDFLAGS = $(AM_LDFLAGS)

gallery_gallery_SOURCES = gallery/gallery.cpp
gallery_gallery_CXXFLAGS = $(AM_CXXFLAGS)
gallery_gallery_LDADD = $(top_builddir)/src/libmui.la $(LDADD)
gallery_gallerydir = $(prefix)/share/mui/examples/gallery
gallery_gallery_DATA = $(wildcard $(top_srcdir)/examples/gallery/*.png) \
	$(wildcard $(top_srcdir)/examples/gallery/movies/*.png) \
	$(wildcard $(top_srcdir)/examples/gallery/movies/*.jpg)
gallery_gallery_LDFLAGS = $(AM_LDFLAGS)

launcher_launcher_SOURCES = launcher/launcher.cpp
launcher_launcher_CXXFLAGS = $(AM_CXXFLAGS)
launcher_launcher_LDADD = $(top_builddir)/src/libmui.la $(LDADD)
launcher_launcherdir = $(prefix)/share/mui/examples/launcher
launcher_launcher_DATA = $(wildcard $(top_srcdir)/examples/launcher/*.png) \
	$(top_srcdir)/examples/launcher/menu.xml \
	$(top_srcdir)/examples/launcher/background.jpg
launcher_launcher_LDFLAGS = $(AM_LDFLAGS)

space_space_SOURCES = space/space.cpp
space_space_CXXFLAGS = $(AM_CXXFLAGS)
space_space_LDADD = $(top_builddir)/src/libmui.la $(LDADD)
space_spacedir = $(prefix)/share/mui/examples/space
space_space_DATA = $(wildcard $(top_srcdir)/examples/space/*.png)
space_space_LDFLAGS = $(AM_LDFLAGS)

sprite_sprite_SOURCES = sprite/sprite.cpp
sprite_sprite_CXXFLAGS = $(AM_CXXFLAGS)
sprite_sprite_LDADD = $(top_builddir)/src/libmui.la $(LDADD)
sprite_spritedir = $(prefix)/share/mui/examples/sprite
sprite_sprite_DATA = $(wildcard $(top_srcdir)/examples/sprite/*.png)
sprite_sprite_LDFLAGS = $(AM_LDFLAGS)

water_water_SOURCES = water/water.cpp
water_water_CXXFLAGS = $(AM_CXXFLAGS)
water_water_LDADD = $(top_builddir)/src/libmui.la $(LDADD)
water_waterdir = $(prefix)/share/mui/examples/water
water_water_DATA = $(wildcard $(top_srcdir)/examples/water/*.png)
water_water_LDFLAGS = $(AM_LDFLAGS)

infotain_infotain_SOURCES = infotain/infotain.cpp
infotain_infotain_CXXFLAGS = $(AM_CXXFLAGS)
infotain_infotain_LDADD = $(top_builddir)/src/libmui.la $(LDADD)
infotain_infotaindir = $(prefix)/share/mui/examples/infotain
infotain_infotain_DATA = $(wildcard $(top_srcdir)/examples/infotain/res/*.png)
infotain_infotain_LDFLAGS = $(AM_LDFLAGS)

widgets_widgets_SOURCES = widgets/widgets.cpp
widgets_widgets_CXXFLAGS = $(AM_CXXFLAGS)
widgets_widgets_LDADD = $(top_builddir)/src/libmui.la $(LDADD)
widgets_widgetsdir = $(prefix)/share/mui/examples/widgets
widgets_widgets_DATA = $(wildcard $(top_srcdir)/examples/widgets/*.png)
widgets_widgets_LDFLAGS = $(AM_LDFLAGS)

basic_basic_SOURCES = basic/basic.cpp
basic_basic_CXXFLAGS = $(AM_CXXFLAGS)
basic_basic_LDADD = $(top_builddir)/src/libmui.la $(LDADD)
basic_basic_LDFLAGS = $(AM_LDFLAGS)

whiteboard_whiteboard_SOURCES = whiteboard/whiteboard.cpp
whiteboard_whiteboard_CXXFLAGS = $(AM_CXXFLAGS)
whiteboard_whiteboard_LDADD = $(top_builddir)/src/libmui.la $(LDADD)
whiteboard_whiteboard_LDFLAGS = $(AM_LDFLAGS)
