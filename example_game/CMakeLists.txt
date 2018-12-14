set(SAMPLE_NAME "SampleGame")

# Add source files
file(GLOB_RECURSE SOURCE_FILES_EXE 
	 *.c
	 *.cpp)

# Add header files
file(GLOB_RECURSE HEADER_FILES_EXE 
	 *.h
	 *.hpp)

# Define the executable
add_executable(${SAMPLE_NAME} ${HEADER_FILES_EXE} ${SOURCE_FILES_EXE})
set_property(TARGET ${SAMPLE_NAME} PROPERTY CXX_STANDARD 11)

# Define the include DIRs
target_include_directories(${SAMPLE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${SAMPLE_NAME} PRIVATE ${VertexEngine_SOURCE_DIR}/include)

# Define the link libraries
target_link_libraries(${SAMPLE_NAME} "${PROJECT_NAME}")

add_custom_command(TARGET  ${SAMPLE_NAME} POST_BUILD
				   COMMAND ${CMAKE_COMMAND} -E copy_directory
						   ${VertexEngine_SOURCE_DIR}/res
						   ${CMAKE_CURRENT_BINARY_DIR}/res)

source_group("sources" FILES ${SOURCE_FILES_EXE})						   
source_group("headers" FILES ${HEADER_FILES_EXE})