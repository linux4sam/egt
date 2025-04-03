cmake_minimum_required(VERSION 3.23)

project(
    libegt
    VERSION 1.11
    DESCRIPTION "A linux graphic library for Microchip MPUs"
    LANGUAGES C;CXX;ASM
)
set(PROJECT_VERSION_RC 2)
if (DEFINED PROJECT_VERSION_RC)
    string(APPEND PROJECT_VERSION "-rc${PROJECT_VERSION_RC}")
endif()

include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckCXXSymbolExists)
include(CheckSymbolExists)
include(GNUInstallDirs)

set(CMAKE_VERBOSE_MAKEFILE True)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-fvisibility=hidden)
add_compile_options(-fvisibility-inlines-hidden)
add_compile_options(-Wall -Wextra)
add_compile_options(-pedantic)
add_compile_options(-Wfloat-equal)
add_compile_options(-Wsign-compare)
add_compile_options(-Wpointer-arith)
add_compile_options(-Wwrite-strings)
add_compile_options(-Wmissing-declarations)
add_compile_options(-Wpacked)
add_compile_options(-Wno-variadic-macros)
add_compile_options(-Wstrict-aliasing)
add_compile_options(-Winit-self)
add_compile_options(-Woverloaded-virtual)
add_compile_options(-Wno-psabi)

add_compile_definitions($<$<CONFIG:Release>:NDEBUG>)

CHECK_INCLUDE_FILE_CXX(cxxabi.h HAVE_CXXABI_H)
CHECK_INCLUDE_FILE(glob.h HAVE_GLOB_H)
CHECK_INCLUDE_FILE(linux/input.h HAVE_LINUX_INPUT_H)
CHECK_INCLUDE_FILE(linux/gpio.h HAVE_LINUX_GPIO_H)
CHECK_INCLUDE_FILE(windows.h HAVE_WINDOWS_H)

check_cxx_symbol_exists(__cxa_demangle "cxxabi.h" HAVE_CXA_DEMANGLE)
check_symbol_exists(gettext "libintl.h" HAVE_LIBINTL)

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

find_package(PkgConfig REQUIRED)

pkg_check_modules(CAIRO REQUIRED cairo>=1.14.6)
set(AX_PACKAGE_REQUIRES_PRIVATE "cairo >= 1.14.6")

check_symbol_exists(CAIRO_HAS_FT_FONT "cairo/cairo-features.h" HAVE_FREETYPE)
if(HAVE_FREETYPE)
    set(LIBEGT_PRIVATE_LDFLAGS "-lfreetype ${LIBEGT_PRIVATE_LDFLAGS}")
else()
    message(FATAL_ERROR "cairo is not compiled with freetype support and it is required.")
endif()

option(WITH_FONTCONFIG "enable/disable fontconfig" ON)
if(WITH_FONTCONFIG)
    check_symbol_exists(CAIRO_HAS_FC_FONT "cairo/cairo-features.h" HAVE_FONTCONFIG)
    if(HAVE_FONTCONFIG)
        set(LIBEGT_PRIVATE_LDFLAGS "-lfontconfig ${LIBEGT_PRIVATE_LDFLAGS}")
    endif()
endif()

option(WITH_X11 "enable/disable x11" ON)
if(WITH_X11)
    pkg_check_modules(X11 x11>=1.6.3)
    if(X11_FOUND)
        set(AX_PACKAGE_REQUIRES_PRIVATE "${AX_PACKAGE_REQUIRES_PRIVATE} x11 >= 1.6.3")
    endif()
endif()

option(WITH_XKBCOMMON "enable/disable xkbcommon" ON)
if(WITH_XKBCOMMON)
    pkg_check_modules(XKBCOMMON xkbcommon)
    if(XKBCOMMON_FOUND)
        set(AX_PACKAGE_REQUIRES_PRIVATE "${AX_PACKAGE_REQUIRES_PRIVATE} xkbcommon")
    endif()
endif()

option(WITH_SDL2 "enable/disable sdl2" ON)
if(WITH_SDL2)
    pkg_check_modules(SDL2 sdl2)
    if(SDL2_FOUND)
        set(AX_PACKAGE_REQUIRES_PRIVATE "${AX_PACKAGE_REQUIRES_PRIVATE} sdl2")
    endif()
endif()

