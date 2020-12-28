cmake_minimum_required (VERSION 3.3)

# Include Luna-specific helper functions.
include(Tools/CMake/Functions.cmake)

project(HW)

include_directories(./Code)
include_directories(./Libs)

add_subdirectory(Code)
add_subdirectory(Libs)