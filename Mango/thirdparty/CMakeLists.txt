# CPM - package manager
include(CPM)

# OpenGL
find_package(OpenGL REQUIRED)

# glad
set(GLAD_DIR ${CMAKE_CURRENT_LIST_DIR}/glad)
set(glad_SOURCE_DIR ${GLAD_DIR}/include CACHE INTERNAL "")
add_library(glad STATIC ${GLAD_DIR}/src/glad.c)
target_include_directories(glad PUBLIC ${GLAD_DIR}/include)

# stb_image
set(STB_IMAGE_DIR ${CMAKE_CURRENT_LIST_DIR}/stb_image)
set(stb_image_SOURCE_DIR ${STB_IMAGE_DIR} CACHE INTERNAL "")
add_library(stb_image STATIC ${STB_IMAGE_DIR}/stb_image.cpp 
						     ${STB_IMAGE_DIR}/stb_image_write.cpp 
							 ${STB_IMAGE_DIR}/stb_image_resize.cpp)
target_include_directories(stb_image PUBLIC ${STB_IMAGE_DIR})

# other
CPMAddPackage("gh:g-truc/glm#0.9.9.8")
CPMAddPackage("gh:ocornut/imgui@1.88")
CPMAddPackage("gh:gabime/spdlog@1.10.0")
CPMAddPackage("gh:jarro2783/cxxopts@3.1.1")

CPMAddPackage(
    NAME assimp
	GITHUB_REPOSITORY assimp/assimp
    VERSION 5.3.1
	OPTIONS
	    "BUILD_SHARED_LIBS OFF"
	    "ASSIMP_NO_EXPORT ON"  
	    "ASSIMP_BUILD_ASSIMP_TOOLS OFF"
	    "ASSIMP_BUILD_TESTS OFf"
	    "ASSIMP_INSTALL_PDB OFF"
	    "ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF"
	    "ASSIMP_BUILD_OBJ_IMPORTER ON"
	    "ASSIMP_BUILD_GLTF_IMPORTER ON"  
	    "ASSIMP_BUILD_FBX_IMPORTER ON"
	    "ASSIMP_BUILD_MD5_IMPORTER ON" 
	    "ASSIMP_BUILD_X_IMPORTER ON"
)

CPMAddPackage(
    NAME glfw
	GITHUB_REPOSITORY glfw/glfw
    GIT_TAG 3.3.8
	OPTIONS
	    "GLFW_BUILD_EXAMPLES OFF"
	    "GLFW_BUILD_TESTS OFF"
	    "GLFW_BUILD_DOCS OFF"
	    "GLFW_INSTALL OFF"
)

CPMAddPackage(
    GITHUB_REPOSITORY benikabocha/tinyddsloader
    GIT_TAG master
    GIT_SHALLOW TRUE
    DOWNLOAD_ONLY TRUE)

if (tinyddsloader_ADDED)
    add_library(tinyddsloader INTERFACE)
    target_include_directories(tinyddsloader INTERFACE ${tinyddsloader_SOURCE_DIR})
endif()

CPMAddPackage(
    NAME entt
	GITHUB_REPOSITORY skypjack/entt
    VERSION 3.12.2
    DOWNLOAD_ONLY True
	GIT_SHALLOW TRUE
)

if(entt_ADDED)
  add_library(entt INTERFACE)
  target_include_directories(entt INTERFACE ${entt_SOURCE_DIR}/single_include/entt)
endif()

add_library(imgui STATIC ${imgui_SOURCE_DIR}/imgui.cpp
					     ${imgui_SOURCE_DIR}/imgui_demo.cpp
					     ${imgui_SOURCE_DIR}/imgui_draw.cpp
						 ${imgui_SOURCE_DIR}/imgui_tables.cpp
					     ${imgui_SOURCE_DIR}/imgui_widgets.cpp)
target_include_directories(imgui INTERFACE ${imgui_SOURCE_DIR})
						 
set_target_properties(glad
                      stb_image 
                      assimp
                      glfw 
                      glm 
                      imgui 
                      spdlog PROPERTIES FOLDER "thirdparty")
set_target_properties(update_mappings PROPERTIES FOLDER "thirdparty/GLFW3")

if(MSVC)
	set_target_properties(UpdateAssimpLibsDebugSymbolsAndDLLs uninstall PROPERTIES FOLDER "thirdparty/assimps")
endif()

if (TARGET zlibstatic)
    set_target_properties(zlibstatic PROPERTIES FOLDER "thirdparty")
endif()