set(SAMPLE_NAME "SampleGame")

# Add source files
file(GLOB_RECURSE SOURCE_FILES_EXE 
	 ${CMAKE_CURRENT_SOURCE_DIR}/*.c
	 ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Add header files
file(GLOB_RECURSE HEADER_FILES_EXE 
	 ${CMAKE_CURRENT_SOURCE_DIR}/*.h
	 ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

# Define the executable
add_executable(${SAMPLE_NAME} ${HEADER_FILES_EXE} ${SOURCE_FILES_EXE})
set_property(TARGET ${SAMPLE_NAME} PROPERTY CXX_STANDARD 11)

# Define the include DIRs
target_include_directories(${SAMPLE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${SAMPLE_NAME} PRIVATE ${VertexEngine_SOURCE_DIR}/include)

# Define the link libraries
target_link_libraries(${SAMPLE_NAME} "${PROJECT_NAME}")

# TODO: Better use config file with macro to res directory than copying it after building
add_custom_command(TARGET  ${SAMPLE_NAME} POST_BUILD
				   COMMAND ${CMAKE_COMMAND} -E copy_directory
						   ${VertexEngine_SOURCE_DIR}/res
						   ${CMAKE_CURRENT_BINARY_DIR}/res)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "sources" FILES ${SOURCE_FILES_EXE})						   
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "headers" FILES ${HEADER_FILES_EXE})