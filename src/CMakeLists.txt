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

# Define the library
add_library(${PROJECT_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES} ${RES_FILES})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)

set_source_files_properties(${RES_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

# Define the include DIRs
target_include_directories(${PROJECT_NAME} PUBLIC "${VertexEngine_SOURCE_DIR}/include")
target_include_directories(${PROJECT_NAME} PUBLIC "${ASSIMP_INCLUDE_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC "${ENTITYX_INCLUDE_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC "${GLFW_INCLUDE_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC "${FREETYPE_INCLUDE_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC "${GLAD_INCLUDE_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC "${GLM_INCLUDE_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC "${IMGUI_INCLUDE_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC "${STB_IMAGE_INCLUDE_DIR}")

target_link_libraries(${PROJECT_NAME} "${OPENGL_LIBRARY}")
target_link_libraries(${PROJECT_NAME} "${ASSIMP_LIBRARY}")
target_link_libraries(${PROJECT_NAME} "${ENTITYX_LIBRARY}")
target_link_libraries(${PROJECT_NAME} "${GLFW_LIBRARY}")
target_link_libraries(${PROJECT_NAME} "${FREETYPE_LIBRARY}")
target_link_libraries(${PROJECT_NAME} "${GLAD_LIBRARY}"      "${CMAKE_DL_LIBS}")
target_link_libraries(${PROJECT_NAME} "${IMGUI_LIBRARY}"     "${CMAKE_DL_LIBS}")
target_link_libraries(${PROJECT_NAME} "${STB_IMAGE_LIBRARY}" "${CMAKE_DL_LIBS}")

target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE)
target_compile_definitions(${PROJECT_NAME} PRIVATE LIBRARY_SUFFIX="")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "sources"   FILES ${SOURCE_FILES})						   
source_group(TREE ${CMAKE_SOURCE_DIR}/include PREFIX "headers"   FILES ${HEADER_FILES})
source_group(TREE ${CMAKE_SOURCE_DIR}/res     PREFIX "res" 	     FILES ${RES_FILES})