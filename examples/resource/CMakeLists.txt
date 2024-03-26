file(GLOB resources
     ${CMAKE_SOURCE_DIR}/examples/resource/*.png
     ${CMAKE_SOURCE_DIR}/examples/resource/*.bmp)

add_custom_command(
    OUTPUT rc.cpp
    COMMAND ${CMAKE_SOURCE_DIR}/scripts/mresg ${resources} -o rc.cpp
    DEPENDS ${resources}
    VERBATIM)

add_executable(egt_resource resource.cpp rc.cpp)
target_compile_definitions(egt_resource PRIVATE EXAMPLEDATA="${CMAKE_INSTALL_FULL_DATADIR}/egt/examples/resource")
target_link_libraries(egt_resource PRIVATE egt)
install(TARGETS egt_resource RUNTIME)
install(DIRECTORY ./
        DESTINATION ${CMAKE_INSTALL_DATADIR}/egt/examples/resource
        FILES_MATCHING
        PATTERN *.png
        PATTERN *.bmp
        PATTERN *.jpg
        PATTERN *.svg)
