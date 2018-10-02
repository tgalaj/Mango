# Copyright (C) 2018 Tomasz Gałaj

cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(VertexEngine VERSION 0.1)

option(BUILD_EXAMPLE_GAME 
	   "Build example game project to see Vertex Engine in action."
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

# Create virtual folders to make it look nicer in VS
if(MSVC_IDE)
	# Macro to preserve source files hierarchy in the IDE
	macro(GroupSources curdir)
		file(GLOB children RELATIVE ${VertexEngine_SOURCE_DIR}/${curdir} ${VertexEngine_SOURCE_DIR}/${curdir}/*)

		foreach(child ${children})
			if(IS_DIRECTORY ${VertexEngine_SOURCE_DIR}/${curdir}/${child})
				GroupSources(${curdir}/${child})
			else()
				string(REPLACE "/" "\\" groupname ${curdir})
				string(REPLACE "src" "sources" groupname ${groupname})
				source_group(${groupname} FILES ${VertexEngine_SOURCE_DIR}/${curdir}/${child})
			endif()
		endforeach()
	endmacro()

	# Run macro
	GroupSources("src")
	GroupSources("include")
endif()