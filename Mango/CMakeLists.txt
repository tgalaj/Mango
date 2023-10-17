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

target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE LIBRARY_SUFFIX="")

list(APPEND INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)
						 
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "$<BUILD_INTERFACE:${INCLUDE_PATH}>")

target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC ${OPENGL_LIBRARIES} 
													glad
										            stb_image
                                                    assimp
									                entityx
                                                    glfw
                                                    imgui
                                                    spdlog
                                                    glm::glm
                                                    tinyddsloader)

if(MinGW)
    target_link_libraries(${CMAKE_PROJECT_NAME} bz2)
endif()

if(MSVC)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC NOMINMAX)
endif()