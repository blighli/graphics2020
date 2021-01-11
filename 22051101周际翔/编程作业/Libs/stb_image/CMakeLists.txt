cmake_minimum_required (VERSION 3.3)

set(SRC_FILES 
	./stb_image.h
	./stb_image.cpp)
add_library(stb_image STATIC ${SRC_FILES})