add_executable(egt_snippets snippets.cpp)
target_link_libraries(egt_snippets PRIVATE egt)
install(TARGETS egt_snippets RUNTIME)
install(DIRECTORY assets/
        DESTINATION ${CMAKE_INSTALL_DATADIR}/egt/examples/snippets
        FILES_MATCHING
        PATTERN *.png
        PATTERN *.wav
        PATTERN *.mp3)

add_executable(egt_snippet0 snippet0.cpp)
target_link_libraries(egt_snippet0 PRIVATE egt)
install(TARGETS egt_snippet0 RUNTIME)

add_executable(egt_snippet1 snippet1.cpp)
target_link_libraries(egt_snippet1 PRIVATE egt)
install(TARGETS egt_snippet1 RUNTIME)

add_executable(egt_snippet2 snippet2.cpp)
install(TARGETS egt_snippet2)

add_executable(egt_snippet3 snippet3.cpp)
install(TARGETS egt_snippet3)

add_executable(egt_snippet4 snippet4.cpp)
install(TARGETS egt_snippet4)

add_executable(egt_snippet5 snippet5.cpp)
target_link_libraries(egt_snippet5 PRIVATE egt)
install(TARGETS egt_snippet5)

add_executable(egt_snippet6 snippet6.cpp)
target_link_libraries(egt_snippet6 PRIVATE egt)
install(TARGETS egt_snippet6)

add_executable(egt_snippet7 snippet7.cpp)
install(TARGETS egt_snippet7)

add_executable(egt_snippet8 snippet8.cpp)
install(TARGETS egt_snippet8)

add_executable(egt_snippet9 snippet9.cpp)
install(TARGETS egt_snippet9)

if(PLPLOT_FOUND)
    add_executable(egt_snippet10 snippet10.cpp)
    target_link_libraries(egt_snippet10 PRIVATE egt)
    install(TARGETS egt_snippet10)
endif()
