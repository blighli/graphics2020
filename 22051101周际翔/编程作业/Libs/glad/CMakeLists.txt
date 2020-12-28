cmake_minimum_required (VERSION 3.3)

set(SRC_FILES ./src/gl.c)
include_directories(./include)
add_library(glad STATIC ${SRC_FILES})