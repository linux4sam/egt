AC_PREREQ([2.63])
AC_INIT([libegt],[0.8.2])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_MACRO_DIR([m4])
AC_USE_SYSTEM_EXTENSIONS

if test "x$program_prefix" = xNONE; then
  program_prefix="egt_"
fi

AC_CANONICAL_SYSTEM
AC_ARG_PROGRAM

PKG_PROG_PKG_CONFIG

AM_INIT_AUTOMAKE([1.10 foreign subdir-objects tar-ustar])

# Enable quiet compiles on automake 1.11.
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# Initialize libtool
LT_PREREQ([2.2])
LT_INIT([win32-dll])

AC_MSG_CHECKING([for native Win32])
case "$host" in
  *-*-mingw*)
      egt_os_win32=yes
      ;;
  *)
      egt_os_win32=no
      ;;
esac
AC_MSG_RESULT([$egt_os_win32])
AM_CONDITIONAL(OS_WIN32, test "$egt_os_win32" = "yes")

CXXFLAGS="$CXXFLAGS -g"
CFLAGS="$CFLAGS -g"

if test "x$egt_os_win32" = "xyes"; then
  CXXFLAGS="$CXXFLAGS -std=gnu++14"
else
  CXXFLAGS="$CXXFLAGS -std=c++14"
fi

AC_DISABLE_STATIC
AC_PROG_CXX
AX_CODE_COVERAGE
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_MKDIR_P

pkgconfigdir=${libdir}/pkgconfig
AC_SUBST(pkgconfigdir)

AC_DEFUN([EGT_CC_TRY_FLAG], [
  AC_MSG_CHECKING([whether $CC supports $1])

  egt_save_CFLAGS="$CFLAGS"
  CFLAGS="$CFLAGS $1 -Werror"

  AC_COMPILE_IFELSE([AC_LANG_SOURCE([ ])], [egt_cc_flag=yes], [egt_cc_flag=no])
  CFLAGS="$egt_save_CFLAGS"

  if test "x$egt_cc_flag" = "xyes"; then
    ifelse([$2], , :, [$2])
  else
    ifelse([$3], , :, [$3])
  fi
  AC_MSG_RESULT([$egt_cc_flag])
])

MAYBE_WARN="-Wall -Wextra \
-pedantic \
-Wfloat-equal \
-Wsign-compare \
-Werror-implicit-function-declaration \
-Wpointer-arith \
-Wwrite-strings \
-Wmissing-declarations \
-Wpacked \
-Wstrict-aliasing=2 -Winit-self \
-Wno-attributes -Wno-long-long \
-Wno-missing-field-initializers \
-Wno-unused-parameter \
-Wno-psabi" # no warnings about gcc abi changes

# invalidate cached value if MAYBE_WARN has changed
if test "x$egt_cv_warn_maybe" != "x$MAYBE_WARN"; then
	unset egt_cv_warn_cflags
fi
AC_CACHE_CHECK([for supported warning flags], egt_cv_warn_cflags, [
	echo
	WARN_CFLAGS=""

	# Some warning options are not supported by all versions of
	# gcc, so test all desired options against the current
	# compiler.
	#
	# Note that there are some order dependencies
	# here. Specifically, an option that disables a warning will
	# have no net effect if a later option then enables that
	# warnings, (perhaps implicitly). So we put some grouped
	# options (-Wall and -Wextra) up front and the -Wno options
	# last.

	for W in $MAYBE_WARN; do
		EGT_CC_TRY_FLAG([$W], [WARN_CFLAGS="$WARN_CFLAGS $W"])
	done

	egt_cv_warn_cflags=$WARN_CFLAGS
	egt_cv_warn_maybe=$MAYBE_WARN

	AC_MSG_CHECKING([which warning flags were supported])])
WARN_CFLAGS="$egt_cv_warn_cflags"
AC_SUBST(WARN_CFLAGS)

# Checks for header files.
AC_PATH_X
AC_CHECK_HEADERS([fcntl.h float.h inttypes.h libintl.h locale.h stdint.h stdlib.h string.h sys/ioctl.h sys/socket.h sys/time.h unistd.h glob.h])

AC_CHECK_HEADERS([linux/gpio.h],[have_linux_gpio_h=yes],[])
AM_CONDITIONAL([HAVE_LINUX_GPIO_H], [test "x${have_linux_gpio_h}" = xyes])

CXXFLAGS="$CXXFLAGS -DSPDLOG_COMPILED_LIB -DEGT_DLL_EXPORTS"

AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([--disable-debug], [enable debugging support [default=yes]])],
  [enable_debug=$enableval], [enable_debug=yes])

