add_library(egt SHARED
    animation.cpp
    app.cpp
    button.cpp
    buttongroup.cpp
    canvas.cpp
    checkbox.cpp
    color.cpp
    combo.cpp
    detail/alignment.cpp
    detail/base64.cpp
    detail/collision.cpp
    detail/egtlog.cpp
    detail/eraw.cpp
    detail/filesystem.cpp
    detail/image.cpp
    detail/imagecache.cpp
    detail/input/inputkeyboard.cpp
    detail/layout.cpp
    detail/mousegesture.cpp
    detail/screen/composerscreen.cpp
    detail/screen/memoryscreen.cpp
    detail/string.cpp
    detail/utf8text.cpp
    detail/window/basicwindow.cpp
    detail/window/windowimpl.cpp
    dialog.cpp
    easing.cpp
    event.cpp
    eventloop.cpp
    filedialog.cpp
    font.cpp
    form.cpp
    frame.cpp
    gauge.cpp
    geometry.cpp
    grid.cpp
    image.cpp
    imagegroup.cpp
    images/bmp/cairo_bmp.c
    input.cpp
    keycode.cpp
    label.cpp
    list.cpp
    notebook.cpp
    object.cpp
    painter.cpp
    palette.cpp
    pattern.cpp
    progressbar.cpp
    radial.cpp
    radiobox.cpp
    resource.cpp
    respath.cpp
    screen.cpp
    script.cpp
    scrollwheel.cpp
    serialize.cpp
    shapes.cpp
    sideboard.cpp
    sizer.cpp
    slider.cpp
    sprite.cpp
    text.cpp
    textwidget.cpp
    theme.cpp
    themes/midnight.cpp
    themes/shamrock.cpp
    themes/sky.cpp
    timer.cpp
    tools.cpp
    types.cpp
    uiloader.cpp
    uri.cpp
    utils.cpp
    view.cpp
    widget.cpp
    widgetflags.cpp
    window.cpp
)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^arm.*")
    target_sources(egt PRIVATE detail/memset32.S)
endif()

set_target_properties(egt PROPERTIES VERSION 7.1.0 SOVERSION 7)

