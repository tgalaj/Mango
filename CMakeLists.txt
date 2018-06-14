# Copyright (C) 2018 Tomasz Gałaj

cmake_minimum_required(VERSION 3.6 FATAL_ERROR)
project(VertexEngine)

option(BUILD_EXAMPLE_GAME 
	   "Build example game project to see Vertex Engine in action."
	   OFF)
	   
if(BUILD_EXAMPLE_GAME)
	message(STATUS "Creating Example Game Project")
	add_subdirectory(example_game)
endif()

# Add .lib files
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
	link_directories(${CMAKE_SOURCE_DIR}/lib/x32)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
	link_directories(${CMAKE_SOURCE_DIR}/lib/x64)
endif()

# Add source files
file(GLOB_RECURSE SOURCE_FILES 
	"${CMAKE_SOURCE_DIR}/src/*.c"
	"${CMAKE_SOURCE_DIR}/src/*.cpp")
	
# Add header files
file(GLOB_RECURSE HEADER_FILES 
	"${CMAKE_SOURCE_DIR}/include/*.h"
	"${CMAKE_SOURCE_DIR}/include/*.hpp")
	
# Add imgui source files
file(GLOB THIRDPARTY_IMGUI
	"${CMAKE_SOURCE_DIR}/thirdparty/imgui/imgui.cpp"
	"${CMAKE_SOURCE_DIR}/thirdparty/imgui/imgui_draw.cpp"
	"${CMAKE_SOURCE_DIR}/thirdparty/imgui/imgui_demo.cpp")

# Add GLAD source file
file(GLOB THIRDPARTY_GLAD
	"${CMAKE_SOURCE_DIR}/thirdparty/glad.c")

set(THIRDPARTY ${THIRDPARTY_IMGUI} ${THIRDPARTY_GLAD})

# Define the library
add_library(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES} ${THIRDPARTY})

# Define the include DIRs
include_directories("${CMAKE_SOURCE_DIR}/include"
					"${CMAKE_SOURCE_DIR}/thirdparty")

#######################################
# LOOK for the packages that we need! #
#######################################

# We need a CMAKE_DIR with some code to find external dependencies
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# OpenGL
find_package(OpenGL REQUIRED)

# GLM
find_package(GLM REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")

# GLFW
find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")

# ASSIMP
find_package(ASSIMP REQUIRED)
message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIR}")

# FREEIMAGE
find_package(FREEIMAGE REQUIRED)
message(STATUS "Found FREEIMAGE in ${FREEIMAGE_INCLUDE_DIR}")

# FREEIMAGE
find_package(ENTITYX REQUIRED)
message(STATUS "Found ENTITYX in ${ENTITYX_INCLUDE_DIR}")

# Put all libraries into a variable
set(LIBS glfw3 
		 opengl32 
		 assimp 
		 FreeImage 
		 optimized entityx debug entityx_d)

# Define the link libraries
target_link_libraries(${PROJECT_NAME} ${LIBS})

# Create virtual folders to make it look nicer in VS
if(MSVC_IDE)
	# Macro to preserve source files hierarchy in the IDE
	macro(GroupSources curdir)
		file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir} ${PROJECT_SOURCE_DIR}/${curdir}/*)

		foreach(child ${children})
			if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
				GroupSources(${curdir}/${child})
			else()
				string(REPLACE "/" "\\" groupname ${curdir})
				string(REPLACE "src" "sources" groupname ${groupname})
				source_group(${groupname} FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
			endif()
		endforeach()
	endmacro()

	# Run macro
	GroupSources("src")
	GroupSources("include")
	
	source_group(thirdparty\\imgui FILES ${THIRDPARTY_IMGUI})
	source_group(thirdparty\\glad  FILES ${THIRDPARTY_GLAD})
endif()