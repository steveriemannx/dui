cmake_minimum_required(VERSION 3.18)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(basic_code CXX)

get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

include("${DUI_SRC_ROOT_DIR}/cmake/dui_common.cmake")

# Embed only the platform theme (self-contained: macos26/public now carries all
# shared images, no default-theme fallback) plus this example's own skin
# (themes/<platform-theme>/basic). Always: fonts + lang.
set(EMBED_RES_DIR "${DUI_ROOT}/resources")
if(APPLE)
    set(BASIC_EMBED_THEME macos26)
elseif(WIN32)
    set(BASIC_EMBED_THEME windows11)
else()
    set(BASIC_EMBED_THEME default)
endif()
set(EMBED_RES_PATHS
    "themes/${BASIC_EMBED_THEME}/global.xml"
    "themes/${BASIC_EMBED_THEME}/public"
    "themes/${BASIC_EMBED_THEME}/basic"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

include("${DUI_SRC_ROOT_DIR}/cmake/dui_bin.cmake")
