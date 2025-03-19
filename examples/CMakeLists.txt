include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/include
)

include_directories(SYSTEM
    ${CMAKE_SOURCE_DIR}/external/rapidxml
    ${CMAKE_SOURCE_DIR}/external/cxxopts/include
)

if(GSTREAMER_PLUGINS_BASE_DEV_FOUND)
    add_executable(egt_audioplayer audioplayer/audioplayer.cpp)
    target_compile_definitions(egt_audioplayer PRIVATE EXAMPLEDATA="${CMAKE_INSTALL_FULL_DATADIR}/egt/examples/audioplayer")
    target_link_libraries(egt_audioplayer PRIVATE egt)
    install(TARGETS egt_audioplayer RUNTIME)
    install(DIRECTORY audioplayer
            DESTINATION ${CMAKE_INSTALL_DATADIR}/egt/examples
            FILES_MATCHING
            PATTERN *.png
            PATTERN *.mp3
            PATTERN audioplayer.xml)
endif()

add_executable(egt_basic basic/basic.cpp)
target_link_libraries(egt_basic PRIVATE egt)
install(TARGETS egt_basic RUNTIME)

add_executable(egt_basicui basicui/basicui.cpp)
target_compile_definitions(egt_basicui PRIVATE EXAMPLEDATA="${CMAKE_INSTALL_FULL_DATADIR}/egt/examples/basicui")
target_link_libraries(egt_basicui PRIVATE egt)
install(TARGETS egt_basicui RUNTIME)
install(DIRECTORY basicui
        DESTINATION ${CMAKE_INSTALL_DATADIR}/egt/examples
        FILES_MATCHING
        PATTERN *.xml)

add_executable(egt_boards boards/boards.cpp)
target_link_libraries(egt_boards PRIVATE egt)
install(TARGETS egt_boards RUNTIME)
install(DIRECTORY boards
        DESTINATION ${CMAKE_INSTALL_DATADIR}/egt/examples
        FILES_MATCHING
        PATTERN *.png
        PATTERN boards.xml)

if(LUA_FOUND)
    add_executable(egt_calc calc/calc.cpp)
    target_link_libraries(egt_calc PRIVATE egt)
    install(TARGETS egt_calc RUNTIME)
    install(DIRECTORY calc
            DESTINATION ${CMAKE_INSTALL_DATADIR}/egt/examples
            FILES_MATCHING
            PATTERN *.png
            PATTERN calc.xml)
endif()

if(PLPLOT_FOUND)
    add_executable(egt_charts charts/charts.cpp)
    target_compile_definitions(egt_charts PRIVATE EXAMPLEDATA="${CMAKE_INSTALL_FULL_DATADIR}/egt/examples/charts")
    target_link_libraries(egt_charts PRIVATE egt)
    install(TARGETS egt_charts RUNTIME)
    install(DIRECTORY charts
            DESTINATION ${CMAKE_INSTALL_DATADIR}/egt/examples
            FILES_MATCHING
            PATTERN *.png
            PATTERN charts.xml)
endif()

add_executable(egt_colors colors/colors.cpp)
target_link_libraries(egt_colors PRIVATE egt)
install(TARGETS egt_colors RUNTIME)

if(LIBRSVG_FOUND)
    add_executable(egt_dash dash/dash.cpp)
    target_compile_definitions(egt_dash PRIVATE EXAMPLEDATA="${CMAKE_INSTALL_FULL_DATADIR}/egt/examples/dash")
    target_link_libraries(egt_dash PRIVATE egt)
    install(TARGETS egt_dash RUNTIME)
    install(DIRECTORY dash
            DESTINATION ${CMAKE_INSTALL_DATADIR}/egt/examples
            FILES_MATCHING
            PATTERN *.svg
            PATTERN *.png
            PATTERN dash.xml)
endif()

add_executable(egt_dialog dialog/dialog.cpp)
target_link_libraries(egt_dialog PRIVATE egt)
install(TARGETS egt_dialog RUNTIME)

add_executable(egt_drag drag/drag.cpp)
target_compile_definitions(egt_drag PRIVATE EXAMPLEDATA="${CMAKE_INSTALL_FULL_DATADIR}/egt/examples/drag")
target_link_libraries(egt_drag PRIVATE egt)
install(TARGETS egt_drag RUNTIME)
install(DIRECTORY drag
        DESTINATION ${CMAKE_INSTALL_DATADIR}/egt/examples
        FILES_MATCHING
        PATTERN *.png
        PATTERN drag.xml)

