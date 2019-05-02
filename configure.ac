AC_PREREQ([2.63])
AC_INIT([libegt],[0.3-alpha])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_SYSTEM
PKG_PROG_PKG_CONFIG

AM_INIT_AUTOMAKE([1.10 foreign subdir-objects])

# Enable quiet compiles on automake 1.11.
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

CXXFLAGS="$CXXFLAGS -std=c++11 -pedantic -g"
CFLAGS="$CFLAGS -g"

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

AC_USE_SYSTEM_EXTENSIONS

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
-Wsign-compare -Werror-implicit-function-declaration \
-Wpointer-arith -Wwrite-strings \
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
AC_CHECK_HEADERS([fcntl.h float.h inttypes.h libintl.h locale.h stdint.h stdlib.h string.h sys/ioctl.h sys/socket.h sys/time.h unistd.h])

AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([--disable-debug], [build without debugging support [default=yes]])],
  [enable_debug=no], [ ])

if test "$enable_debug" = "no" ; then
  AC_MSG_NOTICE([Disabling debugging])
  CFLAGS="$CFLAGS -DNDEBUG -Ofast"
  CXXFLAGS="$CXXFLAGS -DNDEBUG -Ofast"
else
  CXXFLAGS="$CXXFLAGS -O0"
  CFLAGS="$CFLAGS -O0"
fi