if test "x$enable_debug" = "xyes" ; then
  AC_MSG_NOTICE([Disabling debugging])
  CXXFLAGS="$CXXFLAGS -DNDEBUG -Ofast \
  		      -fvisibility=hidden \
		      -fvisibility-inlines-hidden \
		      -fno-math-errno \
		      -fmerge-all-constants \
		      -ftree-vectorize \
		      -ffunction-sections \
		      -fdata-sections"
  CFLAGS="$CFLAGS -DNDEBUG -Ofast"
else
  CXXFLAGS="$CXXFLAGS -O0 -DSPDLOG_ACTIVE_LEVEL=0"
  CFLAGS="$CFLAGS -O0"
fi

AC_ARG_ENABLE(gcov,
  [AC_HELP_STRING([--enable-gcov],[turn on code coverage analysis tools])],
  [enable_gcov=$enableval], [enable_gcov=no])

dnl Check for --enable-gcov and add appropriate flags for gcc
dnl Note that these extra switches are NOT applied to the loader
if test "x$enable_gcov" = "xyes";
then
  dnl Check for -fprofile-arcs and -ftest-coverage option
  EGT_CC_TRY_FLAG([--coverage], ac_cv_c_gcc_ftest_coverage="yes")

  if test "$ac_cv_c_gcc_ftest_coverage" = "yes"
  then
    CFLAGS="$CFLAGS --coverage"
    CXXFLAGS="$CXXFLAGS --coverage"
    LDFLAGS="${LDFLAGS} -lgcov"
    dnl Turn off optimization so code coverage tool
    dnl can get accurate line numbers
    CFLAGS=`echo "$CFLAGS" | sed -e 's/-O[0-9a-z]*//g'`
    CXXFLAGS=`echo "$CXXFLAGS" | sed -e 's/-O[0-9a-z]*//g'`
  else
    ac_cv_c_gcc_ftest_coverage=no
  fi
fi

AC_ARG_ENABLE(profile,
  [AC_HELP_STRING([--enable-profile],[turn on profiling tools])],
  [enable_profile=$enableval], [enable_profile=no])

if test "x$enable_profile" = "xyes";
then
  EGT_CC_TRY_FLAG([-pg], ac_cv_c_gcc_pg="yes")

  if test "$ac_cv_c_gcc_pg" = "yes"
  then
    CFLAGS="$CFLAGS -pg"
    CXXFLAGS="$CXXFLAGS -pg"
  else
    ac_cv_c_gcc_pg=no
  fi
fi

AC_ARG_ENABLE([examples],
  [AS_HELP_STRING([--enable-examples], [build examples [default=yes]])],
  [enable_examples=$enableval], [enable_examples=yes])
AM_CONDITIONAL([ENABLE_EXAMPLES], [test "x${enable_examples}" = xyes])

