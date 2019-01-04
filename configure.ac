AC_PREREQ([2.63])
AC_INIT([egt],[0.1])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_SYSTEM
PKG_PROG_PKG_CONFIG

AM_INIT_AUTOMAKE([1.10 foreign subdir-objects])

# Enable quiet compiles on automake 1.11.
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

CXXFLAGS="$CXXFLAGS -std=c++11 -pedantic -Ofast -g"
CFLAGS="$CFLAGS -Ofast -g"

AC_DISABLE_STATIC
AC_PROG_CXX
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

dnl ===========================================================================
dnl check compiler flags
AC_DEFUN([LIBDRM_CC_TRY_FLAG], [
  AC_MSG_CHECKING([whether $CC supports $1])

  libdrm_save_CFLAGS="$CFLAGS"
  CFLAGS="$CFLAGS $1"

  AC_COMPILE_IFELSE([AC_LANG_SOURCE([ ])], [libdrm_cc_flag=yes], [libdrm_cc_flag=no])
  CFLAGS="$libdrm_save_CFLAGS"

  if test "x$libdrm_cc_flag" = "xyes"; then
    ifelse([$2], , :, [$2])
  else
    ifelse([$3], , :, [$3])
  fi
  AC_MSG_RESULT([$libdrm_cc_flag])
])

MAYBE_WARN="-Wall -Wextra \
-Wsign-compare -Werror-implicit-function-declaration \
-Wpointer-arith -Wwrite-strings \
-Wmissing-declarations \
-Wpacked \
-Wstrict-aliasing=2 -Winit-self \
-Wno-attributes -Wno-long-long \
-Wno-missing-field-initializers"

#-Wno-unused-parameter
#-Winline
#-Wswitch-enum
#-Wshadow

# invalidate cached value if MAYBE_WARN has changed
if test "x$libdrm_cv_warn_maybe" != "x$MAYBE_WARN"; then
	unset libdrm_cv_warn_cflags
fi
AC_CACHE_CHECK([for supported warning flags], libdrm_cv_warn_cflags, [
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
		LIBDRM_CC_TRY_FLAG([$W], [WARN_CFLAGS="$WARN_CFLAGS $W"])
	done

	libdrm_cv_warn_cflags=$WARN_CFLAGS
	libdrm_cv_warn_maybe=$MAYBE_WARN

	AC_MSG_CHECKING([which warning flags were supported])])
WARN_CFLAGS="$libdrm_cv_warn_cflags"
AC_SUBST(WARN_CFLAGS)

# Checks for header files.
AC_PATH_X
AC_CHECK_HEADERS([fcntl.h float.h inttypes.h libintl.h locale.h stdint.h stdlib.h string.h sys/ioctl.h sys/socket.h sys/time.h unistd.h])

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

PKG_CHECK_MODULES(X11, [x11 >= 1.6.3], [have_x11=yes], [have_x11=no])
AM_CONDITIONAL([HAVE_X11], [test "x${have_x11}" = xyes])
if test "x${have_x11}" = xyes; then
   AC_DEFINE(HAVE_X11, 1, [Have x11 support])
fi

AX_PKG_CHECK_MODULES(TSLIB, [], [tslib >= 1.15], [have_tslib=yes], [have_tslib=no])
if test "x${have_tslib}" = xyes; then
   AC_DEFINE(HAVE_TSLIB, 1, [Have tslib support])
fi

AX_PKG_CHECK_MODULES(LIBINPUT, [], [libinput >= 1.6.3], [have_libinput=yes], [have_libinput=no])
if test "x${have_libinput}" = xyes; then
   AC_DEFINE(HAVE_LIBINPUT, 1, [Have libinput support])
fi

AX_PKG_CHECK_MODULES(GSTREAMER, [], [gstreamer-1.0 >= 1.12], [have_gstreamer=yes], [have_gstreamer=no])
if test "x${have_gstreamer}" = xyes; then
   AC_DEFINE(HAVE_GSTREAMER, 1, [Have gstreamer support])
fi
AM_CONDITIONAL([HAVE_GSTREAMER], [test "x${have_gstreamer}" = xyes])

AX_PKG_CHECK_MODULES(LIBDRM, [], [libdrm >= 2.4.0], [], [AC_MSG_ERROR([
     libdrm is a hard dependency, but a sufficient version was not found.])])

AX_PKG_CHECK_MODULES(LUA, [], [lua >= 5.3.0], [have_lua=yes], [have_lua=no])
if test "x${have_lua}" = xno; then
AX_PKG_CHECK_MODULES(LUA, [], [lua5.3], [have_lua=yes], [have_lua=no])
fi
AM_CONDITIONAL([HAVE_LUA], [test "x${have_lua}" = xyes])
if test "x${have_lua}" = xyes; then
   AC_DEFINE(HAVE_LUA, 1, [Have lua support])
fi

# remove when libplanes is fixed
PKG_CHECK_MODULES(CJSON, [libcjson >= 1.6.0], [have_cjson=yes], [have_cjson=no])

AX_PKG_CHECK_MODULES(LIBPLANES, [], [libplanes >= 1.0.0], [have_libplanes=yes], [have_libplanes=no])
AM_CONDITIONAL([HAVE_LIBPLANES], [test "x${have_libplanes}" = xyes])
if test "x${have_libplanes}" = xyes; then
   AC_DEFINE(HAVE_LIBPLANES, 1, [Have libplanes support])
fi

AX_PKG_CHECK_MODULES(CAIRO, [], [cairo >= 1.14.6], [], [AC_MSG_ERROR([
     cairo is a hard dependency, but a sufficient version was not found.])])

AX_PKG_CHECK_MODULES(LIBJPEG, [], [libjpeg >= 0.28], [have_libjpeg=yes], [have_libjpeg=no])
AM_CONDITIONAL([HAVE_LIBJPEG], [test "x${have_libjpeg}" = xyes])
if test "x${have_libjpeg}" = xyes; then
   AC_DEFINE(HAVE_LIBJPEG, 1, [Have libjpeg support])
fi

AC_SEARCH_LIBS([magic_open], [magic], [MAGIC_LIB=-lmagic], [
  AC_MSG_ERROR([required library libmagic not found])
])

AC_CHECK_HEADER([magic.h],
  [AC_DEFINE(HAVE_LIBMAGIC, 1, [Have libmagic support])],
  [AC_MSG_ERROR([required header magic.h not found])
])

AC_LANG_PUSH([C++])
AC_CHECK_HEADERS([rapidxml/rapidxml.hpp], [have_rapidxml=yes], [have_rapidxml=no])
AM_CONDITIONAL([HAVE_RAPIDXML_RAPIDXML_HPP], [test "x${have_rapidxml}" = xyes])
AC_CHECK_HEADERS([rapidxml.hpp], [have_rapidxml=yes], [have_rapidxml=no])
AM_CONDITIONAL([HAVE_RAPIDXML_HPP], [test "x${have_rapidxml}" = xyes])
AC_LANG_POP([C++])

AM_CONDITIONAL([HAVE_KPLOT], [test true])
AC_DEFINE(HAVE_KPLOT, 1, [Have kplot support])

AC_USE_SYSTEM_EXTENSIONS
AC_SYS_LARGEFILE
AC_FUNC_ALLOCA

# Initialize libtool
LT_PREREQ([2.2])
LT_INIT()

AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN";
   then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi

AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])
AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([docs/Doxyfile])])

AC_CONFIG_FILES([Makefile
	src/Makefile
	external/Makefile
	examples/Makefile
	scripts/Makefile
	libegt.pc
	include/egt/version.h
	examples/video/Makefile])
AC_OUTPUT