option(WITH_TSLIB "enable/disable tslib" ON)
if(WITH_TSLIB)
    pkg_check_modules(TSLIB tslib>=1.15)
    if(TSLIB_FOUND)
        set(AX_PACKAGE_REQUIRES_PRIVATE "${AX_PACKAGE_REQUIRES_PRIVATE} tslib >= 1.15")
    endif()
endif()

option(WITH_LIBINPUT "enable/disable libinput" ON)
if(WITH_LIBINPUT)
    pkg_check_modules(LIBINPUT libinput>=1.6.3)
    if(LIBINPUT_FOUND)
        set(AX_PACKAGE_REQUIRES_PRIVATE "${AX_PACKAGE_REQUIRES_PRIVATE} libinput >= 1.6.3")
    endif()
endif()

option(WITH_GSTREAMER "enable/disable gstreamer" ON)
if(WITH_GSTREAMER)
    pkg_check_modules(GSTREAMER gstreamer-1.0>=1.8)
    if(GSTREAMER_FOUND)
        set(AX_PACKAGE_REQUIRES_PRIVATE "${AX_PACKAGE_REQUIRES_PRIVATE} gstreamer-1.0 >= 1.8")

        pkg_check_modules(GSTREAMER_PLUGINS_BASE_DEV gstreamer-app-1.0>=1.8)
        if(GSTREAMER_PLUGINS_BASE_DEV_FOUND)
            set(AX_PACKAGE_REQUIRES_PRIVATE "${AX_PACKAGE_REQUIRES_PRIVATE} gstreamer-app-1.0 >= 1.8")
        endif()

        pkg_check_modules(GSTREAMER_PBUTILS gstreamer-pbutils-1.0>=1.8)
        if(GSTREAMER_PBUTILS_FOUND)
            set(AX_PACKAGE_REQUIRES_PRIVATE "${AX_PACKAGE_REQUIRES_PRIVATE} gstreamer-pbutils-1.0 >= 1.8")
        endif()
    endif()
endif()

option(WITH_LUA "enable/disable lua" ON)
if(WITH_LUA)
    pkg_search_module(LUA lua>=5.3.0 lua5.3)
    if(LUA_FOUND)
        set(AX_PACKAGE_REQUIRES_PRIVATE "${AX_PACKAGE_REQUIRES_PRIVATE} ${LUA_MODULE_NAME}")
    endif()
endif()

option(WITH_READLINE "enable/disable readline" ON)
if(WITH_READLINE)
    pkg_check_modules(READLINE readline)
    if(READLINE_FOUND)
        set(AX_PACKAGE_REQUIRES_PRIVATE "${AX_PACKAGE_REQUIRES_PRIVATE} readline")
    endif()
endif()

pkg_check_modules(LIBPLANES libplanes>=1.1.0)
if(LIBPLANES_FOUND)
    set(AX_PACKAGE_REQUIRES_PRIVATE "${AX_PACKAGE_REQUIRES_PRIVATE} libplanes >= 1.1.0")

    pkg_check_modules(LIBDRM REQUIRED libdrm>=2.4.0)
    if(LIBDRM_FOUND)
        set(AX_PACKAGE_REQUIRES_PRIVATE "${AX_PACKAGE_REQUIRES_PRIVATE} libdrm >= 2.4.0")
    endif()
endif()

pkg_check_modules(LIBM2D libm2d>=2.0.0)
if(LIBM2D_FOUND)
    set(AX_PACKAGE_REQUIRES_PRIVATE "${AX_PACKAGE_REQUIRES_PRIVATE} libm2d >= 2.0.0")
endif()

option(WITH_PLPLOT "enable/disable plplot" ON)
if(WITH_PLPLOT)
    pkg_check_modules(PLPLOT plplot-c++)
    if(PLPLOT_FOUND)
        set(AX_PACKAGE_REQUIRES_PRIVATE "${AX_PACKAGE_REQUIRES_PRIVATE} plplot-c++")
    endif()
endif()

option(WITH_LIBJPEG "enable/disable libjpeg" ON)
if(WITH_LIBJPEG)
    pkg_check_modules(LIBJPEG libjpeg)
    if(LIBJPEG_FOUND)
        set(AX_PACKAGE_REQUIRES_PRIVATE "${AX_PACKAGE_REQUIRES_PRIVATE} libjpeg")
    endif()
endif()

