# Copyright (C) 2023 Tomasz Gałaj
set(CORE_LIB_NAME ${CMAKE_PROJECT_NAME})

# Add source files
file(GLOB_RECURSE SOURCE_FILES 
	 ${CMAKE_CURRENT_SOURCE_DIR}/*.c
	 ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
	
# Add header files
file(GLOB_RECURSE HEADER_FILES 
	 ${CMAKE_SOURCE_DIR}/include/*.h
	 ${CMAKE_SOURCE_DIR}/include/*.hpp)
	 
# Search for resource files
file(GLOB_RECURSE RES_FILES 
	 ${CMAKE_SOURCE_DIR}/res/*.*)

set_source_files_properties(${RES_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)
source_group(TREE ${CMAKE_SOURCE_DIR}/res PREFIX "res" FILES ${RES_FILES})

# Define the library
add_library(${CORE_LIB_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES} ${RES_FILES})

target_compile_definitions(${CORE_LIB_NAME} PRIVATE GLFW_INCLUDE_NONE)
target_compile_definitions(${CORE_LIB_NAME} PRIVATE LIBRARY_SUFFIX="")

target_include_directories(${CORE_LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
												   ${CMAKE_SOURCE_DIR}/include
												   ${CMAKE_SOURCE_DIR}/include/framework/gui
												   ${CMAKE_SOURCE_DIR}/thirdparty
												   ${glad_SOURCE_DIR}
												   ${stb_image_SOURCE_DIR}
												   ${imgui_SOURCE_DIR})

target_link_libraries(${CORE_LIB_NAME} ${OPENGL_LIBRARIES}
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
    target_link_libraries(${CORE_LIB_NAME} bz2)
endif()

if(MSVC)
    target_compile_definitions(${CORE_LIB_NAME} PUBLIC NOMINMAX)
endif()