cmake_minimum_required(VERSION 3.18)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(layout_gen CXX)

get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

include("${DUI_SRC_ROOT_DIR}/cmake/dui_common.cmake")

# Mode 2 (XML-to-code generation): convert the platform layout.xml to pure C++ code.
if(APPLE)
    set(LAYOUT_THEME macos26)
elseif(WIN32)
    set(LAYOUT_THEME windows11)
else()
    set(LAYOUT_THEME default)
endif()
set(GEN_XML_FILES "${DUI_ROOT}/resources/themes/${LAYOUT_THEME}/layout/layout.xml")
set(GEN_FUNC_NAME Init)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_gen_code.cmake")

set(EMBED_RES_DIR "${DUI_ROOT}/resources")
set(EMBED_RES_PATHS "themes/${LAYOUT_THEME}/global.xml" "themes/${LAYOUT_THEME}/public" "themes/${LAYOUT_THEME}/layout" "fonts" "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_bin.cmake")
