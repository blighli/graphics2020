cmake_minimum_required (VERSION 3.3)

set(SRC_FILES 
	main.cpp
    Camera.hpp)

include_directories(../../Libs/glad/include)

add_executable(Exp01 ${SRC_FILES})

target_link_libraries(Exp01 glfw)
target_link_libraries(Exp01 glad)
target_link_libraries(Exp01 Runtime)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})
