cmake_minimum_required(VERSION 3.15)

project(Project02)
message(${CMAKE_CURRENT_SOURCE_DIR})




# Source files
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty")
set(SOURCES "${SRC_DIR}/main.cpp" "${SRC_DIR}/sphere.h" "${SRC_DIR}/sphere.cpp")
set(PERSONAL_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/../include")
set(GLM "${LIB_DIR}")
# include header file
include_directories(${PERSONAL_INCLUDE} ${GLM})

# Executable definition and properties
add_executable(${PROJECT_NAME} ${SOURCES})


target_include_directories(${PROJECT_NAME} PRIVATE "${SRC_DIR}")
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)

# GLFW
set(GLFW_DIR "${LIB_DIR}/glfw")
target_link_libraries(${PROJECT_NAME} "glfw" "${GLFW_LIBRARIES}")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLFW_DIR}/include")
target_compile_definitions(${PROJECT_NAME} PRIVATE "GLFW_INCLUDE_NONE")

# glad
set(GLAD_DIR "${LIB_DIR}/glad")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")
target_link_libraries(${PROJECT_NAME} "glad" "${CMAKE_DL_LIBS}")


set(SHADER_FILE
        ${CMAKE_SOURCE_DIR}/shaders/solarsystem.vs
        ${CMAKE_SOURCE_DIR}/shaders/solarsystem.fs
        ${CMAKE_SOURCE_DIR}/shaders/sun.vs
        ${CMAKE_SOURCE_DIR}/shaders/sun.fs
        )

set(TEXTURE_FILE
        ${CMAKE_SOURCE_DIR}/textures/earth2048.bmp
        ${CMAKE_SOURCE_DIR}/textures/moon1024.bmp
        ${CMAKE_SOURCE_DIR}/textures/Albedo-diffuse.jpg
        ${CMAKE_SOURCE_DIR}/textures/2k_sun.jpg)

# copy shader file to build dir
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${SHADER_FILE}
        $<TARGET_FILE_DIR:Project02>
        )



# copy texture file to build dir
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${TEXTURE_FILE}
        $<TARGET_FILE_DIR:Project02>
        )