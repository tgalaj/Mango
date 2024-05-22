# CPM - package manager
include(CPM)

# OpenGL
find_package(OpenGL REQUIRED)

# glad
set(GLAD_DIR ${CMAKE_CURRENT_LIST_DIR}/glad)
set(glad_SOURCE_DIR ${GLAD_DIR}/include CACHE INTERNAL "")
add_library(glad STATIC ${GLAD_DIR}/src/glad.c)
target_include_directories(glad PUBLIC ${GLAD_DIR}/include)

CPMAddPackage("gh:g-truc/glm#1.0.0")
CPMAddPackage("gh:gabime/spdlog@1.10.0")
CPMAddPackage("gh:jarro2783/cxxopts@3.1.1")

CPMAddPackage(
    NAME assimp
    GITHUB_REPOSITORY assimp/assimp
    VERSION 5.3.1
    GIT_SHALLOW TRUE
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
    GIT_SHALLOW TRUE
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
    GITHUB_REPOSITORY tgalaj/strutil
    VERSION 1.1.0
    GIT_SHALLOW TRUE
    DOWNLOAD_ONLY TRUE)

if (strutil_ADDED)
    add_library(strutil INTERFACE)
    target_include_directories(strutil INTERFACE ${strutil_SOURCE_DIR})
endif()

CPMAddPackage(
    GITHUB_REPOSITORY nothings/stb
    GIT_TAG master
    GIT_SHALLOW TRUE
    DOWNLOAD_ONLY TRUE)

if (stb_ADDED)
    add_library(stb INTERFACE)
    target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})
endif()

CPMAddPackage(
    NAME entt
    GITHUB_REPOSITORY skypjack/entt
    VERSION 3.13.1
    DOWNLOAD_ONLY True
    GIT_SHALLOW TRUE
)

if (entt_ADDED)
  add_library(entt INTERFACE)
  target_include_directories(entt INTERFACE ${entt_SOURCE_DIR}/single_include/entt)
endif()

set(tracy_VERSION 0.10)
CPMAddPackage(
    NAME tracy
    GITHUB_REPOSITORY wolfpld/tracy
    VERSION ${tracy_VERSION}
    GIT_SHALLOW TRUE
    OPTIONS
        "TRACY_ENABLE ON"
        "TRACY_ON_DEMAND OFF"
        "TRACY_ONLY_LOCALHOST ON"
)

set(TRACY_SERVER_ARCHIVE_PATH "${CMAKE_BINARY_DIR}/TracyServer/Tracy-${tracy_VERSION}.7z")
if (NOT EXISTS "${TRACY_SERVER_ARCHIVE_PATH}")
    file(DOWNLOAD "https://github.com/wolfpld/tracy/releases/download/v${tracy_VERSION}/Tracy-${tracy_VERSION}.7z"  
         "${TRACY_SERVER_ARCHIVE_PATH}" 
         SHOW_PROGRESS)
    file(ARCHIVE_EXTRACT INPUT ${TRACY_SERVER_ARCHIVE_PATH} DESTINATION "${CMAKE_BINARY_DIR}/TracyServer/Tracy-${tracy_VERSION}")
endif()

CPMAddPackage(
    NAME physfs
    GITHUB_REPOSITORY icculus/physfs
    GIT_TAG main
    GIT_SHALLOW TRUE
    OPTIONS
        "PHYSFS_ARCHIVE_GRP OFF"
        "PHYSFS_ARCHIVE_WAD OFF"
        "PHYSFS_ARCHIVE_CSM OFF"
        "PHYSFS_ARCHIVE_HOG OFF"
        "PHYSFS_ARCHIVE_MVL OFF"
        "PHYSFS_ARCHIVE_QPAK OFF"
        "PHYSFS_ARCHIVE_SLB OFF"
        "PHYSFS_ARCHIVE_ISO9660 OFF"
        "PHYSFS_ARCHIVE_VDF OFF"
        "PHYSFS_BUILD_STATIC ON"
        "PHYSFS_BUILD_SHARED OFF"
        "PHYSFS_BUILD_TEST OFF"
        "PHYSFS_DISABLE_INSTALL ON"
        "PHYSFS_BUILD_DOCS OFF"
)

