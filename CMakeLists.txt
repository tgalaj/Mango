# Copyright (C) 2023 Tomasz Gałaj

cmake_minimum_required(VERSION 3.21 FATAL_ERROR)
project(Mango VERSION 1.0)

set(OpenGL_GL_PREFERENCE GLVND)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(global_settings)

option(BUILD_EXAMPLE_GAME 
	   "Build example game project to see Mango in action."
	   ON)

# ---- Dependencies ----
add_subdirectory(thirdparty)

# ---- Main project's files ----
add_subdirectory(src)

if(BUILD_EXAMPLE_GAME)
	message(STATUS "Creating Example Game Project")
	add_subdirectory(example_game)
endif()