AC_ARG_ENABLE([lto],
  [AS_HELP_STRING([--enable-lto], [enable gcc's LTO [default=no]])],
  [enable_lto=$enableval], [enable_lto=no])
if test "x$enable_lto" = "xyes" ; then
  AC_MSG_NOTICE([enabling LTO])
  CFLAGS="$CFLAGS -flto"
  CXXFLAGS="$CXXFLAGS -flto"
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MMAP
AC_CHECK_FUNCS([floor memmove memset munmap select setlocale sqrt strerror])

AX_PTHREAD([LIBS="$PTHREAD_LIBS $LIBS"
            CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
            CXXFLAGS="$CXXFLAGS $PTHREAD_CFLAGS"],
            AC_MSG_ERROR(Can not find pthreads.  This is required.))

AC_ARG_WITH([x11],
    AS_HELP_STRING([--without-x11], [Ignore presence of x11 and disable it]),
    [with_x11=$withval],
    [with_x11=auto])
AS_IF([test "x$with_x11" != "xno"],[
   AX_PKG_CHECK_MODULES2(X11, [], [x11 >= 1.6.3], [have_x11=yes], [have_x11=no])
   if test "x${have_x11}" = xyes; then
      AC_DEFINE(HAVE_X11, 1, [Have x11 support])
      LIBEGT_EXTRA_CXXFLAGS="${X11_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
      LIBEGT_EXTRA_LDFLAGS="${X11_LIBS} ${LIBEGT_EXTRA_LDFLAGS}"
   fi
])
if test "x$with_x11" = xyes && test "x${have_x11}" != xyes; then
   AC_MSG_FAILURE([--with-x11 was given, but x11 not found])
fi
AM_CONDITIONAL([HAVE_X11], [test "x${have_x11}" = xyes])

AC_ARG_WITH([xkbcommon],
    AS_HELP_STRING([--without-xkbcommon], [Ignore presence of xkbcommon and disable it]),
    [with_xkbcommon=$withval],
    [with_xkbcommon=auto])
AS_IF([test "x$with_xkbcommon" != "xno"],[
   AX_PKG_CHECK_MODULES2(XKBCOMMON, [], [xkbcommon], [have_xkbcommon=yes], [have_xkbcommon=no])
   if test "x${have_xkbcommon}" = xyes; then
      AC_DEFINE(HAVE_XKBCOMMON, 1, [Have xkbcommon support])
      LIBEGT_EXTRA_CXXFLAGS="${XKBCOMMON_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
      LIBEGT_EXTRA_LDFLAGS="${XKBCOMMON_LIBS} ${LIBEGT_EXTRA_LDFLAGS}"
   fi
])
if test "x$with_xkbcommon" = xyes && test "x${have_xkbcommon}" != xyes; then
   AC_MSG_FAILURE([--with-xkbcommon was given, but xkbcommon not found])
fi
AM_CONDITIONAL([HAVE_XKBCOMMON], [test "x${have_xkbcommon}" = xyes])

AC_ARG_WITH([tslib],
    AS_HELP_STRING([--without-tslib], [Ignore presence of tslib and disable it]),
    [with_tslib=$withval],
    [with_tslib=auto])
AS_IF([test "x$with_tslib" != "xno"],[
   AX_PKG_CHECK_MODULES2(TSLIB, [], [tslib >= 1.15], [have_tslib=yes], [have_tslib=no])
   if test "x${have_tslib}" = xyes; then
      AC_DEFINE(HAVE_TSLIB, 1, [Have tslib support])
      LIBEGT_EXTRA_CXXFLAGS="${TSLIB_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
      LIBEGT_EXTRA_LDFLAGS="${TSLIB_LIBS} ${LIBEGT_EXTRA_LDFLAGS}"
   fi
])
if test "x$with_tslib" = xyes && test "x${have_tslib}" != xyes; then
   AC_MSG_FAILURE([--with-tslib was given, but tslib not found])
fi
AM_CONDITIONAL([HAVE_TSLIB], [test "x${have_tslib}" = xyes])

AC_ARG_WITH([libinput],
    AS_HELP_STRING([--without-libinput], [Ignore presence of libinput and disable it]),
    [with_libinput=$withval],
    [with_libinput=auto])
AS_IF([test "x$with_libinput" != "xno"],[
   AX_PKG_CHECK_MODULES2(LIBINPUT, [], [libinput >= 1.6.3], [have_libinput=yes], [have_libinput=no])
   if test "x${have_libinput}" = xyes; then
      AC_DEFINE(HAVE_LIBINPUT, 1, [Have libinput support])
      LIBEGT_EXTRA_CXXFLAGS="${LIBINPUT_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
      LIBEGT_EXTRA_LDFLAGS="${LIBINPUT_LIBS} -ludev ${LIBEGT_EXTRA_LDFLAGS}"
   fi
])
if test "x$with_libinput" = xyes && test "x${have_libinput}" != xyes; then
   AC_MSG_FAILURE([--with-libinput was given, but libinput not found])
fi
AM_CONDITIONAL([HAVE_LIBINPUT], [test "x${have_libinput}" = xyes])

AC_ARG_WITH([gstreamer],
    AS_HELP_STRING([--without-gstreamer], [Ignore presence of gstreamer and disable it]),
    [with_gstreamer=$withval],
    [with_gstreamer=auto])
AS_IF([test "x$with_gstreamer" != "xno"],[
   AX_PKG_CHECK_MODULES2(GSTREAMER, [], [gstreamer-1.0 >= 1.8], [have_gstreamer=yes], [have_gstreamer=no])
   if test "x${have_gstreamer}" = xyes; then
      AC_DEFINE(HAVE_GSTREAMER, 1, [Have gstreamer support])
      LIBEGT_EXTRA_CXXFLAGS="${GSTREAMER_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
      LIBEGT_EXTRA_LDFLAGS="${GSTREAMER_LIBS} ${LIBEGT_EXTRA_LDFLAGS}"

      AX_PKG_CHECK_MODULES2(GSTREAMER_PBUTILS, [], [gstreamer-pbutils-1.0 >= 1.8], [have_gst_pbutils=yes], [have_gst_pbutils=no])
      if test "x${have_gst_pbutils}" = xyes; then
         AC_DEFINE(HAVE_GSTREAMER_PBUTILS, 1, [Have gstreamer pbutils support])
         LIBEGT_EXTRA_CXXFLAGS="${GSTREAMER_PBUTILS_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
         LIBEGT_EXTRA_LDFLAGS="${GSTREAMER_PBUTILS_LIBS} ${LIBEGT_EXTRA_LDFLAGS}"
      fi
   fi
])
if test "x$with_gstreamer" = xyes && test "x${have_gstreamer}" != xyes; then
   AC_MSG_FAILURE([--with-gstreamer was given, but gstreamer not found])
fi
AM_CONDITIONAL([HAVE_GSTREAMER], [test "x${have_gstreamer}" = xyes])
AM_CONDITIONAL([HAVE_GSTREAMER_PBUTILS], [test "x${have_gst_pbutils}" = xyes])

AC_ARG_WITH([lua],
    AS_HELP_STRING([--without-lua], [Ignore presence of lua and disable it]),
    [with_lua=$withval],
    [with_lua=auto])
AS_IF([test "x$with_lua" != "xno"],[
   AX_PKG_CHECK_MODULES2(LUA, [], [lua >= 5.3.0], [have_lua=yes], [have_lua=no])
   if test "x${have_lua}" = xno; then
      AX_PKG_CHECK_MODULES2(LUA, [], [lua5.3], [have_lua=yes], [have_lua=no])
   fi
   if test "x${have_lua}" = xyes; then
      AC_DEFINE(HAVE_LUA, 1, [Have lua support])
      LIBEGT_EXTRA_CXXFLAGS="${LUA_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
      LIBEGT_EXTRA_LDFLAGS="${LUA_LIBS} ${LIBEGT_EXTRA_LDFLAGS}"

      AC_ARG_ENABLE(lua-bindings,
        [AC_HELP_STRING([--enable-lua-bindings],[enable lua bindings (default: no)])],
        [enable_lua_bindings=$enableval], [enable_lua_bindings=no])
      if test "x$enable_lua_bindings" = "xyes";
      then
         AC_PATH_PROG([SWIG_BIN], [swig])
         if test "x${have_lua}" = xyes && test -n "$SWIG_BIN"; then
            have_lua_bindings=yes
            AC_SUBST(include_luaapp, ["#define EGT_HAS_LUA_BINDINGS 1"])
	 else
	    AC_MSG_ERROR([lua bindings requested but swig not found.])
         fi
      fi
   fi
])
if test "x$with_lua" = xyes && test "x${have_lua}" != xyes; then
   AC_MSG_FAILURE([--with-lua was given, but lua not found])
fi
AM_CONDITIONAL([HAVE_LUA], [test "x${have_lua}" = xyes])
AM_CONDITIONAL([ENABLE_LUA_BINDINGS], [test "x${have_lua_bindings}" = xyes])

AC_CHECK_HEADER([readline/readline.h], [have_readline=yes], [have_readline=no])
if test "x${have_readline}" = xyes; then
   LIBEGT_EXTRA_CXXFLAGS="-DLUA_USE_READLINE ${LIBEGT_EXTRA_CXXFLAGS}"
   LIBEGT_EXTRA_LDFLAGS="-lreadline ${LIBEGT_EXTRA_LDFLAGS}"
fi

AX_PKG_CHECK_MODULES2(LIBPLANES, [], [libplanes >= 1.0.0], [have_libplanes=yes], [have_libplanes=no])
AM_CONDITIONAL([HAVE_LIBPLANES], [test "x${have_libplanes}" = xyes])
if test "x${have_libplanes}" = xyes; then
   AX_PKG_CHECK_MODULES2(LIBDRM, [], [libdrm >= 2.4.0], [], [AC_MSG_ERROR([
     libdrm is a hard dependency, but a sufficient version was not found.])])
   AC_DEFINE(HAVE_LIBDRM, 1, [Have libdrm support])
   AC_DEFINE(HAVE_LIBPLANES, 1, [Have libplanes support])
   LIBEGT_EXTRA_CXXFLAGS="${LIBPLANES_CFLAGS} ${LIBDRM_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
   LIBEGT_EXTRA_LDFLAGS="${LIBPLANES_LIBS} ${LIBDRM_LDFLAGS} ${LIBEGT_EXTRA_LDFLAGS}"
fi

AX_PKG_CHECK_MODULES2(CAIRO, [cairo >= 1.14.6], [], [have_cairo=yes], [AC_MSG_ERROR([
     cairo is a hard dependency, but a sufficient version was not found.])])
AM_CONDITIONAL([HAVE_CAIRO], [test "x${have_cairo}" = xyes])
if test "x${have_cairo}" = xyes; then
  LIBEGT_EXTRA_CXXFLAGS="${CAIRO_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
  LIBEGT_EXTRA_LDFLAGS="${CAIRO_LIBS} ${LIBEGT_EXTRA_LDFLAGS}"
  AC_SEARCH_LIBS([cairo_gfx2d_surface_create], [cairo], [have_cairo_gfx2d=yes], [have_cairo_gfx2d=no])
  AC_CHECK_HEADER([cairo/cairo-gfx2d.h],
    [],
    [have_cairo_gfx2d=no
  ])
  if test "x${have_cairo_gfx2d}" = xyes; then
    AC_DEFINE(HAVE_CAIRO_GFX2D, 1, [Have cairo gfx2d support])
  fi
fi

AX_PKG_CHECK_MODULES2(FONTCONFIG, [], [fontconfig], [], [AC_MSG_ERROR([
     fontconfig is a hard dependency, but a sufficient version was not found.])])
LIBEGT_EXTRA_CXXFLAGS="${FONTCONFIG_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
LIBEGT_EXTRA_LDFLAGS="${FONTCONFIG_LIBS} ${LIBEGT_EXTRA_LDFLAGS}"

AC_ARG_WITH([plplot],
    AS_HELP_STRING([--without-plplot], [Ignore presence of plplot and disable it]),
    [with_plplot=$withval],
    [with_plplot=auto])
AS_IF([test "x$with_plplot" != "xno"],[
   AX_PKG_CHECK_MODULES2(PLPLOT, [plplot-c++], [], [have_plplot=yes], [have_plplot=no])
   if test "x${have_plplot}" = xno; then
      AX_PKG_CHECK_MODULES2(PLPLOT, [plplotd-c++], [], [have_plplot=yes], [have_plplot=no])
   fi
   if test "x${have_plplot}" = xyes; then
      AC_DEFINE(HAVE_PLPLOT, 1, [Have plplot support])
      AC_SUBST(include_chart, ["#define EGT_HAS_CHART 1"])
      LIBEGT_EXTRA_CXXFLAGS="${PLPLOT_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
      LIBEGT_EXTRA_LDFLAGS="${PLPLOT_LIBS} ${LIBEGT_EXTRA_LDFLAGS}"
   fi
])
if test "x$with_plplot" = xyes && test "x${have_plplot}" != xyes; then
   AC_MSG_FAILURE([--with-plplot was given, but plplot not found])
fi
AM_CONDITIONAL([HAVE_PLPLOT], [test "x${have_plplot}" = xyes])

AC_ARG_WITH([libjpeg],
    AS_HELP_STRING([--without-libjpeg], [Ignore presence of libjpeg and disable it]),
    [with_libjpeg=$withval],
    [with_libjpeg=auto])
AS_IF([test "x$with_libjpeg" != "xno"],[
   AX_PKG_CHECK_MODULES2(LIBJPEG, [], [libjpeg], [have_libjpeg=yes], [have_libjpeg=no])
   if test "x${have_libjpeg}" = xyes; then
      AC_DEFINE(HAVE_LIBJPEG, 1, [Have libjpeg support])
      LIBEGT_EXTRA_CXXFLAGS="${LIBJPEG_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
      LIBEGT_EXTRA_LDFLAGS="${LIBJPEG_LIBS} ${LIBEGT_EXTRA_LDFLAGS}"
   fi
])
if test "x$with_libjpeg" = xyes && test "x${have_libjpeg}" != xyes; then
   AC_MSG_FAILURE([--with-libjpeg was given, but libjpeg not found])
fi
AM_CONDITIONAL([HAVE_LIBJPEG], [test "x${have_libjpeg}" = xyes])

AC_ARG_WITH([libcurl],
    AS_HELP_STRING([--without-libcurl], [Ignore presence of libcurl and disable it]),
    [with_libcurl=$withval],
    [with_libcurl=auto])
AS_IF([test "x$with_libcurl" != "xno"],[
   AX_PKG_CHECK_MODULES2(LIBCURL, [], [libcurl >= 4.5], [have_libcurl=yes], [have_libcurl=no])
   if test "x${have_libcurl}" = xyes; then
      AC_DEFINE(HAVE_LIBCURL, 1, [Have libcurl support])
      AC_SUBST(include_http, ["#define EGT_HAS_HTTP 1"])
      LIBEGT_EXTRA_CXXFLAGS="${LIBCURL_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
      LIBEGT_EXTRA_LDFLAGS="${LIBCURL_LIBS} ${LIBEGT_EXTRA_LDFLAGS}"
   fi
])
if test "x$with_libcurl" = xyes && test "x${have_libcurl}" != xyes; then
   AC_MSG_FAILURE([--with-libcurl was given, but libcurl not found])
fi
AM_CONDITIONAL([HAVE_LIBCURL], [test "x${have_libcurl}" = xyes])

AC_ARG_WITH([librsvg],
    AS_HELP_STRING([--without-librsvg], [Ignore presence of librsvg and disable it]),
    [with_librsvg=$withval],
    [with_librsvg=auto])
AS_IF([test "x$with_librsvg" != "xno"],[
   AX_PKG_CHECK_MODULES2(LIBRSVG, [], [librsvg-2.0], [have_librsvg=yes], [have_librsvg=no])
   if test "x${have_librsvg}" = xyes; then
      AC_DEFINE(HAVE_LIBRSVG, 1, [Have librsvg support])
      AC_SUBST(include_svg, ["#define EGT_HAS_SVG 1"])
      LIBEGT_EXTRA_CXXFLAGS="${LIBRSVG_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
      LIBEGT_EXTRA_LDFLAGS="${LIBRSVG_LIBS} ${LIBEGT_EXTRA_LDFLAGS}"
   fi
])
if test "x$with_librsvg" = xyes && test "x${have_librsvg}" != xyes; then
   AC_MSG_FAILURE([--with-librsvg was given, but librsvg not found])
fi
AM_CONDITIONAL([HAVE_LIBRSVG], [test "x${have_librsvg}" = xyes])

AC_ARG_WITH([zlib],
    AS_HELP_STRING([--without-zlib], [Ignore presence of zlib and disable it]))
AS_IF([test "x$with_zlib" != "xno"],[
   AX_PKG_CHECK_MODULES2(ZLIB, [], [zlib], [have_zlib=yes], [have_zlib=no])
   if test "x${have_zlib}" = xyes; then
      AC_DEFINE(HAVE_ZLIB, 1, [Have zlib support])
      LIBEGT_EXTRA_CXXFLAGS="${ZLIB_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
      LIBEGT_EXTRA_LDFLAGS="${ZLIB_LIBS} ${LIBEGT_EXTRA_LDFLAGS}"
   fi
])
AM_CONDITIONAL([HAVE_ZLIB], [test "x${have_zlib}" = xyes])

AC_SEARCH_LIBS([magic_open], [magic], [MAGIC_LIB=-lmagic have_libmagic=yes], [have_libmagic=no])
AC_CHECK_HEADER([magic.h],
  [],
  [have_libmagic=no
])
AM_CONDITIONAL([HAVE_LIBMAGIC], [test "x${have_libmagic}" = xyes])
if test "x${have_libmagic}" = xyes; then
   AC_DEFINE(HAVE_LIBMAGIC, 1, [Have libmagic support])
   LIBEGT_EXTRA_LDFLAGS="${MAGIC_LIB} ${LIBEGT_EXTRA_LDFLAGS}"
fi

AC_CHECK_LIB([stdc++fs], [main], [have_libstdcppfs=yes], [have_libstdcppfs=no])
AM_CONDITIONAL([HAVE_EXPERIMENTAL_FILESYSTEM], [test "x${have_libstdcppfs}" = xyes])
if test "x${have_libstdcppfs}" = xyes;
then
   LDFLAGS="${LDFLAGS} -lstdc++fs"
   AC_DEFINE(HAVE_EXPERIMENTAL_FILESYSTEM, 1, [Have experimental filesystem support])
fi
AM_CONDITIONAL([HAVE_EXPERIMENTAL_FILESYSTEM], [test "x${have_libstdcppfs}" = xyes])

if test "x${have_gstreamer}" = xyes; then
   AC_SUBST(include_audio, ["#define EGT_HAS_AUDIO 1"])
   AC_SUBST(include_video, ["#define EGT_HAS_VIDEO 1"])
   AC_SUBST(include_camera, ["#define EGT_HAS_CAMERA 1"])
   AC_SUBST(include_capture, ["#define EGT_HAS_CAPTURE 1"])
fi

AC_CHECK_HEADER([linux/fb.h], [have_fbdev=yes], [have_fbdev=no])
AM_CONDITIONAL([HAVE_FBDEV], [test "x${have_fbdev}" = xyes])
if test "x${have_fbdev}" = xyes; then
   AC_DEFINE(HAVE_FBDEV, 1, [Have fbdev support])
fi

AC_CHECK_HEADER([linux/input.h], [have_linux_input_h=yes], [have_linux_input_h=no])
AM_CONDITIONAL([HAVE_LINUX_INPUT_H], [test "x${have_linux_input_h}" = xyes])
if test "x${have_linux_input_h}" = xyes; then
   AC_DEFINE(HAVE_LINUX_INPUT_H, 1, [Have linux/input.h support])
fi

AC_CHECK_PROG(XXD, xxd, yes)
if test "x${XXD}" != x"yes"; then
   if test "x${enable_examples}" = xyes; then
      AC_MSG_ERROR("Cannot find xxd. This is required.")
   fi
fi

AC_CHECK_PROG(MSGFMT, msgfmt, yes)
if test "x${MSGFMT}" != x"yes"; then
   if test "x${enable_examples}" = xyes; then
      AC_MSG_WARN("Missing msgfmt: install gettext")
   fi
fi
AM_CONDITIONAL([HAVE_MSGFMT], [test "x${MSGFMT}" = xyes])

AC_ARG_WITH([alsa],
    AS_HELP_STRING([--without-alsa], [Ignore presence of alsa and disable it]),
    [with_alsa=$withval],
    [with_alsa=auto])
AS_IF([test "x$with_alsa" != "xno"],[
   AX_PKG_CHECK_MODULES2(ALSA, [], [alsa], [have_alsa=yes], [have_alsa=no])
   if test "x${have_alsa}" = xyes; then
      AC_DEFINE(HAVE_ALSA, 1, [Have alsa support])
      LIBEGT_EXTRA_CXXFLAGS="${ALSA_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
      LIBEGT_EXTRA_LDFLAGS="${ALSA_LIBS} ${LIBEGT_EXTRA_LDFLAGS}"
      AC_SUBST(include_sound, ["#define EGT_HAS_SOUND 1"])
   fi
])
if test "x$with_alsa" = xyes && test "x${have_alsa}" != xyes; then
   AC_MSG_FAILURE([--with-alsa was given, but alsa not found])
fi
AM_CONDITIONAL([HAVE_ALSA], [test "x${have_alsa}" = xyes])

AC_ARG_WITH([sndfile],
    AS_HELP_STRING([--without-sndfile], [Ignore presence of sndfile and disable it]),
    [with_sndfile=$withval],
    [with_sndfile=auto])
AS_IF([test "x$with_sndfile" != "xno"],[
   AX_PKG_CHECK_MODULES2(SNDFILE, [], [sndfile], [have_sndfile=yes], [have_sndfile=no])
   if test "x${have_sndfile}" = xyes; then
      AC_DEFINE(HAVE_SNDFILE, 1, [Have sndfile support])
      LIBEGT_EXTRA_CXXFLAGS="${SNDFILE_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
      LIBEGT_EXTRA_LDFLAGS="${SNDFILE_LIBS} ${LIBEGT_EXTRA_LDFLAGS}"
   fi
])
if test "x$with_sndfile" = xyes && test "x${have_sndfile}" != xyes; then
   AC_MSG_FAILURE([--with-sndfile was given, but sndfile not found])
fi
AM_CONDITIONAL([HAVE_SNDFILE], [test "x${have_sndfile}" = xyes])

# Subst LIBEGT_EXTRA_CFLAGS & CXXFLAGS & LDFLAGS
AC_SUBST(LIBEGT_EXTRA_CFLAGS)
AC_SUBST(LIBEGT_EXTRA_CXXFLAGS)
AC_SUBST(LIBEGT_EXTRA_LDFLAGS)

AC_USE_SYSTEM_EXTENSIONS
AC_SYS_LARGEFILE
AC_FUNC_ALLOCA

AC_PATH_PROG([ASTYLE_BIN], [astyle])
AM_CONDITIONAL([HAVE_ASTYLE_BIN], [test -n "$ASTYLE_BIN"])

AC_PATH_PROG([CLOC_BIN], [cloc])
AM_CONDITIONAL([HAVE_CLOC_BIN], [test -n "$CLOC_BIN"])

AC_PATH_PROG([CPPCHECK_BIN], [cppcheck])
AM_CONDITIONAL([HAVE_CPPCHECK_BIN], [test -n "$CPPCHECK_BIN"])

AC_PATH_PROG([DOXYGEN_BIN], [doxygen])
AM_CONDITIONAL([HAVE_DOXYGEN_BIN], [test -n "$DOXYGEN_BIN"])
AM_COND_IF([HAVE_DOXYGEN_BIN], [AC_CONFIG_FILES([docs/Doxyfile docs/Info.plist])])

AC_PATH_PROG([ASPELL_BIN], [aspell])
AM_CONDITIONAL([HAVE_ASPELL_BIN], [test -n "$ASPELL_BIN"])

AC_PATH_PROGS([CLANG_TIDY_BIN], [clang-tidy-6.0 clang-tidy])
AM_CONDITIONAL([HAVE_CLANG_TIDY_BIN], [test -n "$CLANG_TIDY_BIN"])

AC_MSG_NOTICE([configuring simd])
(
  SIMD_CXXFLAGS=`echo "$CXXFLAGS" | sed -e 's/-W[-0-9a-z=]*//g'`
  # if -Wno-psabi was there, add it back
  SIMD_CXXFLAGS+=`echo "$CXXFLAGS" | grep -o '\-Wno-psabi'`
  SIMD_CXXFLAGS=`echo "$SIMD_CXXFLAGS" | sed -e 's/-pedantic//g'`
  SIMD_CXXFLAGS=`echo "$SIMD_CXXFLAGS" | sed -e 's/-flto//g'`

  mkdir -p external/Simd/prj/cmake/ &&
  cd external/Simd/prj/cmake/ &&
  cmake ../../../../${srcdir}/external/Simd/prj/cmake/ -DTOOLCHAIN="$CXX" -DTARGET="$host_cpu" \
  	-DCMAKE_BUILD_TYPE="Release" -DLIBRARY="STATIC" \
	-DSIMD_TEST=off -DCMAKE_CXX_FLAGS="${SIMD_CXXFLAGS} -DCHECK_VERSION=0"
)

AC_ARG_ENABLE([simd],
  [AS_HELP_STRING([--enable-simd], [build with simd support [default=no]])],
  [enable_simd=$enableval], [enable_simd=no])
if test "x$enable_simd" = "xyes";
then
  AC_DEFINE(HAVE_SIMD, 1, [Have simd support])
fi
AM_CONDITIONAL([HAVE_SIMD], [test "x${enable_simd}" = xyes])

AC_ARG_ENABLE([snippets],
  [AS_HELP_STRING([--enable-snippets], [build snippets examples [default=no]])],
  [enable_snippets=$enableval], [enable_snippets=no])
AM_CONDITIONAL([ENABLE_SNIPPETS], [test "x${enable_snippets}" = xyes])

AC_CONFIG_FILES([Makefile
	src/Makefile
	external/Makefile
	examples/Makefile
	scripts/Makefile
	libegt.pc
	include/egt/ui
	include/egt/version.h
	examples/i18n/Makefile
	examples/resource/Makefile
	examples/snippets/Makefile
	examples/video/Makefile
	docs/Makefile
	test/Makefile
	lua/Makefile])
AC_OUTPUT

dnl report how we have been configured
echo
echo "Configured EGT ${PACKAGE_VERSION} for \`${host}'"
echo
echo "Compile:"
echo "  Debug                  ${enable_debug:-yes}"
echo "  Coverage               ${ac_cv_c_gcc_ftest_coverage:-no}"
echo "  Profile                ${ac_cv_c_gcc_pg:-no}"
echo "  LTO                    ${enable_lto:-no}"
echo "  SIMD                   ${enable_simd:-no}"
echo "  Examples               ${enable_examples}"
echo "  CXXFLAGS               ${CXXFLAGS}"
echo

echo "Display:"
echo "  DRM/KMS                ${have_libplanes:-no}"
echo "  X11                    ${have_x11:-no}"
echo "  fbdev                  ${have_fbdev:-no}"
echo

echo "Input:"
echo "  libinput               ${have_libinput:-no}"
echo "  tslib                  ${have_tslib:-no}"
echo "  evdev                  ${have_linux_input_h:-no}"
echo "  X11                    ${have_x11:-no}"
echo "  xkbcommon              ${have_xkbcommon:-no}"
echo

echo "Media:"
echo "  gstreamer              ${have_gstreamer:-no}"
echo "  SVG                    ${have_librsvg:-no}"
echo "  JPEG                   ${have_libjpeg:-no}"
echo "  PNG                    yes"
echo "  BMP                    yes"
echo

echo "Features:"
echo "  Networking             ${have_libcurl:-no}"
echo "  libmagic               ${have_libmagic:-no (mime type detection disabled)}"
echo "  LUA Interpreter        ${have_lua:-no}"
echo "  LUA Bindings           ${have_lua_bindings:-no}"
echo "  C++17 Filesystem       ${have_libstdcppfs:-no}"
echo "  ALSA Sound             ${have_alsa:-no}"
echo "  sndfile                ${have_sndfile:-no}"
echo "  plplot                 ${have_plplot:-no}"
echo
