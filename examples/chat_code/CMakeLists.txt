cmake_minimum_required(VERSION 3.18)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(chat_code CXX)

get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

include("${DUI_SRC_ROOT_DIR}/cmake/dui_common.cmake")

set(EMBED_RES_DIR "${DUI_ROOT}/resources")
if(APPLE)
    set(CHAT_THEME macos26)
elseif(WIN32)
    set(CHAT_THEME windows11)
else()
    set(CHAT_THEME default)
endif()
set(EMBED_RES_PATHS
    "themes/${CHAT_THEME}/global.xml"
    "themes/${CHAT_THEME}/public"
    "themes/${CHAT_THEME}/chat"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

include("${DUI_SRC_ROOT_DIR}/cmake/dui_bin.cmake")