target_sources(egt PUBLIC FILE_SET HEADERS
    BASE_DIRS ${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/include
    FILES
    ${CMAKE_BINARY_DIR}/include/egt/ui
    ${CMAKE_BINARY_DIR}/include/egt/version.h
    ${CMAKE_SOURCE_DIR}/include/egt/animation.h
    ${CMAKE_SOURCE_DIR}/include/egt/app.h
    ${CMAKE_SOURCE_DIR}/include/egt/bitfields.h
    ${CMAKE_SOURCE_DIR}/include/egt/button.h
    ${CMAKE_SOURCE_DIR}/include/egt/buttongroup.h
    ${CMAKE_SOURCE_DIR}/include/egt/canvas.h
    ${CMAKE_SOURCE_DIR}/include/egt/checkbox.h
    ${CMAKE_SOURCE_DIR}/include/egt/color.h
    ${CMAKE_SOURCE_DIR}/include/egt/combo.h
    ${CMAKE_SOURCE_DIR}/include/egt/detail/alignment.h
    ${CMAKE_SOURCE_DIR}/include/egt/detail/collision.h
    ${CMAKE_SOURCE_DIR}/include/egt/detail/cow.h
    ${CMAKE_SOURCE_DIR}/include/egt/detail/enum.h
    ${CMAKE_SOURCE_DIR}/include/egt/detail/filesystem.h
    ${CMAKE_SOURCE_DIR}/include/egt/detail/image.h
    ${CMAKE_SOURCE_DIR}/include/egt/detail/imagecache.h
    ${CMAKE_SOURCE_DIR}/include/egt/detail/incbin.h
    ${CMAKE_SOURCE_DIR}/include/egt/detail/layout.h
    ${CMAKE_SOURCE_DIR}/include/egt/detail/math.h
    ${CMAKE_SOURCE_DIR}/include/egt/detail/meta.h
    ${CMAKE_SOURCE_DIR}/include/egt/detail/mousegesture.h
    ${CMAKE_SOURCE_DIR}/include/egt/detail/range.h
    ${CMAKE_SOURCE_DIR}/include/egt/detail/screen/composerscreen.h
    ${CMAKE_SOURCE_DIR}/include/egt/detail/screen/memoryscreen.h
    ${CMAKE_SOURCE_DIR}/include/egt/detail/string.h
    ${CMAKE_SOURCE_DIR}/include/egt/detail/stringhash.h
    ${CMAKE_SOURCE_DIR}/include/egt/dialog.h
    ${CMAKE_SOURCE_DIR}/include/egt/easing.h
    ${CMAKE_SOURCE_DIR}/include/egt/embed.h
    ${CMAKE_SOURCE_DIR}/include/egt/event.h
    ${CMAKE_SOURCE_DIR}/include/egt/eventloop.h
    ${CMAKE_SOURCE_DIR}/include/egt/filedialog.h
    ${CMAKE_SOURCE_DIR}/include/egt/fixedvector.h
    ${CMAKE_SOURCE_DIR}/include/egt/flags.h
    ${CMAKE_SOURCE_DIR}/include/egt/flagsbase.h
    ${CMAKE_SOURCE_DIR}/include/egt/font.h
    ${CMAKE_SOURCE_DIR}/include/egt/form.h
    ${CMAKE_SOURCE_DIR}/include/egt/frame.h
    ${CMAKE_SOURCE_DIR}/include/egt/gauge.h
    ${CMAKE_SOURCE_DIR}/include/egt/geometry.h
    ${CMAKE_SOURCE_DIR}/include/egt/grid.h
    ${CMAKE_SOURCE_DIR}/include/egt/image.h
    ${CMAKE_SOURCE_DIR}/include/egt/input.h
    ${CMAKE_SOURCE_DIR}/include/egt/imagegroup.h
    ${CMAKE_SOURCE_DIR}/include/egt/imageholder.h
    ${CMAKE_SOURCE_DIR}/include/egt/keycode.h
    ${CMAKE_SOURCE_DIR}/include/egt/label.h
    ${CMAKE_SOURCE_DIR}/include/egt/list.h
    ${CMAKE_SOURCE_DIR}/include/egt/notebook.h
    ${CMAKE_SOURCE_DIR}/include/egt/object.h
    ${CMAKE_SOURCE_DIR}/include/egt/painter.h
    ${CMAKE_SOURCE_DIR}/include/egt/palette.h
    ${CMAKE_SOURCE_DIR}/include/egt/pattern.h
    ${CMAKE_SOURCE_DIR}/include/egt/popup.h
    ${CMAKE_SOURCE_DIR}/include/egt/progressbar.h
    ${CMAKE_SOURCE_DIR}/include/egt/radial.h
    ${CMAKE_SOURCE_DIR}/include/egt/radiobox.h
    ${CMAKE_SOURCE_DIR}/include/egt/resource.h
    ${CMAKE_SOURCE_DIR}/include/egt/respath.h
    ${CMAKE_SOURCE_DIR}/include/egt/screen.h
    ${CMAKE_SOURCE_DIR}/include/egt/script.h
    ${CMAKE_SOURCE_DIR}/include/egt/scrollwheel.h
    ${CMAKE_SOURCE_DIR}/include/egt/serialize.h
    ${CMAKE_SOURCE_DIR}/include/egt/shapes.h
    ${CMAKE_SOURCE_DIR}/include/egt/sideboard.h
    ${CMAKE_SOURCE_DIR}/include/egt/signal.h
    ${CMAKE_SOURCE_DIR}/include/egt/sizer.h
    ${CMAKE_SOURCE_DIR}/include/egt/slider.h
    ${CMAKE_SOURCE_DIR}/include/egt/sprite.h
    ${CMAKE_SOURCE_DIR}/include/egt/string.h
    ${CMAKE_SOURCE_DIR}/include/egt/text.h
    ${CMAKE_SOURCE_DIR}/include/egt/textwidget.h
    ${CMAKE_SOURCE_DIR}/include/egt/theme.h
    ${CMAKE_SOURCE_DIR}/include/egt/themes/coconut.h
    ${CMAKE_SOURCE_DIR}/include/egt/themes/lapis.h
    ${CMAKE_SOURCE_DIR}/include/egt/themes/midnight.h
    ${CMAKE_SOURCE_DIR}/include/egt/themes/shamrock.h
    ${CMAKE_SOURCE_DIR}/include/egt/themes/sky.h
    ${CMAKE_SOURCE_DIR}/include/egt/themes/ultraviolet.h
    ${CMAKE_SOURCE_DIR}/include/egt/timer.h
    ${CMAKE_SOURCE_DIR}/include/egt/tools.h
    ${CMAKE_SOURCE_DIR}/include/egt/types.h
    ${CMAKE_SOURCE_DIR}/include/egt/uiloader.h
    ${CMAKE_SOURCE_DIR}/include/egt/uri.h
    ${CMAKE_SOURCE_DIR}/include/egt/utils.h
    ${CMAKE_SOURCE_DIR}/include/egt/value.h
    ${CMAKE_SOURCE_DIR}/include/egt/valuewidget.h
    ${CMAKE_SOURCE_DIR}/include/egt/view.h
    ${CMAKE_SOURCE_DIR}/include/egt/widget.h
    ${CMAKE_SOURCE_DIR}/include/egt/widgetflags.h
    ${CMAKE_SOURCE_DIR}/include/egt/window.h
)

include(asio.cmake)

add_custom_target(generate_gitversion_h
    COMMAND ${CMAKE_COMMAND} -DTOP_SOURCE_DIR=${CMAKE_SOURCE_DIR} -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -P ${CMAKE_CURRENT_SOURCE_DIR}/gitversion.cmake
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_dependencies(egt generate_gitversion_h)

target_compile_definitions(egt PRIVATE DATAPATH="${CMAKE_INSTALL_FULL_DATADIR}")
target_compile_definitions(egt PRIVATE SRCDIR="${CMAKE_SOURCE_DIR}")
target_compile_definitions(egt PRIVATE EGT_DLL_EXPORTS)
target_compile_definitions(egt PRIVATE FMT_HEADER_ONLY)
target_compile_definitions(egt PRIVATE $<$<CONFIG:Debug>:EGTLOG_ACTIVE_LEVEL=0>)

target_include_directories(egt PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/include
)

target_include_directories(egt SYSTEM PRIVATE
    ${CMAKE_SOURCE_DIR}/external/rapidxml
    ${CMAKE_SOURCE_DIR}/external/utfcpp/source
    ${CMAKE_SOURCE_DIR}/external/layout
    ${CMAKE_SOURCE_DIR}/external/fmt/include
)

target_include_directories(egt PUBLIC ${CAIRO_INCLUDE_DIRS})
target_compile_options(egt PUBLIC ${CAIRO_CFLAGS_OTHER})
target_link_directories(egt PUBLIC ${CAIRO_LIBRARY_DIRS})
target_link_libraries(egt PUBLIC ${CAIRO_LIBRARIES})
target_link_options(egt PUBLIC ${CAIRO_LDFLAGS_OTHER})

if(X11_FOUND)
    set(HAVE_X11 1)

    target_include_directories(egt PRIVATE ${X11_INCLUDE_DIRS})
    target_compile_options(egt PRIVATE ${X11_CFLAGS_OTHER})
    target_link_directories(egt PRIVATE ${X11_LIBRARY_DIRS})
    target_link_libraries(egt PRIVATE ${X11_LIBRARIES})
    target_link_options(egt PRIVATE ${X11_LDFLAGS_OTHER})
    target_sources(egt PRIVATE
        detail/screen/x11screen.cpp
        detail/screen/keyboard_code_conversion_x.cpp
    )
    target_sources(egt PUBLIC FILE_SET HEADERS FILES ${CMAKE_SOURCE_DIR}/include/egt/detail/screen/x11screen.h)
endif()

if(XKBCOMMON_FOUND)
    set(HAVE_XKBCOMMON 1)

    target_include_directories(egt PRIVATE ${XKBCOMMON_INCLUDE_DIRS})
    target_compile_options(egt PRIVATE ${XKBCOMMON_CFLAGS_OTHER})
    target_link_directories(egt PRIVATE ${XKBCOMMON_LIBRARY_DIRS})
    target_link_libraries(egt PRIVATE ${XKBCOMMON_LIBRARIES})
    target_link_options(egt PRIVATE ${XKBCOMMON_LDFLAGS_OTHER})
endif()

if(SDL2_FOUND)
    set(HAVE_SDL2 1)

    target_include_directories(egt PRIVATE ${SDL2_INCLUDE_DIRS})
    target_compile_options(egt PRIVATE ${SDL2_CFLAGS_OTHER})
    target_link_directories(egt PRIVATE ${SDL2_LIBRARY_DIRS})
    target_link_libraries(egt PRIVATE ${SDL2_LIBRARIES})
    target_link_options(egt PRIVATE ${SDL2_LDFLAGS_OTHER})
    target_sources(egt PRIVATE detail/screen/sdlscreen.cpp)
    target_sources(egt PUBLIC FILE_SET HEADERS FILES ${CMAKE_SOURCE_DIR}/include/egt/detail/screen/sdlscreen.h)
endif()

if(TSLIB_FOUND)
    set(HAVE_TSLIB 1)

    target_include_directories(egt PRIVATE ${TSLIB_INCLUDE_DIRS})
    target_compile_options(egt PRIVATE ${TSLIB_CFLAGS_OTHER})
    target_link_directories(egt PRIVATE ${TSLIB_LIBRARY_DIRS})
    target_link_libraries(egt PRIVATE ${TSLIB_LIBRARIES})
    target_link_options(egt PRIVATE ${TSLIB_LDFLAGS_OTHER})
    target_sources(egt PRIVATE detail/input/inputtslib.cpp)
    target_sources(egt PUBLIC FILE_SET HEADERS FILES ${CMAKE_SOURCE_DIR}/include/egt/detail/input/inputtslib.h)
endif()

if(LIBINPUT_FOUND)
    set(HAVE_LIBINPUT 1)

    target_include_directories(egt PRIVATE ${LIBINPUT_INCLUDE_DIRS})
    target_compile_options(egt PRIVATE ${LIBINPUT_CFLAGS_OTHER})
    target_link_directories(egt PRIVATE ${LIBINPUT_LIBRARY_DIRS})
    target_link_libraries(egt PRIVATE ${LIBINPUT_LIBRARIES})
    target_link_options(egt PRIVATE ${LIBINPUT_LDFLAGS_OTHER})
    target_sources(egt PRIVATE detail/input/inputlibinput.cpp)
    target_sources(egt PUBLIC FILE_SET HEADERS FILES ${CMAKE_SOURCE_DIR}/include/egt/detail/input/inputlibinput.h)
endif()

if(GSTREAMER_PLUGINS_BASE_DEV_FOUND)
    set(HAVE_GSTREAMER 1)
    set(HAVE_GSTREAMER_PBDEV 1)
    set(include_audio "#define EGT_HAS_AUDIO 1")
    set(include_video "#define EGT_HAS_VIDEO 1")
    set(include_camera "#define EGT_HAS_CAMERA 1")
    set(include_capture "#define EGT_HAS_CAPTURE 1")

    target_include_directories(egt PRIVATE ${GSTREAMER_INCLUDE_DIRS})
    target_compile_options(egt PRIVATE ${GSTREAMER_CFLAGS_OTHER})
    target_link_directories(egt PRIVATE ${GSTREAMER_LIBRARY_DIRS})
    target_link_libraries(egt PRIVATE ${GSTREAMER_LIBRARIES})
    target_link_options(egt PRIVATE ${GSTREAMER_LDFLAGS_OTHER})

    target_include_directories(egt PRIVATE ${GSTREAMER_PLUGINS_BASE_DEV_INCLUDE_DIRS})
    target_compile_options(egt PRIVATE ${GSTREAMER_PLUGINS_BASE_DEV_CFLAGS_OTHER})
    target_link_directories(egt PRIVATE ${GSTREAMER_PLUGINS_BASE_DEV_LIBRARY_DIRS})
    target_link_libraries(egt PRIVATE ${GSTREAMER_PLUGINS_BASE_DEV_LIBRARIES})
    target_link_options(egt PRIVATE ${GSTREAMER_PLUGINS_BASE_DEV_LDFLAGS_OTHER})

    if(GSTREAMER_PBUTILS_FOUND)
        set(HAVE_GSTREAMER_PBUTILS 1)

        target_include_directories(egt PRIVATE ${GSTREAMER_PBUTILS_INCLUDE_DIRS})
        target_compile_options(egt PRIVATE ${GSTREAMER_PBUTILS_CFLAGS_OTHER})
        target_link_directories(egt PRIVATE ${GSTREAMER_PBUTILS_LIBRARY_DIRS})
        target_link_libraries(egt PRIVATE ${GSTREAMER_PBUTILS_LIBRARIES})
        target_link_options(egt PRIVATE ${GSTREAMER_PBUTILS_LDFLAGS_OTHER})
    endif()

    target_sources(egt PRIVATE
        audio.cpp
        video.cpp
        detail/video/gstappsinkimpl.cpp
        detail/video/gstdecoderimpl.cpp
        camera.cpp
        detail/camera/gstcameraimpl.cpp
        capture.cpp
        detail/camera/gstcaptureimpl.cpp
        detail/video/gstmeta.cpp
    )
    target_sources(egt PUBLIC FILE_SET HEADERS FILES
        ${CMAKE_SOURCE_DIR}/include/egt/audio.h
        ${CMAKE_SOURCE_DIR}/include/egt/video.h
        ${CMAKE_SOURCE_DIR}/include/egt/camera.h
        ${CMAKE_SOURCE_DIR}/include/egt/capture.h
    )

    if(LIBPLANES_FOUND)
        target_sources(egt PRIVATE detail/video/gstkmssinkimpl.cpp)
    endif()
endif()

if(HAVE_LINUX_INPUT_H)
    target_sources(egt PRIVATE detail/input/inputevdev.cpp)
    target_sources(egt PUBLIC FILE_SET HEADERS FILES ${CMAKE_SOURCE_DIR}/include/egt/detail/input/inputevdev.h)
endif()

if(LUA_FOUND)
    set(HAVE_LUA 1)

    target_include_directories(egt PRIVATE ${LUA_INCLUDE_DIRS})
    target_compile_options(egt PRIVATE ${LUA_CFLAGS_OTHER})
    target_link_directories(egt PRIVATE ${LUA_LIBRARY_DIRS})
    target_link_libraries(egt PRIVATE ${LUA_LIBRARIES})
    target_link_options(egt PRIVATE ${LUA_LDFLAGS_OTHER})
    target_sources(egt PRIVATE detail/lua/script.c)

    if(ENABLE_LUA_BINDINGS)
        set(include_luaapp "#define EGT_HAS_LUA_BINDINGS 1")
        target_sources(egt PRIVATE luaapp.cpp)
        target_sources(egt PUBLIC FILE_SET HEADERS FILES ${CMAKE_SOURCE_DIR}/include/egt/luaapp.h)
        target_link_libraries(egt PRIVATE egtlua)
    endif()
endif()

if(READLINE_FOUND)
    target_compile_definitions(egt PRIVATE LUA_USE_READLINE)
    target_include_directories(egt PRIVATE ${READLINE_INCLUDE_DIRS})
    target_compile_options(egt PRIVATE ${READLINE_CFLAGS_OTHER})
    target_link_directories(egt PRIVATE ${READLINE_LIBRARY_DIRS})
    target_link_libraries(egt PRIVATE ${READLINE_LIBRARIES})
    target_link_options(egt PRIVATE ${READLINE_LDFLAGS_OTHER})
endif()

if(LIBPLANES_FOUND)
    set(HAVE_LIBPLANES 1)

    target_include_directories(egt PRIVATE ${LIBPLANES_INCLUDE_DIRS})
    target_compile_options(egt PRIVATE ${LIBPLANES_CFLAGS_OTHER})
    target_link_directories(egt PRIVATE ${LIBPLANES_LIBRARY_DIRS})
    target_link_libraries(egt PRIVATE ${LIBPLANES_LIBRARIES})
    target_link_options(egt PRIVATE ${LIBPLANES_LDFLAGS_OTHER})
    target_sources(egt PRIVATE
        detail/screen/kmsoverlay.cpp
        detail/screen/kmsscreen.cpp
        detail/window/planewindow.cpp
    )
    target_sources(egt PUBLIC FILE_SET HEADERS FILES
        ${CMAKE_SOURCE_DIR}/include/egt/detail/screen/kmsoverlay.h
        ${CMAKE_SOURCE_DIR}/include/egt/detail/screen/kmsscreen.h
        ${CMAKE_SOURCE_DIR}/include/egt/detail/screen/kmstype.h
    )
endif()

if(LIBDRM_FOUND)
    set(HAVE_LIBDRM 1)

    target_include_directories(egt PRIVATE ${LIBDRM_INCLUDE_DIRS})
    target_compile_options(egt PRIVATE ${LIBDRM_CFLAGS_OTHER})
    target_link_directories(egt PRIVATE ${LIBDRM_LIBRARY_DIRS})
    target_link_libraries(egt PRIVATE ${LIBDRM_LIBRARIES})
    target_link_options(egt PRIVATE ${LIBDRM_LDFLAGS_OTHER})
endif()

if(PLPLOT_FOUND)
    check_symbol_exists(PLD_extcairo "plplot/plDevs.h" HAVE_EXTCAIRO)

    set(HAVE_PLPLOT 1)
    set(include_chart "#define EGT_HAS_CHART 1")

    target_include_directories(egt PRIVATE ${PLPLOT_INCLUDE_DIRS})
    target_compile_options(egt PRIVATE ${PLPLOT_CFLAGS_OTHER})
    target_link_directories(egt PRIVATE ${PLPLOT_LIBRARY_DIRS})
    target_link_libraries(egt PRIVATE ${PLPLOT_LIBRARIES})
    target_link_options(egt PRIVATE ${PLPLOT_LDFLAGS_OTHER})
    target_sources(egt PRIVATE
        chart.cpp
        detail/charts/plplotimpl.cpp
    )
    target_sources(egt PUBLIC FILE_SET HEADERS FILES ${CMAKE_SOURCE_DIR}/include/egt/chart.h)
endif()

if(LIBJPEG_FOUND)
    set(HAVE_LIBJPEG 1)

    target_include_directories(egt PRIVATE ${LIBJPEG_INCLUDE_DIRS})
    target_compile_options(egt PRIVATE ${LIBJPEG_CFLAGS_OTHER})
    target_link_directories(egt PRIVATE ${LIBJPEG_LIBRARY_DIRS})
    target_link_libraries(egt PRIVATE ${LIBJPEG_LIBRARIES})
    target_link_options(egt PRIVATE ${LIBJPEG_LDFLAGS_OTHER})
    target_sources(egt PRIVATE images/jpeg/cairo_jpg.c)
endif()

if(LIBCURL_FOUND)
    set(HAVE_LIBCURL 1)
    set(include_http "#define EGT_HAS_HTTP 1")

    target_include_directories(egt PRIVATE ${LIBCURL_INCLUDE_DIRS})
    target_compile_options(egt PRIVATE ${LIBCURL_CFLAGS_OTHER})
    target_link_directories(egt PRIVATE ${LIBCURL_LIBRARY_DIRS})
    target_link_libraries(egt PRIVATE ${LIBCURL_LIBRARIES})
    target_link_options(egt PRIVATE ${LIBCURL_LDFLAGS_OTHER})
    target_sources(egt PRIVATE network/http.cpp)
    target_sources(egt PUBLIC FILE_SET HEADERS FILES ${CMAKE_SOURCE_DIR}/include/egt/network/http.h)
endif()

if(LIBRSVG_FOUND)
    set(HAVE_LIBRSVG 1)
    set(include_svg "#define EGT_HAS_SVG 1")

    target_include_directories(egt PRIVATE ${LIBRSVG_INCLUDE_DIRS})
    target_compile_options(egt PRIVATE ${LIBRSVG_CFLAGS_OTHER})
    target_link_directories(egt PRIVATE ${LIBRSVG_LIBRARY_DIRS})
    target_link_libraries(egt PRIVATE ${LIBRSVG_LIBRARIES})
    target_link_options(egt PRIVATE ${LIBRSVG_LDFLAGS_OTHER})
    target_sources(egt PRIVATE
        detail/svg.cpp
        svgimage.cpp
    )
    target_sources(egt PUBLIC FILE_SET HEADERS FILES ${CMAKE_SOURCE_DIR}/include/egt/svgimage.h)
endif()

if(ZLIB_FOUND)
    set(HAVE_ZLIB 1)

    target_include_directories(egt PRIVATE ${ZLIB_INCLUDE_DIRS})
    target_compile_options(egt PRIVATE ${ZLIB_CFLAGS_OTHER})
    target_link_directories(egt PRIVATE ${ZLIB_LIBRARY_DIRS})
    target_link_libraries(egt PRIVATE ${ZLIB_LIBRARIES})
    target_link_options(egt PRIVATE ${ZLIB_LDFLAGS_OTHER})
endif()

if(LIBMAGIC_FOUND)
    set(HAVE_LIBMAGIC 1)

    target_include_directories(egt PRIVATE ${LIBMAGIC_INCLUDE_DIRS})
    target_compile_options(egt PRIVATE ${LIBMAGIC_CFLAGS_OTHER})
    target_link_directories(egt PRIVATE ${LIBMAGIC_LIBRARY_DIRS})
    target_link_libraries(egt PRIVATE ${LIBMAGIC_LIBRARIES})
    target_link_options(egt PRIVATE ${LIBMAGIC_LDFLAGS_OTHER})
endif()

if(ALSA_FOUND)
    set(HAVE_ALSA 1)
    set(include_sound "#define EGT_HAS_SOUND 1")

    target_include_directories(egt PRIVATE ${ALSA_INCLUDE_DIRS})
    target_compile_options(egt PRIVATE ${ALSA_CFLAGS_OTHER})
    target_link_directories(egt PRIVATE ${ALSA_LIBRARY_DIRS})
    target_link_libraries(egt PRIVATE ${ALSA_LIBRARIES})
    target_link_options(egt PRIVATE ${ALSA_LDFLAGS_OTHER})
    target_sources(egt PRIVATE sound.cpp)
    target_sources(egt PUBLIC FILE_SET HEADERS FILES ${CMAKE_SOURCE_DIR}/include/egt/sound.h)
endif()

if(SNDFILE_FOUND)
    set(HAVE_SNDFILE 1)

    target_include_directories(egt PRIVATE ${SNDFILE_INCLUDE_DIRS})
    target_compile_options(egt PRIVATE ${SNDFILE_CFLAGS_OTHER})
    target_link_directories(egt PRIVATE ${SNDFILE_LIBRARY_DIRS})
    target_link_libraries(egt PRIVATE ${SNDFILE_LIBRARIES})
    target_link_options(egt PRIVATE ${SNDFILE_LDFLAGS_OTHER})
endif()

if(VIRTUALKEYBOARD)
    set(ENABLE_VIRTUALKEYBOARD 1)
    set(include_virtualkeyboard "#define EGT_HAS_VIRTUALKEYBOARD 1")

    target_sources(egt PRIVATE virtualkeyboard.cpp)
    target_sources(egt PUBLIC FILE_SET HEADERS FILES ${CMAKE_SOURCE_DIR}/include/egt/virtualkeyboard.h)
endif()

if(ENABLE_SVGDESERIAL)
    set(include_svgdeserial "#define EGT_HAS_SVGDESERIAL 1")

    target_sources(egt PRIVATE svgdeserial.cpp)
    target_sources(egt PUBLIC FILE_SET HEADERS FILES ${CMAKE_SOURCE_DIR}/include/egt/svgdeserial.h)
endif()

if(ENABLE_SIMD)
    set(HAVE_SIMD 1)

    include(ExternalProject)

    ExternalProject_Add(simd
        URL ${CMAKE_SOURCE_DIR}/external/Simd
        SOURCE_SUBDIR prj/cmake
        CMAKE_ARGS -Wno-dev -DTOOLCHAIN=${CMAKE_CXX_COMPILER} -DTARGET=${CMAKE_SYSTEM_PROCESSOR} -DCMAKE_BUILD_TYPE=Release -DLIBRARY=STATIC -DSIMD_TEST=off -DCHECK_VERSION=0
    )

    add_dependencies(egt simd)
endif()

set(prefix ${CMAKE_INSTALL_PREFIX})
set(exec_prefix \${prefix})
set(libdir \${exec_prefix}/lib)
set(includedir \${prefix}/include)

target_compile_definitions(egt PRIVATE HAVE_CONFIG_H)
target_include_directories(egt PRIVATE ${CMAKE_BINARY_DIR})
configure_file(config.h.in ${CMAKE_BINARY_DIR}/config.h @ONLY)

configure_file(../libegt.pc.in ../libegt.pc @ONLY)
configure_file(../include/egt/ui.in ../include/egt/ui)
configure_file(../include/egt/version.h.in ../include/egt/version.h)

install(TARGETS egt
        LIBRARY
        FILE_SET HEADERS
)
install(FILES ${CMAKE_BINARY_DIR}/libegt.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
