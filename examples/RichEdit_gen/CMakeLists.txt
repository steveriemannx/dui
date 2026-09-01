cmake_minimum_required(VERSION 3.18)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(RichEdit_gen CXX)

get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

include("${DUI_SRC_ROOT_DIR}/cmake/dui_common.cmake")

if(APPLE)
    set(EMBED_THEME macos26)
elseif(WIN32)
    set(EMBED_THEME windows11)
else()
    set(EMBED_THEME default)
endif()
# Mode 2 (XML-to-code generation): convert the active theme's XML layouts to C++.
# color_combox.xml remains a code-built internal template.
set(GEN_XML_FILES
    "${DUI_ROOT}/resources/themes/${EMBED_THEME}/rich_edit/rich_edit.xml"
    "${DUI_ROOT}/resources/themes/${EMBED_THEME}/rich_edit/find.xml"
    "${DUI_ROOT}/resources/themes/${EMBED_THEME}/rich_edit/replace.xml")
set(GEN_FUNC_NAME Init)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_gen_code.cmake")

# Embed resources into the executable; embedded resources are accessed directly from memory.
set(EMBED_RES_DIR "${DUI_ROOT}/resources")
set(EMBED_RES_PATHS
    "themes/${EMBED_THEME}/global.xml"
    "themes/${EMBED_THEME}/public"
    "themes/${EMBED_THEME}/rich_edit"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

include("${DUI_SRC_ROOT_DIR}/cmake/dui_bin.cmake")
