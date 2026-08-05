# Embed a resources directory into the executable as a custom binary archive
# (Qt qrc style, no zip container). Generates "embedded_resources.inc" in the
# build directory; the resources are accessed directly from memory at runtime.
#
# Usage in example CMakeLists.txt:
#   set(EMBED_RES_DIR "${DUI_ROOT}/resources")
#   include(dui_embed_res.cmake)
#
# In your code (exactly one .cpp of the executable):
#   #include "embedded_resources.inc"
#   ui::GlobalManager::Instance().Startup(
#       ui::MemoryResParam(GetEmbeddedResourcesData(), GetEmbeddedResourcesSize()));
#
# Note: the resource files are collected with file(GLOB_RECURSE) at configure
# time; after adding new files to the resources directory, re-run cmake.
#
# Tool:
#   - Windows : compiled at configure time into cmake/embed_resources.exe when
#               missing or the sources changed (run inside the vcvarsall environment)
#   - Others  : compiled from cmake/embed_resources.cpp at build time.

if(NOT DEFINED EMBED_RES_DIR)
    set(EMBED_RES_DIR "${DUI_ROOT}/resources")
endif()

set(TOOL_SRC "${DUI_SRC_ROOT_DIR}/cmake/embed_resources.cpp")
set(TOOL_BIN "${CMAKE_CURRENT_BINARY_DIR}/embed_resources")
set(GENERATED_INC "${CMAKE_CURRENT_BINARY_DIR}/embedded_resources.inc")

if(DUI_OS_WINDOWS)
    # Windows: compiled at configure time when missing or the sources changed
    set(TOOL_EXE "${DUI_SRC_ROOT_DIR}/cmake/embed_resources.exe")

    if("${TOOL_SRC}" IS_NEWER_THAN "${TOOL_EXE}")
        message(STATUS "Rebuilding embed_resources.exe (source changed)")
        dui_build_msvc_tool(_tool_ok "${TOOL_SRC}" "${TOOL_EXE}" "")
        if(NOT _tool_ok)
            message(WARNING "Failed to rebuild embed_resources.exe automatically.\n"
                            "Compile it manually in a VS Developer Command Prompt:\n"
                            "  cl /nologo /std:c++17 /O2 /EHsc cmake/embed_resources.cpp /Fe:cmake/embed_resources.exe")
        endif()
    endif()

    if(NOT EXISTS "${TOOL_EXE}")
        message(FATAL_ERROR "embed_resources.exe not found at ${TOOL_EXE}.\n"
                "Compile it manually in a VS Developer Command Prompt:\n"
                "  cl /nologo /std:c++17 /O2 /EHsc cmake/embed_resources.cpp /Fe:cmake/embed_resources.exe")
    endif()
else()
    # macOS / Linux / FreeBSD: compile from source at build time
    set(TOOL_EXE "${TOOL_BIN}")
    add_custom_command(
        OUTPUT "${TOOL_EXE}"
        COMMAND ${CMAKE_CXX_COMPILER} -std=c++17 -O2 "${TOOL_SRC}" -o "${TOOL_EXE}"
        DEPENDS "${TOOL_SRC}"
        COMMENT "Building embed_resources tool"
    )
endif()

# Collect the resource files as dependencies
file(GLOB_RECURSE RES_FILES "${EMBED_RES_DIR}/*")

# Generate the embedded resources .inc
add_custom_command(
    OUTPUT "${GENERATED_INC}"
    COMMAND "${TOOL_EXE}" "${EMBED_RES_DIR}" "${GENERATED_INC}"
    DEPENDS "${TOOL_EXE}" ${RES_FILES}
    COMMENT "Embedding resources from ${EMBED_RES_DIR}"
)

add_custom_target("${PROJECT_NAME}_embed_res" DEPENDS "${GENERATED_INC}")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Stash for dui_bin_*.cmake to add the dependency
set(DUI_EMBED_RES_SRC "${GENERATED_INC}" CACHE INTERNAL "")