if(ALSA_FOUND AND SNDFILE_FOUND)
    add_executable(egt_drummachine drummachine/drummachine.cpp)
    target_compile_definitions(egt_drummachine PRIVATE EXAMPLEDATA="${CMAKE_INSTALL_FULL_DATADIR}/egt/examples/drummachine")
    target_link_libraries(egt_drummachine PRIVATE egt)
    install(TARGETS egt_drummachine RUNTIME)
    install(DIRECTORY drummachine
            DESTINATION ${CMAKE_INSTALL_DATADIR}/egt/examples
            FILES_MATCHING
            PATTERN *.wav
            PATTERN *.png
            PATTERN drummachine.xml)
endif()

if(PLPLOT_FOUND)
    add_executable(egt_easing easing/easing.cpp)
    target_compile_definitions(egt_easing PRIVATE EXAMPLEDATA="${CMAKE_INSTALL_FULL_DATADIR}/egt/examples/easing")
    target_link_libraries(egt_easing PRIVATE egt)
    install(TARGETS egt_easing RUNTIME)
    install(DIRECTORY easing
            DESTINATION ${CMAKE_INSTALL_DATADIR}/egt/examples
            FILES_MATCHING
            PATTERN *.png
            PATTERN easing.xml)
endif()

add_executable(egt_floating floating/floating.cpp)
target_compile_definitions(egt_floating PRIVATE EXAMPLEDATA="${CMAKE_INSTALL_FULL_DATADIR}/egt/examples/floating")
target_link_libraries(egt_floating PRIVATE egt)
install(TARGETS egt_floating RUNTIME)
install(DIRECTORY floating
        DESTINATION ${CMAKE_INSTALL_DATADIR}/egt/examples
        FILES_MATCHING
        PATTERN *.png)

add_executable(egt_frames frames/frames.cpp)
target_link_libraries(egt_frames PRIVATE egt)
install(TARGETS egt_frames RUNTIME)

if(LIBRSVG_FOUND)
    add_executable(egt_gauge gauge/gauge.cpp)
    target_compile_definitions(egt_gauge PRIVATE EXAMPLEDATA="${CMAKE_INSTALL_FULL_DATADIR}/egt/examples/gauge")
    target_link_libraries(egt_gauge PRIVATE egt)
    install(TARGETS egt_gauge RUNTIME)
    install(DIRECTORY gauge
            DESTINATION ${CMAKE_INSTALL_DATADIR}/egt/examples
            FILES_MATCHING
            PATTERN *.png
            PATTERN *.svg
            PATTERN gauge.xml)
endif()

if(HAVE_LINUX_GPIO_H)
    add_executable(egt_gpio gpio/gpio.cpp)
    target_link_libraries(egt_gpio PRIVATE egt)
    install(TARGETS egt_gpio RUNTIME)
endif()

add_executable(egt_grid grid/grid.cpp)
target_link_libraries(egt_grid PRIVATE egt)
install(TARGETS egt_grid RUNTIME)

if(HAVE_LIBINTL)
    add_subdirectory(i18n)
endif()

add_executable(egt_imagebutton imagebutton/imagebutton.cpp)
target_link_libraries(egt_imagebutton PRIVATE egt)
install(TARGETS egt_imagebutton RUNTIME)

add_executable(egt_imagestack imagestack/imagestack.cpp)
target_compile_definitions(egt_imagestack PRIVATE EXAMPLEDATA="${CMAKE_INSTALL_FULL_DATADIR}/egt/examples/imagestack")
target_link_libraries(egt_imagestack PRIVATE egt)
install(TARGETS egt_imagestack RUNTIME)
install(DIRECTORY imagestack
        DESTINATION ${CMAKE_INSTALL_DATADIR}/egt/examples
        FILES_MATCHING
        PATTERN *.png
        PATTERN imagestack.xml)

add_executable(egt_keys keys/keys.cpp)
target_link_libraries(egt_keys PRIVATE egt)
install(TARGETS egt_keys RUNTIME)

add_executable(egt_listboxmulti listboxmulti/listboxmulti.cpp)
target_link_libraries(egt_listboxmulti PRIVATE egt)
install(TARGETS egt_listboxmulti RUNTIME)

if(LUA_FOUND AND ENABLE_LUA_BINDINGS)
    add_executable(egt_luarepl luarepl/luarepl.cpp)
    target_link_libraries(egt_luarepl PRIVATE egt)
    install(TARGETS egt_luarepl RUNTIME)
endif()

if(PLPLOT_FOUND)
    add_executable(egt_monitor monitor/monitor.cpp)
    target_link_libraries(egt_monitor PRIVATE egt)
    install(TARGETS egt_monitor RUNTIME)
    install(DIRECTORY monitor
            DESTINATION ${CMAKE_INSTALL_DATADIR}/egt/examples
            FILES_MATCHING
            PATTERN *.png
            PATTERN monitor.xml)
