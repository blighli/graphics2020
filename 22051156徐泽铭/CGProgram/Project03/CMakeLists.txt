cmake_minimum_required(VERSION 3.15)

project(Project03)
message(Hello + ${CMAKE_SOURCE_DIR})




# Source files
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty")
set(SOURCES "${SRC_DIR}/main.cpp")
set(PERSONAL_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/../include")
set(GLM "${LIB_DIR}")
set(ASSIMP "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/assimp")
#set(ASSMIP_H "${CMAKE_SOURCE_DIR}/3rdparty/assimp/include")
#set(ASSMIP_BUILD_H "${CMAKE_SOURCE_DIR}/3rdparty/assimp/build/include")
#set(ASSIMP_LINK ${CMAKE_SOURCE_DIR}/3rdparty/assimp/build/bin/libassimp.5.0.1.dylib)

# include header file
include_directories(${PERSONAL_INCLUDE} ${GLM} ${ASSIMP})

#link_libraries(${ASSIMP_LINK})
# Executable definition and properties
add_executable(${PROJECT_NAME} ${SOURCES})

# ASSIMP
target_link_libraries(${PROJECT_NAME} assimp)


target_include_directories(${PROJECT_NAME} PRIVATE "${SRC_DIR}")
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)

# ASSIMP
#target_link_libraries(${PROJECT_NAME} "assimp" ${ASSIMP}/build/bin/libassimp.5.0.1.dylib)
#target_include_directories(${PROJECT_NAME} PRIVATE ${ASSIMP}/include)
#target_include_directories(${PROJECT_NAME} PRIVATE ${ASSIMP}/build/include)





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
        ${CMAKE_SOURCE_DIR}/shaders/modellight.vs
        ${CMAKE_SOURCE_DIR}/shaders/modellight.fs

        )
set(MODEL ${CMAKE_SOURCE_DIR}/models/kuuga)
#set(TEXTURE_FILE
#        ${CMAKE_SOURCE_DIR}/textures/earth2048.bmp
#        ${CMAKE_SOURCE_DIR}/textures/moon1024.bmp
#        ${CMAKE_SOURCE_DIR}/textures/Albedo-diffuse.jpg
#        ${CMAKE_SOURCE_DIR}/textures/2k_sun.jpg)

# copy shader file to build dir
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${SHADER_FILE}
        $<TARGET_FILE_DIR:Project03>
        )



# copy texture file to build dir
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${MODEL}
        $<TARGET_FILE_DIR:Project03>/kuuga
        )