option(WITH_LIBCURL "enable/disable libcurl" ON)
if(WITH_LIBCURL)
    pkg_check_modules(LIBCURL libcurl>=4.5)
    if(LIBCURL_FOUND)
        set(AX_PACKAGE_REQUIRES_PRIVATE "${AX_PACKAGE_REQUIRES_PRIVATE} libcurl >= 4.5")
    endif()
endif()

option(WITH_LIBRSVG "enable/disable librsvg" ON)
if(WITH_LIBRSVG)
    pkg_check_modules(LIBRSVG librsvg-2.0)
    if(LIBRSVG_FOUND)
        set(AX_PACKAGE_REQUIRES_PRIVATE "${AX_PACKAGE_REQUIRES_PRIVATE} librsvg-2.0")
    endif()
endif()

option(WITH_ZLIB "enable/disable zlib" ON)
if(WITH_ZLIB)
    pkg_check_modules(ZLIB zlib)
    if(ZLIB_FOUND)
        set(AX_PACKAGE_REQUIRES_PRIVATE "${AX_PACKAGE_REQUIRES_PRIVATE} zlib")
    endif()
endif()

option(WITH_LIBMAGIC "enable/disable libmagic" ON)
if(WITH_LIBMAGIC)
    pkg_check_modules(LIBMAGIC libmagic)
    if(LIBMAGIC_FOUND)
        set(AX_PACKAGE_REQUIRES_PRIVATE "${AX_PACKAGE_REQUIRES_PRIVATE} libmagic")
    endif()
endif()

option(WITH_ALSA "enable/disable alsa" ON)
if(WITH_ALSA)
    pkg_check_modules(ALSA alsa)
    if(ALSA_FOUND)
        set(AX_PACKAGE_REQUIRES_PRIVATE "${AX_PACKAGE_REQUIRES_PRIVATE} alsa")
    endif()
endif()

option(WITH_SNDFILE "enable/disable sndfile" ON)
if(WITH_SNDFILE)
    pkg_check_modules(SNDFILE sndfile)
    if(SNDFILE_FOUND)
        set(AX_PACKAGE_REQUIRES_PRIVATE "${AX_PACKAGE_REQUIRES_PRIVATE} sndfile")
    endif()
endif()

option(VIRTUALKEYBOARD "include support for virtualkeyboard [default=ON]" ON)

option(ENABLE_ICONS "install icons [default=ON]" ON)

option(ENABLE_EXAMPLES "build examples [default=ON]" ON)

option(ENABLE_LUA_BINDINGS "enable/disable lua bindings [default=OFF]" OFF)

option(ENABLE_SNIPPETS "build snippets examples [default=OFF]" OFF)

option(ENABLE_UNITTESTS "build unit tests [default=OFF]" OFF)

option(ENABLE_SVGDESERIAL "build svgdeserial functionality [default=OFF]" OFF)

option(ENABLE_SIMD "build with simd support [default=OFF]" OFF)

find_program(ASTYLE astyle)
if(ASTYLE)
    add_custom_target(style
                      ASTYLE_BIN=${ASTYLE} scripts/style.sh --recursive \"examples/\\\*.cpp\" \"include/egt/\\\*.h\" \"src/\\\*.c\" \"src/\\\*.cpp\" \"src/\\\*.h\" \"test/\\\*.cpp\" --exclude=images/jpeg -i
                      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

set(PACKAGE_NAME libegt)
set(PACKAGE_VERSION ${PROJECT_VERSION})

set(top_srcdir ${CMAKE_SOURCE_DIR})
set(top_builddir ${CMAKE_BINARY_DIR})

set(libs_private "$ENV{LDFLAGS} ${LIBEGT_PRIVATE_LDFLAGS}")

if(LUA_FOUND AND ENABLE_LUA_BINDINGS)
    add_subdirectory(lua)
endif()

add_subdirectory(src)

if(ENABLE_EXAMPLES)
    add_subdirectory(examples)
endif()

find_program(DOXYGEN doxygen)
if(DOXYGEN)
    add_subdirectory(docs)
endif()

if(ENABLE_UNITTESTS)
    add_subdirectory(test)
endif()

if(ENABLE_ICONS)
    install(DIRECTORY icons
            DESTINATION ${CMAKE_INSTALL_DATADIR}/libegt
            PATTERN Makefile.icons EXCLUDE)
endif()
