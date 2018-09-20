AUTOMAKE_OPTIONS = subdir-objects
SUBDIRS = video

AM_CXXFLAGS = $(WARN_CFLAGS) -I$(top_srcdir) -I$(top_srcdir)/include

#CXXFLAGS="$CXXFLAGS -std=c++11 -pedantic -Ofast -flto -fwhole-program"
#LDFLAGS="$LDFLAGS -flto -fwhole-program"

bin_PROGRAMS = \
brick/brick \
coin/coin \
easing/easing \
floating/floating \
launcher/launcher \
space/space \
sprite/sprite \
water/water \
infotain/infotain \
widgets/widgets

brick_brick_SOURCES = brick/brick.cpp
brick_brick_CXXFLAGS = $(AM_CXXFLAGS) $(LIBPLANES_CFLAGS) $(CAIRO_CFLAGS) $(LIBDRM_CFLAGS) $(TSLIB_CFLAGS) $(LUA_CFLAGS) $(CJSON_CFLAGS) $(GSTREAMER_CFLAGS)
brick_brick_LDADD = $(top_builddir)/src/libmui.la $(LIBPLANES_LIBS) $(CAIRO_LIBS) $(LIBDRM_LIBS) $(TSLIB_LIBS) $(LUA_LIBS) $(CJSON_LIBS) $(GSTREAMER_LIBS)
brick_brickdir = $(prefix)/share/mui/examples/brick
brick_brick_DATA = $(wildcard $(top_srcdir)/examples/brick/*.png)
brick_brick_LDFLAGS = $(AM_LDFLAGS)

coin_coin_SOURCES = coin/coin.cpp
coin_coin_CXXFLAGS = $(AM_CXXFLAGS) $(LIBPLANES_CFLAGS) $(CAIRO_CFLAGS) $(LIBDRM_CFLAGS) $(TSLIB_CFLAGS) $(LUA_CFLAGS) $(CJSON_CFLAGS) $(GSTREAMER_CFLAGS)
coin_coin_LDADD = $(top_builddir)/src/libmui.la $(LIBPLANES_LIBS) $(CAIRO_LIBS) $(LIBDRM_LIBS) $(TSLIB_LIBS) $(LUA_LIBS) $(CJSON_LIBS) $(GSTREAMER_LIBS)
coin_coindir = $(prefix)/share/mui/examples/coin
coin_coin_DATA = $(wildcard $(top_srcdir)/examples/coin/*.png)
coin_coin_LDFLAGS = $(AM_LDFLAGS)

easing_easing_SOURCES = easing/easing.cpp
easing_easing_CXXFLAGS = $(AM_CXXFLAGS) $(LIBPLANES_CFLAGS) $(CAIRO_CFLAGS) $(LIBDRM_CFLAGS) $(TSLIB_CFLAGS) $(LUA_CFLAGS) $(CJSON_CFLAGS) $(GSTREAMER_CFLAGS)
easing_easing_LDADD = $(top_builddir)/src/libmui.la $(LIBPLANES_LIBS) $(CAIRO_LIBS) $(LIBDRM_LIBS) $(TSLIB_LIBS) $(LUA_LIBS) $(CJSON_LIBS) $(GSTREAMER_LIBS)
easing_easingdir = $(prefix)/share/mui/examples/easing
easing_easing_DATA = $(wildcard $(top_srcdir)/examples/easing/*.png)
easing_easing_LDFLAGS = $(AM_LDFLAGS)

floating_floating_SOURCES = floating/floating.cpp
floating_floating_CXXFLAGS = $(AM_CXXFLAGS) $(LIBPLANES_CFLAGS) $(CAIRO_CFLAGS) $(LIBDRM_CFLAGS) $(TSLIB_CFLAGS) $(LUA_CFLAGS) $(CJSON_CFLAGS) $(GSTREAMER_CFLAGS)
floating_floating_LDADD = $(top_builddir)/src/libmui.la $(LIBPLANES_LIBS) $(CAIRO_LIBS) $(LIBDRM_LIBS) $(TSLIB_LIBS) $(LUA_LIBS) $(CJSON_LIBS) $(GSTREAMER_LIBS)
floating_floatingdir = $(prefix)/share/mui/examples/floating
floating_floating_DATA = $(wildcard $(top_srcdir)/examples/floating/*.png)
floating_floating_LDFLAGS = $(AM_LDFLAGS)

launcher_launcher_SOURCES = launcher/launcher.cpp
launcher_launcher_CXXFLAGS = $(AM_CXXFLAGS) $(LIBPLANES_CFLAGS) $(CAIRO_CFLAGS) $(LIBDRM_CFLAGS) $(TSLIB_CFLAGS) $(LUA_CFLAGS) $(CJSON_CFLAGS) $(GSTREAMER_CFLAGS)
launcher_launcher_LDADD = $(top_builddir)/src/libmui.la $(LIBPLANES_LIBS) $(CAIRO_LIBS) $(LIBDRM_LIBS) $(TSLIB_LIBS) $(LUA_LIBS) $(CJSON_LIBS) $(GSTREAMER_LIBS)
launcher_launcherdir = $(prefix)/share/mui/examples/launcher
launcher_launcher_DATA = $(wildcard $(top_srcdir)/examples/launcher/*.png)
launcher_launcher_LDFLAGS = $(AM_LDFLAGS)

space_space_SOURCES = space/space.cpp
space_space_CXXFLAGS = $(AM_CXXFLAGS) $(LIBPLANES_CFLAGS) $(CAIRO_CFLAGS) $(LIBDRM_CFLAGS) $(TSLIB_CFLAGS) $(LUA_CFLAGS) $(CJSON_CFLAGS) $(GSTREAMER_CFLAGS)
space_space_LDADD = $(top_builddir)/src/libmui.la $(LIBPLANES_LIBS) $(CAIRO_LIBS) $(LIBDRM_LIBS) $(TSLIB_LIBS) $(LUA_LIBS) $(CJSON_LIBS) $(GSTREAMER_LIBS)
space_spacedir = $(prefix)/share/mui/examples/space
space_space_DATA = $(wildcard $(top_srcdir)/examples/space/*.png)
space_space_LDFLAGS = $(AM_LDFLAGS)

sprite_sprite_SOURCES = sprite/sprite.cpp
sprite_sprite_CXXFLAGS = $(AM_CXXFLAGS) $(LIBPLANES_CFLAGS) $(CAIRO_CFLAGS) $(LIBDRM_CFLAGS) $(TSLIB_CFLAGS) $(LUA_CFLAGS) $(CJSON_CFLAGS) $(GSTREAMER_CFLAGS)
sprite_sprite_LDADD = $(top_builddir)/src/libmui.la $(LIBPLANES_LIBS) $(CAIRO_LIBS) $(LIBDRM_LIBS) $(TSLIB_LIBS) $(LUA_LIBS) $(CJSON_LIBS) $(GSTREAMER_LIBS)
sprite_spritedir = $(prefix)/share/mui/examples/sprite
sprite_sprite_DATA = $(wildcard $(top_srcdir)/examples/sprite/*.png)
sprite_sprite_LDFLAGS = $(AM_LDFLAGS)

water_water_SOURCES = water/water.cpp
water_water_CXXFLAGS = $(AM_CXXFLAGS) $(LIBPLANES_CFLAGS) $(CAIRO_CFLAGS) $(LIBDRM_CFLAGS) $(TSLIB_CFLAGS) $(LUA_CFLAGS) $(CJSON_CFLAGS) $(GSTREAMER_CFLAGS)
water_water_LDADD = $(top_builddir)/src/libmui.la $(LIBPLANES_LIBS) $(CAIRO_LIBS) $(LIBDRM_LIBS) $(TSLIB_LIBS) $(LUA_LIBS) $(CJSON_LIBS) $(GSTREAMER_LIBS)
water_waterdir = $(prefix)/share/mui/examples/water
water_water_DATA = $(wildcard $(top_srcdir)/examples/water/*.png)
water_water_LDFLAGS = $(AM_LDFLAGS)

infotain_infotain_SOURCES = infotain/infotain.cpp
infotain_infotain_CXXFLAGS = $(AM_CXXFLAGS) $(LIBPLANES_CFLAGS) $(CAIRO_CFLAGS) $(LIBDRM_CFLAGS) $(TSLIB_CFLAGS) $(LUA_CFLAGS) $(CJSON_CFLAGS) $(GSTREAMER_CFLAGS)
infotain_infotain_LDADD = $(top_builddir)/src/libmui.la $(LIBPLANES_LIBS) $(CAIRO_LIBS) $(LIBDRM_LIBS) $(TSLIB_LIBS) $(LUA_LIBS) $(CJSON_LIBS) $(GSTREAMER_LIBS)
infotain_infotaindir = $(prefix)/share/mui/examples/infotain
infotain_infotain_DATA = $(wildcard $(top_srcdir)/examples/infotain/res/*.png)
infotain_infotain_LDFLAGS = $(AM_LDFLAGS)

widgets_widgets_SOURCES = widgets/widgets.cpp
widgets_widgets_CXXFLAGS = $(AM_CXXFLAGS) $(LIBPLANES_CFLAGS) $(CAIRO_CFLAGS) $(LIBDRM_CFLAGS) $(TSLIB_CFLAGS) $(LUA_CFLAGS) $(CJSON_CFLAGS) $(GSTREAMER_CFLAGS)
widgets_widgets_LDADD = $(top_builddir)/src/libmui.la $(LIBPLANES_LIBS) $(CAIRO_LIBS) $(LIBDRM_LIBS) $(TSLIB_LIBS) $(LUA_LIBS) $(CJSON_LIBS) $(GSTREAMER_LIBS)
widgets_widgetsdir = $(prefix)/share/mui/examples/widgets
widgets_widgets_DATA = $(wildcard $(top_srcdir)/examples/widgets/*.png)
widgets_widgets_LDFLAGS = $(AM_LDFLAGS)
