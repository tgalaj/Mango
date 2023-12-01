# Copyright (C) 2023 Tomasz Gałaj

cmake_minimum_required(VERSION 3.21 FATAL_ERROR)
project(Mango VERSION 1.0.0)

set(OpenGL_GL_PREFERENCE GLVND)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(global_settings)

option(MG_BUILD_MANGO_SANDBOX 
       "Build example game project to see Mango in action."
       ON)

option(BUILD_MANGO_EDITOR
       "Build Mango Editor."
       OFF)

# ---- Mango Engine ----
add_subdirectory(Mango)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Mango)

if(MG_BUILD_MANGO_SANDBOX)
    message(STATUS "Creating Mango Sanbox Project")
    add_subdirectory(MangoSandbox)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT MangoSandbox)
endif()

if(BUILD_MANGO_EDITOR)
    message(STATUS "Creating Mango Editor Project")
    add_subdirectory(MangoEditor)
endif()

# ---- MangoTestAssets ----
find_package(Git QUIET)
if(GIT_FOUND)
    option(MANGO_DOWNLOAD_TEST_ASSETS "Download MangoTestAssets repo." OFF)
    if(MANGO_DOWNLOAD_TEST_ASSETS)
        if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/MangoTestAssets/.git)
            message(STATUS "Fetching updates from MangoTestAssets...")
            execute_process(COMMAND ${GIT_EXECUTABLE} pull origin main
                            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/MangoTestAssets/
                            RESULT_VARIABLE GIT_SUBMOD_RESULT)
            if(NOT GIT_SUBMOD_RESULT EQUAL "0")
                message(FATAL_ERROR "git pull failed with ${GIT_SUBMOD_RESULT}")
            endif()
        else()
            message(STATUS "Cloning MangoTestAssets...")
            execute_process(COMMAND ${GIT_EXECUTABLE} clone https://github.com/tgalaj/MangoTestAssets.git
                            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
                            RESULT_VARIABLE GIT_SUBMOD_RESULT)
            if(NOT GIT_SUBMOD_RESULT EQUAL "0")
                message(FATAL_ERROR "git clone failed with ${GIT_SUBMOD_RESULT}")
            endif()
        endif()
    endif()
endif()