cmake_minimum_required(VERSION 3.15)

project(CGProgram)
#message(${CMAKE_CURRENT_SOURCE_DIR})

#build GLFW
add_subdirectory(${CMAKE_SOURCE_DIR}/3rdparty/glfw)

#build GLAD
add_library("glad" "${CMAKE_SOURCE_DIR}/3rdparty/glad/src/glad.c")
add_subdirectory(Project01)
add_subdirectory(Project02)






