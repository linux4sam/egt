AC_PREREQ([2.63])
AC_INIT([libegt],[0.6-beta])

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

AM_INIT_AUTOMAKE([1.10 foreign subdir-objects])

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
  CXXFLAGS="$CXXFLAGS -std=gnu++11"
else
  CXXFLAGS="$CXXFLAGS -std=c++11"
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
  CFLAGS="$CFLAGS $1"

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

AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([--disable-debug], [build without debugging support [default=no]])],
  [enable_debug=no], [ ])

if test "$enable_debug" = "no" ; then
  AC_MSG_NOTICE([Disabling debugging])
  CXXFLAGS="$CXXFLAGS -DNDEBUG -Ofast -DSPDLOG_COMPILED_LIB"
  CFLAGS="$CFLAGS -DNDEBUG -Ofast"
else
  CXXFLAGS="$CXXFLAGS -O0 -DSPDLOG_ACTIVE_LEVEL=0 -DSPDLOG_COMPILED_LIB"
  CFLAGS="$CFLAGS -O0"
fi

AC_ARG_ENABLE(gcov, AC_HELP_STRING([--enable-gcov],[turn on code coverage analysis tools]))

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
    dnl Turn off optimization so code coverage tool
    dnl can get accurate line numbers
    CFLAGS=`echo "$CFLAGS" | sed -e 's/-O[0-9a-z]*//g'`
    CXXFLAGS=`echo "$CXXFLAGS" | sed -e 's/-O[0-9a-z]*//g'`
  else
    ac_cv_c_gcc_ftest_coverage=no
  fi
fi

AC_ARG_ENABLE(profile, AC_HELP_STRING([--enable-profile],[turn on profiling tools]))

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
  [enable_examples=${enableval}], [enable_examples=yes])
AM_CONDITIONAL([ENABLE_EXAMPLES], [test "x${enable_examples}" = xyes])

