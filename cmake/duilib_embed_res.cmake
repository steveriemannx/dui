# Embed a resources directory into the executable as a custom binary archive
# (Qt qrc style, no zip container). Generates "embedded_resources.inc" in the
# build directory; the resources are accessed directly from memory at runtime.
#
# Usage in example CMakeLists.txt:
#   set(EMBED_RES_DIR "${DUILIB_ROOT}/bin/resources")
#   include(duilib_embed_res.cmake)
#
# In your code (exactly one .cpp of the executable):
#   #include "embedded_resources.inc"
#   ui::GlobalManager::Instance().Startup(
#       ui::MemoryResParam(GetEmbeddedResourcesData(), GetEmbeddedResourcesSize()));
#
# Note: the resource files are collected with file(GLOB_RECURSE) at configure
# time; after adding new files to the resources directory, re-run cmake.

if(NOT DEFINED EMBED_RES_DIR)
    set(EMBED_RES_DIR "${DUILIB_ROOT}/bin/resources")
endif()

set(TOOL_SRC "${DUILIB_SRC_ROOT_DIR}/cmake/embed_resources.cpp")
set(TOOL_BIN "${CMAKE_CURRENT_BINARY_DIR}/embed_resources")
set(GENERATED_INC "${CMAKE_CURRENT_BINARY_DIR}/embedded_resources.inc")

# Build the embed_resources tool
add_custom_command(
    OUTPUT "${TOOL_BIN}"
    COMMAND ${CMAKE_CXX_COMPILER} -std=c++17 -O2 "${TOOL_SRC}" -o "${TOOL_BIN}"
    DEPENDS "${TOOL_SRC}"
    COMMENT "Building embed_resources tool"
)

# Collect the resource files as dependencies
file(GLOB_RECURSE RES_FILES "${EMBED_RES_DIR}/*")

# Generate the embedded resources .inc
add_custom_command(
    OUTPUT "${GENERATED_INC}"
    COMMAND "${TOOL_BIN}" "${EMBED_RES_DIR}" "${GENERATED_INC}"
    DEPENDS "${TOOL_BIN}" ${RES_FILES}
    COMMENT "Embedding resources from ${EMBED_RES_DIR}"
)

add_custom_target("${PROJECT_NAME}_embed_res" DEPENDS "${GENERATED_INC}")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Stash for duilib_bin_*.cmake to add the dependency
set(DUILIB_EMBED_RES_SRC "${GENERATED_INC}" CACHE INTERNAL "")
