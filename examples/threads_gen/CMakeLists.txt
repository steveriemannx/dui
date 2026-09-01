cmake_minimum_required(VERSION 3.18)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(threads_gen CXX)

get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

include("${DUI_SRC_ROOT_DIR}/cmake/dui_common.cmake")

# Mode 2 (XML-to-code generation): convert threads.xml to pure C++ code at build time.
if(APPLE)
    set(THREADS_THEME macos26)
elseif(WIN32)
    set(THREADS_THEME windows11)
else()
    set(THREADS_THEME default)
endif()
set(GEN_XML_FILES "${DUI_ROOT}/resources/themes/${THREADS_THEME}/threads/threads.xml")
set(GEN_FUNC_NAME Init)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_gen_code.cmake")

# Embed resources into the executable; embedded resources are accessed directly from memory.
set(EMBED_RES_DIR "${DUI_ROOT}/resources")
set(EMBED_RES_PATHS
    "themes/${THREADS_THEME}/global.xml"
    "themes/${THREADS_THEME}/public"
    "themes/${THREADS_THEME}/threads"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

include("${DUI_SRC_ROOT_DIR}/cmake/dui_bin.cmake")
