cmake_minimum_required(VERSION 3.18)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(RichEdit_code CXX)

get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

include("${DUI_SRC_ROOT_DIR}/cmake/dui_common.cmake")

# Embed resources into the executable; embedded resources are accessed directly from memory.
# Only embed the common resources (active theme's global.xml + public assets, plus fonts and
# lang) and this example's own skin (themes/<platform-theme>/rich_edit), not the whole tree.
set(EMBED_RES_DIR "${DUI_ROOT}/resources")
if(APPLE)
    set(EMBED_THEME macos26)
elseif(WIN32)
    set(EMBED_THEME windows11)
else()
    set(EMBED_THEME default)
endif()
set(EMBED_RES_PATHS
    "themes/${EMBED_THEME}/global.xml"
    "themes/${EMBED_THEME}/public"
    "themes/${EMBED_THEME}/rich_edit"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

include("${DUI_SRC_ROOT_DIR}/cmake/dui_bin.cmake")