endif()

if(ENABLE_SVGDESERIAL)
    add_executable(egt_motorcycledash motorcycledash/motorcycledash.cpp)
    target_compile_definitions(egt_motorcycledash PRIVATE EXAMPLEDATA="${CMAKE_INSTALL_FULL_DATADIR}/egt/examples/motorcycledash")
    target_link_libraries(egt_motorcycledash PRIVATE egt)
    install(TARGETS egt_motorcycledash RUNTIME)
    install(DIRECTORY motorcycledash
            DESTINATION ${CMAKE_INSTALL_DATADIR}/egt/examples
            FILES_MATCHING
            PATTERN eraw.bin
            PATTERN *.png)
endif()

add_executable(egt_press press/press.cpp)
target_link_libraries(egt_press PRIVATE egt)
install(TARGETS egt_press RUNTIME)

if(LIBRSVG_FOUND AND HAVE_LIBINTL)
    add_subdirectory(resource)
endif()

if(LIBRSVG_FOUND)
    add_executable(egt_sevensegment sevensegment/sevensegment.cpp)
    target_compile_definitions(egt_sevensegment PRIVATE EXAMPLEDATA="${CMAKE_INSTALL_FULL_DATADIR}/egt/examples/sevensegment")
    target_link_libraries(egt_sevensegment PRIVATE egt)
    install(TARGETS egt_sevensegment RUNTIME)
    install(DIRECTORY sevensegment
            DESTINATION ${CMAKE_INSTALL_DATADIR}/egt/examples
            FILES_MATCHING
            PATTERN *.svg)
endif()

add_executable(egt_sizers sizers/sizers.cpp)
target_link_libraries(egt_sizers PRIVATE egt)
install(TARGETS egt_sizers RUNTIME)

if(ENABLE_SNIPPETS)
    add_subdirectory(snippets)
endif()

add_executable(egt_space space/space.cpp)
target_compile_definitions(egt_space PRIVATE EXAMPLEDATA="${CMAKE_INSTALL_FULL_DATADIR}/egt/examples/space")
target_link_libraries(egt_space PRIVATE egt)
install(TARGETS egt_space RUNTIME)
install(DIRECTORY space
        DESTINATION ${CMAKE_INSTALL_DATADIR}/egt/examples
        FILES_MATCHING
        PATTERN *.png
        PATTERN space.xml)

add_executable(egt_sprite sprite/sprite.cpp)
target_compile_definitions(egt_sprite PRIVATE EXAMPLEDATA="${CMAKE_INSTALL_FULL_DATADIR}/egt/examples/sprite")
target_link_libraries(egt_sprite PRIVATE egt)
install(TARGETS egt_sprite RUNTIME)
install(DIRECTORY sprite
        DESTINATION ${CMAKE_INSTALL_DATADIR}/egt/examples
        FILES_MATCHING
        PATTERN *.png
        PATTERN rotate EXCLUDE)

add_executable(egt_squares squares/squares.cpp)
target_link_libraries(egt_squares PRIVATE egt)
install(TARGETS egt_squares RUNTIME)

if(GSTREAMER_PLUGINS_BASE_DEV_FOUND)
    add_subdirectory(video)
endif()

if(VIRTUALKEYBOARD)
    add_executable(egt_virtualkeyboard virtualkeyboard/virtualkeyboard.cpp)
    target_link_libraries(egt_virtualkeyboard PRIVATE egt)
    install(TARGETS egt_virtualkeyboard RUNTIME)
endif()

add_executable(egt_whiteboard whiteboard/whiteboard.cpp)
target_compile_definitions(egt_whiteboard PRIVATE EXAMPLEDATA="${CMAKE_INSTALL_FULL_DATADIR}/egt/examples/whiteboard")
target_link_libraries(egt_whiteboard PRIVATE egt)
install(TARGETS egt_whiteboard RUNTIME)
install(DIRECTORY whiteboard
        DESTINATION ${CMAKE_INSTALL_DATADIR}/egt/examples
        FILES_MATCHING
        PATTERN *.png
        PATTERN whiteboard.xml)

add_executable(egt_widgets widgets/widgets.cpp)
target_link_libraries(egt_widgets PRIVATE egt)
install(TARGETS egt_widgets RUNTIME)
install(DIRECTORY widgets
        DESTINATION ${CMAKE_INSTALL_DATADIR}/egt/examples
        FILES_MATCHING
        PATTERN *.png
        PATTERN widgets.xml)