AC_ARG_ENABLE([examples],
  [AS_HELP_STRING([--disable-examples], [don't build examples [default=no]])],
  [enable_examples=no], [enable_examples=yes])
AM_CONDITIONAL([ENABLE_EXAMPLES], [test "x${enable_examples}" != xno])

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

AX_PKG_CHECK_MODULES2(X11, [x11 >= 1.6.3], [], [have_x11=yes], [have_x11=no])
AM_CONDITIONAL([HAVE_X11], [test "x${have_x11}" = xyes])
if test "x${have_x11}" = xyes; then
   AC_DEFINE(HAVE_X11, 1, [Have x11 support])
   LIBEGT_EXTRA_CXXFLAGS="${X11_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
   LIBEGT_EXTRA_LDFLAGS="${X11_LDFLAGS} ${LIBEGT_EXTRA_LDFLAGS}"
fi

AX_PKG_CHECK_MODULES2(TSLIB, [tslib >= 1.15], [], [have_tslib=yes], [have_tslib=no])
AM_CONDITIONAL([HAVE_TSLIB], [test "x${have_tslib}" = xyes])
if test "x${have_tslib}" = xyes; then
   AC_DEFINE(HAVE_TSLIB, 1, [Have tslib support])
   LIBEGT_EXTRA_CXXFLAGS="${TSLIB_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
   LIBEGT_EXTRA_LDFLAGS="${TSLIB_LDFLAGS} ${LIBEGT_EXTRA_LDFLAGS}"
fi

AX_PKG_CHECK_MODULES2(LIBINPUT, [libinput >= 1.6.3], [], [have_libinput=yes], [have_libinput=no])
AM_CONDITIONAL([HAVE_LIBINPUT], [test "x${have_libinput}" = xyes])
if test "x${have_libinput}" = xyes; then
   AC_DEFINE(HAVE_LIBINPUT, 1, [Have libinput support])
   LIBEGT_EXTRA_CXXFLAGS="${LIBINPUT_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
   LIBEGT_EXTRA_LDFLAGS="${LIBINPUT_LDFLAGS} ${LIBEGT_EXTRA_LDFLAGS}"
fi

AX_PKG_CHECK_MODULES2(GSTREAMER, [gstreamer-1.0 >= 1.12 gstreamer-plugins-base-1.0], [], [have_gstreamer=yes], [have_gstreamer=no])
if test "x${have_gstreamer}" = xyes; then
   AC_DEFINE(HAVE_GSTREAMER, 1, [Have gstreamer support])
   LIBEGT_EXTRA_CXXFLAGS="${GSTREAMER_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
   LIBEGT_EXTRA_LDFLAGS="${GSTREAMER_LDFLAGS} ${LIBEGT_EXTRA_LDFLAGS}"
fi
AM_CONDITIONAL([HAVE_GSTREAMER], [test "x${have_gstreamer}" = xyes])

AX_PKG_CHECK_MODULES2(LUA, [lua >= 5.3.0], [], [have_lua=yes], [have_lua=no])
if test "x${have_lua}" = xno; then
AX_PKG_CHECK_MODULES2(LUA, [lua5.3], [], [have_lua=yes], [have_lua=no])
fi
AM_CONDITIONAL([HAVE_LUA], [test "x${have_lua}" = xyes])
if test "x${have_lua}" = xyes; then
   AC_DEFINE(HAVE_LUA, 1, [Have lua support])
   LIBEGT_EXTRA_CXXFLAGS="${LUA_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
   LIBEGT_EXTRA_LDFLAGS="${LUA_LDFLAGS} ${LIBEGT_EXTRA_LDFLAGS}"
fi

# remove when libplanes is fixed
PKG_CHECK_MODULES(CJSON, [libcjson >= 1.6.0], [have_cjson=yes], [have_cjson=no])

AX_PKG_CHECK_MODULES2(LIBPLANES, [libplanes >= 1.0.0], [], [have_libplanes=yes], [have_libplanes=no])
AM_CONDITIONAL([HAVE_LIBPLANES], [test "x${have_libplanes}" = xyes])
if test "x${have_libplanes}" = xyes; then
   AX_PKG_CHECK_MODULES2(LIBDRM, [], [libdrm >= 2.4.0], [], [AC_MSG_ERROR([
     libdrm is a hard dependency, but a sufficient version was not found.])])
   AC_DEFINE(HAVE_LIBDRM, 1, [Have libdrm support])
   AC_DEFINE(HAVE_LIBPLANES, 1, [Have libplanes support])
   LIBEGT_EXTRA_CXXFLAGS="${LIBPLANES_CFLAGS} ${LIBDRM_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
   LIBEGT_EXTRA_LDFLAGS="${LIBPLANES_LDFLAGS} ${LIBDRM_LDFLAGS} ${LIBEGT_EXTRA_LDFLAGS}"
fi

AX_PKG_CHECK_MODULES2(CAIRO, [cairo >= 1.14.6], [], [], [AC_MSG_ERROR([
     cairo is a hard dependency, but a sufficient version was not found.])])
LIBEGT_EXTRA_CXXFLAGS="${CAIRO_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
LIBEGT_EXTRA_LDFLAGS="${CAIRO_LDFLAGS} ${LIBEGT_EXTRA_LDFLAGS}"

AX_PKG_CHECK_MODULES2(LIBJPEG, [libjpeg], [], [have_libjpeg=yes], [have_libjpeg=no])
AM_CONDITIONAL([HAVE_LIBJPEG], [test "x${have_libjpeg}" = xyes])
if test "x${have_libjpeg}" = xyes; then
   AC_DEFINE(HAVE_LIBJPEG, 1, [Have libjpeg support])
   LIBEGT_EXTRA_CXXFLAGS="${LIBJPEG_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
   LIBEGT_EXTRA_LDFLAGS="${LIBJPEG_LDFLAGS} ${LIBEGT_EXTRA_LDFLAGS}"
fi

AX_PKG_CHECK_MODULES2(LIBCURL, [libcurl >= 4.5], [], [have_libcurl=yes], [have_libcurl=no])
AM_CONDITIONAL([HAVE_LIBCURL], [test "x${have_libcurl}" = xyes])
if test "x${have_libcurl}" = xyes; then
   AC_DEFINE(HAVE_LIBCURL, 1, [Have libcurl support])
   AC_SUBST(include_http, ["#include <egt/network/http.h>"])
   LIBEGT_EXTRA_CXXFLAGS="${LIBCURL_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
   LIBEGT_EXTRA_LDFLAGS="${LIBCURL_LDFLAGS} ${LIBEGT_EXTRA_LDFLAGS}"
fi

AX_PKG_CHECK_MODULES2(LIBRSVG, [librsvg-2.0], [], [have_librsvg=yes], [have_librsvg=no])
AM_CONDITIONAL([HAVE_LIBRSVG], [test "x${have_librsvg}" = xyes])
if test "x${have_librsvg}" = xyes; then
   AC_DEFINE(HAVE_LIBRSVG, 1, [Have librsvg support])
   LIBEGT_EXTRA_CXXFLAGS="${LIBRSVG_CFLAGS} ${LIBEGT_EXTRA_CXXFLAGS}"
   LIBEGT_EXTRA_LDFLAGS="${LIBRSVG_LDFLAGS} ${LIBEGT_EXTRA_LDFLAGS}"
fi

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
   if test "x${have_libplanes}" = xyes; then
      AC_SUBST(include_video, ["#include <egt/video.h>"])
   fi
fi
AM_CONDITIONAL([HAVE_VIDEO_SUPPORT], [test "x${have_libplanes}" = xyes && test "x${have_gstreamer}" = xyes])

# only used by examples
PKG_CHECK_MODULES(JSONCPP, [jsoncpp], [have_jsoncpp=yes], [have_jsoncpp=no])
if test "x${have_jsoncpp}" = xyes; then
   AC_DEFINE(HAVE_JSONCPP, 1, [Have jsoncpp support])
fi
AM_CONDITIONAL([HAVE_JSONCPP], [test "x${have_jsoncpp}" = xyes])

AM_CONDITIONAL([HAVE_BOX2D], [test true])
AC_DEFINE(HAVE_BOX2D, 1, [Have Box2D support])

AM_CONDITIONAL([HAVE_KPLOT], [test true])
AC_DEFINE(HAVE_KPLOT, 1, [Have kplot support])

# Subst LIBEGT_EXTRA_CFLAGS & CXXFLAGS & LDFLAGS
AC_SUBST(LIBEGT_EXTRA_CFLAGS)
AC_SUBST(LIBEGT_EXTRA_CXXFLAGS)
AC_SUBST(LIBEGT_EXTRA_LDFLAGS)

AC_USE_SYSTEM_EXTENSIONS
AC_SYS_LARGEFILE
AC_FUNC_ALLOCA

# Initialize libtool
LT_PREREQ([2.2])
LT_INIT()

AC_PATH_PROG([ASTYLE_BIN], [astyle])
AM_CONDITIONAL([HAVE_ASTYLE_BIN], [test -n "$ASTYLE_BIN"])

AC_PATH_PROG([CLOC_BIN], [cloc])
AM_CONDITIONAL([HAVE_CLOC_BIN], [test -n "$CLOC_BIN"])

AC_PATH_PROG([CPPCHECK_BIN], [cppcheck])
AM_CONDITIONAL([HAVE_CPPCHECK_BIN], [test -n "$CPPCHECK_BIN"])

AC_PATH_PROG([DOXYGEN_BIN], [doxygen])
AM_CONDITIONAL([HAVE_DOXYGEN_BIN], [test -n "$DOXYGEN_BIN"])
AM_COND_IF([HAVE_DOXYGEN_BIN], [AC_CONFIG_FILES([docs/Doxyfile])])

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
	docs/Makefile])
AC_OUTPUT

dnl report how we have been configured
echo
echo "Configured EGT ${PACKAGE_VERSION} for \`${host}'"
echo
echo "Compile:"
echo "  Debug                  ${enable_debug:-yes}"
echo "  Examples               ${enable_examples}"
echo

echo "Display:"
echo "  DRM/KMS                ${have_libplanes:-no}"
echo "  X11                    ${have_x11:-no}"
echo "  fbdev                  yes"
echo

echo "Input:"
echo "  libinput               ${have_libinput:-no}"
echo "  tslib                  ${have_tslib:-no}"
echo "  evdev                  yes"
echo "  X11                    ${have_x11:-no}"
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
echo "  LUA                    ${have_lua:-no}"
echo "  C++17 Filesystem       ${have_libstdcppfs:-no}"
echo
