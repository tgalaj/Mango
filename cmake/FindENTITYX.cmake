# - Try to find Assimp
# Once done, this will define
#
# ENTITYX_FOUND - system has Assimp
# ENTITYX_INCLUDE_DIR - the Assimp include directories
# ENTITYX_LIBRARIES - link these to use Assimp
FIND_PATH( ENTITYX_INCLUDE_DIR entityx/entityx.h
	/usr/include
	/usr/local/include
	/opt/local/include
	"${VertexEngine_SOURCE_DIR}/thirdparty"
)

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
	FIND_LIBRARY( ENTITYX_LIBRARY entityx
		/usr/lib
		/usr/local/lib
		/opt/local/lib
		"${VertexEngine_SOURCE_DIR}/lib/x32"
	)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
	FIND_LIBRARY( ENTITYX_LIBRARY entityx
		/usr/lib64
		/usr/local/lib
		/opt/local/lib
		"${VertexEngine_SOURCE_DIR}/lib/x64"
	)
endif()

IF(ENTITYX_INCLUDE_DIR AND ENTITYX_LIBRARY)
	SET( ENTITYX_FOUND TRUE )
	SET( ENTITYX_LIBRARIES ${ENTITYX_LIBRARY} )
ENDIF(ENTITYX_INCLUDE_DIR AND ENTITYX_LIBRARY)
IF(ENTITYX_FOUND)
	IF(NOT ENTITYX_FIND_QUIETLY)
	MESSAGE(STATUS "Found ENTITYX: ${ENTITYX_LIBRARY}")
	ENDIF(NOT ENTITYX_FIND_QUIETLY)
ELSE(ENTITYX_FOUND)
	IF(ENTITYX_FIND_REQUIRED)
	MESSAGE(FATAL_ERROR "Could not find libASSIMP")
	ENDIF(ENTITYX_FIND_REQUIRED)
ENDIF(ENTITYX_FOUND)
