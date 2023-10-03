# Copyright (C) 2023 Tomasz Gałaj

cmake_minimum_required(VERSION 3.9.2 FATAL_ERROR)
project(Mango VERSION 0.1)

set(OpenGL_GL_PREFERENCE GLVND)

option(BUILD_EXAMPLE_GAME 
	   "Build example game project to see Mango in action."
	   ON)

set(THIRDPARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")

# add thirdparties
include(thirdparty/thirdparty.cmake)

# subdirectories
add_subdirectory(src)

if(BUILD_EXAMPLE_GAME)
	message(STATUS "Creating Example Game Project")
	add_subdirectory(example_game)
endif()
