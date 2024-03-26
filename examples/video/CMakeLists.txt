file(GLOB resources ${CMAKE_CURRENT_SOURCE_DIR}/*.png)

add_custom_command(
    OUTPUT rc.cpp
    COMMAND ${CMAKE_SOURCE_DIR}/scripts/mresg ${resources} -o rc.cpp
    DEPENDS ${resources}
    VERBATIM)
add_custom_target(generate_rc_cpp DEPENDS rc.cpp)

add_executable(egt_video video.cpp rc.cpp)
target_compile_definitions(egt_video PRIVATE EXAMPLEDATA="${CMAKE_INSTALL_FULL_DATADIR}/egt/examples/video")
target_link_libraries(egt_video PRIVATE egt)
add_dependencies(egt_video generate_rc_cpp)

add_executable(egt_camera camera.cpp rc.cpp)
target_compile_definitions(egt_camera PRIVATE EXAMPLEDATA="${CMAKE_INSTALL_FULL_DATADIR}/egt/examples/video")
target_link_libraries(egt_camera PRIVATE egt)
add_dependencies(egt_camera generate_rc_cpp)

add_executable(egt_capture capture.cpp rc.cpp)
target_compile_definitions(egt_capture PRIVATE EXAMPLEDATA="${CMAKE_INSTALL_FULL_DATADIR}/egt/examples/video")
target_link_libraries(egt_capture PRIVATE egt)
add_dependencies(egt_capture generate_rc_cpp)

install(TARGETS egt_video egt_camera egt_capture RUNTIME)
install(FILES video.xml background.jpg video.png camera.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/egt/examples/video)
