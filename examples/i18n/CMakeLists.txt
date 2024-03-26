set(languages de es fr zh_CN hi)
list(TRANSFORM languages APPEND .mo OUTPUT_VARIABLE binary_message_catalogs)

foreach(language ${languages})
    add_custom_command(
        OUTPUT ${language}.mo
        COMMAND msgfmt --no-hash --check --output-file=${language}.mo ${CMAKE_CURRENT_SOURCE_DIR}/${language}.po
        DEPENDS ${language}.po
        VERBATIM)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${language}.mo
            DESTINATION ${CMAKE_INSTALL_DATADIR}/locale/${language}/LC_MESSAGES
            RENAME i18n.mo)
endforeach()

add_custom_target(generate_binary_message_catalogs DEPENDS ${binary_message_catalogs})

add_executable(egt_i18n i18n.cpp)
target_compile_definitions(egt_i18n PRIVATE EXAMPLEDATA="${CMAKE_INSTALL_FULL_DATADIR}/egt/examples/i18n")
target_include_directories(egt_i18n SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/external/utfcpp/source)
target_link_libraries(egt_i18n PRIVATE egt)
add_dependencies(egt_i18n generate_binary_message_catalogs)
install(TARGETS egt_i18n RUNTIME)
install(DIRECTORY ./
        DESTINATION ${CMAKE_INSTALL_DATADIR}/egt/examples/i18n
        FILES_MATCHING
        PATTERN *.png
        PATTERN i18n.xml)