AC_ARG_ENABLE([lto],
  [AS_HELP_STRING([--enable-lto], [enable gcc's LTO [default=no]])],
  [enable_lto=yes], [enable_lto=no])
if test "$enable_lto" = "yes" ; then
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
            CFLAGS="$CFLAGS $PTHREAD_CFLAGS"], AC_MSG_ERROR(Can not find pthreads.  This is required.))

AC_ARG_WITH([x11],
    AS_HELP_STRING([--without-x11], [Ignore presence of x11 and disable it]))
AS_IF([test "x$with_x11" != "xno"],[
   AX_PKG_CHECK_MODULES2(X11, [x11 >= 1.6.3], [], [have_x11=yes], [have_x11=no])
   if test "x${have_x11}" = xyes; then
      AC_DEFINE(HAVE_X11, 1, [Have x11 support])
      LIBEGT_EXTRA_CXXFLAGS="${X11_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
      LIBEGT_EXTRA_LDFLAGS="${X11_LIBS} ${LIBEGT_EXTRA_LDFLAGS}"
   fi
])
AM_CONDITIONAL([HAVE_X11], [test "x${have_x11}" = xyes])

AC_ARG_WITH([xkbcommon],
    AS_HELP_STRING([--without-xkbcommon], [Ignore presence of xkbcommon and disable it]))
AS_IF([test "x$with_xkbcommon" != "xno"],[
   AX_PKG_CHECK_MODULES2(XKBCOMMON, [xkbcommon], [], [have_xkbcommon=yes], [have_xkbcommon=no])
   if test "x${have_xkbcommon}" = xyes; then
      AC_DEFINE(HAVE_XKBCOMMON, 1, [Have xkbcommon support])
      LIBEGT_EXTRA_CXXFLAGS="${XKBCOMMON_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
      LIBEGT_EXTRA_LDFLAGS="${XKBCOMMON_LIBS} ${LIBEGT_EXTRA_LDFLAGS}"
   fi
])
AM_CONDITIONAL([HAVE_XKBCOMMON], [test "x${have_xkbcommon}" = xyes])

AC_ARG_WITH([tslib],
    AS_HELP_STRING([--without-tslib], [Ignore presence of tslib and disable it]))
AS_IF([test "x$with_tslib" != "xno"],[
   AX_PKG_CHECK_MODULES2(TSLIB, [tslib >= 1.15], [], [have_tslib=yes], [have_tslib=no])
   if test "x${have_tslib}" = xyes; then
      AC_DEFINE(HAVE_TSLIB, 1, [Have tslib support])
      LIBEGT_EXTRA_CXXFLAGS="${TSLIB_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
      LIBEGT_EXTRA_LDFLAGS="${TSLIB_LIBS} ${LIBEGT_EXTRA_LDFLAGS}"
   fi
])
AM_CONDITIONAL([HAVE_TSLIB], [test "x${have_tslib}" = xyes])

AC_ARG_WITH([libinput],
    AS_HELP_STRING([--without-libinput], [Ignore presence of libinput and disable it]))
AS_IF([test "x$with_libinput" != "xno"],[
   AX_PKG_CHECK_MODULES2(LIBINPUT, [libinput >= 1.6.3], [], [have_libinput=yes], [have_libinput=no])
   if test "x${have_libinput}" = xyes; then
      AC_DEFINE(HAVE_LIBINPUT, 1, [Have libinput support])
      LIBEGT_EXTRA_CXXFLAGS="${LIBINPUT_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
      LIBEGT_EXTRA_LDFLAGS="${LIBINPUT_LIBS} ${LIBEGT_EXTRA_LDFLAGS}"
   fi
])
AM_CONDITIONAL([HAVE_LIBINPUT], [test "x${have_libinput}" = xyes])

AC_ARG_WITH([gstreamer],
    AS_HELP_STRING([--without-gstreamer], [Ignore presence of gstreamer and disable it]))
AS_IF([test "x$with_gstreamer" != "xno"],[
   AX_PKG_CHECK_MODULES2(GSTREAMER, [gstreamer-1.0 >= 1.8], [], [have_gstreamer=yes], [have_gstreamer=no])
   if test "x${have_gstreamer}" = xyes; then
      AC_DEFINE(HAVE_GSTREAMER, 1, [Have gstreamer support])
      LIBEGT_EXTRA_CXXFLAGS="${GSTREAMER_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
      LIBEGT_EXTRA_LDFLAGS="${GSTREAMER_LIBS} ${LIBEGT_EXTRA_LDFLAGS}"

      AX_PKG_CHECK_MODULES2(GSTREAMER_PBUTILS, [gstreamer-pbutils-1.0 >= 1.8], [], [have_gst_pbutils=yes], [have_gst_pbutils=no])
      if test "x${have_gst_pbutils}" = xyes; then
         AC_DEFINE(HAVE_GSTREAMER_PBUTILS, 1, [Have gstreamer pbutils support])
         LIBEGT_EXTRA_CXXFLAGS="${GSTREAMER_PBUTILS_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
         LIBEGT_EXTRA_LDFLAGS="${GSTREAMER_PBUTILS_LIBS} ${LIBEGT_EXTRA_LDFLAGS}"
      fi
   fi
])
AM_CONDITIONAL([HAVE_GSTREAMER], [test "x${have_gstreamer}" = xyes])
AM_CONDITIONAL([HAVE_GSTREAMER_PBUTILS], [test "x${have_gst_pbutils}" = xyes])

AC_ARG_WITH([lua],
    AS_HELP_STRING([--without-lua], [Ignore presence of lua and disable it]))
AS_IF([test "x$with_lua" != "xno"],[
   AX_PKG_CHECK_MODULES2(LUA, [lua >= 5.3.0], [], [have_lua=yes], [have_lua=no])
   if test "x${have_lua}" = xno; then
      AX_PKG_CHECK_MODULES2(LUA, [lua5.3], [], [have_lua=yes], [have_lua=no])
   fi
   if test "x${have_lua}" = xyes; then
      AC_DEFINE(HAVE_LUA, 1, [Have lua support])
      LIBEGT_EXTRA_CXXFLAGS="${LUA_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
      LIBEGT_EXTRA_LDFLAGS="${LUA_LIBS} ${LIBEGT_EXTRA_LDFLAGS}"

      AC_ARG_ENABLE(lua-bindings, AC_HELP_STRING([--enable-lua-bindings],[enable lua bindings (default: no)]))
      if test "x$enable_lua_bindings" = "xyes";
      then
         AC_PATH_PROG([SWIG_BIN], [swig])
         if test "x${have_lua}" = xyes && test -n "$SWIG_BIN"; then
            have_lua_bindings=yes
            AC_SUBST(include_luaapp, ["#include <egt/luaapp.h>"])
	 else
	    AC_MSG_ERROR([bindings requested but swig not found.])
         fi
      fi
   fi
])
AM_CONDITIONAL([HAVE_LUA], [test "x${have_lua}" = xyes])
AM_CONDITIONAL([ENABLE_LUA_BINDINGS], [test "x${have_lua_bindings}" = xyes])

AC_CHECK_HEADER([readline/readline.h], [have_readline=yes], [have_readline=no])
if test "x${have_readline}" = xyes; then
   LIBEGT_EXTRA_CXXFLAGS="-DLUA_USE_READLINE ${LIBEGT_EXTRA_CXXFLAGS}"
   LIBEGT_EXTRA_LDFLAGS="-lreadline ${LIBEGT_EXTRA_LDFLAGS}"
fi

AX_PKG_CHECK_MODULES2(LIBPLANES, [libplanes >= 1.0.0], [], [have_libplanes=yes], [have_libplanes=no])
AM_CONDITIONAL([HAVE_LIBPLANES], [test "x${have_libplanes}" = xyes])
if test "x${have_libplanes}" = xyes; then
   AX_PKG_CHECK_MODULES2(LIBDRM, [], [libdrm >= 2.4.0], [], [AC_MSG_ERROR([
     libdrm is a hard dependency, but a sufficient version was not found.])])
   AC_DEFINE(HAVE_LIBDRM, 1, [Have libdrm support])
   AC_DEFINE(HAVE_LIBPLANES, 1, [Have libplanes support])
   LIBEGT_EXTRA_CXXFLAGS="${LIBPLANES_CFLAGS} ${LIBDRM_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
   LIBEGT_EXTRA_LDFLAGS="${LIBPLANES_LIBS} ${LIBDRM_LDFLAGS} ${LIBEGT_EXTRA_LDFLAGS}"
fi

AX_PKG_CHECK_MODULES2(CAIRO, [cairo >= 1.14.6], [], [], [AC_MSG_ERROR([
     cairo is a hard dependency, but a sufficient version was not found.])])
LIBEGT_EXTRA_CXXFLAGS="${CAIRO_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
LIBEGT_EXTRA_LDFLAGS="${CAIRO_LIBS} ${LIBEGT_EXTRA_LDFLAGS}"

AX_PKG_CHECK_MODULES2(FONTCONFIG, [fontconfig], [], [], [AC_MSG_ERROR([
     fontconfig is a hard dependency, but a sufficient version was not found.])])
LIBEGT_EXTRA_CXXFLAGS="${FONTCONFIG_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
LIBEGT_EXTRA_LDFLAGS="${FONTCONFIG_LIBS} ${LIBEGT_EXTRA_LDFLAGS}"

AC_ARG_WITH([libjpeg],
    AS_HELP_STRING([--without-libjpeg], [Ignore presence of libjpeg and disable it]))
AS_IF([test "x$with_libjpeg" != "xno"],[
   AX_PKG_CHECK_MODULES2(LIBJPEG, [libjpeg], [], [have_libjpeg=yes], [have_libjpeg=no])
   if test "x${have_libjpeg}" = xyes; then
      AC_DEFINE(HAVE_LIBJPEG, 1, [Have libjpeg support])
      LIBEGT_EXTRA_CXXFLAGS="${LIBJPEG_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
      LIBEGT_EXTRA_LDFLAGS="${LIBJPEG_LIBS} ${LIBEGT_EXTRA_LDFLAGS}"
   fi
])
AM_CONDITIONAL([HAVE_LIBJPEG], [test "x${have_libjpeg}" = xyes])

AC_ARG_WITH([libcurl],
    AS_HELP_STRING([--without-libcurl], [Ignore presence of libcurl and disable it]))
AS_IF([test "x$with_libcurl" != "xno"],[
   AX_PKG_CHECK_MODULES2(LIBCURL, [libcurl >= 4.5], [], [have_libcurl=yes], [have_libcurl=no])
   if test "x${have_libcurl}" = xyes; then
      AC_DEFINE(HAVE_LIBCURL, 1, [Have libcurl support])
      AC_SUBST(include_http, ["#include <egt/network/http.h>"])
      LIBEGT_EXTRA_CXXFLAGS="${LIBCURL_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
      LIBEGT_EXTRA_LDFLAGS="${LIBCURL_LIBS} ${LIBEGT_EXTRA_LDFLAGS}"
   fi
])
AM_CONDITIONAL([HAVE_LIBCURL], [test "x${have_libcurl}" = xyes])

AC_ARG_WITH([librsvg],
    AS_HELP_STRING([--without-librsvg], [Ignore presence of librsvg and disable it]))
AS_IF([test "x$with_librsvg" != "xno"],[
   AX_PKG_CHECK_MODULES2(LIBRSVG, [librsvg-2.0], [], [have_librsvg=yes], [have_librsvg=no])
   if test "x${have_librsvg}" = xyes; then
      AC_DEFINE(HAVE_LIBRSVG, 1, [Have librsvg support])
      AC_SUBST(include_svg, ["#include <egt/svgimage.h>"])
      LIBEGT_EXTRA_CXXFLAGS="${LIBRSVG_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
      LIBEGT_EXTRA_LDFLAGS="${LIBRSVG_LIBS} ${LIBEGT_EXTRA_LDFLAGS}"
   fi
])
AM_CONDITIONAL([HAVE_LIBRSVG], [test "x${have_librsvg}" = xyes])

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

SAVED_LDFLAGS="${LDFLAGS}"

AC_CHECK_LIB([stdc++fs], [main], [have_libstdcppfs=yes], [have_libstdcppfs=no])
AM_CONDITIONAL([HAVE_EXPERIMENTAL_FILESYSTEM], [test "x${have_libstdcppfs}" = xyes])
if test "x${have_libstdcppfs}" = xyes;
then
   LDFLAGS="${SAVED_LDFLAGS} -lstdc++fs"
   AC_DEFINE(HAVE_EXPERIMENTAL_FILESYSTEM, 1, [Have experimental filesystem support])
fi

if test "x${have_gstreamer}" = xyes; then
   AC_SUBST(include_audio, ["#include <egt/audio.h>"])
   AC_SUBST(include_video, ["#include <egt/video.h>"])
   AC_SUBST(include_camera, ["#include <egt/camera.h>"])
fi

AC_CHECK_HEADER([linux/fb.h], [have_fbdev=yes], [have_fbdev=no])
AM_CONDITIONAL([HAVE_FBDEV], [test "x${have_fbdev}" = xyes])
if test "x${have_fbdev}" = xyes; then
   AC_DEFINE(HAVE_FBDEV, 1, [Have fbdev support])
fi

AC_CHECK_HEADER([linux/input.h], [have_linux_input_h=yes], [have_linux_input_h=no])
AM_CONDITIONAL([HAVE_LINUX_INPUT_H], [test "x${have_linux_input_h}" = xyes])

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

AC_ARG_ENABLE([simd],
  [AS_HELP_STRING([--enable-simd], [build with simd support [default=no]])])
if test "x$enable_simd" = "xyes";
then
  AC_MSG_NOTICE([Enabling simd])
  (
    SIMD_CXXFLAGS=`echo "$CXXFLAGS" | sed -e 's/-W[-0-9a-z=]*//g'`
    SIMD_CXXFLAGS=`echo "$SIMD_CXXFLAGS" | sed -e 's/-pedantic//g'`
    SIMD_CXXFLAGS=`echo "$SIMD_CXXFLAGS" | sed -e 's/-flto//g'`
    SIMD_CXXFLAGS="$SIMD_CXXFLAGS -Wno-psabi"

    cd external/Simd/prj/cmake/ &&
    cmake . -DTOOLCHAIN="$CXX" -DTARGET="$host_cpu" \
    	  -DCMAKE_BUILD_TYPE="Release" -DLIBRARY="STATIC" \
	  -DSIMD_TEST=off -DCMAKE_CXX_FLAGS="${SIMD_CXXFLAGS}"
  )
  AC_DEFINE(HAVE_SIMD, 1, [Have simd support])
fi
AM_CONDITIONAL([HAVE_SIMD], [test "x${enable_simd}" = xyes])

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
echo
