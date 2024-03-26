include_directories(
    ${CMAKE_SOURCE_DIR}/external/googletest/googletest/include
    ${CMAKE_SOURCE_DIR}/external/googletest/googletest
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/include
)

add_library(gtest SHARED
    ${CMAKE_SOURCE_DIR}/external/googletest/googletest/src/gtest-all.cc
)
install(TARGETS gtest LIBRARY)

add_executable(egt_unittests
   main.cpp
   widgets/button.cpp
   widgets/combobox.cpp
   widgets/form.cpp
   widgets/frame.cpp
   widgets/grid.cpp
   widgets/layout.cpp
   widgets/listbox.cpp
   widgets/notebook.cpp
   widgets/scrollwheel.cpp
   widgets/sizer.cpp
   widgets/slider.cpp
   widgets/valuerange.cpp
   widgets/view.cpp
   widgets/window.cpp
)
target_link_libraries(egt_unittests PRIVATE egt gtest)
install(TARGETS egt_unittests RUNTIME)

if(GSTREAMER_PLUGINS_BASE_DEV_FOUND)
    target_sources(egt_unittests PRIVATE
        audio/audio.cpp
        video/camera.cpp
        video/video.cpp
    )
endif()