CPMAddPackage(
    NAME taskflow
    GITHUB_REPOSITORY taskflow/taskflow
    VERSION 3.6.0
    GIT_SHALLOW TRUE
    OPTIONS
        "TF_BUILD_TESTS OFF"
        "TF_BUILD_EXAMPLES OFF"
)

CPMAddPackage(
    NAME JoltPhysics
    GITHUB_REPOSITORY jrouwe/JoltPhysics
    VERSION 5.0.0
    GIT_SHALLOW TRUE
    SOURCE_SUBDIR "Build"
    OPTIONS
        "DOUBLE_PRECISION OFF"
        "GENERATE_DEBUG_SYMBOLS ON"
        "CROSS_PLATFORM_DETERMINISTIC OFF"
        "INTERPROCEDURAL_OPTIMIZATION ON"
        "FLOATING_POINT_EXCEPTIONS_ENABLED OFF"
        "OBJECT_LAYER_BITS 16"
        "USE_STATIC_MSVC_RUNTIME_LIBRARY ${MG_USE_STATIC_MSVC_RUNTIME_LIBRARY}"
)

CPMAddPackage(
    NAME imgui
    GITHUB_REPOSITORY ocornut/imgui
    GIT_TAG docking # use the docking branch
    GIT_SHALLOW TRUE
    DOWNLOAD_ONLY TRUE
)

CPMAddPackage(
    NAME imguizmo
    GITHUB_REPOSITORY CedricGuillemet/ImGuizmo
    GIT_TAG master
    GIT_SHALLOW TRUE
    DOWNLOAD_ONLY TRUE
)

if (imgui_ADDED AND imguizmo_ADDED)
    add_library(imgui STATIC ${imgui_SOURCE_DIR}/imgui.cpp
                             ${imgui_SOURCE_DIR}/imgui_demo.cpp
                             ${imgui_SOURCE_DIR}/imgui_draw.cpp
                             ${imgui_SOURCE_DIR}/imgui_tables.cpp
                             ${imgui_SOURCE_DIR}/imgui_widgets.cpp
                             ${imguizmo_SOURCE_DIR}/ImGuizmo.cpp)
    target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${imguizmo_SOURCE_DIR})
    target_compile_definitions(imgui PUBLIC -DIMGUI_USE_WCHAR32)
endif()

CPMAddPackage(
    NAME portableFileDialogs
    GITHUB_REPOSITORY samhocevar/portable-file-dialogs
    GIT_TAG main
    GIT_SHALLOW TRUE
    DOWNLOAD_ONLY TRUE
)

if (portableFileDialogs_ADDED)
    add_library(portableFileDialogs INTERFACE)
    target_include_directories(portableFileDialogs INTERFACE ${portableFileDialogs_SOURCE_DIR})
endif()

CPMAddPackage(
    NAME yaml-cpp
    GITHUB_REPOSITORY jbeder/yaml-cpp
    GIT_TAG 0.8.0
    GIT_SHALLOW TRUE
)

set_target_properties(glad 
                      assimp
                      glfw 
                      glm 
                      imgui 
                      spdlog
                      TracyClient
                      physfs-static 
                      Jolt
                      yaml-cpp
                      yaml-cpp-parse
                      yaml-cpp-read
                      yaml-cpp-sandbox
                      PROPERTIES FOLDER "thirdparty")

set_target_properties(update_mappings PROPERTIES FOLDER "thirdparty/GLFW3")
set_target_properties(Continuous Experimental Nightly NightlyMemoryCheck PROPERTIES FOLDER "thirdparty/CTestDashboardTargets")

if (MSVC)
    set_target_properties(UpdateAssimpLibsDebugSymbolsAndDLLs uninstall PROPERTIES FOLDER "thirdparty/assimps")
endif()

if (TARGET zlibstatic)
    set_target_properties(zlibstatic PROPERTIES FOLDER "thirdparty")
endif()