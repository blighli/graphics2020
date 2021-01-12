cmake_minimum_required (VERSION 3.3)

set(SRC_FILES 
	main.cpp
    Camera.hpp)

include_directories(../../Libs/glad/include)

add_executable(Exp02 ${SRC_FILES})

target_link_libraries(Exp02 glfw)
target_link_libraries(Exp02 glad)
target_link_libraries(Exp02 Runtime)
target_link_libraries(Exp02 stb_image)

add_custom_command(TARGET Exp02 POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Res/Exp02 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Shaders ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG})

add_custom_command(TARGET Exp02 POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Res/Exp02 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO}
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Shaders ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO})

add_custom_command(TARGET Exp02 POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Res/Exp02 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Shaders ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE})

add_custom_command(TARGET Exp02 POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Res/Exp02 ${CMAKE_BINARY_DIR}/Code/Exp02
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Shaders ${CMAKE_BINARY_DIR}/Code/Exp02)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})
