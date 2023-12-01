# Copyright (C) 2023 Tomasz Gałaj

# Configure thirdparty libraries
add_subdirectory(thirdparty)

# Add source files
file(GLOB_RECURSE SOURCE_FILES 
     ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
     ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
    
# Add header files
file(GLOB_RECURSE HEADER_FILES 
     ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
     ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)
     
# Search for the assets files
file(GLOB_RECURSE ASSETS_FILES 
     ${CMAKE_CURRENT_SOURCE_DIR}/assets/*.*)

set_source_files_properties(${ASSETS_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ASSETS_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADER_FILES})

# Define the library
add_library(Mango STATIC ${HEADER_FILES} ${SOURCE_FILES} ${ASSETS_FILES})
add_library(Mango::Mango ALIAS Mango)

set_target_properties(Mango PROPERTIES
                      VERSION ${Mango_VERSION}
                      SOVERSION ${Mango_VERSION_MAJOR})

target_precompile_headers(Mango PRIVATE "src/mgpch.h")

# Options
option(MG_PROFILE_ENABLE "Enable profiling with Tracy profiler" OFF)

if(MG_PROFILE_ENABLE MATCHES ON)
    target_compile_definitions(Mango PRIVATE MG_PROFILE_ENABLE)
endif()

# Definitions
target_compile_definitions(Mango PRIVATE GLFW_INCLUDE_NONE)
target_compile_definitions(Mango PRIVATE LIBRARY_SUFFIX="")

# Includes and libs
target_include_directories(Mango PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>")

target_link_libraries(Mango PUBLIC ${OPENGL_LIBRARIES} 
                                   glad
                                   stb_image
                                   assimp
                                   glfw
                                   imgui
                                   spdlog
                                   cxxopts
                                   glm::glm
                                   tinyddsloader
                                   entt
                                   Tracy::TracyClient
                                   physfs-static)

if(MinGW)
    target_link_libraries(Mango bz2)
endif()

if(MSVC)
    target_compile_definitions(Mango PUBLIC NOMINMAX)
endif()

macro(message_option option)
    if(${option})
        message(STATUS "  ${option}: ON")
    else()
        message(STATUS "  ${option}: OFF")
    endif()
endmacro()

message(STATUS "Mango will be built with the following options:")
message_option(MG_PROFILE_ENABLE)
message_option(MG_BUILD_MANGO_SANDBOX)