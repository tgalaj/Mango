set(SAMPLE_NAME "SampleGame")

# Add .lib files
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
	link_directories(${VertexEngine_SOURCE_DIR}/lib/x32)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
	link_directories(${VertexEngine_SOURCE_DIR}/lib/x64)
endif()

# Add source files
file(GLOB_RECURSE SOURCE_FILES_EXE 
	${CMAKE_CURRENT_SOURCE_DIR}/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
	
# Add header files
file(GLOB_RECURSE HEADER_FILES_EXE 
	${CMAKE_CURRENT_SOURCE_DIR}/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
	
# Define the include DIRs
include_directories("${VertexEngine_SOURCE_DIR}/include"
				    "${VertexEngine_SOURCE_DIR}/thirdparty"
				    ${CMAKE_CURRENT_SOURCE_DIR})	

# Define the executable
add_executable(${SAMPLE_NAME} ${HEADER_FILES_EXE} ${SOURCE_FILES_EXE})

# Set SampleGame as startup project in Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
			 PROPERTY VS_STARTUP_PROJECT ${SAMPLE_NAME})

# Define the link libraries
target_link_libraries(${SAMPLE_NAME} ${PROJECT_NAME})

# Copy dlls and resources
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
	add_custom_command(TARGET  ${SAMPLE_NAME} POST_BUILD
			           COMMAND ${CMAKE_COMMAND} -E copy_directory
			           ${VertexEngine_SOURCE_DIR}/dlls/bin
			           ${CMAKE_CURRENT_BINARY_DIR})
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
	add_custom_command(TARGET  ${SAMPLE_NAME} POST_BUILD
				       COMMAND ${CMAKE_COMMAND} -E copy_directory
				       ${VertexEngine_SOURCE_DIR}/dlls/bin64
				       ${CMAKE_CURRENT_BINARY_DIR})
endif()

add_custom_command(TARGET  ${SAMPLE_NAME} POST_BUILD
				   COMMAND ${CMAKE_COMMAND} -E copy_directory
						   ${VertexEngine_SOURCE_DIR}/res
						   ${CMAKE_CURRENT_BINARY_DIR}/res)

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
				string(REPLACE "example_game" "sources" groupname ${groupname})
				source_group(${groupname} FILES ${VertexEngine_SOURCE_DIR}/${curdir}/${child})
			endif()
		endforeach()
	endmacro()
	
	# Run macro
	GroupSources(example_game)
endif()