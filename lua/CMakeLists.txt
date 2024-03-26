find_package(SWIG REQUIRED)
include(UseSWIG)

cmake_policy(SET CMP0078 NEW)

set_property(SOURCE ui.i PROPERTY CPLUSPLUS ON)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/include
)

swig_add_library(egtlua
    TYPE STATIC
    LANGUAGE lua
    SOURCES ui.i
)
