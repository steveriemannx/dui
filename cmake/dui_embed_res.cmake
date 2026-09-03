# Embed resources into the executable. Generates "embedded_resources.inc" in the
# build directory; embedded resources are accessed directly from memory at runtime.
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
#   - Windows : compiled at configure time into build/tools/embed_resources.exe when
#               missing or the sources changed (run inside the vcvarsall environment)
#   - Others  : compiled from tools/embed_resources.cpp at build time.

if(NOT DEFINED EMBED_RES_DIR)
    set(EMBED_RES_DIR "${DUI_ROOT}/resources")
endif()

# Optional subset of resources to embed. When EMBED_RES_PATHS is set (list of
# paths, absolute or relative to EMBED_RES_DIR), only those subpaths are
# embedded; otherwise the whole EMBED_RES_DIR tree is embedded (default).
# This lets an example package just the common (theme global.xml / public /
# fonts) resources plus its own project resources, instead of everything.
set(EMBED_RES_FILTERS)
if(DEFINED EMBED_RES_PATHS)
    foreach(_p ${EMBED_RES_PATHS})
        if(IS_ABSOLUTE "${_p}")
            file(RELATIVE_PATH _relp "${EMBED_RES_DIR}" "${_p}")
        else()
            set(_relp "${_p}")
        endif()
        list(APPEND EMBED_RES_FILTERS "${_relp}")
    endforeach()
endif()

set(TOOL_SRC "${DUI_SRC_ROOT_DIR}/tools/embed_resources.cpp")
set(TOOL_BIN "${CMAKE_BINARY_DIR}/tools/embed_resources")
set(GENERATED_INC "${CMAKE_CURRENT_BINARY_DIR}/embedded_resources.inc")

if(DUI_OS_WINDOWS)
    # Windows: compiled at configure time when missing or the sources changed
    set(TOOL_EXE "${CMAKE_BINARY_DIR}/tools/embed_resources.exe")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/tools")

    if("${TOOL_SRC}" IS_NEWER_THAN "${TOOL_EXE}")
        message(STATUS "Rebuilding embed_resources.exe (source changed)")
        dui_build_msvc_tool(_tool_ok "${TOOL_SRC}" "${TOOL_EXE}" "")
        if(NOT _tool_ok)
            message(WARNING "Failed to rebuild embed_resources.exe automatically.\n"
                            "Compile it manually in a VS Developer Command Prompt:\n"
                            "  cl /nologo /std:c++17 /O2 /EHsc tools/embed_resources.cpp /Fe:build/tools/embed_resources.exe")
        endif()
    endif()

    if(NOT EXISTS "${TOOL_EXE}")
        message(FATAL_ERROR "embed_resources.exe not found at ${TOOL_EXE}.\n"
                "Compile it manually in a VS Developer Command Prompt:\n"
                "  cl /nologo /std:c++17 /O2 /EHsc tools/embed_resources.cpp /Fe:build/tools/embed_resources.exe")
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

# Collect the resource files as dependencies.
# EMBED_RES_PATHS entries are either single files (e.g. themes/windows11/global.xml)
# or directories (e.g. themes/windows11/public); a plain GLOB_RECURSE over a
# directory path matches nothing, so resolve the directory contents recursively
# (otherwise editing a file inside such a folder never re-runs the embed step).
if(DEFINED EMBED_RES_PATHS)
    set(RES_FILES)
    foreach(_p ${EMBED_RES_PATHS})
        if(IS_ABSOLUTE "${_p}")
            set(_match "${_p}")
        else()
            set(_match "${EMBED_RES_DIR}/${_p}")
        endif()
        if(IS_DIRECTORY "${_match}")
            file(GLOB_RECURSE _sub "${_match}/*")
        else()
            set(_sub "${_match}")
        endif()
        list(APPEND RES_FILES ${_sub})
    endforeach()
else()
    file(GLOB_RECURSE RES_FILES "${EMBED_RES_DIR}/*")
endif()

# Generate the embedded resources .inc
add_custom_command(
    OUTPUT "${GENERATED_INC}"
    COMMAND "${TOOL_EXE}" "${EMBED_RES_DIR}" "${GENERATED_INC}" ${EMBED_RES_FILTERS}
    DEPENDS "${TOOL_EXE}" ${RES_FILES}
    COMMENT "Embedding resources from ${EMBED_RES_DIR}"
)

add_custom_target("${PROJECT_NAME}_embed_res" DEPENDS "${GENERATED_INC}")
list(APPEND DUI_GENERATED_INCLUDE_DIRS "${CMAKE_CURRENT_BINARY_DIR}")

# Stash for dui_bin_*.cmake to add the dependency
set(DUI_EMBED_RES_SRC "${GENERATED_INC}" CACHE INTERNAL "")
