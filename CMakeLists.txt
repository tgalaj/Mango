# Copyright (C) 2023 Tomasz Gałaj

cmake_minimum_required(VERSION 3.21 FATAL_ERROR)
project(Mango VERSION 1.0.0)

set(OpenGL_GL_PREFERENCE GLVND)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(global_settings)

option(BUILD_MANGO_SANDBOX 
	   "Build example game project to see Mango in action."
	   ON)

option(BUILD_MANGO_EDITOR
	   "Build Mango Editor."
	   OFF)

# ---- Mango Engine ----
add_subdirectory(Mango)

if(BUILD_MANGO_SANDBOX)
	message(STATUS "Creating Mango Sanbox Project")
	add_subdirectory(MangoSandbox)
endif()

if(BUILD_MANGO_EDITOR)
	message(STATUS "Creating Mango Editor Project")
	add_subdirectory(MangoEditor)
endif()
