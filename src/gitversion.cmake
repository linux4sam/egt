execute_process(COMMAND ${TOP_SOURCE_DIR}/scripts/gitversion
                WORKING_DIRECTORY ${TOP_SOURCE_DIR}
                OUTPUT_VARIABLE GIT_VERSION)
string(REGEX REPLACE "(.*)\n" "\"\\1\"" GIT_VERSION "${GIT_VERSION}")
message(STATUS "GIT_VERSION=${GIT_VERSION}")
configure_file(${SOURCE_DIR}/gitversion.h.in ../gitversion.h)
