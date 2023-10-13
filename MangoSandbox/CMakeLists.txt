project("MangoSandbox")

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

# Define the executable
add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES} ${ASSETS_FILES})

# Define the include DIRs
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
#target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Define the link libraries
target_link_libraries(${PROJECT_NAME} "${CMAKE_PROJECT_NAME}")

# TODO: Better use config file with macro to res directory than copying it after building
add_custom_command(TARGET  ${PROJECT_NAME} POST_BUILD
				   COMMAND ${CMAKE_COMMAND} -E copy_directory
						   ${CMAKE_CURRENT_SOURCE_DIR}/assets
						   ${CMAKE_CURRENT_BINARY_DIR}/assets)

add_custom_command(TARGET  ${PROJECT_NAME} POST_BUILD
				   COMMAND ${CMAKE_COMMAND} -E copy_directory
						   ${CMAKE_SOURCE_DIR}/Mango/assets
						   ${CMAKE_CURRENT_BINARY_DIR}/assets)